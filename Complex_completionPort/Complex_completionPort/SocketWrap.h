#ifndef COMPLEXCOMPLETIONPORT_SOCKETWRAP
#define COMPLEXCOMPLETIONPORT_SOCKETWRAP

#ifdef WIN32
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>

#include "log/InterfaceLogger.h"
#define SOCKETWRAPLOGNAME  "socketwrap"
#else            //linux

#define  SOCKET unsigned int
#define  INVALID_SOCKET  -1
#define  SOCKET_ERROR    -1
#endif  //WIN32


SOCKET Socket(int af, int type, int protocol)
{
	
	SOCKET sock;
	if((sock = socket(af, type, protocol)) == INVALID_SOCKET)
	{
#ifdef WIN32
		DWORD error_code = GetLastError();
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(SOCKETWRAPLOGNAME, "create socket is error !!! error_code = %d\n", error_code);
		assert(false);
#else            //linux

#endif  //WIN32

	}	

	return sock;
}

int Bind(SOCKET s, const struct sockaddr *name, int nameLen)
{
	if(SOCKET_ERROR == bind(s, name, nameLen))
	{
#ifdef WIN32
		DWORD error_code = GetLastError();
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(SOCKETWRAPLOGNAME, "bind socket is error !!! error_code = %d\n", error_code);
		assert(false);
#else            //linux

#endif  //WIN32

	}

	return 0;  //success
}

int Listen(SOCKET s, int backlog)
{
	if(-1 == listen(s, backlog))
	{
#ifdef WIN32
		DWORD error_code = GetLastError();
		if(logger::CLogger::CanPrint())
			logger::CLogger::PrintA(SOCKETWRAPLOGNAME, "listen socket is error !!! error_code = %d\n", error_code);
		assert(false);
#else            //linux

#endif  //WIN32

	}

	return 0; //success
}


#endif  //COMPLEXCOMPLETIONPORT_SOCKETWRAP
