#define STUN_DLL _declspec(dllexport)
#include "JudgeNatType.h"

CInitSock NetworkEnvInit;

CStunJudgeNatType CStunJudgeNatType::m_Instance;

CStunJudgeNatType::CStunJudgeNatType()
: m_pStunNetwork(NULL)
, m_IsRunning(true)
, m_LocalPort(0)
{

}

CStunJudgeNatType::~CStunJudgeNatType()
{
	Stop();
}

void CStunJudgeNatType::Stop()
{
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

bool CStunJudgeNatType::AddStunServer(std::string localSvrIP, unsigned short localSvrPort, std::string stunServerIP, unsigned short stunServerPort1/* = 3478*/, unsigned short stunServerPort2/* = 3479*/)
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

			m_LocalPort = localSvrPort;
			m_LocalIP   = localSvrIP;

			std::map< std::string, boost::shared_ptr<CStunClient> >::iterator it = m_StunCliPtr.find(stunServerIP);
			if(it == m_StunCliPtr.end())
			{
				boost::shared_ptr<CStunClient> stunClient(new CStunClient);
				m_StunCliPtr[stunServerIP]	= stunClient;
				stunClient->AdviseSink(this);
				bResult = stunClient->Start(inet_addr(stunServerIP.c_str()), m_LocalIP, m_LocalPort, stunServerIP, stunServerPort1, stunServerPort2);
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
		if(m_IsRunning && m_pStunNetwork)
		{
			lResult = m_pStunNetwork->StunSend(buffer, length, remoteIP, remotePort);
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

long CStunJudgeNatType::OnReceivePacket(std::string stunSvrIP, unsigned short stunSvrPort, const char *buffer, unsigned long length)
{
	//原函数在这里加了锁
	in_addr task_id;
	memcpy(&task_id, buffer, sizeof(in_addr));
	std::string findUrl = inet_ntoa(task_id);

	std::map<std::string, boost::shared_ptr<CStunClient> >::iterator it = m_StunCliPtr.find(findUrl);
	if (it != m_StunCliPtr.end())
	{
		it->second->ProcessResponse(stunSvrIP, stunSvrPort, buffer, length);
	}

	return 0;
}

bool CStunJudgeNatType::ResetJudgeNatType(bool upnpFound,bool force_readd)
{
	//STUNClient::upnpfound_ = upnpFound;   (Finally open)

	std::vector<std::string> stunServers;
	{
	//	//psl::AutoLock lock(lock_);
	//	//stunNatTypes_.clear();
		for(std::map<std::string, boost::shared_ptr<CStunClient> >::iterator it = m_StunCliPtr.begin(); it != m_StunCliPtr.end(); ++it)
		{
			stunServers.push_back(it->first);
			it->second->UnAdviseSink();
			it->second->Stop();
		}
		m_StunCliPtr.clear();
	////	stunNetType_	= NAT_TYPE_UNKNOWN;
	}

	int iStunSvrCnt = 0;
	if( (force_readd /*|| STUNClient::upnpfound_*/) && m_LocalPort && !m_LocalIP.empty() && !stunServers.empty())
	{
		for(std::vector<std::string>::iterator it=stunServers.begin(); it!=stunServers.end(); ++it)
		{
			AddStunServer(m_LocalIP, m_LocalPort, *it);
			++iStunSvrCnt;
		}
	}
	if( iStunSvrCnt >= 4 )
	{
		return true;
	}
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




