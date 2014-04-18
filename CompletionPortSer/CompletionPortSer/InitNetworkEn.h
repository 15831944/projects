#ifndef COMPLETIONPORTSER_INITNETWORKEN_HEAD
#define COMPLETIONPORTSER_INITNETWORKEN_HEAD
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "WS2_32")

class CInitSock
{
public:
	CInitSock(BYTE minor = 2, BYTE major = 2)
	{
		//ini ws2_32.dll
		WSADATA wsaData;
		if(0 != WSAStartup(MAKEWORD(major, minor), &wsaData))
		{
			//error log
			exit(0);
		}
	}
};

#endif

