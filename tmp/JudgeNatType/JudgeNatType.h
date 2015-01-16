#ifndef JUDGENATTYPE_H
#define JUDGENATTYPE_H
 
#include <map>
#include <string>
#include <vector>
#include <process.h>

#include "InitNetworkEn.h"
#include "StunClient.h"
#include "StunInterface.h"

#include <boost/smart_ptr.hpp>


class CStunJudgeNatType: public IStunSink
{
public:
	CStunJudgeNatType();
	~CStunJudgeNatType(void);

	//implement interface
	virtual bool AdviseSink(IStunNetwork * pStunNetwork);
	virtual bool UnAdviseSink();
	virtual bool AddStunServer(std::string localSvrIP, unsigned short localSvrPort, std::string stunServerIP, unsigned short stunServerPort1 = 3478, unsigned short stunServerPort2 = 3479);
	virtual bool ResetJudgeNatType(bool upnpFound,bool force_readd);
	virtual long OnReceivePacket(std::string stunSvrIP, unsigned short stunSvrPort, const char *buffer, unsigned long length);
	

	
	virtual void Release(void);

	void Stop();

	//内部函数(供stunclient调用)
	long OnSendStunPacket(const char * buffer, unsigned long length, std::string remoteIP, unsigned short remotePort);
	long OnStunFinished(NAT_TYPE natType, std::string stunServerIP,  std::string publicIP, unsigned short publicPort);

	static CStunJudgeNatType* CreateObject();

private:
	static CStunJudgeNatType   m_Instance;
	IStunNetwork               *m_pStunNetwork;
	
	bool              m_IsRunning;
	std::string       m_LocalIP;
	unsigned short    m_LocalPort;

	std::map<std::string, boost::shared_ptr<CStunClient> > m_StunCliPtr;
	
};

#endif   //JUDGENATTYPE_H