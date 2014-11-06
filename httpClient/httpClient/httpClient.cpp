// httpClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "http_client.h"
#include <WinSock2.h>
#pragma comment(lib, "WS2_32")


class Test:public IRecvInterface
{
	bool OnRecv(char *buf){return true;}
};


int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wd;
	WSAStartup(MAKEWORD(2,2), &wd);
	Test t;
	CHttpClient http;
	http.StartSer();
	http.RegisterTask(1234, &t);
	int errorCode = 0;
	http.Get(1234,"http://list3.ppstream.com/cfg/ClientPolicies.xml", errorCode);
	getchar();
	return 0;
}

