#pragma once

#include <string>
#include <Windows.h>
#include "StunProtocol.h"

/*************************************************
Stun�Ŀͻ���ʵ�ִ���
������Ҫ����������״̬���Ļ�������ɵ�
*************************************************/
enum STUN_STATUS 
{
	TEST_UNSTART = 0, 
	TEST_STEP1, 
	TEST_STEP2, 
	TEST_STEP3
};

enum TEST_RESULT
{
	UNKNOWN_ERROR = 0, 
	NO_RESPONSE, 
	RESPONSE, 
	SAME_IP, 
	DIFF_IP,
	SERIALIZE_ERROR,
	SEND_ERROR,
	SINK_NULL,
	EXIT_THREAD = 10
};

 
class CStunJudgeNatType;
class CStunClient
{
	friend CStunJudgeNatType;
public:
	CStunClient();
	~CStunClient();

	bool Start(std::string strLocalIP, unsigned short dLocalPort, std::string strStunIP, 
			   unsigned short dStunPort1 = 3478, unsigned short dStunPort2 = 3479);
	void Stop();

	bool AdviseSink(CStunJudgeNatType *sink);
	void ProcessResponse(std::string strStunSerIP, unsigned short unStunSerPort, char *buffer, unsigned long len);
private:

	static unsigned int __stdcall StateMachineThread(void *param);
	unsigned int __stdcall StateMachineThread();

	//����״̬���ĸ������̴�����
	int Pro_Test1();
	int Pro_Test2();
	int Pro_Test3();
	int Pro_Another_Test1();

	void SetStatus(STUN_STATUS st){m_Status = st;}
private:
	CStunJudgeNatType       *m_sink;
	STUN_STATUS             m_Status;
	SStunMessage            m_MsgForStatus[4];

	std::string             m_ServerIP; 
	unsigned short          m_ServerPort1;    
	unsigned short          m_ServerPort2;  

	std::string             m_LocalIP;
	unsigned short          m_LocalPort;

	std::string             m_ResponseIPForStepOne;   //��ʵ����һ������IP
	unsigned short          m_ResponsePortForStepOne; //��ʵ����һ������PORT

	HANDLE	    m_hRecvThread;
	HANDLE	    m_hEvnetForStep1;     
	HANDLE	    m_hEvnetForStep2;     
	HANDLE		m_hEvnetForStep3;  
	HANDLE      m_hStopEvent;   //ֹͣ�̵߳��¼�����

	static bool  m_bIsOpenUpnp;  //��ʱ����
};
