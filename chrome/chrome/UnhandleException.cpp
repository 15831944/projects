#include "UnhandleException.h"
#include <shlobj.h>
#include <shlwapi.h>
#pragma  comment(lib,"shlwapi.lib")
#include <DbgHelp.h>
#include <iostream>


CUnhandleException g_UnhandleException;

CUnhandleException::CUnhandleException()
: m_pTPEF(NULL)
{
	Init();
}

CUnhandleException::~CUnhandleException()
{

}

void CUnhandleException::Init()
{
	if(NULL == m_pTPEF)
		m_pTPEF = SetUnhandledExceptionFilter(StackTraceExceptionFilter);
}

long CALLBACK CUnhandleException::StackTraceExceptionFilter(LPEXCEPTION_POINTERS e)
{

	WriteMiniDump(e);

	return 1;
}

long CUnhandleException::WriteMiniDump(LPEXCEPTION_POINTERS e)
{
	std::string strWirDmpPath;
	bool bUseDefPath = true;
	char exePath[MAX_PATH] = {0};
	if(0 != GetModuleFileNameA(NULL, exePath, MAX_PATH))
	{
		std::string strExePath(exePath);
		std::string::size_type pos1 = strExePath.rfind("\\");
		if(std::string::npos != pos1)
		{
			strExePath = strExePath.substr(0, pos1 + 1) + "zhonghao.dmp";
			strWirDmpPath = strExePath;
			bUseDefPath = false;
		}
	}

	if(bUseDefPath)
	{
		char szDirPath[MAX_PATH] = {0};
		if( SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, szDirPath)) )
		{
			PathAppendA(szDirPath,"\zhonghao.dmp");
		}
		strWirDmpPath = szDirPath;
	}

	HANDLE hMiniDumpFile = CreateFileA(
		strWirDmpPath.c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	if (hMiniDumpFile != INVALID_HANDLE_VALUE)
	{
		DumpMiniDump(hMiniDumpFile, e);

		// Close file
		CloseHandle(hMiniDumpFile);
	}
	

	return 1;
}

long CUnhandleException::DumpMiniDump(HANDLE hFile, LPEXCEPTION_POINTERS e)
{
	if(hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = e;
		eInfo.ClientPointers = FALSE;

		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpNormal,//MiniDumpWithFullMemory,//MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpScanMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo | MiniDumpWithCodeSegs,
			e ? &eInfo : NULL,
			NULL,
			NULL);
	}

	return 0;
}