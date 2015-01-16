#define STUN_DLL _declspec(dllexport)
#include "JudgeNatType.h"

CInitSock NetworkEnvInit;

CStunJudgeNatType CStunJudgeNatType::m_Instance;

CStunJudgeNatType::CStunJudgeNatType()
: m_pStunNetwork(NULL)
, m_IsRunning(false)
, m_ProbeSocket(INVALID_SOCKET)
, m_LocalPort(0)
, m_hWorkThread(NULL)
{
	
}

CStunJudgeNatType::~CStunJudgeNatType()
{
	Stop();
}

void CStunJudgeNatType::Stop()
{
	if (m_ProbeSocket && INVALID_SOCKET != m_ProbeSocket)
	{
		closesocket(m_ProbeSocket);
		m_ProbeSocket = INVALID_SOCKET;
	}


	if (NULL != m_hWorkThread)
	{
		unsigned long exitCode = 0;
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hWorkThread, 100))
		{
			TerminateThread(m_hWorkThread, exitCode);
		}

		CloseHandle(m_hWorkThread);
		m_hWorkThread = NULL;
	}

	for (std::map<std::string, boost::shared_ptr<CStunClient> >::iterator it = m_StunCliPtr.begin(); it != m_StunCliPtr.end(); ++it)
	{
		if(it->second)
			it->second->Stop();
	}

	if (NULL != m_pStunNetwork)
	{
		m_pStunNetwork = NULL;
	}
	
	m_IsRunning    = false;
}


bool CStunJudgeNatType::AdviseSink(IStunNetwork * pStunNetwork)
{
	bool bResult = false;
	do 
	{
		if (NULL != pStunNetwork)
		{
			m_pStunNetwork = pStunNetwork;
			bResult = true;
		}
	} while (false);

	return bResult;
}

bool CStunJudgeNatType::UnAdviseSink()
{
	m_pStunNetwork = NULL;
	return true;
}

bool CStunJudgeNatType::Start()
{
	do 
	{
		if (m_IsRunning)
		{
			break;
		}

		char szHostName[128];
		if(0 == gethostname(szHostName, 128))
		{
			struct hostent * pHost;	
			pHost = gethostbyname(szHostName); 
			if (NULL == pHost)
			{
				//error log
				break;
			}
			m_LocalIP = inet_ntoa (*(struct in_addr *)pHost->h_addr_list[0]);//Select the first activities of the IP  as a test IP
		}
		else
		{
			;//error log
			break;
		}

		//create test1 socket
		m_ProbeSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(INVALID_SOCKET == m_ProbeSocket)
		{
			;//error log
			break;
		}

		m_LocalPort = STUN_DEFAULT_CLINET_PORT;

		SOCKADDR_IN sockAddr;
		memset(&sockAddr,0,sizeof(sockAddr));
		sockAddr.sin_family      = AF_INET;
		sockAddr.sin_addr.s_addr = inet_addr(m_LocalIP.c_str());
		sockAddr.sin_port        = htons(m_LocalPort);



		int nRet, tryCount = 0;
		while( SOCKET_ERROR == (nRet= bind(m_ProbeSocket, (SOCKADDR*)&sockAddr, sizeof(sockAddr))) && tryCount < 20)
			sockAddr.sin_port = htons((unsigned short)++m_LocalPort);

		if (tryCount >= 20)
		{
			;//error log
			break;
		}

		m_IsRunning = InitThread();
	} while (false);

	if (!m_IsRunning)
	{
		if (m_ProbeSocket && INVALID_SOCKET != m_ProbeSocket)
		{
			closesocket(m_ProbeSocket);
			m_ProbeSocket = INVALID_SOCKET;
		}

		if (NULL != m_hWorkThread)
		{
			CloseHandle(m_hWorkThread);
			m_hWorkThread = NULL;
		}
	}
	return m_IsRunning;
}

bool CStunJudgeNatType::InitThread()
{
	unsigned int  dwThreadID = 0;
	m_hWorkThread = (HANDLE)_beginthreadex(NULL, 0, WorkThread, (void*)this, 0, &dwThreadID);
	if (NULL == m_hWorkThread)
	{
		;//error log
		return false; 
	}

	return true;
}

unsigned int __stdcall CStunJudgeNatType::WorkThread(void *param)
{
	do 
	{
		CStunJudgeNatType* pThis = reinterpret_cast<CStunJudgeNatType*>(param);
		if(NULL == pThis)
		{
			break;
		}
		return pThis->WorkThread();
	} while(false);

	//thread exit log
	return 0;
}

unsigned int __stdcall CStunJudgeNatType::WorkThread()
{
	while (true)
	{
		sockaddr_in remoteAddr;
		int remoteAddrLen = sizeof(sockaddr_in);
		memset(&remoteAddr, 0x0, sizeof(sockaddr_in));

		char buf[128] = {0};
		int recvLen = recvfrom(m_ProbeSocket, buf, 128, 0, (sockaddr*)(&remoteAddr), &remoteAddrLen);
		if(recvLen <= 0)
		{
			unsigned long error_code = GetLastError();
			//error_log
			break;
		}
		else
		{
			std::map<std::string, boost::shared_ptr<CStunClient> >::iterator it = m_StunCliPtr.find(inet_ntoa(remoteAddr.sin_addr));
			if (it != m_StunCliPtr.end())
			{
				it->second->ProcessResponse(inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port), buf, recvLen);
			}
		}
	}

	m_IsRunning = false;
	return 0;
}

bool CStunJudgeNatType::AddStunServer(std::string stunServerIP, unsigned short stunServerPort1/* = 3478*/, unsigned short stunServerPort2/* = 3479*/)
{
	bool bResult = false;
	do 
	{
		if(m_IsRunning)
		{
			//psl::AutoLock lock(lock_);
			if(m_StunCliPtr.size() >= 4)
			{
				bResult = true;
				break;
			}

			std::map< std::string, boost::shared_ptr<CStunClient> >::iterator it = m_StunCliPtr.find(stunServerIP);
			if(it == m_StunCliPtr.end())
			{
				boost::shared_ptr<CStunClient> stunClient(new CStunClient);
				m_StunCliPtr[stunServerIP]	= stunClient;
				stunClient->AdviseSink(this);
				bResult = stunClient->Start(m_LocalIP, m_LocalPort, stunServerIP, stunServerPort1, stunServerPort2);
			}
			else
			{
				bResult = true;
				break;
			}
		}
	} while (false);

	return bResult;
}

long CStunJudgeNatType::OnSendStunPacket(const char *buffer, unsigned long length, std::string remoteIP, unsigned short remotePort)
{
	long lResult = -1;
	do 
	{
		if(m_IsRunning)
		{
			SOCKADDR_IN ServerAddr;
			memset(&ServerAddr, 0x0, sizeof(SOCKADDR_IN));
			ServerAddr.sin_family = AF_INET;
			ServerAddr.sin_addr.s_addr = inet_addr(remoteIP.c_str());
			ServerAddr.sin_port = htons((u_short)remotePort);

			if (INVALID_SOCKET == m_ProbeSocket)
			{
				//error log
				break;
			}
			else
			{
				lResult = sendto(m_ProbeSocket, buffer, length, 0, (SOCKADDR *)(&ServerAddr), sizeof(SOCKADDR));
				if (lResult < 0)
				{
					//error_code
					unsigned long error = GetLastError();
					break;
				}
			}
		}
	} while (false);

	return lResult;
}

long CStunJudgeNatType::OnStunFinished(NAT_TYPE natType, std::string stunServerIP, std::string publicIP, unsigned short publicPort)
{
	if (m_pStunNetwork)
	{
		m_pStunNetwork->OnStunNatFinished(natType, publicIP, publicPort);
	}
	
	return 0;
}

bool CStunJudgeNatType::ResetJudgeNatType(bool upnpFound,bool force_readd)
{
	//STUNClient::upnpfound_ = upnpFound;

	//vector<DWORD> stunServers;
	//{
	//	//psl::AutoLock lock(lock_);
	//	//stunNatTypes_.clear();
	//	for(std::map<DWORD, StunClientPtr>::iterator it=stunClients_.begin(); it!=stunClients_.end(); ++it)
	//	{
	//		stunServers.push_back(it->first);
	//		it->second->UnadviseSink();
	//		it->second->Stop();
	//	}
	//	stunClients_.clear();
	////	stunNetType_	= NAT_TYPE_UNKNOWN;
	//}

	//int iStunSvrCnt = 0;
	//if( (force_readd || STUNClient::upnpfound_) && m_wStunSvrPort && !stunServers.empty())
	//{
	//	//Sleep(1000);
	//	for(vector<DWORD>::iterator it=stunServers.begin(); it!=stunServers.end(); ++it)
	//	{
	//		AddStunServer(*it);
	//		++iStunSvrCnt;
	//	}
	//}
	//if( iStunSvrCnt >= 4 )
	//{
	//	return true;
	//}
	return false;
}

CStunJudgeNatType* CStunJudgeNatType::CreateObject()
{
	return &m_Instance;
}

void CStunJudgeNatType::Release(void)
{

}

//export interface
IStunSink* CreateJudgeNatTypeClass()
{
	return CStunJudgeNatType::CreateObject();
}
void FreeJudgeNatTypeClass()
{
	CStunJudgeNatType::CreateObject()->Stop();

}




