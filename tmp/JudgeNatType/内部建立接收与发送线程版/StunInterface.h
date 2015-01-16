#ifndef STUNINTERFACE_H
#define STUNINTERFACE_H

#ifdef STUN_DLL
#else
#define STUN_DLL _declspec(dllimport)
#endif

#include <map>
#include <string>

enum NAT_TYPE
{ 
	UNKNOWN = 0, 
	NAT_UDPBLOCKED, 
	NAT_OPENINTERNET, 
	NAT_SYMMETRICUDPFIREWALL, 
	NAT_FULLCONE, 
	NAT_IPRESTRICTEDCONE, 
	NAT_PORTRESTRICTEDCONE, 
	NAT_SYMMETRIC
};

//interface
class IStunNetwork
{
public:
//	virtual long OnStunReceived(std::string stunSvrIP, unsigned short stunSvrPort, const char *buffer, unsigned long length)   = 0;
//	virtual long StunSend(const char *buffer, unsigned long length, std::string stunSvrIP, unsigned short stunSvrPort)         = 0;


	virtual long OnStunNatFinished(NAT_TYPE natType, std::string publicIP, unsigned short publicPort)                          = 0;
};

//interface
class IStunSink
{
public :
	
	virtual bool Start() = 0;
	virtual bool AdviseSink(IStunNetwork * pStunNetwork) = 0;
	virtual bool UnAdviseSink() = 0;
	virtual bool AddStunServer(std::string stunServerIP, unsigned short stunServerPort1 = 3478, unsigned short stunServerPort2 = 3479) = 0;
	virtual bool ResetJudgeNatType(bool upnpFound,bool force_readd) = 0;

	//新加入的接口
	//virtual long OnReceivePacket(DWORD stunSvrIP, DWORD stunSvrPort, const char *buffer, unsigned long length) = 0;

	//下面的需考虑
	virtual void Release(void) = 0;	
};

//DLL export interface
STUN_DLL IStunSink* CreateJudgeNatTypeClass();
STUN_DLL void FreeJudgeNatTypeClass();

#endif  //STUNINTERFACE_H