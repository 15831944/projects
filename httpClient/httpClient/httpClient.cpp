// httpClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "http_client.h"
#include <WinSock2.h>
#pragma comment(lib, "WS2_32")

class Test:public IRecvInterface
{
public:
	Test(CHttpClient &y)
	: t(&y)
	{

	}

	void Get()
	{
		int errorCode = 0;
		
		t->Get(1234,"http://list3.ppstream.com/cfg/ClientPolicies.xml", errorCode);

	}
	void OnDownLoadFinish(bool bResult, int errorCode)
	{
		std::string data;
		if(t->GetDataBuf(1234, data))
		{
			int fd = 0;
			fd++;

		}

	}
private:
	CHttpClient *t;
};


int _tmain(int argc, _TCHAR* argv[])
{
	WSAData wd;
	WSAStartup(MAKEWORD(2,2), &wd);

	CHttpClient http;
	http.StartSer();


	Test t(http);
	http.RegisterTask(1234, &t);

	t.Get();

	getchar();
	return 0;
}

