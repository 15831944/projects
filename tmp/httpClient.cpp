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

	void Get(bool f)
	{
		int errorCode = 0;
		if (f)
		{
			//t->Get(1234,"zhidao.baidu.com/question/351319520.html", errorCode/*, "Accept-Encoding: gzip, deflate\r\n"*/);
			t->Get(1234,"http://www.baidu.com/", errorCode);
		}
		else
		{
			t->Get(1234,"http://list3.ppstream.com/cfg/ClientPolicies.xml", errorCode, "Accept-Encoding: gzip, deflate\r\n");
		}
		int  df = 5;
		df++;
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
	{
	CHttpClient http;
	http.StartSer();


	Test t(http);
	http.RegisterTask(1234, &t);

	t.Get(false);

	getchar();
	t.Get(true);
	getchar();
	}
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	getchar();
	return 0;
}

