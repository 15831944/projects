#pragma once
#include <string>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "IStunInterface.h"
#include "StunClient.h"
#include "ThreadLock.h"



class CStunJudgeNatType: public IStunSink
{
public:
	CStunJudgeNatType();
	~CStunJudgeNatType();

	//virtual interface
	virtual bool AdviseSink(IStunNetwork * pStunNetwork);
	virtual bool UnAdviseSink();
	virtual bool AddStunServer(std::string localSvrIP, unsigned short localSvrPort, std::string stunServerIP, unsigned short stunServerPort1 = 3478, unsigned short stunServerPort2 = 3479);
	virtual bool ResetJudgeNatType(bool upnpFound, bool force_readd);
	virtual long OnReceivePacket(std::string stunSvrIP, unsigned short stunSvrPort, char *buffer, unsigned long length);
	virtual void Stop(void);

	//�ڲ�����(��stunclient����)
	long OnSendStunPacket(const char * buffer, unsigned long length, std::string remoteIP, unsigned short remotePort);
	void OnStunFinished(NAT_TYPE natType, std::string publicIP, unsigned short publicPort);
private:
	struct StunReturnInfo
	{
		unsigned int   countNum;        //��������nat���͵Ĵ���
		std::string    strPublicIP;  //���صĹ���IP
		unsigned short unPublicPort; //���صĹ����˿�

		StunReturnInfo()
		: countNum(0)
		, unPublicPort(0)
		{
		}
	};

	struct StunPortInfo
	{
		unsigned short unStunPort1;
		unsigned short unStunPort2;

		StunPortInfo()
		: unStunPort1(0)
		, unStunPort2(0)
		{

		}
	};

	std::map< std::string, boost::shared_ptr<CStunClient> > m_mapStunCliPtr;
	std::map< std::string, struct StunPortInfo >            m_mapStunPortInfo;
	std::map<enum NAT_TYPE, struct StunReturnInfo>          m_mapNetType;
	IStunNetwork               *m_pStunNetwork;
	std::string                m_LocalIP;
	unsigned short             m_LocalPort;
	bool                       m_bIsRunning;
	CMutexLock                 m_lock;
};