#include "StunServer.h"
#include "datastream.h"

CStunServer::CStunServer()
: m_hRecvThread(NULL)
, m_socketTest1(NULL)
, m_socketTest2(NULL)
, m_socketTest3(NULL)
, m_socketTest4(NULL)
, m_Isrunning(false)
{
	FD_ZERO(&m_fdReadSet);
}

CStunServer::~CStunServer()
{
	Stop();
}

void CStunServer::Stop()
{
	if (m_Isrunning)
	{
		if(INVALID_SOCKET != m_socketTest1 && NULL == m_socketTest1)
		{
			closesocket(m_socketTest1);
		}

		if(INVALID_SOCKET != m_socketTest2 && NULL == m_socketTest2)
		{
			closesocket(m_socketTest2);
		}

		if(INVALID_SOCKET != m_socketTest3 && NULL == m_socketTest3)
		{
			closesocket(m_socketTest3);
		}

		if(INVALID_SOCKET != m_socketTest4 && NULL == m_socketTest4)
		{
			closesocket(m_socketTest4);
		}

		WaitForSingleObject(m_hRecvThread, INFINITE);

		m_Isrunning = false;
	}
}

void CStunServer::CleanEvn()
{
	if(INVALID_SOCKET != m_socketTest1 && NULL == m_socketTest1)
	{
		closesocket(m_socketTest1);
	}

	if(INVALID_SOCKET != m_socketTest2 && NULL == m_socketTest2)
	{
		closesocket(m_socketTest2);
	}

	if(INVALID_SOCKET != m_socketTest3 && NULL == m_socketTest3)
	{
		closesocket(m_socketTest3);
	}

	if(INVALID_SOCKET != m_socketTest4 && NULL == m_socketTest4)
	{
		closesocket(m_socketTest4);
	}
}

bool CStunServer::Start(const char* ip1, const char* ip2, unsigned short port1/* = 3478*/, 
						unsigned short port2/* = 3479*/, unsigned short port3/* = 3480*/, unsigned short port4/* = 3480*/)
{
	bool bResult = false;
	do 
	{
		m_socketTest1 = socket(AF_INET, SOCK_DGRAM, 0);
		if(INVALID_SOCKET == m_socketTest1 || NULL == m_socketTest1)
			break;

		sockaddr_in addr1 = {0};
		addr1.sin_family  = AF_INET;
		addr1.sin_port    = htons(port1);
		addr1.sin_addr.S_un.S_addr    = inet_addr(ip1);
		if(SOCKET_ERROR == bind(m_socketTest1, (sockaddr *)&addr1, sizeof(sockaddr)))
			break;

		m_socketTest2 = socket(AF_INET, SOCK_DGRAM, 0);
		if(INVALID_SOCKET == m_socketTest2 || NULL == m_socketTest2)
			break;

		sockaddr_in addr2 = {0};
		addr2.sin_family  = AF_INET;
		addr2.sin_port    = htons(port2);
		addr2.sin_addr.S_un.S_addr    = inet_addr(ip1);
		if(SOCKET_ERROR == bind(m_socketTest2, (sockaddr *)&addr2, sizeof(sockaddr)))
			break;

		m_socketTest3 = socket(AF_INET, SOCK_DGRAM, 0);
		if(INVALID_SOCKET == m_socketTest3 || NULL == m_socketTest3)
			break;

		sockaddr_in addr3 = {0};
		addr3.sin_family  = AF_INET;
		addr3.sin_port    = htons(port3);
		addr3.sin_addr.S_un.S_addr    = inet_addr(ip1);
		if(SOCKET_ERROR == bind(m_socketTest1, (sockaddr *)&addr1, sizeof(sockaddr)))
			break;

		m_socketTest4 = socket(AF_INET, SOCK_DGRAM, 0);
		if(INVALID_SOCKET == m_socketTest4 || NULL == m_socketTest4)
			break;

		sockaddr_in addr4 = {0};
		addr4.sin_family  = AF_INET;
		addr4.sin_port    = htons(port4);
		addr4.sin_addr.S_un.S_addr    = inet_addr(ip2);
		if(SOCKET_ERROR == bind(m_socketTest1, (sockaddr *)&addr1, sizeof(sockaddr)))
			break;

		unsigned int dwThreadID = 0;
		m_hRecvThread = (HANDLE)_beginthreadex(NULL, 0, OnRecvThread, (void *)this, 0, &dwThreadID);
		if(NULL == m_hRecvThread || INVALID_HANDLE_VALUE == m_hRecvThread)
			break;

		FD_SET(m_socketTest1, &m_fdReadSet);
		FD_SET(m_socketTest2, &m_fdReadSet);

		bResult = true;
	} while (false);

	if(false == bResult)
	{
		CleanEvn();
	}

	return bResult;
}

unsigned int __stdcall CStunServer::OnRecvThread(void *parm)
{
	do 
	{
		CStunServer* pThis = reinterpret_cast<CStunServer *>(parm);
		if(NULL == pThis)
			break;

		return pThis->OnRecvThread();
	} while (false);

	return 0;
}

unsigned int __stdcall CStunServer::OnRecvThread()
{
	fd_set remainReadSet;
	
	
	while(true)
	{
		FD_ZERO(&remainReadSet);
		remainReadSet = m_fdReadSet;
		int result = select(0, &remainReadSet, NULL, NULL, NULL);
		if(result <= 0)
			break;
		else
		{
			for (unsigned int i = 0; i < m_fdReadSet.fd_count; ++i)
			{
				if(FD_ISSET(m_fdReadSet.fd_array[i], &remainReadSet))
				{
					char dataBuf[128] = {0};
					sockaddr_in remoteAddr;
					memset(&remoteAddr, 0x0, sizeof(sockaddr_in));
					int len = sizeof(sockaddr_in);
					int num = recvfrom(m_fdReadSet.fd_array[i], dataBuf, 128, 0, (sockaddr *)&remoteAddr, &len);
					if(num <= 0)
						break;
					
					SStunMessage sm;
					CDataStream recvSream(dataBuf, 128);
					recvSream >> sm;
					if(!recvSream.good_bit())
						break;

					OnRecvMsg(m_fdReadSet.fd_array[i], sm, remoteAddr);
				}
			}
		}
	}

	return 0;
}

void CStunServer::OnRecvMsg(SOCKET sk, SStunMessage sm, sockaddr_in remoteAddr)
{
	SOCKET sendSocket = NULL;
	SStunMessage response;
	response.ip_ = inet_ntoa(remoteAddr.sin_addr);
	response.port_ = ntohs(remoteAddr.sin_port);
	switch(sm.messageID_)
	{
	case STUN_BINDING_REQUEST:
		{
			response.messageID_ = STUN_BINDING_RESPONSE;
			sendSocket = sk;
		}
		break;
	case STUN_CHANGE_IP_PORT:
		{
			response.messageID_ = STUN_CHANGE_IP_PORT_RESPONSE;
			sendSocket = m_socketTest4;
		}
		break;
	case STUN_CHANGE_PORT:
		{
			response.messageID_ = STUN_CHANGE_PORT_RESPONSE;
			sendSocket = m_socketTest3;
		}
		break;
	default:
		//error log
		break;
	}
	
	if(sendSocket)
	{
		char ResBuffer[128] = {0}; 
		CDataStream sendStream(ResBuffer, 128);
		sendStream << response;

		int sendNum = sendto(sendSocket, sendStream.getbuffer(), sendStream.size(), 0, (sockaddr *)&remoteAddr, sizeof(sockaddr));
		if (sendNum <= 0)
		{
			//error_log
		}
	}
}