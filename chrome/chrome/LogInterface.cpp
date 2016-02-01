#include <stdio.h>
#include <stdarg.h>

#include "LogInterface.h"
#if defined(OS_WIN)
#include "LogImplementForWin.h"
#endif

ILogImplement*  CLogInter::m_pLogImple  = NULL; 
CCriSecLock     CLogInter::m_lock;
eLogLevel       CLogInter::m_eLogLevel = _Info;

bool CLogInter::StartLog(const char *cszDirPath/* = "d:\\MyLog"*/)
{
	if(NULL == m_pLogImple)
	{
		CScopeLock guardLock(&m_lock);
		if(NULL == m_pLogImple)
		{
#if defined(OS_WIN)
			m_pLogImple = new CLogImpleForWin;
#endif

			m_pLogImple->SetLogDirPath(cszDirPath);
			m_pLogImple->StartLog();
			
		}
	}

	return true;
}

void CLogInter::StopLog()
{
	if(m_pLogImple)
	{
		CScopeLock guardLock(&m_lock);
		if(m_pLogImple)
		{
			m_pLogImple->StopLog();
#if defined(OS_WIN)
			delete m_pLogImple;
#endif	
		}
	}
}

bool CLogInter::CanPrintLog()
{
	return m_pLogImple != NULL;
}

bool CLogInter::PrintLog(const char* cszFileName, eLogLevel emLevel, const char* cszFormat, ...)
{
	bool bResult = false;
	do 
	{
		if(NULL == cszFileName || '\0' == cszFileName[0] || NULL == cszFormat || '\0' == cszFormat[0])
			break;
		if(!CanPrintLog())
			break;

		if(emLevel < m_eLogLevel)
			break;

		va_list args;
		va_start(args, cszFormat);

		char szBuffer[32 * 1024] = {0};
		int nBuf = 0;

#if defined(OS_WIN)
		nBuf = _vsnprintf_s(szBuffer, sizeof(szBuffer)/sizeof(char), sizeof(szBuffer)/sizeof(char), cszFormat, args);
#else
		nBuf = vsnprintf(szBuffer, sizeof(szBuffer)/sizeof(char), cszFormat, args);
#endif

		if(-1 == nBuf)
			break;

		va_end(args);

		bResult = m_pLogImple->PrintLog(cszFileName, emLevel, szBuffer);
	} while (false);

	return bResult;
}







