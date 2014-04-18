#ifndef COMPLETIONPORTSER_IOCPSERSIMPLE_HEAD
#define COMPLETIONPORTSER_IOCPSERSIMPLE_HEAD

#include "DataStruct.h"
#include "./log/InterfaceLogger.h"

#include <WinSock2.h>
#include <Windows.h>

#define LISTENPORT 4567
#define LOGFILENAME "IOCPSer_Simple"

class CIOCPSerSimple
{
public:
	CIOCPSerSimple();
	~CIOCPSerSimple();

	bool StartSer();         //start server
	void StopSer();          //stop server

private:

	//work thread
	static unsigned int _stdcall WorkThread(LPVOID param);
	unsigned int _stdcall WorkThread();

private:
	HANDLE m_hworkThread;
	HANDLE m_hcompletion;
};

#endif



