#include "StunClient.h"
#include "JudgeNatType.h"


CStunClient::CStunClient()
: m_sink(NULL)
, m_Status(TEST_UNSTART)
, m_hRecvThread(NULL)
, m_ServerPort1(0)
, m_ServerPort2(0)
, m_LocalPort(0)
, m_ResponsePort(0)
, m_taskId(0)
, m_totolWaitSecond(1600)
{
	m_hEvnetForStep1 = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvnetForStep2 = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvnetForStep3 = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hStopEvent     = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CStunClient::~CStunClient()
{
	Stop();
}

void CStunClient::Stop()
{
	if (m_hStopEvent)
	{
		SetEvent(m_hStopEvent);
	}

	if (m_hRecvThread)
	{
		unsigned int exitCode = 0;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hRecvThread, 100))
		{
			TerminateThread(m_hRecvThread, exitCode);
		}

		CloseHandle(m_hRecvThread);
		m_hRecvThread = NULL;
	}

	if (m_hEvnetForStep1)
	{
		CloseHandle(m_hEvnetForStep1);
		m_hEvnetForStep1 = NULL;
	}

	if (m_hEvnetForStep2)
	{
		CloseHandle(m_hEvnetForStep2);
		m_hEvnetForStep2 = NULL;
	}

	if (m_hEvnetForStep3)
	{
		CloseHandle(m_hEvnetForStep3);
		m_hEvnetForStep3 = NULL;
	}

	if (m_hStopEvent)
	{
		CloseHandle(m_hStopEvent);
		m_hStopEvent = NULL;
	}
}


bool CStunClient::Start(unsigned long Task_id, std::string localIP, WORD localPort, std::string stunServerIP, unsigned short stunServerPort1, unsigned short stunServerPort2)
{
	bool bResult = false;

	do 
	{
		if(stunServerIP.empty() || NULL == m_hEvnetForStep1
			|| NULL == m_hEvnetForStep2 || NULL == m_hEvnetForStep3)
			break;

		m_ServerIP    = stunServerIP;
		m_ServerPort1 = stunServerPort1;
		m_ServerPort2 = stunServerPort2;
		m_LocalIP     = localIP;
		m_LocalPort   = localPort;

		m_taskId      = Task_id;

		ResetEvent(m_hEvnetForStep1);
		ResetEvent(m_hEvnetForStep2);
		ResetEvent(m_hEvnetForStep3);

		unsigned int  dwThreadID = 0;
		m_hRecvThread = (HANDLE)_beginthreadex(NULL, 0, StateMachineThread, (void*)this, 0, &dwThreadID);
		if (NULL == m_hRecvThread)
		{
			break;
		}

		bResult = true;
	} while (false);

	return bResult;
}

bool CStunClient::AdviseSink(CStunJudgeNatType *sink)
{
	bool bResult = false;
	do 
	{
		if (NULL != sink)
		{
			m_sink = sink;
			bResult = true;
		}
	} while (false);

	return bResult;
}

bool CStunClient::UnAdviseSink()
{ 
	m_sink = NULL;
	return true;
}

void CStunClient::SetStatus(STUN_STATUS ss)
{
	m_Status = ss;
}


//The upper back to call
void CStunClient::ProcessResponse(std::string stunSvrIP, unsigned short stunSvrPort, const char* recvds, unsigned long dataLen)
{ 
	assert(dataLen == sizeof(SStunMessage));
	if (dataLen != sizeof(SStunMessage))
	{
		return;
	}

	SStunMessage sm;
	while (true)
	{
		memcpy(&sm, recvds, dataLen);

		switch(m_Status)
		{
		case TEST_STEP1:
			{
				if(STUN_BINDING_RESPONSE == sm.header_)
				{
					memcpy(&m_MsgForStatus[TEST_STEP1], &sm, sizeof(SStunMessage));
					SetEvent(m_hEvnetForStep1);
				}
				break;
			}
		case TEST_STEP2:
			{
				if(STUN_CHANGE_IP_PORT_RESPONSE == sm.header_)
				{
					memcpy(&m_MsgForStatus[TEST_STEP2], &sm, sizeof(SStunMessage));
					SetEvent(m_hEvnetForStep2);
				}
				break;
			}
		case TEST_STEP3:
			{
				if(STUN_CHANGE_PORT_RESPONSE == sm.header_)
				{
					memcpy(&m_MsgForStatus[TEST_STEP3], &sm, sizeof(SStunMessage));
					SetEvent(m_hEvnetForStep3);
				}
				break;
			}
		default:
			break;
		}
	}
}

unsigned int __stdcall CStunClient::StateMachineThread(void *param)
{
	do 
	{
		CStunClient* pThis = reinterpret_cast<CStunClient*>(param);
		if(NULL == pThis)
		{
			break;
		}
		return pThis->StateMachineThread();
	} while(false);
	return 0;
}

//Implement the state machine logic
unsigned int __stdcall CStunClient::StateMachineThread()
{
	int ret = UNKNOWN_ERROR;
	SetStatus(TEST_STEP1);
#ifdef WIN32
	OutputDebugStringA("start Test1 ...\n");
#endif

	ret = Pro_Test1();
	if (EXIT_THREAD == ret)
	{
#ifdef WIN32
		OutputDebugStringA("receive exit thread mesage!\n");
#endif
		return 0;
	}
	else if(NO_RESPONSE == ret )//udp blockked
	{

#ifdef WIN32
		OutputDebugStringA("stun server no resonse, may be network is error, or udp block\n");
#endif
		m_sink->OnStunFinished(NAT_UDPBLOCKED, m_ServerIP, "", 0);
	}
	else if( SAME_IP == ret)
	{
		ResetEvent(m_hEvnetForStep1);
		SetStatus(TEST_STEP2);
#ifdef WIN32
		OutputDebugStringA("The server returns the IP and PORT is the same as the local IP and PORT, start Test2 ...\n");
#endif
		ret = Pro_Test2();
		if (EXIT_THREAD == ret)
		{
#ifdef WIN32
			OutputDebugStringA("receive exit thread mesage!\n");
#endif
			return 0;
		}
		else if (RESPONSE == ret)//public
		{
#ifdef WIN32
			OutputDebugStringA("open internet\n");
#endif
			m_sink->OnStunFinished(NAT_OPENINTERNET, m_ServerIP, m_ResponseIP, m_ResponsePort);
		}
		else//symme udp firewall
		{
#ifdef WIN32
			OutputDebugStringA("symmetric udp firewall\n");
#endif
			m_sink->OnStunFinished(NAT_SYMMETRICUDPFIREWALL, m_ServerIP, m_ResponseIP, m_ResponsePort);
		}
	}
	else // DIFF_IP
	{
		ResetEvent(m_hEvnetForStep1);
		SetStatus(TEST_STEP2);
#ifdef WIN32
		OutputDebugStringA("The server returns the IP and PORT is not the same as the local IP and PORT, start Test2 ...\n");
#endif
		ret = Pro_Test2();
		if (EXIT_THREAD == ret)
		{
#ifdef WIN32
			OutputDebugStringA("receive exit thread mesage!\n");
#endif
			return 0;
		}
		else if(ret == RESPONSE) //full cone
		{
#ifdef WIN32
			OutputDebugStringA("full cone!\n");
#endif
			m_sink->OnStunFinished(NAT_FULLCONE, m_ServerIP, m_ResponseIP, m_ResponsePort);
		}
		else
		{
			ResetEvent(m_hEvnetForStep2);
			SetStatus(TEST_STEP1);
#ifdef WIN32
			OutputDebugStringA("The server no response, To another serve start Test1 ...\n");
#endif
			ret = Pro_Another_Test1();
			if (EXIT_THREAD == ret)
			{
#ifdef WIN32
				OutputDebugStringA("receive exit thread mesage!\n");
#endif
				return 0;
			}
			else if(ret == DIFF_IP) //symmetric
			{
#ifdef WIN32
				OutputDebugStringA("symmetric!\n");
#endif
				m_sink->OnStunFinished(NAT_SYMMETRIC, m_ServerIP, m_ResponseIP, m_ResponsePort);
			}
			//欠缺未回复的处理？？？？？(应该是由于丢包引起了，处理方案暂未想出来)
			else
			{
				ResetEvent(m_hEvnetForStep1);
				SetStatus(TEST_STEP3);
#ifdef WIN32
				OutputDebugStringA("The server returns the IP and PORT is the same as test1 return the IP and PORT, start Test3 ...\n");
#endif
				ret = Pro_Test3();
				if (EXIT_THREAD == ret)
				{
#ifdef WIN32
					OutputDebugStringA("receive exit thread mesage!\n");
#endif
					return 0;
				}
				else if (ret == NO_RESPONSE)//port restricted cone
				{
#ifdef WIN32
					OutputDebugStringA("port restricted cone\n");
#endif
					m_sink->OnStunFinished(NAT_PORTRESTRICTEDCONE, m_ServerIP, m_ResponseIP, m_ResponsePort);
				}
				else//restricted cone
				{
#ifdef WIN32
					OutputDebugStringA("IP restricted cone\n");
#endif
					m_sink->OnStunFinished(NAT_IPRESTRICTEDCONE, m_ServerIP, m_ResponseIP, m_ResponsePort);
				}
			}
		}
	}

	return 0;
}


int CStunClient::Pro_Test1()
{
	SStunMessage sm;
	sm.task_id_  = m_taskId;
	sm.header_   = STUN_BINDING_REQUEST;
	sm.ip_       = m_LocalIP;
	sm.port_     = m_LocalPort;


	unsigned long sendLen = m_sink->OnSendStunPacket((const char *)(&sm), sizeof(sm), m_ServerIP,  m_ServerPort1);
	if (sendLen <= 0)
	{
		return EXIT_THREAD;
	}

	unsigned long statr_time = GetTickCount();
	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep1};
	unsigned long ret = WaitForMultipleObjects(2, waitEvent, FALSE, m_totolWaitSecond);
	if(WAIT_TIMEOUT == ret)
	{
		return NO_RESPONSE;
	}
	else
	{
		if (0 == (ret - WAIT_OBJECT_0))
		{
			return EXIT_THREAD;
		}
		else
		{
			assert(1 == (ret - WAIT_OBJECT_0));
			m_totolWaitSecond -= (GetTickCount() - statr_time);
			//CScopeLock scopeLock(m_pLock);
			m_ResponseIP   = m_MsgForStatus[TEST_STEP1].ip_;
			m_ResponsePort = m_MsgForStatus[TEST_STEP1].port_;

			if(m_LocalIP == m_MsgForStatus[TEST_STEP1].ip_ && m_LocalPort == m_MsgForStatus[TEST_STEP1].port_)
				return SAME_IP;
			else
				return DIFF_IP;
		}
	}
}

int CStunClient::Pro_Another_Test1()
{
	SStunMessage sm;
	sm.task_id_ = m_taskId;
	sm.header_  = STUN_BINDING_REQUEST;
	sm.ip_      = m_LocalIP;
	sm.port_    = m_LocalPort;

	unsigned long sendLen = m_sink->OnSendStunPacket((const char *)(&sm), sizeof(sm), m_ServerIP,  m_ServerPort2);
	if (sendLen <= 0)
	{
		return EXIT_THREAD;
	}

	unsigned long statr_time = GetTickCount();
	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep1};
	unsigned long ret = WaitForMultipleObjects(2, waitEvent, FALSE, m_totolWaitSecond);
	if(WAIT_TIMEOUT == ret)
	{
		return NO_RESPONSE;
	}
	else
	{
		if (0 == (ret - WAIT_OBJECT_0))
		{
			return EXIT_THREAD;
		}
		else
		{
			assert(1 == (ret - WAIT_OBJECT_0));
			m_totolWaitSecond -= (GetTickCount() - statr_time);
			//CScopeLock scopeLock(m_pLock);
			if(m_ResponseIP == m_MsgForStatus[TEST_STEP1].ip_ && m_ResponsePort == m_MsgForStatus[TEST_STEP1].port_)
				return SAME_IP;
			else
				return DIFF_IP;
		}
	}
}

int CStunClient::Pro_Test2()
{
	SStunMessage sm;
	sm.task_id_ = m_taskId;
	sm.header_  = STUN_CHANGE_IP_PORT;
	sm.ip_      = m_LocalIP;
	sm.port_    = m_LocalPort;

	unsigned long sendLen = m_sink->OnSendStunPacket((const char *)(&sm), sizeof(sm), m_ServerIP,  m_ServerPort1);
	if (sendLen <= 0)
	{
		return EXIT_THREAD;
	}

	unsigned long statr_time = GetTickCount();
	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep2};
	unsigned long ret = WaitForMultipleObjects(2, waitEvent, FALSE, 500);
	if(WAIT_TIMEOUT == ret)
	{
		m_totolWaitSecond -= 500;
		return NO_RESPONSE;
	}
	else
	{
		if (0 == (ret - WAIT_OBJECT_0))
		{
			return EXIT_THREAD;
		}
		else
		{
			assert(1 == (ret - WAIT_OBJECT_0));
			return RESPONSE;
		}
	}
}

int CStunClient::Pro_Test3()
{
	SStunMessage sm;
	sm.task_id_ = m_taskId;
	sm.header_  = STUN_CHANGE_PORT;
	sm.ip_      = m_LocalIP;
	sm.port_    = m_LocalPort;

	unsigned long sendLen = m_sink->OnSendStunPacket((const char *)(&sm), sizeof(sm), m_ServerIP,  m_ServerPort1);
	if (sendLen <= 0)
	{
		return EXIT_THREAD;
	}


	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep3};
	unsigned long ret = WaitForMultipleObjects(2, waitEvent, FALSE, m_totolWaitSecond);
	if(WAIT_TIMEOUT == ret)
	{
		return NO_RESPONSE;
	}
	else
	{
		if (0 == (ret - WAIT_OBJECT_0))
		{
			return EXIT_THREAD;
		}
		else
		{
			assert(1 == (ret - WAIT_OBJECT_0));
			return RESPONSE;
		}
	}
}



