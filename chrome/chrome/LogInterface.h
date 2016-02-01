#ifndef LOGINTERFACE_SERVER_H
#define LOGINTERFACE_SERVER_H

#include "basictypes.h"
#include "ThreadLock.h"
#include "ILogImplement.h"

class CLogInter
{
public:
	static bool StartLog(const char *cszDirPath = "d:\\MyLog");
	static void StopLog();

	static void SetLogLevel(eLogLevel eLogLev = _Debug){m_eLogLevel = eLogLev;}
	
	static bool CanPrintLog();
	static bool PrintLog(const char* cszFileName, eLogLevel emLevel, const char* cszFormat, ...);

private:
	static ILogImplement *m_pLogImple; 
	static CCriSecLock   m_lock;
	static eLogLevel     m_eLogLevel;
};




#endif  //LOGINTERFACE_SERVER_H