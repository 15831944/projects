#include "Complex_IOCPSer.h"
#include "log/InterfaceLogger.h"
#include "SocketWrap.h"

CIOCPSer::CIOCPSer()
	: m_bServerStarted(false)
	, m_wPort(LISTENPORT)
	, m_nMaxConnections(MAXCONNECTEDCOUNT)
	, m_nMaxFreeBuffers(MAXFREEBUFFER)
	, m_nMaxFreeContexts(MAXFREECONTEXT)
	, m_nInitialReads(4)
	, m_sListen(INVALID_SOCKET)
	, m_lpfnAcceptEx(NULL)
	, m_lpfnGetAcceptExSockaddrs(NULL)
	, m_hListenThread(NULL)
	, m_pFreeBufferList(NULL)
	, m_pFreeContextList(NULL)
	, m_pConnectedList(NULL)
	, m_pPostAcceptBufList(NULL)
	, m_unFreeBufferCount(0)
	, m_unFreeContextCount(0)
	, m_unCurrentConnectedCount(0)
	, m_unCuttentPostAcceptBufCount(0)
	, m_unInitAsynAcceptCnt(5)
{
	m_hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);  //no manually reset and nonsignal
	DWORD error_code = GetLastError();
	if(logger::CLogger::CanPrint())
		logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create AcceptEvent is error !!!\n");

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

bool CIOCPSer::StartSer(WORD wPort /* = 4567 */, unsigned int nMaxConnections /* = 2000 */, unsigned int nMaxFreeBuffers /* = 200 */, 
	unsigned int nMaxFreeContexts /* = 100 */, unsigned int nInitialReads /* = 4 */)
{
	if (m_bServerStarted)
		return true;
	m_wPort = wPort;
	m_nMaxConnections = nMaxConnections;
	m_nMaxFreeBuffers = nMaxFreeBuffers;
	m_nMaxFreeContexts = nMaxFreeContexts;
	m_nInitialReads = nInitialReads;

	m_bShutDown = false;
	m_bServerStarted = true;

	m_sListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if( INVALID_SOCKET == m_sListen)
	{
		DWORD error_code = GetLastError();
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create socket is error !!! error_code = %d\n", error_code);
		return false;
	}

	sockaddr_in listenAddr = {0};
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port   = htons(m_wPort);
	listenAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	Bind(m_sListen, (sockaddr *)&listenAddr, sizeof(listenAddr));

	Listen(m_sListen, 200);

	m_hCompletion = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (NULL == m_hCompletion)
	{
		DWORD error_code = GetLastError();
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create IOcompletion port is error !!! error_code = %d\n", error_code);
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
		return false;
	}

	CreateIoCompletionPort((HANDLE)m_sListen, m_hCompletion, (DWORD)0, 0);

	//如果为null着不注册FD_ACCEPT事件
	if (NULL != m_hAcceptEvent)
		WSAEventSelect(m_sListen, m_hAcceptEvent, FD_ACCEPT);

	unsigned int  unThreadID;
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenWorkThread, this, 0, &unThreadID);
	if (NULL == m_hListenThread )
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create listen thread funcation is error , exit listen thread funcation!!!\n");
		return false;
	}

	return true;
}


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
		IOPer->buffLen = bufLen;
		IOPer->buff    = (char *)(IOPer + 1);
	}

	return IOPer;
}

void CIOCPSer::FreeIOBuffer(PCIOCPBuffer pIOBuf)
{
	if (pIOBuf)
	{
		EnterCriticalSection(&m_csFreeBufferListLock);
		if (m_unFreeBufferCount < MAXFREEBUFFER)  //加入到空闲IOBuffer链表当中去
		{
			memset(pIOBuf, 0x0, sizeof(CIOCPBuffer) + pIOBuf->buffLen);
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
		free(pTmpBuffer);
	}
	LeaveCriticalSection(&m_csFreeBufferListLock);

	m_unFreeBufferCount = 0;
}


PCIOCPContext CIOCPSer::AllocContextPer(SOCKET s)
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
	}
	else
	{
		pConntext = m_pFreeContextList;
		m_pFreeContextList = m_pFreeContextList->pNext;
		pConntext = NULL;
		--m_unFreeContextCount;
	}
	LeaveCriticalSection(&m_csFreeContextListLock);

	if(pConntext) 
	{ 
		pConntext->s = s;
	}

	return pConntext;
}


void CIOCPSer::FreeContextPer(PCIOCPContext pContextPer)
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
		if (m_unFreeContextCount < MAXFREECONTEXT)
		{
			memset(pContextPer, 0x0, sizeof(CIOCPContext));
			pContextPer->pNext = m_pFreeContextList;
			m_pFreeContextList = pContextPer;

			++m_unFreeContextCount;
		}
		else  //真正的释放空间
		{
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
		free(pTmpContext);
	}
	LeaveCriticalSection(&m_csFreeContextListLock);
	m_unFreeContextCount = 0;
}

BOOL CIOCPSer::AddConnected(PCIOCPContext pContext)
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
			pContext->eSockState = Open;
			result = TRUE;
		}
		LeaveCriticalSection(&m_csConnectedListLock);
	}

	return result;
}


void CIOCPSer::CloseConnected(PCIOCPContext pContext)
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
			while(pTmpContext)
			{
				if (pTmpContext->pNext == pContext)
				{
					pTmpContext->pNext = pTmpContext->pNext->pNext;
					--m_unCurrentConnectedCount;
				}
			}
		}
		LeaveCriticalSection(&m_csConnectedListLock);

		if (INVALID_SOCKET != pContext->s)
		{
			closesocket(pContext->s);
			pContext->s = INVALID_SOCKET;
		}
		pContext->eSockState = Close;
		pContext->pNext      = NULL;

		FreeContextPer(pContext);   //句柄唯一数据内存回收？？？？？？？？？？？？？？？？？、、、????
	}
}


//关闭了链接但是句柄唯一数据未回收，可能会引起内存泄漏的问题？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
void CIOCPSer::CloseAllConnected()
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
		pTmpContext->eSockState = Close;
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

	if (pIOBuffer)
	{
		EnterCriticalSection(&m_csPostAcceptBufListLock);
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
		LeaveCriticalSection(&m_csPostAcceptBufListLock);
	}

	if (bResult)
		++m_unCuttentPostAcceptBufCount;

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
		--m_unCuttentPostAcceptBufCount;

	return bResult;
}



BOOL CIOCPSer::PostAccept(PCIOCPBuffer pBuffer)
{
	BOOL bResult = FALSE;
	if (pBuffer)
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

	return bResult;
}

BOOL CIOCPSer::PostRecv(PCIOCPContext pContext, PCIOCPBuffer pBuffer)
{
	BOOL bResult = FALSE;
	if (pBuffer && pContext)
	{
		if(pContext->nMaxAsynRecvCount > pContext->nOutstandingRecv)
		{
			pBuffer->nOperation = OP_READ;
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
		++pContext->nOutstandingRecv;

	return bResult;
}

unsigned int _stdcall CIOCPSer::ListenWorkThread(LPVOID param)
{
	do 
	{
		CIOCPSer *pThis = reinterpret_cast<CIOCPSer *>(param);
		if(NULL == pThis)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "run listen thread funcation is error 1, exit listen thread funcation!!!\n");
			break;
		}

		return pThis->ListenWorkThread();
	} while (false);

	return 0;
}

/*任务：(1)预投递m_unInitAsynAcceptCnt个异步accept请求
(2)开辟工作线程
(3)
*/
unsigned int _stdcall CIOCPSer::ListenWorkThread()
{

	PCIOCPBuffer pBuffer;
	for(unsigned int i = 0; i < m_unInitAsynAcceptCnt; ++i)
	{
		pBuffer = AllocIOBuffer();
		if(NULL == pBuffer)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "run listen thread funcation is error 0 , exit listen thread funcation!!!\n");
			return -1;
		}
		AddPendingAccept(pBuffer);
		PostAccept(pBuffer);
	}

	HANDLE hWaitEvvents[2 + MAXWORKTHREADNUM];
	int nEventNum = 0;
	hWaitEvvents[nEventNum ++] = m_hAcceptEvent;

	for(unsigned int i = 0; i < m_unInitWorkThreadCnt; ++i)
	{
		hWaitEvvents[nEventNum ++] = (HANDLE)_beginthreadex(NULL, 0, WorkThread, (void *)this, 0, NULL);
	}

	while(TRUE)
	{
		DWORD dwWaitResult = WaitForMultipleObjects(nEventNum, hWaitEvvents, FALSE, 60 * 1000);

		if(WAIT_FAILED == dwWaitResult || m_bShutDown)  //收到关闭服务通知或发生了异常
		{
			CloseAllConnected();
			Sleep(1000);

			closesocket(m_sListen);
			m_sListen = INVALID_SOCKET;

			for (unsigned int i = 2; i < m_unInitWorkThreadCnt; ++i)
			{
				PostQueuedCompletionStatus(m_hCompletion, -1, 0, NULL);
			}

			WaitForMultipleObjects(m_unInitWorkThreadCnt, &hWaitEvvents[2], TRUE, 5 * 1000);
			for (unsigned int i = 2; i < m_unInitWorkThreadCnt; ++i)
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


		}
	}

	return 0;	
}

unsigned int _stdcall CIOCPSer::WorkThread(LPVOID param)
{
	do 
	{
		CIOCPSer *pThis = reinterpret_cast<CIOCPSer *>(param);
		if(NULL == pThis)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "run work thread funcation is error , exit listen thread funcation!!!\n");
			break;
		}

		return pThis->WorkThread();
	} while (false);

	return 0;
}

unsigned int _stdcall CIOCPSer::WorkThread()
{
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

	if (!bResult)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "check vicious link is error!!!\n");
	}
	return bResult;
}