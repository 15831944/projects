#ifndef JUDGENATTYPE_H
#define JUDGENATTYPE_H
 
#include <map>
#include <string>
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
	virtual bool Start();
	virtual bool AdviseSink(IStunNetwork * pStunNetwork);
	virtual bool AddStunServer(std::string stunServerIP, unsigned short stunServerPort1 = 3478, unsigned short stunServerPort2 = 3479);
	virtual bool ResetJudgeNatType(bool upnpFound,bool force_readd);
	

	virtual bool UnAdviseSink();
	virtual void Release(void);

	void Stop();

	//内部函数(供stunclient调用)
	long OnSendStunPacket(const char * buffer, unsigned long length, std::string remoteIP, unsigned short remotePort);
	long OnStunFinished(NAT_TYPE natType, std::string stunServerIP,  std::string publicIP, unsigned short publicPort);

	static CStunJudgeNatType* CreateObject();

private:
	bool InitThread();
	static unsigned int __stdcall WorkThread(void *param);
	unsigned int __stdcall WorkThread();


private:

	static CStunJudgeNatType   m_Instance;
	IStunNetwork               *m_pStunNetwork;
	
	bool              m_IsRunning;
	SOCKET            m_ProbeSocket;
	std::string       m_LocalIP;
	unsigned short    m_LocalPort;
	HANDLE			  m_hWorkThread;

	std::map<std::string, boost::shared_ptr<CStunClient> > m_StunCliPtr;
	
};

#endif   //JUDGENATTYPE_H