#include "Complex_IOCPSer.h"
#include "log/InterfaceLogger.h"
#include "SocketWrap.h"

CIOCPSer::CIOCPSer()
: m_bServerStarted(false)
, m_wPort(LISTENPORT)
, m_nMaxConnections(MAXCONNECTEDCOUNT)
, m_nMaxFreeBuffers(200)
, m_nMaxFreeContexts(200)
, m_nInitialReads(4)
, m_sListen(INVALID_SOCKET)
, m_lpfnAcceptEx(NULL)
, m_lpfnGetAcceptExSockaddrs(NULL)
, m_hListenThread(NULL)
, m_pFreeBufferList(NULL)
, m_pFreeContextList(NULL)
{
	m_hAcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);  //no manually reset and nonsignal
	DWORD error_code = GetLastError();
	if(logger::CLogger::CanPrint())
		logger::CLogger::PrintA(COMPLLEXIOCPSERLOG, "create AcceptEvent is error !!!\n");


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
	if (m_pFreeBufferList)
	{
		IOPer =(PCIOCPBuffer)malloc(sizeof(PCIOCPBuffer) + bufLen);
		memset(IOPer, 0x0, sizeof(PCIOCPBuffer) + bufLen);
	}
	else
	{
		IOPer = m_pFreeContextList;
		m_pFreeContextList = m_pFreeContextList->pNext;
		IOPer->pBuffer     = NULL;
	}

	if (NULL != IOPer)
	{
		IOPer->buffLen = bufLen;
		IOPer->buff    = (char *)(IOPer + 1);
	}

	return IOPer;
}

void CIOCPSer::FreeIOBuffer(PCIOCPBuffer pIOBuf)
{
	if (PCIOCPBuffer)
	{
		if (m_nMaxFreeBuffers < MAXFREEBUFFER)
		{
		}
		else
		{
			free(pIOBuf);
		}
	}
}

void CIOCPSer::FreeAllIOBuffer()
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

	
}