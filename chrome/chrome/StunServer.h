#pragma once
#include "IStunSerInterace.h"
#include "StunProtocol.h"
#include <Windows.h>
#include <process.h>
#include <WinSock2.h>
#pragma comment(lib, "WS2_32")

/*
������ǲ����õģ�ֻ���������Կͻ��˴����
*/


class CStunServer:public IStunSerInter
{
public:
	CStunServer();
	~CStunServer();

	//interface
	virtual bool Start(const char* ip1, const char* ip2, unsigned short port1 = 3478, 
		unsigned short port2 = 3479, unsigned short port3 = 3480, unsigned short port4 = 3480);

	virtual void Stop();
private:
	static unsigned int __stdcall OnRecvThread(void *parm);
	unsigned int __stdcall OnRecvThread();
	void OnRecvMsg(SOCKET sk, SStunMessage sm, sockaddr_in remoteAddr);

	void CleanEvn();

private:
	HANDLE m_hRecvThread;
	SOCKET m_socketTest1;
	SOCKET m_socketTest2;
	SOCKET m_socketTest3;
	SOCKET m_socketTest4;

	fd_set m_fdReadSet;

	bool   m_Isrunning;

};