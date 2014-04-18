#include "PPSLoggerExport_ex.h"
#include <string>
#include <Windows.h>
#include "CreatDir.h"
#include <time.h>

#define __VA_ARGS__ "logger"

using namespace std;

namespace logger
{
	bool PPSLoggerExport_Ex::start()
	{
		m_filemap.clear();
		m_savepath = "E:\\mylog";
		CreatDir(m_savepath);
		return true;
	}

	bool PPSLoggerExport_Ex::stop()
	{
		map<string,FILE*>::iterator it;
		for(it = m_filemap.begin();it!=m_filemap.end();it++)
		{
			fclose(it->second);
		}
		return true;
	}

	bool PPSLoggerExport_Ex::isExistFile(const char *pszFileName)
	{
		FILE *fp = fopen(pszFileName, "rb");
		if(fp == NULL)
			return false;
		fclose(fp);
		return true;
	}

	void PPSLoggerExport_Ex::SetPath(char* path)
	{
		m_savepath = "";
		m_savepath = path;
		CreatDir(m_savepath);
	}

    bool PPSLoggerExport_Ex::CanPrint(void){return true;}

    logger::log_level PPSLoggerExport_Ex::GetLogLevel(void)
    {return logger::_Error;}

	bool PPSLoggerExport_Ex::PrintA(const char* szFileName, logger::log_level emLevel, const char* lpszContent)
	{
		if(!szFileName || !szFileName[0]||!lpszContent||!lpszContent[0])
			return false;
		map<string,FILE*>::iterator it;
		it = m_filemap.find(szFileName);

		//std::string sContent;
		//
		////get system timer
		//time_t rawtime; 
		//struct tm * timeinfo; 
		//time ( &rawtime ); 
		//timeinfo = localtime ( &rawtime ); 
		//sContent += asctime(timeinfo); 
		//sContent += ":";
		//sContent += lpszContent;
		if (it != m_filemap.end())
		{
			fputs (lpszContent, it->second);
		}
		else
		{
			FILE    *fp;
			string strFilePath = m_savepath +"\\";
			strFilePath += szFileName;
			strFilePath +=".log";
			fp = _fsopen(strFilePath.c_str(), "a",_SH_DENYNO);
			m_filemap.insert(pair<string,FILE*>(strFilePath,fp));
			fputs (lpszContent, fp);
		}
		return true;
	}
}