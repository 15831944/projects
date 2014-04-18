#include <stdarg.h>
#include "InterfaceLogger.h"
#include "PPSLoggerExport.h"


namespace logger
{
	IPPSLogger* CLogger::m_pPpsLogger = NULL;
	bool CLogger::Start(void)
	{
		bool bRet = false;
		if(m_pPpsLogger == NULL)
		{
			m_pPpsLogger = CreateInterface();
		}
		if (m_pPpsLogger)
		{
			bRet = m_pPpsLogger->start();
		}
		return bRet;
	}
	void CLogger::Stop(void)
	{
		if(m_pPpsLogger)
		{
			if(m_pPpsLogger->stop())
			{
				m_pPpsLogger = NULL;
				DeleteInterface();
			}
		}
	}

	void CLogger::SetPath(char* path)
	{
		m_pPpsLogger->SetPath(path);
	}
	bool CLogger::CanPrint(void)
	{
		bool bRet = false;
		if(m_pPpsLogger)
			bRet = m_pPpsLogger->CanPrint();
		return bRet;
	}
	bool CLogger::Configure(const log_param& param)
	{
		bool bRet = false;

		do 
		{
			//只有输出到文件类型时才检验文件名字符串	xuxiaoyui 2012-5-28
			if (param.dwLogPrintType == PRINT_TO_FILE && (!param.szFileName[0] || !param.szKeyName[0]))
				break;

			if (param.optName <= 0 || param.optVal[0] <=0 || param.optLen <=0)
				break;

			if (m_pPpsLogger)
			{
				bRet = m_pPpsLogger->configure(param);
			}
		} while (false);

		return bRet;
	}

	bool CLogger::PrintA(const char* szFileName, log_level emLevel, const char* lpszFormat, ...)
	{
		bool bRet = false;
		do 
		{
			if (!szFileName || !szFileName[0] || !lpszFormat || !lpszFormat[0])
				break;
			if (m_pPpsLogger)
			{
				if(m_pPpsLogger->CanPrint() == false)
					break;
				if(emLevel < m_pPpsLogger->GetLogLevel())
					break;

				int nBuf;
				char szBuffer[32*1024];

				va_list args;
				va_start(args, lpszFormat);
#ifdef WIN32
				nBuf = _vsnprintf_s(szBuffer,sizeof(szBuffer)/sizeof(char),sizeof(szBuffer)/sizeof(char),lpszFormat,args);
#else
				nBuf = vsnprintf(szBuffer,sizeof(szBuffer)/sizeof(char),lpszFormat,args);
#endif
				assert(nBuf < sizeof(szBuffer));
				if(-1 == nBuf)
					break;
				szBuffer[nBuf] = 0;
				va_end(args);
#ifdef WIN32
				string strUTF8 = szBuffer;
				bRet = m_pPpsLogger->PrintA(szFileName,emLevel,szBuffer);
#else
				bRet = m_pPpsLogger->PrintA(szFileName,emLevel,szBuffer);
#endif
			}
		} while (false);

		return bRet;
	}

	 bool CLogger::PrintA(const char* szFileName, const char* lpszFormat, ...)
	{
		bool bRet = false;

		do 
		{
			if (!szFileName || !szFileName[0] || !lpszFormat || !lpszFormat[0])
				break;
			if (m_pPpsLogger)
			{
				if(m_pPpsLogger->CanPrint() == false)
					break;

				int nBuf;
				char szBuffer[32*1024];

				va_list args;
				va_start(args, lpszFormat);
#ifdef WIN32
				nBuf = _vsnprintf_s(szBuffer,sizeof(szBuffer)/sizeof(char),sizeof(szBuffer)/sizeof(char),lpszFormat,args);
#else
				nBuf = vsnprintf(szBuffer,sizeof(szBuffer)/sizeof(char),lpszFormat,args);
#endif

				assert(nBuf < sizeof(szBuffer));
				if(-1 == nBuf)
					break;
				szBuffer[nBuf] = 0;
				va_end(args);

				string strUTF8 = szBuffer;
				bRet = m_pPpsLogger->PrintA(szFileName,_Debug,szBuffer);
			}
		} while (false);

		return bRet;
	}

}
