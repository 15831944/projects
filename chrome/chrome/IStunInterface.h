#pragma once

#include <string>
#include <map>

enum NAT_TYPE
{ 
	UNKNOWN = 0, 
	JUDGE_RUN_ERROR,              //˵�����жϹ����д����׽��֡����л����ص�ָ���Ϊ�ջ����ж����ж��̱߳�Ҫ���˳��Ĵ���
	NAT_UDPBLOCKED,               //udp����
	NAT_SYMMETRIC,                //�Գ�ʽ
	NAT_SYMMETRICUDPFIREWALL,     //�Գ�ʽ����ǽ
	NAT_PORTRESTRICTEDCONE,      //�˿������Կ�¡
	NAT_IPRESTRICTEDCONE,        //�����Կ�¡
	NAT_FULLCONE,                //ȫ��¡ 
    NAT_OPENINTERNET             //��ȫ���ţ�������
};


//�����ϲ�̳У��̳���������һ����Ҫ�������ݵĽ����뷢���׽��֣�
class IStunNetwork
{
public:
	//���ڵײ�ص����жϳ��Ľ��֪ͨ�ϲ�
	virtual long OnStunNatFinished(NAT_TYPE natType, std::string publicIP, unsigned short publicPort)                          = 0;
	//���ڴ�����յ�stun���������������ݰ�
	virtual long OnStunReceived(std::string stunSvrIP, unsigned short stunSvrPort, const char *buffer, unsigned long length)   = 0;
	//���ڵײ�ص������͵ײ�����ݰ�
	virtual long StunSend(const char *buffer, unsigned long length, std::string stunSvrIP, unsigned short stunSvrPort)         = 0;
};


class IStunSink
{
public:
	virtual bool AdviseSink(IStunNetwork * pStunNetwork) = 0;
	virtual bool UnAdviseSink() = 0;
	//��Ҫ�ӿڣ��������stunclient
	virtual bool AddStunServer(std::string localSvrIP, unsigned short localSvrPort, std::string stunServerIP, unsigned short stunServerPort1 = 3478, unsigned short stunServerPort2 = 3479) = 0;
	virtual bool ResetJudgeNatType(bool upnpFound, bool force_readd) = 0;
	virtual long OnReceivePacket(std::string stunSvrIP, unsigned short stunSvrPort, char *buffer, unsigned long length) = 0;

	//������迼��
	virtual void Stop(void) = 0;
	//virtual void Release(void) = 0;	
};