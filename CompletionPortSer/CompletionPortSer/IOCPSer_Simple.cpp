#include "IOCPSer_Simple.h"
#include <process.h>


CIOCPSerSimple::CIOCPSerSimple()
: m_hworkThread(NULL)
, m_hcompletion(NULL)
{

}

CIOCPSerSimple::~CIOCPSerSimple()
{
	if (NULL != m_hworkThread)
	{
		CloseHandle(m_hworkThread);
		m_hworkThread = NULL;
	}
}

void CIOCPSerSimple::StopSer()
{
	if(WaitForSingleObject(m_hworkThread, 1000))
		CloseHandle(m_hworkThread);
}


//complete init work
bool CIOCPSerSimple::StartSer()
{
	bool bResult = false;
	do 
	{
		m_hcompletion = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

		if (NULL == m_hcompletion)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(LOGFILENAME,"create completion port is error !!!\n");

			break;
		}

		unsigned int dwThreadID = 0;
		m_hworkThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, (void *)this, 0, &dwThreadID);

		SOCKET sListenSock = socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == sListenSock)
		{
			DWORD errorCode = WSAGetLastError();
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(LOGFILENAME,"create listen socket is error !!! errorCode = %d\n", errorCode);
			exit(0);
		}

		sockaddr_in si;
		si.sin_family           = AF_INET;
		si.sin_port             = htons(LISTENPORT);
		si.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

		if(SOCKET_ERROR == bind(sListenSock, (sockaddr *)&si, sizeof(si)))
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(LOGFILENAME,"bind listen socket is error !!!\n");
			exit(0);
		}

		listen(sListenSock, 5);
		printf("*********server is ok, start to listen ************\n");

		while (true)
		{
			sockaddr_in remoteAddr;
			int nRemoteLen = sizeof(remoteAddr);

			SOCKET sNew = accept(sListenSock, (sockaddr *)&remoteAddr, &nRemoteLen);
			if (INVALID_SOCKET == sNew)
			{
				if(logger::CLogger::CanPrint())
					logger::CLogger::PrintA(LOGFILENAME,"accept is error !!!\n");
				continue;
			}

			PPER_HANDLE_DATA pPerHandle = new PER_HANDLE_DATA;
			pPerHandle->sockConCli      = sNew;
			memcpy(&pPerHandle->addr, &remoteAddr, nRemoteLen);

			CreateIoCompletionPort((HANDLE)pPerHandle->sockConCli, m_hcompletion, (DWORD)pPerHandle, 0);

			//delivery recv request
			PPER_IO_DATA pPerIO = new PER_IO_DATA;
			pPerIO->OperationType = OP_READ;
			//pPerIO->ol.hEvent     = WSACreateEvent();                //(?????????????????????????????????????????????????????????????????????)
			
			WSABUF buf;
			buf.buf = pPerIO->buf;
			buf.len = BUFFER_SIZE;
			DWORD dwRecv;
			DWORD dwFlags = 0;

			WSARecv(pPerHandle->sockConCli, &buf, 1, &dwRecv, &dwFlags, &(pPerIO->ol), NULL);
		}

	} while (false);

	return bResult;
}

unsigned int _stdcall CIOCPSerSimple::WorkThread(LPVOID param)
{
	do 
	{
		CIOCPSerSimple *pThis = reinterpret_cast<CIOCPSerSimple *>(param);
		if(NULL == pThis)
		{
			if(logger::CLogger::CanPrint())
				logger::CLogger::PrintA(LOGFILENAME, "num1 work thread exception exit !!!");
			break;
		}

		return pThis->WorkThread();
	} while (false);

	return 0;
}

unsigned int _stdcall CIOCPSerSimple::WorkThread()
{
	if (NULL == m_hcompletion)
	{
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(LOGFILENAME, "num2 work thread exit !!! because m_hcompletion is null !!!");
		return 0;
	}

	DWORD            dwTrans;
	PPER_HANDLE_DATA pPerHandle;
	PPER_IO_DATA     pPerIO;

	while (true)
	{
		BOOL bOK = GetQueuedCompletionStatus(m_hcompletion, &dwTrans, (LPDWORD)&pPerHandle, (LPOVERLAPPED*)&pPerIO, WSA_INFINITE);

		if (!bOK)
		{
			closesocket(pPerHandle->sockConCli);
			delete(pPerHandle);
			delete(pPerIO);
			continue;
		}
		else if (0 == dwTrans && (OP_READ == pPerIO->OperationType || OP_WRITE == pPerIO->OperationType))
		{
			closesocket(pPerHandle->sockConCli);
			delete(pPerHandle);
			delete(pPerIO);
			continue;
		}
		else
		{
			switch(pPerIO->OperationType)
			{
			case OP_READ:
				{
					pPerIO->buf[dwTrans] = '\0';
					printf(pPerIO->buf);

                    //delivery recv request
					WSABUF buf;
					buf.buf = pPerIO->buf;
					buf.len = BUFFER_SIZE;
					DWORD dwRecv;
					DWORD dwFlags = 0;
					WSARecv(pPerHandle->sockConCli, &buf, 1, &dwRecv, &dwFlags, &(pPerIO->ol), NULL);

					break;
				}

			case OP_WRITE:
				{
					break;
				}

			case OP_ACCEPT:
				{
					break;
				}
			}
		}
	}
	
	return 0;
}