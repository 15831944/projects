// CompletionPortSer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InitNetworkEn.h"
#include "./log/InterfaceLogger.h"
#include "IOCPSer_Simple.h"
CInitSock sockEnv;

int _tmain(int argc, _TCHAR* argv[])
{
	//start log
	logger::CLogger::Start();
	logger::CLogger::SetPath("D:\\testlog");

	CIOCPSerSimple test;
	test.StartSer();


	logger::CLogger::Stop();
	return 0;
}

