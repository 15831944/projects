#include <direct.h>
#include <io.h>
#include <Windows.h>
#include "LogImplementForWin.h"


bool CreatDir(std::string pszDir)
{
	int i = 0;

	int iRet = 0;
	int iLen = strlen(pszDir.c_str());


	if (pszDir[iLen - 1] != '\\')
	{
		pszDir += "\\";
	}
	iLen = strlen(pszDir.c_str());
	for (i = 0;i < iLen + 1 ;i ++)
	{
		if (pszDir[i] == '\\')
		{
			pszDir[i] = '\0';

			iRet = _access(pszDir.c_str(),0);  
			if (iRet != 0)
			{
				iRet = _mkdir(pszDir.c_str());

				if (iRet != 0)
				{
					return false;
				} 
			}
			pszDir[i] = '\\';
		}
	}
	return true;
}


bool GetTimePrefix(std::string &strTimePrefix)
{
	SYSTEMTIME localTime = {0};
	GetLocalTime(&localTime);

	char tmpBuf[64] = {0};
	sprintf_s(tmpBuf, 64, "%04d-%02d-%02d %02d:%02d:%02d.%03d", 
		      localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, 
			  localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);

	strTimePrefix = tmpBuf;

	return true;

}


bool GetDirName(std::string &strDirName)
{
	SYSTEMTIME localTime = {0};
	GetLocalTime(&localTime);

	char tmpBuf[64] = {0};
	sprintf_s(tmpBuf, 64, "%04d_%02d_%02d_%02d_%02d_%02d_%03d", 
		localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, 
		localTime.wMinute, localTime.wSecond, localTime.wMilliseconds);

	strDirName = tmpBuf;

	return true;

}


CLogImpleForWin::CLogImpleForWin()
: m_bStart(false)
, m_strDirPath("d:\\mylog")
{

}

CLogImpleForWin::~CLogImpleForWin()
{
	std::map<std::string, FILE *>::iterator iter =  m_mapFileNameToFileObj.begin();
	for(; iter != m_mapFileNameToFileObj.end(); ++iter)
	{
		if(iter->second)
			fclose(iter->second);
	}

	m_mapFileNameToFileObj.clear();
}

bool CLogImpleForWin::StartLog()
{
	char buf[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	std::string strFileName(buf);
	std::string::size_type posStart = strFileName.rfind("\\");
	std::string::size_type posEnd = strFileName.rfind(".");
	if(posStart != std::string::npos && posEnd != std::string::npos)
		strFileName = strFileName.substr(posStart + 1, posEnd - (posStart + 1) );
	else
		strFileName = "Template";

	std::string strDirName;
	GetDirName(strDirName);

	strDirName = strFileName + strDirName;

	std::string::size_type pos = m_strDirPath.rfind("\\");
	if(std::string::npos != pos && pos == m_strDirPath.size() - 1)
		m_strDirPath += strDirName;
	else
		m_strDirPath += "\\" + strDirName; 

	m_bStart = CreatDir(m_strDirPath);
	return m_bStart;
}

void CLogImpleForWin::StopLog()
{
	m_bStart = false;
}


bool CLogImpleForWin::PrintLog(const char* cszFileName, eLogLevel emLevel, const char *cszDataBuf)
{
	bool bResult = false;
	std::string strTime;
	do 
	{
		if(cszFileName == NULL || '\0' == cszFileName[0] || !m_bStart)
			break;

		GetTimePrefix(strTime);
		std::string strOut;
		switch(emLevel)
		{
		case _Info:
			strOut += "[Info]";
			break;
		case _Debug:
			strOut += "[Debug]";
			break;
		case _Warn:
			strOut += "[Warn]";
			break;
		case _Error:
			strOut += "[Error]";
			break;
		case _Fatal:
			strOut += "[Fatal]";
			break;
		}
		strOut += strTime + ": ";
		strOut += cszDataBuf;
		strOut += "\r\n";

		std::map<std::string, FILE *>::iterator iter = m_mapFileNameToFileObj.find(cszFileName);
		if(m_mapFileNameToFileObj.end() != iter)
			fputs(strOut.c_str(), iter->second);
		else
		{
			std::string absPath;
			std::string::size_type pos = m_strDirPath.rfind('\\');
			if(pos == std::string::npos || pos == m_strDirPath.size() - 1)
				absPath = m_strDirPath + cszFileName + ".log";
			else
				absPath = m_strDirPath + '\\' + cszFileName + ".log";
			FILE *fp = fopen(absPath.c_str(), "ab+");
			if(!fp)
				break;

			m_mapFileNameToFileObj.insert(std::make_pair(cszFileName, fp));
			fputs(strOut.c_str(), fp);
		}

		bResult = true;
	} while (false);

	return bResult;
}


