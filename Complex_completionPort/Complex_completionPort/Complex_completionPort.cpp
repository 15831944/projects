// Complex_completionPort.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "InitNetworkEn.h"
#include "Complex_IOCPSer.h"

static CInitSock IintNet;


int _tmain(int argc, _TCHAR* argv[])
{
	CIOCPSer test;
	test.StartSer();
	return 0;
}

