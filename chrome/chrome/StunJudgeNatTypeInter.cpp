#include "StunJudgeNatTypeInter.h"

CStunJudgeNatType::CStunJudgeNatType()
: m_pStunNetwork(NULL)
, m_LocalPort(0)
, m_bIsRunning(true)
{

}

CStunJudgeNatType::~CStunJudgeNatType()
{
	if(m_bIsRunning)
		Stop();
}

void CStunJudgeNatType::Stop()
{
	if(m_bIsRunning)
	{
		CScopeLock AutoLock(&m_lock);
		std::map< std::string, boost::shared_ptr<CStunClient> >::const_iterator it = m_mapStunCliPtr.begin();
		for(; it != m_mapStunCliPtr.end(); ++it)
		{
			(it->second)->Stop();
		}

		//这里可能需要加入判断，判断CStunClient的有限状态机线程已经退出，否则可能崩溃。
		m_mapStunCliPtr.clear();  //利用智能指针的自动释放内存功能
		m_bIsRunning = false;
	}
}

bool CStunJudgeNatType::AdviseSink(IStunNetwork * pStunNetwork)
{
	bool bResult = false;
	do 
	{
		if(NULL == pStunNetwork)
			break;

		m_pStunNetwork = pStunNetwork;
		bResult = true;
	} while (false);

	return bResult;
}

bool CStunJudgeNatType::UnAdviseSink()
{
	m_pStunNetwork = NULL;
	return true;
}

bool CStunJudgeNatType::AddStunServer(std::string localSvrIP, unsigned short localSvrPort, std::string stunServerIP, unsigned short stunServerPort1 /* = 3478 */, unsigned short stunServerPort2 /* = 3479 */)
{
	bool bResult = false;
	do 
	{
		if(m_bIsRunning)
		{
			CScopeLock AutoLock(&m_lock);
			if(m_mapStunCliPtr.size() >= 4)
			{
				bResult = true;
				break;
			}

			m_LocalPort = localSvrPort;
			m_LocalIP   = localSvrIP;

			std::map< std::string, boost::shared_ptr<CStunClient> >::iterator it = m_mapStunCliPtr.find(stunServerIP);
			if(it == m_mapStunCliPtr.end())
			{
				boost::shared_ptr<CStunClient> stunClient(new CStunClient);
				m_mapStunCliPtr[stunServerIP]	= stunClient;
				struct StunPortInfo portInfo;
				portInfo.unStunPort1 = stunServerPort1;
				portInfo.unStunPort2 = stunServerPort2;
				m_mapStunPortInfo[stunServerIP] = portInfo;
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

bool CStunJudgeNatType::ResetJudgeNatType(bool upnpFound, bool force_readd)
{
	bool bResult = false;
	do 
	{
		CStunClient::m_bIsOpenUpnp = upnpFound;
		m_mapNetType.clear();

		std::map< std::string, struct StunPortInfo > mapStunIPAndPort;
		{
			CScopeLock AutoLock(&m_lock);
			std::map< std::string, boost::shared_ptr<CStunClient> >::const_iterator it = m_mapStunCliPtr.begin();
			for (; it != m_mapStunCliPtr.end(); ++it)
			{
				(it->second)->Stop();
			}
			m_mapStunCliPtr.clear();
			mapStunIPAndPort = m_mapStunPortInfo;
			m_mapStunPortInfo.clear();
		}


		if(force_readd)
		{
			for (std::map< std::string, struct StunPortInfo >::const_iterator it = mapStunIPAndPort.begin(); 
				 it != mapStunIPAndPort.end(); ++it)
			{
				AddStunServer(m_LocalIP, m_LocalPort, it->first, (it->second).unStunPort1, (it->second).unStunPort2);
			}
		}
		else
			m_mapStunPortInfo.clear();
			
		bResult = true;
	} while (false);

	return bResult;
}

long CStunJudgeNatType::OnReceivePacket(std::string stunSvrIP, unsigned short stunSvrPort, char *buffer, unsigned long length)
{
	if (m_bIsRunning)
	{
		CScopeLock AutoLock(&m_lock);
		std::map< std::string, boost::shared_ptr<CStunClient> >::const_iterator it = m_mapStunCliPtr.find(stunSvrIP);
		if (it != m_mapStunCliPtr.end())
		{
			it->second->ProcessResponse(stunSvrIP, stunSvrPort, buffer, length);
		}
	}
	return 0;
}

long CStunJudgeNatType::OnSendStunPacket(const char * buffer, unsigned long length, std::string remoteIP, unsigned short remotePort)
{
	long lResult = -1;
	do 
	{
		if(m_bIsRunning && m_pStunNetwork)
		{
			//lResult为send的发送字节数或错误码
			lResult = m_pStunNetwork->StunSend(buffer, length, remoteIP, remotePort);
		}
	} while (false);

	return lResult;
}

void CStunJudgeNatType::OnStunFinished(NAT_TYPE natType, std::string publicIP, unsigned short publicPort)
{
	if(m_bIsRunning && m_pStunNetwork)
	{
		if(m_mapNetType.end() == m_mapNetType.find(natType))
		{
			m_mapNetType[natType] = StunReturnInfo();
			++m_mapNetType[natType].countNum;
			m_mapNetType[natType].strPublicIP = publicIP;
			m_mapNetType[natType].unPublicPort = publicPort;
		}
		else
			++m_mapNetType[natType].countNum;


		if(m_mapNetType.size() > 1)
		{
			enum NAT_TYPE maxNAT = UNKNOWN;
			unsigned maxCount = 0;
			std::map<enum NAT_TYPE, struct StunReturnInfo>::const_iterator it = m_mapNetType.begin();
			for (; it != m_mapNetType.end(); ++it)
			{
				if(it->first == UNKNOWN)
					continue;

				if(maxCount < (it->second).countNum)
				{
					maxCount = (it->second).countNum;
					maxNAT = it->first;
				}
			}

			m_pStunNetwork->OnStunNatFinished(maxNAT, m_mapNetType[maxNAT].strPublicIP, m_mapNetType[maxNAT].unPublicPort);
		}
		else
			m_pStunNetwork->OnStunNatFinished(natType, publicIP, publicPort);
	}
}



