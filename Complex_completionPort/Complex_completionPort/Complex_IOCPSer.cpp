#include "Complex_IOCPSer.h"
#include "log/InterfaceLogger.h"
#include "SocketWrap.h"
#include "ErrorCodeDef.h"

#include <stdio.h>

CIOCPSer::CIOCPSer()
	: m_bServerStarted(false)
	, m_bShutDown(true)
	, m_wPort(LISTENPORT)
	, m_nMaxConnections(MAXCONNECTEDCOUNT)
	, m_nMaxFreeBuffers(MAXFREEBUFFER)
	, m_nMaxFreeContexts(MAXFREECONTEXT)
	, m_nInitialReads(4)
	, m_pRecvSink(NULL)
	, m_sListen(INVALID_SOCKET)
	, m_lpfnAcceptEx(NULL)
	, m_lpfnGetAcceptExSockaddrs(NULL)
	, m_hListenThread(NULL)
	, m_hAcceptEvent(NULL)
	, m_hRepostEvent(NULL)
	, m_hShutDownEvent(NULL)
	, m_pFreeBufferList(NULL)
	, m_pFreeContextList(NULL)
	, m_pConnectedList(NULL)
	, m_pPostAcceptBufList(NULL)
	, m_unFreeBufferCount(0)
	, m_unFreeContextCount(0)
	, m_unCurrentConnectedCount(0)
	, m_unCuttentPostAcceptBufCount(0)
	, m_unInitAsynAcceptCnt(5)
	, m_unInitWorkThreadCnt(5)
	, m_errorCode(SUCCESS)
{
	m_hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);  //no manually reset and nonsignal
	if(NULL == m_hAcceptEvent)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create AcceptEvent is error !!!\n");
	}


	m_hRepostEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hRepostEvent)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create RepostEvent is error !!!\n");
	}


	m_hShutDownEvent =  CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hShutDownEvent)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create shutdownEvent is error !!!\n");
	}

	InitializeCriticalSection(&m_csFreeContextListLock);
	InitializeCriticalSection(&m_csFreeBufferListLock);
	InitializeCriticalSection(&m_csConnectedListLock);
	InitializeCriticalSection(&m_csPostAcceptBufListLock);
}

CIOCPSer::~CIOCPSer()
{
	if(INVALID_SOCKET != m_sListen)
	{
		closesocket(m_sListen);
		m_sListen = INVALID_SOCKET;
	}

	if (NULL != m_hAcceptEvent)
	{
		CloseHandle(m_hAcceptEvent);
		m_hAcceptEvent = NULL;
	}

	if (NULL != m_hRepostEvent)
	{
		CloseHandle(m_hRepostEvent);
		m_hRepostEvent = NULL;
	}

	if (NULL != m_hShutDownEvent)
	{
		CloseHandle(m_hShutDownEvent);
		m_hShutDownEvent = NULL;
	}

	if (NULL != m_hListenThread)
	{
		CloseHandle(m_hListenThread);
		m_hListenThread = NULL;
	}

	FreeAllIOBuffer();
	FreeAllContextPer();

	DeleteCriticalSection(&m_csFreeBufferListLock);
	DeleteCriticalSection(&m_csFreeContextListLock);
	DeleteCriticalSection(&m_csConnectedListLock);
	DeleteCriticalSection(&m_csPostAcceptBufListLock);
}


bool CIOCPSer::StartSer(unsigned long &errorCode, WORD wPort /* = LISTENPORT */, unsigned int nMaxConnections /* = MAXCONNECTEDCOUNT */, unsigned int nMaxFreeBuffers /* = MAXFREEBUFFER */, 
						unsigned int nMaxFreeContexts /* = MAXFREECONTEXT */, unsigned int nInitialReads /* = 4 */)
{
	if (m_bServerStarted)
		return true;

	if (!m_hAcceptEvent || !m_hRepostEvent || !m_hShutDownEvent)
	{
		errorCode = CREATEEVENT_ERROR;
		return false;
	}


	m_wPort = wPort;
	m_nMaxConnections = nMaxConnections;
	m_nMaxFreeBuffers = nMaxFreeBuffers;
	m_nMaxFreeContexts = nMaxFreeContexts;
	m_nInitialReads = nInitialReads;

	m_bShutDown = false;
	

	m_sListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( INVALID_SOCKET == m_sListen)
	{
		DWORD error_code = GetLastError();
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create socket is error !!! error_code = %d\n", error_code);
		errorCode = CREATESOCKET_ERROR;
		return false;
	}

	sockaddr_in listenAddr = {0};
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port   = htons(m_wPort);
	listenAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	Bind(m_sListen, (sockaddr *)&listenAddr, sizeof(listenAddr));

	Listen(m_sListen, 200);

	m_hCompletion = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_hCompletion)
	{
		DWORD error_code = GetLastError();
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create IOcompletion port is error !!! error_code = %d\n", error_code);
		errorCode = CREATECOMPLETIONPORT_ERROR;
		return false;
	}

	// 加载扩展函数AcceptEx
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes;
	::WSAIoctl(m_sListen, 
		SIO_GET_EXTENSION_FUNCTION_POINTER, 
		&GuidAcceptEx, 
		sizeof(GuidAcceptEx),
		&m_lpfnAcceptEx, 
		sizeof(m_lpfnAcceptEx), 
		&dwBytes, 
		NULL, 
		NULL);

	if (NULL == m_lpfnAcceptEx)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "load AcceptEx funcation is error !!!\n");
		errorCode = LOADFUN_ERROR;
		return false;
	}

	// 加载扩展函数GetAcceptExSockaddrs
	GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	::WSAIoctl(m_sListen,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockaddrs,
		sizeof(GuidGetAcceptExSockaddrs),
		&m_lpfnGetAcceptExSockaddrs,
		sizeof(m_lpfnGetAcceptExSockaddrs),
		&dwBytes,
		NULL,
		NULL
		);

	if (NULL == m_lpfnAcceptEx)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "load GetAcceptExSockAddrs funcation is error !!!\n");
		errorCode = LOADFUN_ERROR;
		return false;
	}

	CreateIoCompletionPort((HANDLE)m_sListen, m_hCompletion, (DWORD)0, 0);   //对于监听套接字，没有传句柄唯一对象

	//如果为null则不注册FD_ACCEPT事件
	if (NULL != m_hAcceptEvent)
		WSAEventSelect(m_sListen, m_hAcceptEvent, FD_ACCEPT);

	unsigned int  unThreadID;
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenWorkThread, this, 0, &unThreadID);
	if (NULL == m_hListenThread )
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create listen thread funcation is error , exit listen thread funcation!!!\n");
		errorCode = CREATETHREAD_ERROR;
		return false;
	}

	errorCode = SUCCESS;
	m_bServerStarted = true;
	return true;
}

void CIOCPSer::StopSer()
{
	m_bShutDown = true;
	SetEvent(m_hShutDownEvent);
}


void CIOCPSer::AdviseSink(IRecvSink *sink)
{
	m_pRecvSink = sink;
}
void CIOCPSer::join()
{
	WaitForSingleObject(m_hListenThread, INFINITE);
}

//缺陷：再次利用时，可能需要的长度高于了分配的长度，这样非常不好，需要修改？？？？？？可以考虑值返回类型试试（可以）
PCIOCPBuffer CIOCPSer::AllocIOBuffer(int bufLen/* = MAXIOBUFFERSIZE*/)
{
	if (!bufLen || bufLen > MAXIOBUFFERSIZE)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "do not return IOBuffer!!!\n");
		return NULL;
	}

	PCIOCPBuffer IOPer = NULL;
	EnterCriticalSection(&m_csFreeBufferListLock);
	if (NULL == m_pFreeBufferList)   //实际分配
	{
		IOPer =(PCIOCPBuffer)malloc(sizeof(CIOCPBuffer) + bufLen);
		memset(IOPer, 0x0, sizeof(CIOCPBuffer) + bufLen);
		IOPer->InitStruct();
	}
	else                         //从链表中分配
	{
		IOPer = m_pFreeBufferList;
		m_pFreeBufferList = m_pFreeBufferList->pBuffer;
		IOPer->pBuffer     = NULL;
		--m_pFreeBufferList;
	}
	LeaveCriticalSection(&m_csFreeBufferListLock);

	if (NULL != IOPer)
	{
		if (!IOPer->buffLen)
		{
			IOPer->buffLen = bufLen;
		}

		IOPer->ReturnIniStatus();
		IOPer->buff    = (char *)(IOPer + 1);
	}

	return IOPer;
}

void CIOCPSer::FreeIOBuffer(PCIOCPBuffer pIOBuf, bool bForce/* = false*/)
{
	if (pIOBuf)
	{
		EnterCriticalSection(&m_csFreeBufferListLock);
		if (m_unFreeBufferCount < MAXFREEBUFFER && !bForce)  //加入到空闲IOBuffer链表当中去
		{
			int tmpBufLen = pIOBuf->buffLen;
			memset(pIOBuf, 0x0, sizeof(CIOCPBuffer) + pIOBuf->buffLen);
			pIOBuf->buffLen = tmpBufLen;

			pIOBuf->pBuffer = m_pFreeBufferList;
			m_pFreeBufferList = pIOBuf;

			++m_unFreeBufferCount;
		}
		else    //真正的释放空间
		{
			free(pIOBuf);
		}
		LeaveCriticalSection(&m_csFreeBufferListLock);
	}
}

void CIOCPSer::FreeAllIOBuffer()
{
	PCIOCPBuffer pTmpBuffer = NULL;
	EnterCriticalSection(&m_csFreeBufferListLock);
	while(m_pFreeBufferList)
	{
		pTmpBuffer = m_pFreeBufferList;
		m_pFreeBufferList = m_pFreeBufferList->pBuffer;
		FreeIOBuffer(pTmpBuffer, true);
	}
	
	m_pFreeBufferList = NULL;
	m_unFreeBufferCount = 0;

	LeaveCriticalSection(&m_csFreeBufferListLock);
}


PCIOCPContext CIOCPSer::AllocContextPer(SOCKET s, unsigned int MaxRecvCnt/* = 200*/, unsigned MaxSendCnt/* = 200*/)
{
	if(INVALID_SOCKET == s)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "do not return IOCPContext!!!\n");
		return NULL;
	}

	PCIOCPContext pConntext = NULL;

	EnterCriticalSection(&m_csFreeContextListLock);
	if (NULL == m_pFreeContextList)
	{
		pConntext = (PCIOCPContext)malloc(sizeof(CIOCPContext));
		memset(pConntext, 0x0, sizeof(CIOCPContext));
		pConntext->InitStruct(MaxRecvCnt, MaxSendCnt);
	}
	else
	{
		pConntext = m_pFreeContextList;
		m_pFreeContextList = m_pFreeContextList->pNext;
		--m_unFreeContextCount;
	}
	LeaveCriticalSection(&m_csFreeContextListLock);

	if(pConntext) 
	{ 
		pConntext->ReturnInitStatus(MaxRecvCnt, MaxSendCnt);

		pConntext->s = s;
		pConntext->SetMaxAsynRecvCnt(MaxRecvCnt);
		pConntext->SetMaxAsynSendCnt(MaxSendCnt);
	}

	return pConntext;
}


void CIOCPSer::FreeContextPer(PCIOCPContext pContextPer, bool force/* = false*/)
{
	if (pContextPer)
	{
		if (INVALID_SOCKET != pContextPer->s)
		{
			closesocket(pContextPer->s);
			pContextPer->s = INVALID_SOCKET;
		}

		// 释放此套节字上暂时存放的未按序存放的IO唯一结构
		PCIOCPBuffer pTmpBuffer = NULL;
		while(pContextPer->pOutOfOrderReads)
		{
			pTmpBuffer = pContextPer->pOutOfOrderReads->pBuffer;
			FreeIOBuffer(pContextPer->pOutOfOrderReads);
			pContextPer->pOutOfOrderReads = pTmpBuffer;
		}

		EnterCriticalSection(&m_csFreeContextListLock);
		if (m_unFreeContextCount < MAXFREECONTEXT && !force)
		{
			memset(pContextPer, 0x0, sizeof(CIOCPContext));
			pContextPer->pNext = m_pFreeContextList;
			pContextPer->eSockState = CloseSock;
			m_pFreeContextList = pContextPer;

			++m_unFreeContextCount;
		}
		else  //真正的释放空间
		{
			pContextPer->ReleaseStruct();
			free(pContextPer);
		}
		LeaveCriticalSection(&m_csFreeContextListLock);
	}
}

void CIOCPSer::FreeAllContextPer()
{
	PCIOCPContext pTmpContext = NULL;
	EnterCriticalSection(&m_csFreeContextListLock);
	while(m_pFreeContextList)
	{
		pTmpContext = m_pFreeContextList;
		m_pFreeContextList = m_pFreeContextList->pNext;
		FreeContextPer(pTmpContext, true);
	}
	
	m_pFreeContextList = NULL;
	m_unFreeContextCount = 0;

	LeaveCriticalSection(&m_csFreeContextListLock);
}

//return false时代表本服务器可接收的最大链接数已经满了，这时外部调用函数需要处理？？？？？？
BOOL CIOCPSer::AddConnectedList(PCIOCPContext pContext)
{
	BOOL result = FALSE;
	if (pContext)
	{
		EnterCriticalSection(&m_csConnectedListLock);
		if ( m_unCurrentConnectedCount < m_nMaxConnections)
		{
			pContext->pNext = m_pConnectedList;
			m_pConnectedList = pContext;
			++m_unCurrentConnectedCount;
			pContext->eSockState = OpenSock;
			result = TRUE;
		}
		LeaveCriticalSection(&m_csConnectedListLock);
	}
	return result;
}


/*pContext指向的内存，在外部还需要调用FreeContextPer（pContext）函数*///？？？？？？
/*此函数只是关闭了*/
void CIOCPSer::RemoveAndCloseConnected(PCIOCPContext pContext)
{
	if (pContext)
	{
		EnterCriticalSection(&m_csConnectedListLock);
		PCIOCPContext pTmpContext = m_pConnectedList;
		if (pTmpContext == pContext)
		{
			m_pConnectedList = pTmpContext->pNext;
			--m_unCurrentConnectedCount;
		}
		else
		{
			while(pTmpContext && pTmpContext->pNext != pContext)
				pTmpContext = pTmpContext->pNext;
			
			if (pTmpContext)
			{
				pTmpContext->pNext = pContext->pNext;
				--m_unCurrentConnectedCount;
			}
		}
		LeaveCriticalSection(&m_csConnectedListLock);

		if (INVALID_SOCKET != pContext->s)
		{
			closesocket(pContext->s);
			pContext->s = INVALID_SOCKET;
		}
		pContext->eSockState = CloseSock;
		pContext->pNext      = NULL;
		
		FreeContextPer(pContext);           //用于内存的回收管理（更改上面提出的bug）如果加了这句话就不需要在外部再次调用了
	}
}


//关闭了链接但是句柄唯一数据未回收，在函数内部完成了数据的归还给connect链表
void CIOCPSer::RemoveAllConnected()
{
	EnterCriticalSection(&m_csConnectedListLock);
	PCIOCPContext pTmpContext = m_pConnectedList;
	while(pTmpContext)
	{
		m_pConnectedList = pTmpContext->pNext;

		if (INVALID_SOCKET != pTmpContext->s)
		{
			closesocket(pTmpContext->s);
			pTmpContext->s = INVALID_SOCKET;
		}
		pTmpContext->eSockState = CloseSock;
		pTmpContext->pNext      = NULL;

		FreeContextPer(pTmpContext);           //用于内存的回收管理（更改上面提出的bug）

		pTmpContext = m_pConnectedList;
	}
	LeaveCriticalSection(&m_csConnectedListLock);

	m_unCurrentConnectedCount = 0;
	m_pConnectedList          = NULL;
}

BOOL CIOCPSer::AddPendingAccept(PCIOCPBuffer pIOBuffer)
{
	BOOL bResult = FALSE;
	EnterCriticalSection(&m_csPostAcceptBufListLock);
	if (pIOBuffer)
	{
		
		if (m_pPostAcceptBufList)
		{
			pIOBuffer->pBuffer = m_pPostAcceptBufList;
			m_pPostAcceptBufList = pIOBuffer;
		}
		else
		{
			m_pPostAcceptBufList = pIOBuffer;
			pIOBuffer->pBuffer = NULL;
		}
		bResult = TRUE;
	}

	if (bResult)
		++m_unCuttentPostAcceptBufCount;

	LeaveCriticalSection(&m_csPostAcceptBufListLock);
	return bResult;
}


BOOL CIOCPSer::RemovePendingAccept(PCIOCPBuffer pIOBuffer)
{
	BOOL bResult = FALSE;
	if (pIOBuffer)
	{
		EnterCriticalSection(&m_csPostAcceptBufListLock);
		if (pIOBuffer == m_pPostAcceptBufList)
		{
			m_pPostAcceptBufList = m_pFreeBufferList->pBuffer;
			bResult = TRUE;
		}
		else
		{
			PCIOCPBuffer pTmpBuf = m_pPostAcceptBufList;
			while(pTmpBuf != NULL && pTmpBuf->pBuffer != pIOBuffer)
				pTmpBuf = pTmpBuf->pBuffer;

			if (pTmpBuf)
			{
				pTmpBuf->pBuffer = pIOBuffer->pBuffer;
				bResult = TRUE;
			}
		}
		LeaveCriticalSection(&m_csPostAcceptBufListLock);
	}

	if(bResult)
	{
		pIOBuffer->pBuffer = NULL;
		--m_unCuttentPostAcceptBufCount;
	}

	return bResult;
}



BOOL CIOCPSer::PostAccept(PCIOCPBuffer pBuffer)
{
	BOOL bResult = FALSE;
	if (pBuffer && m_lpfnAcceptEx)
	{
		pBuffer->nOperation = OP_ACCEPT;

		pBuffer->sClient = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		DWORD dwBytes = 0;
		BOOL b = m_lpfnAcceptEx(m_sListen, 
			pBuffer->sClient, 
			pBuffer->buff, 
			pBuffer->buffLen - ((sizeof(sockaddr_in) + 16) * 2),
			sizeof(sockaddr_in) + 16,
			sizeof(sockaddr_in) + 16,
			&dwBytes,
			&pBuffer->ol);

		if ((!b && WSA_IO_PENDING == WSAGetLastError()) || b)
			bResult = TRUE;
	}

	return bResult;
}

BOOL CIOCPSer::PostSend(PCIOCPContext pContext, PCIOCPBuffer pBuffer)
{
	EnterCriticalSection(&pContext->Lock);

	BOOL bResult = FALSE;
	if (pBuffer && pContext)
	{
		
		if (pContext->nMaxAsynSendCount > pContext->nOutstandingSend)
		{
			pBuffer->nOperation = OP_WRITE;
			DWORD dwBytes;
			DWORD dwFlags = 0;
			WSABUF buf;
			buf.buf = pBuffer->buff;
			buf.len = pBuffer->buffLen;
			int nConut = WSASend(pContext->s, &buf, 1, &dwBytes, dwFlags, &pBuffer->ol, NULL);
			if (SOCKET_ERROR != nConut || (SOCKET_ERROR == nConut && WSAGetLastError() == WSA_IO_PENDING))
				bResult = TRUE;
		}
	}

	if (bResult)
		++pContext->nOutstandingSend;

	LeaveCriticalSection(&pContext->Lock);

	return bResult;
}

BOOL CIOCPSer::PostRecv(PCIOCPContext pContext, PCIOCPBuffer pBuffer)
{
	EnterCriticalSection(&pContext->Lock);

	BOOL bResult = FALSE;
	if (pBuffer && pContext)
	{
		if(pContext->nMaxAsynRecvCount > pContext->nOutstandingRecv)
		{
			pBuffer->nOperation = OP_READ;
			pBuffer->nSequenceNum = pContext->nReadSequence;
			DWORD dwBytes;
			DWORD dwFlags = 0;
			WSABUF buf;
			buf.buf = pBuffer->buff;
			buf.len = pBuffer->buffLen;

			int nCount = WSARecv(pContext->s, &buf, 1, &dwBytes, &dwFlags, &pBuffer->ol, NULL);
			if (SOCKET_ERROR != nCount || (SOCKET_ERROR == nCount && WSAGetLastError() == WSA_IO_PENDING))
				bResult = TRUE;
		}
	}

	if (bResult)
	{
		++(pContext->nOutstandingRecv);
		++(pContext->nReadSequence);
	}

	LeaveCriticalSection(&pContext->Lock);

	return bResult;
}

unsigned int _stdcall CIOCPSer::ListenWorkThread(LPVOID param)
{

	unsigned int returnCode = 0;
	do 
	{
		CIOCPSer *pThis = reinterpret_cast<CIOCPSer *>(param);
		if(NULL == pThis)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "run listen thread funcation is error 1, exit listen thread funcation!!!\n");
			break;
		}
		
		returnCode = pThis->ListenWorkThread();
	} while (false);

	if(logger::CLogger::CanPrint())
		logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "listen thread funcation is exit!!!\n");

	//这里好像欠缺处理代码（需思考）？？？？？？？？？？ 
	//应该停止全部的服务或通知上传（回调上层通知函数）

	return returnCode;
}

/*任务：
(1)预投递m_unInitAsynAcceptCnt个异步accept请求
(2)开辟工作线程
(3)超时时检查是否有恶意连接
(4)投递关闭工作线程的"消息"
*/
unsigned int _stdcall CIOCPSer::ListenWorkThread()
{

	PCIOCPBuffer pBuffer = NULL;
	for(unsigned int i = 0; i < m_unInitAsynAcceptCnt; ++i)
	{
		pBuffer = AllocIOBuffer();
		if(NULL == pBuffer)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "Allocate IOBuffer is fail. exit listen thread funcation!!!\n");
#ifdef _DEBUG
			printf("Allocate IOBuffer is fail. exit listen thread funcation\n");
			OutputDebugStringA("Allocate IOBuffer is fail. exit listen thread funcation!!\n");
#endif
			return 0;
		}

		if(PostAccept(pBuffer))
		{
			AddPendingAccept(pBuffer);
		}
		else
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "Post accept operator  is fail.!!!\n");
		}
	}

	HANDLE hWaitEvvents[3 + MAXWORKTHREADNUM];
	int nEventNum = 0;
	hWaitEvvents[nEventNum ++] = m_hAcceptEvent;
	hWaitEvvents[nEventNum ++] = m_hRepostEvent;
	hWaitEvvents[nEventNum ++] = m_hShutDownEvent;

	unsigned failNum = 0;
	for(unsigned int i = 0; i < m_unInitWorkThreadCnt; ++i)
	{
		hWaitEvvents[nEventNum] = (HANDLE)_beginthreadex(NULL, 0, WorkThread, (void *)this, 0, NULL);
		if (NULL == hWaitEvvents[nEventNum])
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create one work thread funcation is error!!!\n");
			++failNum;
			continue;
		}

		++nEventNum;
	}

	//只要有一个线程开辟成功，代码就可以继续运行
	if(3 == nEventNum)   //全部创建失败
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create ALL work thread funcation is error!!!\n");

		m_errorCode = CREATEALLWORKTHREAD_ERROR;
		return 0;
	}
	else if(failNum)
	{
		m_unInitWorkThreadCnt -= failNum;
	}

	while(TRUE)
	{
		DWORD dwWaitResult = WaitForMultipleObjects(nEventNum, hWaitEvvents, FALSE, 60 * 1000);

		if(WAIT_FAILED == dwWaitResult || m_bShutDown)  //收到关闭服务通知或发生了异常
		{
			RemoveAllConnected();
			Sleep(1000);

			closesocket(m_sListen);
			m_sListen = INVALID_SOCKET;

			for (unsigned int i = 0; i < m_unInitWorkThreadCnt; ++i)
			{
				PostQueuedCompletionStatus(m_hCompletion, -1, 0, NULL);
			}

			WaitForMultipleObjects(m_unInitWorkThreadCnt, &hWaitEvvents[3], TRUE, 5 * 1000);
			for (unsigned int i = 3; i < m_unInitWorkThreadCnt + 3; ++i)
			{
				CloseHandle(hWaitEvvents[i]);
			}

			CloseHandle(m_hCompletion);
			FreeAllIOBuffer();
			FreeAllContextPer();
			break;
		}
		else if(WAIT_TIMEOUT == dwWaitResult)    //发生超时，检查是否有恶意连接
		{
			CheckAndGoAwayViciousLink();
		}
		else
		{
			dwWaitResult = dwWaitResult - WAIT_OBJECT_0;
			int nPostAsynAcceptNum = 0;

			if (0 == dwWaitResult)   //m_hAcceptEvent受信
			{
				WSANETWORKEVENTS networkObj;
				WSAEnumNetworkEvents(m_sListen, hWaitEvvents[dwWaitResult], &networkObj);
				if(networkObj.lNetworkEvents & FD_ACCEPT)
				{
					if(0 != networkObj.iErrorCode[FD_ACCEPT_BIT])
					{
						m_bShutDown = TRUE;
						SetEvent(m_hShutDownEvent);
						m_errorCode = networkObj.iErrorCode[FD_ACCEPT_BIT];
						continue;
					}
					else
						nPostAsynAcceptNum = 50;
				}
			}
			else if(1 == dwWaitResult)    //m_hRepostEvent受信
			{
				nPostAsynAcceptNum = 1;
			}
			else                          //某一个工作线程退出了
			{
				m_bShutDown = TRUE;
				SetEvent(m_hShutDownEvent);
				continue;
			}

			//再次投递nPostAsynAcceptNum个accpet
			while (nPostAsynAcceptNum --)
			{
				PCIOCPBuffer pBuffer = AllocIOBuffer();
				if (pBuffer)
				{
					if (PostAccept(pBuffer))
					{
						AddPendingAccept(pBuffer);
					}
					else
					{
						if(logger::CLogger::CanPrint())
							logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "Post accept operator  is fail.!!!\n");
					}	
				}
				else
				{
					if(logger::CLogger::CanPrint())
						logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "run listen thread funcation is error 0 , exit listen thread funcation!!!\n");
					
					m_bShutDown = TRUE;
					SetEvent(m_hShutDownEvent);
					break;
				}
			}
		}
	}
	return 0;	
}

BOOL CIOCPSer::CheckAndGoAwayViciousLink()
{
	BOOL bResult = FALSE;
	do 
	{
		PCIOCPBuffer pIOBuffer = m_pPostAcceptBufList;
		while(pIOBuffer)
		{
			int nSeconds;
			int nLen = sizeof(nSeconds);
			int nResult = getsockopt(pIOBuffer->sClient, SOL_SOCKET, SO_CONNECT_TIME, (char *)&nSeconds, &nLen);

			if(!nResult)
			{
				if (nSeconds != -1 && nSeconds > 120)
				{
					closesocket(pIOBuffer->sClient);
					pIOBuffer->sClient = INVALID_SOCKET;
					RemovePendingAccept(pIOBuffer);
					FreeIOBuffer(pIOBuffer);
				}

				bResult = TRUE;
			}

			pIOBuffer = pIOBuffer->pBuffer;
		}

	} while (false);

	if (!bResult && m_pPostAcceptBufList)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "check vicious link is error!!!\n");
	}

	return bResult;
}

unsigned int _stdcall CIOCPSer::WorkThread(LPVOID param)
{
	do 
	{
		CIOCPSer *pThis = reinterpret_cast<CIOCPSer *>(param);
		if(NULL == pThis)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "run work thread funcation is error , exit work thread funcation!!!\n");
			break;
		}

		return pThis->WorkThread();
	} while (false);

	return 0;
}

unsigned int _stdcall CIOCPSer::WorkThread()
{
#ifdef _DEBUG
	printf("worker thread is starting...!!\n");
	OutputDebugStringA("worker thread is starting...!!\n");
#endif

	DWORD           dwTransBytes;
	PCIOCPBuffer    pBuffer;
	PCIOCPContext   pContext;
	while (true)
	{
		BOOL bOK = GetQueuedCompletionStatus(m_hCompletion, &dwTransBytes, (LPDWORD)&pContext, (LPOVERLAPPED *)&pBuffer, WSA_INFINITE);   //暂时，需测试

		if ((DWORD)-1 == dwTransBytes)    //监听线程或用户通知退出
		{
#ifdef _DEBUG
			printf("workerThread exit because user send exit command!\n");
			OutputDebugStringA("workerThread exit because user send exit command!\n");
			break;
#endif
		}

		//在此套接字上有错误发生
		if (!bOK)    
		{
			//SOCKET s;
			//if (OP_ACCEPT == pBuffer->nOperation)
			//{
			//	s = m_sListen;
			//}
			//else
			//{
			//	if (NULL == pContext)
			//	{
			//		break;
			//	}
			//	s = pContext->s;
			//}

			//DWORD dwFlags = 0;
			//if (WSAGetOverlappedResult(s, &pBuffer->ol, &dwTransBytes, FALSE, &dwFlags))
			//{
			//	m_errorCode = WSAGetLastError();
			//} 
		}
		else if (0 == dwTransBytes)   //连接被对方关闭
		{
		//	DealClientCloseFun(.....);
		}
		else
		{
			DealFunction(pContext, pBuffer, dwTransBytes);
		}
	}

#ifdef _DEBUG
	printf("work thread is exit!\n");
#endif
	return 0;
}

void CIOCPSer::DealFunction(PCIOCPContext pContext, PCIOCPBuffer pBuffer, DWORD dwTransBytes)
{
	// 1）首先减少套节字上的未决I/O计数
	if(pContext != NULL)
	{
		::EnterCriticalSection(&pContext->Lock);

		if(pBuffer->nOperation == OP_READ)
			pContext->nOutstandingRecv --;
		else if(pBuffer->nOperation == OP_WRITE)
			pContext->nOutstandingSend --;

		::LeaveCriticalSection(&pContext->Lock);

		// 2）检查套节字是否已经被我们关闭
		if(CloseSock == pContext->eSockState) 
		{
#ifdef _DEBUG
			::OutputDebugStringA("检查到套节字已经被我们关闭\n");
#endif // _DEBUG
			if(pContext->nOutstandingRecv == 0 && pContext->nOutstandingSend == 0)
			{		
				FreeContextPer(pContext);
			}
			// 释放已关闭套节字的未决I/O
			FreeIOBuffer(pBuffer);	
			return;
		}
	}
	else    //NULL == pContext
	{
		RemovePendingAccept(pBuffer);
	}



	//正式开始处理
	switch(pBuffer->nOperation)
	{
	case OP_ACCEPT:
		{
			PCIOCPContext newContext = AllocContextPer(pBuffer->sClient);
			if(NULL != newContext)
			{
				if(AddConnectedList(newContext))
				{
					//获得本地与远端地址信息
					int nLocalLen, nRmoteLen;
					LPSOCKADDR pLocalAddr, pRemoteAddr;
					m_lpfnGetAcceptExSockaddrs(pBuffer->buff, 
						pBuffer->buffLen - ((sizeof(sockaddr_in) + 16) * 2),
						sizeof(sockaddr_in) + 16,
						sizeof(sockaddr_in) + 16,
						(LPSOCKADDR *)&pLocalAddr,
						&nLocalLen,
						(LPSOCKADDR *)&pRemoteAddr,
						&nRmoteLen);

					memcpy(&newContext->addrLocal, pLocalAddr, nLocalLen);
					memcpy(&newContext->addrRemote, pRemoteAddr, nRmoteLen);

					// 关联新连接到完成端口对象
					::CreateIoCompletionPort((HANDLE)newContext->s, m_hCompletion, (DWORD)newContext, 0);


					// 通知用户
					///pBuffer->nLen = dwTrans;
					//OnConnectionEstablished(newContext, pBuffer);

					// 向新连接投递几个Read请求，这些空间在套节字关闭或出错时释放
					for(int i=0; i<5; i++)
					{
						CIOCPBuffer *p = AllocIOBuffer(MAXIOBUFFERSIZE);
						if(p != NULL)
						{
							if(!PostRecv(newContext, p))
							{
								RemoveAndCloseConnected(newContext);
								break;
							}
						}
					}
				}
				else  
				{
					if(logger::CLogger::CanPrint())
						logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "connect is to many, close connect!!!\n");
					FreeIOBuffer(pBuffer);
					RemoveAndCloseConnected(pContext);
				}
			}
			else
			{
				if(logger::CLogger::CanPrint())
					logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "io resource is to many, close connect!!!\n");
				RemoveAndCloseConnected(pContext);

			}


			// Accept请求完成，释放I/O缓冲区
			FreeIOBuffer(pBuffer);
			::SetEvent(m_hRepostEvent);
		}
		break;
	case OP_READ:
		{
			pBuffer->nLen = dwTransBytes;
			// 按照I/O投递的顺序读取接收到的数据
			CIOCPBuffer *p = GetNextReadBuffer(pContext, pBuffer);
			while(p != NULL)
			{
				// 通知用户
			    if (m_pRecvSink)
			    {
					m_pRecvSink->OnRecv(pBuffer->buff, pBuffer->nLen, inet_ntoa((pContext->addrRemote).sin_addr), ntohs((pContext->addrRemote).sin_port));
			    }
			    
				// 增加要读的序列号的值
				::InterlockedIncrement((LONG*)&pContext->nCurrentReadSequence);
				// 释放这个已完成的I/O
				FreeIOBuffer(p);
				p = GetNextReadBuffer(pContext, NULL);
			}

			// 继续投递一个新的接收请求
			pBuffer = AllocIOBuffer(MAXIOBUFFERSIZE);
			if(pBuffer == NULL || !PostRecv(pContext, pBuffer))
			{
				RemoveAndCloseConnected(pContext);
			}

		}
		break;
	case OP_WRITE:
		{

		}
		break;
	}
}

PCIOCPBuffer CIOCPSer::GetNextReadBuffer(PCIOCPContext pContext, PCIOCPBuffer pBuffer)
{
	if(pBuffer)
	{
		if(pBuffer->nSequenceNum == pContext->nCurrentReadSequence)
		{
			return pBuffer;
		}
		
		pBuffer->pBuffer = NULL;
		CIOCPBuffer *ptr = pContext->pOutOfOrderReads;
		CIOCPBuffer *pPre = NULL;
		while(NULL != ptr)
		{
			if(pBuffer->nSequenceNum < ptr->nSequenceNum)
				break;
			pPre = ptr;
			ptr = ptr->pBuffer;
		}

		if(NULL == pPre)
		{
			pBuffer->pBuffer = pContext->pOutOfOrderReads;
			pContext->pOutOfOrderReads = pBuffer->pBuffer;
		}
		else
		{
			pBuffer->pBuffer = pPre->pBuffer;
			pPre->pBuffer = pBuffer;
		}
	}

	// 检查表头元素的序列号，如果与要读的序列号一致，就将它从表中移除，返回给用户
	CIOCPBuffer *ptr = pContext->pOutOfOrderReads;
	if(ptr != NULL && (ptr->nSequenceNum == pContext->nCurrentReadSequence))
	{
		pContext->pOutOfOrderReads = ptr->pBuffer;
		return ptr;
	}
	return NULL;
}

