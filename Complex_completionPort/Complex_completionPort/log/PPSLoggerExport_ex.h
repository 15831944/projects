#ifndef _PPSLOGGEREXPORT_ANDROID_H
#define _PPSLOGGEREXPORT_ANDROID_H
#include "PPSLoggerExport.h"
#include <stdio.h>
#include <string>
#include <map>

using namespace std;

namespace logger
{
	class PPSLoggerExport_Ex:public logger::IPPSLogger
	{
private:
		map<string,FILE*> m_filemap;
		string m_savepath;

		bool isExistFile(const char *pszFileName);

public:
	PPSLoggerExport_Ex(){};
	~PPSLoggerExport_Ex(){};

		bool start(void);

		bool stop(void);
		bool CanPrint(void);
		logger::log_level GetLogLevel(void);
		void SetPath(char* path);
		bool configure(const logger::log_param& param){return true;};

		bool PrintA(const char* szFileName, logger::log_level emLevel, const char* lpszContent);
		
		IPPSLogger* CreateInterface(void);
		void DeleteInterface(void);

	};
}
#endif