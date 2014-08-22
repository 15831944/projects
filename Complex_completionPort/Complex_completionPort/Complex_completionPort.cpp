// Complex_completionPort.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "InitNetworkEn.h"
#include "Complex_IOCPSer.h"

static CInitSock IintNet;


int _tmain(int argc, _TCHAR* argv[])
{
	CIOCPSer test;
	unsigned long errorCode = 0;
	if(!(test.StartSer(errorCode)))
	{
		printf("errorCode = %u\n", errorCode);
	}


	test.join();
	return 0;
}

