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
, m_unFreeBufferCount(0)
, m_unFreeContextCount(0)
,  m_unCurrentConnectedCount(0)
{
	m_hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);  //no manually reset and nonsignal
	DWORD error_code = GetLastError();
	if(logger::CLogger::CanPrint())
		logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create AcceptEvent is error !!!\n");

	InitializeCriticalSection(&m_csFreeContextListLock);
	InitializeCriticalSection(&m_csFreeBufferListLock);
	InitializeCriticalSection(&m_csConnectedListLock);


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
		pContext->bClosing = TRUE;
	}

}
void CIOCPSer::CloseAllConnected()
{

}


unsigned int _stdcall CIOCPSer::ListenWorkThread(LPVOID param)
{
	do 
	{
		CIOCPSer *pThis = reinterpret_cast<CIOCPSer *>(param);
		if(NULL == pThis)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "run listen thread funcation is error , exit listen thread funcation!!!\n");
			break;
		}

		return pThis->ListenWorkThread();
	} while (false);

	return 0;
}

unsigned int _stdcall CIOCPSer::ListenWorkThread()
{
	return 0;	
}