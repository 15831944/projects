#include "StunClient.h"
#include "StunJudgeNatTypeInter.h"
#include <process.h>

const int TimeOutWait[4] = {500, 600, 700, 1000};
bool CStunClient::m_bIsOpenUpnp = false;

CStunClient::CStunClient()
: m_sink(NULL)
, m_Status(TEST_UNSTART)
, m_ServerPort1(0)
, m_ServerPort2(0)
, m_LocalPort(0)
, m_ResponsePortForStepOne(0)
, m_hRecvThread(NULL)
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

	if(m_hRecvThread)
	{
		WaitForSingleObject(m_hRecvThread, INFINITE);   
		CloseHandle(m_hRecvThread);
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

bool CStunClient::Start(std::string strLocalIP, unsigned short dLocalPort, std::string strStunIP, 
		                unsigned short dStunPort1/* = 3478*/, unsigned short dStunPort2/* = 3479*/)
{
	bool bResult = false;
	do 
	{
		if(strStunIP.empty() || NULL == m_hEvnetForStep1
			|| NULL == m_hEvnetForStep2 || NULL == m_hEvnetForStep3
			|| NULL == m_hStopEvent)
			break;

		m_ServerIP    = strStunIP; 
		m_ServerPort1 = dStunPort1;    
		m_ServerPort2 = dStunPort2;  

		m_LocalIP   = strLocalIP;
		m_LocalPort = dLocalPort;

		ResetEvent(m_hEvnetForStep1);
		ResetEvent(m_hEvnetForStep2);
		ResetEvent(m_hEvnetForStep3);
		ResetEvent(m_hStopEvent);

		unsigned int  dwThreadID = 0;
		m_hRecvThread = (HANDLE)_beginthreadex(NULL, 0, StateMachineThread, (void *)this, 0, &dwThreadID);
		if(NULL == m_hRecvThread)
			break;

		bResult = true;
	} while (false);

	return bResult;
}

bool CStunClient::AdviseSink(CStunJudgeNatType *sink)
{
	bool bResult = false;
	do
	{
		if(!sink)
			break;
		m_sink = sink;

		bResult = true;
	} while (false);

	return bResult;
}

//注意这个len的值，应该在上层的数据包头部记录的（*****）
void CStunClient::ProcessResponse(std::string strStunSerIP, unsigned short unStunSerPort, 
								  char *buffer, unsigned long len)
{
	SStunMessage sm;
	//这里需要反序列化
	CDataStream recvStream(buffer, len);
	recvStream >> sm;
	if(!recvStream.good_bit())
	{
		//error
		return;
	}

	while (true)
	{
		switch(m_Status)
		{
		case TEST_STEP1:
			{
				if(STUN_BINDING_RESPONSE == sm.messageID_)
				{
					m_MsgForStatus[TEST_STEP1] = sm;
					SetEvent(m_hEvnetForStep1);
				}
				break;
			}
		case TEST_STEP2:
			{
				if(STUN_CHANGE_IP_PORT_RESPONSE == sm.messageID_)
				{
					m_MsgForStatus[TEST_STEP2] = sm;
					SetEvent(m_hEvnetForStep2);
				}
				break;
			}
		case TEST_STEP3:
			{
				if(STUN_CHANGE_PORT_RESPONSE == sm.messageID_)
				{
					m_MsgForStatus[TEST_STEP3] = sm;
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

unsigned int __stdcall CStunClient::StateMachineThread()
{
	while(true)
	{
		int result;
		if(!m_sink)
			break;

		SetStatus(TEST_STEP1);
		result = Pro_Test1();
		if(NO_RESPONSE == result)
		{
			if(m_sink)
				m_sink->OnStunFinished(NAT_UDPBLOCKED, "", 0);
			break;
		}
		else if(SERIALIZE_ERROR == result || SINK_NULL == result 
				|| SEND_ERROR == result || EXIT_THREAD == result)
		{
			if(m_sink)
				m_sink->OnStunFinished(JUDGE_RUN_ERROR, "", 0);
			break;
		}
		else  //NO_RESPONSE != result
		{
			if(SAME_IP == result)   //说明前端无NAT
			{
				ResetEvent(m_hEvnetForStep1);
				SetStatus(TEST_STEP2);

				result = Pro_Test2();
				if(NO_RESPONSE == result)
				{
					if(m_sink)
						m_sink->OnStunFinished(NAT_SYMMETRICUDPFIREWALL, m_ResponseIPForStepOne, m_ResponsePortForStepOne);
					break;
				}
				else if(SERIALIZE_ERROR == result || SINK_NULL == result 
						|| SEND_ERROR == result || EXIT_THREAD == result)
				{
					if(m_sink)
						m_sink->OnStunFinished(JUDGE_RUN_ERROR, "", 0);
					break;
				}
				else   //RESPONSE == result
				{
					assert(RESPONSE == result);
					if (m_sink)
						m_sink->OnStunFinished(NAT_OPENINTERNET, m_ResponseIPForStepOne, m_ResponsePortForStepOne);
					break;
				}
			}
			else   //DIFF_IP == result  //说明前端肯定有NAT  
			{
				assert(DIFF_IP == result);
				ResetEvent(m_hEvnetForStep1);
				SetStatus(TEST_STEP2);

				result = Pro_Test2();
				if(NO_RESPONSE == result)
				{
					ResetEvent(m_hEvnetForStep2);
					SetStatus(TEST_STEP1);

					result = Pro_Another_Test1();
					if(NO_RESPONSE == result)
					{
						if(m_sink)
							m_sink->OnStunFinished(NAT_UDPBLOCKED, "", 0);
						break;
					}
					else if(SERIALIZE_ERROR == result || SINK_NULL == result 
						|| SEND_ERROR == result || EXIT_THREAD == result)
					{
						if(m_sink)
							m_sink->OnStunFinished(JUDGE_RUN_ERROR, "", 0);
						break;
					}
					else
					{
						if(SAME_IP == result) 
						{
							ResetEvent(m_hEvnetForStep1);
							SetStatus(TEST_STEP3);	

							result = Pro_Test3();
							if(NO_RESPONSE == result)
							{
								if (m_sink)
									m_sink->OnStunFinished(NAT_PORTRESTRICTEDCONE, m_ResponseIPForStepOne, m_ResponsePortForStepOne);
								break;
							}
							else if(SERIALIZE_ERROR == result || SINK_NULL == result 
								|| SEND_ERROR == result || EXIT_THREAD == result)
							{
								if(m_sink)
									m_sink->OnStunFinished(JUDGE_RUN_ERROR, "", 0);
								break;
							}
							else  //RESPONSE == result
							{
								assert(RESPONSE == result);
								if(m_sink)
									m_sink->OnStunFinished(NAT_IPRESTRICTEDCONE, m_ResponseIPForStepOne, m_ResponsePortForStepOne);
								break;
							}
						}
						else  //DIFF_IP == result 
						{
							assert(DIFF_IP == result);
							if(m_sink)
								m_sink->OnStunFinished(NAT_SYMMETRIC, m_ResponseIPForStepOne, m_ResponsePortForStepOne);
							break;
						}
					}


				}
				else if(SERIALIZE_ERROR == result || SINK_NULL == result 
					|| SEND_ERROR == result || EXIT_THREAD == result)
				{
					if(m_sink)
						m_sink->OnStunFinished(JUDGE_RUN_ERROR, "", 0);
					break;
				}
				else   //RESPONSE == result
				{
					assert(RESPONSE == result);
					if (m_sink)
						m_sink->OnStunFinished(NAT_FULLCONE, m_ResponseIPForStepOne, m_ResponsePortForStepOne);
					break;
				}
			}
		}
	}
	return 0;
}

int CStunClient::Pro_Test1()
{
	SStunMessage sendMess;
	sendMess.messageID_ = STUN_BINDING_REQUEST;
	sendMess.ip_        = m_LocalIP;
	sendMess.port_      = m_LocalPort;

	char sendBuffer[1024] = {0};
	CDataStream sendStream(sendBuffer, 1024);

	sendStream << sendMess;
	if(!sendStream.good_bit())
	{
		//error
		return SERIALIZE_ERROR;
	}

	//发送到StunIP1:Port1
	if(!m_sink)
		return SINK_NULL;

	unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort1);
	if (sendLen <= 0)
	{
		return SEND_ERROR;
	}


	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep1};
	unsigned long ret = 0;
	unsigned int unTimeOutIndex = 0;
	while(true)
	{
		ret = WaitForMultipleObjects(2, waitEvent, FALSE, TimeOutWait[unTimeOutIndex]);
		if(WAIT_TIMEOUT == ret)
		{
			if(++unTimeOutIndex >= 4)
			{
				return NO_RESPONSE;
			}

			if(!m_sink)
				return SINK_NULL;

			unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort1);
			if (sendLen <= 0)
			{
				return SEND_ERROR;
			}

		}
		else if(0 == (ret - WAIT_OBJECT_0))
		{
			return EXIT_THREAD;
		}
		else
		{
			assert(1 == (ret - WAIT_OBJECT_0));
			m_ResponseIPForStepOne   = m_MsgForStatus[TEST_STEP1].ip_;
			m_ResponsePortForStepOne = m_MsgForStatus[TEST_STEP1].port_;


			if(m_LocalIP == m_MsgForStatus[TEST_STEP1].ip_ && m_LocalPort == m_MsgForStatus[TEST_STEP1].port_)
				return SAME_IP;
			else
				return DIFF_IP;
		}
	}
}

int CStunClient::Pro_Test2()
{
	SStunMessage sendMess;
	sendMess.messageID_ = STUN_CHANGE_IP_PORT;
	sendMess.ip_        = m_LocalIP;
	sendMess.port_      = m_LocalPort;

	char sendBuffer[1024] = {0};
	CDataStream sendStream(sendBuffer, 1024);

	sendStream << sendMess;
	if(!sendStream.good_bit())
	{
		//error
		return SERIALIZE_ERROR;
	}

	//发送到StunIP1:Port1
	if(!m_sink)
		return SINK_NULL;

	unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort1);
	if (sendLen <= 0)
	{
		return SEND_ERROR;
	}


	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep2};
	unsigned long ret = 0;
	unsigned int unTimeOutIndex = 0;
	while(true)
	{
		ret = WaitForMultipleObjects(2, waitEvent, FALSE, TimeOutWait[unTimeOutIndex]);
		if(WAIT_TIMEOUT == ret)
		{
			if(++unTimeOutIndex >= 4)
			{
				return NO_RESPONSE;
			}

			if(!m_sink)
				return SINK_NULL;

			unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort1);
			if (sendLen <= 0)
			{
				return SEND_ERROR;
			}

		}
		else if(0 == (ret - WAIT_OBJECT_0))
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

int CStunClient::Pro_Another_Test1()
{
	SStunMessage sendMess;
	sendMess.messageID_ = STUN_BINDING_REQUEST;
	sendMess.ip_        = m_LocalIP;
	sendMess.port_      = m_LocalPort;

	char sendBuffer[1024] = {0};
	CDataStream sendStream(sendBuffer, 1024);

	sendStream << sendMess;
	if(!sendStream.good_bit())
	{
		//error
		return SERIALIZE_ERROR;
	}

	//发送到StunIP1:Port2
	if(!m_sink)
		return SINK_NULL;

	unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort2);
	if (sendLen <= 0)
	{
		return SEND_ERROR;
	}


	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep1};
	unsigned long ret = 0;
	unsigned int unTimeOutIndex = 0;
	while(true)
	{
		ret = WaitForMultipleObjects(2, waitEvent, FALSE, TimeOutWait[unTimeOutIndex]);
		if(WAIT_TIMEOUT == ret)
		{
			if(++unTimeOutIndex >= 4)
			{
				return NO_RESPONSE;
			}

			if(!m_sink)
				return SINK_NULL;

			unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort2);
			if (sendLen <= 0)
			{
				return SEND_ERROR;
			}

		}
		else if(0 == (ret - WAIT_OBJECT_0))
		{
			return EXIT_THREAD;
		}
		else
		{
			assert(1 == (ret - WAIT_OBJECT_0));

			if(m_ResponseIPForStepOne == m_MsgForStatus[TEST_STEP1].ip_ && m_ResponsePortForStepOne == m_MsgForStatus[TEST_STEP1].port_)
				return SAME_IP;
			else
				return DIFF_IP;
		}
	}
}

int CStunClient::Pro_Test3()
{
	SStunMessage sendMess;
	sendMess.messageID_ = STUN_CHANGE_PORT;
	sendMess.ip_        = m_LocalIP;
	sendMess.port_      = m_LocalPort;

	char sendBuffer[1024] = {0};
	CDataStream sendStream(sendBuffer, 1024);

	sendStream << sendMess;
	if(!sendStream.good_bit())
	{
		//error
		return SERIALIZE_ERROR;
	}

	//发送到StunIP1:Port1
	if(!m_sink)
		return SINK_NULL;

	unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort1);
	if (sendLen <= 0)
	{
		return SEND_ERROR;
	}


	HANDLE waitEvent[2] = {m_hStopEvent, m_hEvnetForStep3};
	unsigned long ret = 0;
	unsigned int unTimeOutIndex = 0;
	while(true)
	{
		ret = WaitForMultipleObjects(2, waitEvent, FALSE, TimeOutWait[unTimeOutIndex]);
		if(WAIT_TIMEOUT == ret)
		{
			if(++unTimeOutIndex >= 4)
			{
				return NO_RESPONSE;
			}

			if(!m_sink)
				return SINK_NULL;

			unsigned long sendLen = m_sink->OnSendStunPacket(sendStream.getbuffer(), sendStream.size(), m_ServerIP,  m_ServerPort1);
			if (sendLen <= 0)
			{
				return SEND_ERROR;
			}

		}
		else if(0 == (ret - WAIT_OBJECT_0))
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