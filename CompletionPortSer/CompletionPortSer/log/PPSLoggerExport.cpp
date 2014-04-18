// LogPrinter.cpp : Defines the exported functions for the DLL application.
//
#include "PPSLoggerExport.h"
#include "PPSLoggerExport_ex.h"


namespace logger
{
	static IPPSLogger *pPPSLoggerImpl = NULL;

	IPPSLogger* CreateInterface()
	{	
		if(pPPSLoggerImpl == NULL)
		{
			pPPSLoggerImpl = new PPSLoggerExport_Ex();
		}
		return pPPSLoggerImpl;
	}

	void DeleteInterface()
	{
		if(pPPSLoggerImpl)
		{
			delete pPPSLoggerImpl;
			pPPSLoggerImpl = NULL;
		}
	}
}