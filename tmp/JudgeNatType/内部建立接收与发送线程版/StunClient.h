#ifndef STUNCLIENT_H
#define STUNCLIENT_H

#ifdef WIN32
#include <Windows.h>
#endif

#include "StunProtocol.h"

#include <process.h>
#include <string>

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
	EXIT_THREAD = 10
};

//class IStunSink;
class CStunJudgeNatType;

class CStunClient
{
public:
	CStunClient();
	~CStunClient();

	bool Start(std::string localIP, unsigned short localPort, std::string stunServerIP, unsigned short stunServerPort1, unsigned short stunServerPort2);
	bool AdviseSink(CStunJudgeNatType *sink);
	void ProcessResponse(std::string stunSvrIP, unsigned short stunSvrPort, const char* recvds, unsigned long dataLen);
	void Stop();

private:
	void SetStatus(STUN_STATUS ss);

	static unsigned int __stdcall StateMachineThread(void *param);
	unsigned int __stdcall StateMachineThread();


	int Pro_Test1();
	int Pro_Test2();
	int Pro_Test3();
	int Pro_Another_Test1();


private:
	CStunJudgeNatType       *m_sink;

	STUN_STATUS             m_Status;
	SStunMessage            m_MsgForStatus[4];

	std::string             m_ServerIP; 
	unsigned short          m_ServerPort1;    
	unsigned short          m_ServerPort2;  

	std::string             m_LocalIP;
	unsigned short          m_LocalPort;

	std::string             m_ResponseIP;
	unsigned short          m_ResponsePort;

	HANDLE	                m_hEvnetForStep1;     
	HANDLE	                m_hEvnetForStep2;     
	HANDLE		            m_hEvnetForStep3;  
	HANDLE                  m_hStopEvent;
	HANDLE			        m_hRecvThread;
	

	unsigned long           m_totolWaitSecond;   //default 1600ms
};


#endif   //STUNCLIENT_H