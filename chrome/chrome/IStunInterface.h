#pragma once

#include <string>
#include <map>

enum NAT_TYPE
{ 
	UNKNOWN = 0, 
	JUDGE_RUN_ERROR,              //说明在判断过程中存在套接字、序列化、回调指针变为空或在判断中判断线程被要求退出的错误
	NAT_UDPBLOCKED,               //udp限制
	NAT_SYMMETRIC,                //对称式
	NAT_SYMMETRICUDPFIREWALL,     //对称式防火墙
	NAT_PORTRESTRICTEDCONE,      //端口限制性克隆
	NAT_IPRESTRICTEDCONE,        //限制性克隆
	NAT_FULLCONE,                //全克隆 
    NAT_OPENINTERNET             //完全开放（公网）
};


//用于上层继承（继承这个类的类一般需要包括数据的接收与发送套接字）
class IStunNetwork
{
public:
	//用于底层回调，判断出的结果通知上层
	virtual long OnStunNatFinished(NAT_TYPE natType, std::string publicIP, unsigned short publicPort)                          = 0;
	//用于处理接收的stun服务器发来的数据包
	virtual long OnStunReceived(std::string stunSvrIP, unsigned short stunSvrPort, const char *buffer, unsigned long length)   = 0;
	//用于底层回调，发送底层的数据包
	virtual long StunSend(const char *buffer, unsigned long length, std::string stunSvrIP, unsigned short stunSvrPort)         = 0;
};


class IStunSink
{
public:
	virtual bool AdviseSink(IStunNetwork * pStunNetwork) = 0;
	virtual bool UnAdviseSink() = 0;
	//主要接口，用于添加stunclient
	virtual bool AddStunServer(std::string localSvrIP, unsigned short localSvrPort, std::string stunServerIP, unsigned short stunServerPort1 = 3478, unsigned short stunServerPort2 = 3479) = 0;
	virtual bool ResetJudgeNatType(bool upnpFound, bool force_readd) = 0;
	virtual long OnReceivePacket(std::string stunSvrIP, unsigned short stunSvrPort, char *buffer, unsigned long length) = 0;

	//下面的需考虑
	virtual void Stop(void) = 0;
	//virtual void Release(void) = 0;	
};