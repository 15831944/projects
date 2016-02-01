#ifndef LOGIMPLEMENTFORWIN_H
#define LOGIMPLEMENTFORWIN_H
#include <stdio.h>
#include <map>
#include <string>

#include "ILogImplement.h"

class CLogImpleForWin
: public ILogImplement 
{
public:

	CLogImpleForWin();
	~CLogImpleForWin();


	virtual bool StartLog();
	virtual void StopLog();
	virtual void SetLogDirPath(const char *cszDirPath){m_strDirPath = cszDirPath;}
	virtual bool PrintLog(const char* cszFileName, eLogLevel emLevel, const char *cszDataBuf);

private:
	std::map<std::string, FILE *> m_mapFileNameToFileObj;
	std::string                   m_strDirPath;
	bool                          m_bStart;
};
#endif  //LOGIMPLEMENTFORWIN_H