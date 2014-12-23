#include "UnhandleException.h"
#pragma  comment(lib,"shlwapi.lib")
#include <DbgHelp.h>
#include <iostream>
#include "PublicMethod.h"

//static variable define
std::string CUnhandleException::m_strDumpPath;
bool        CUnhandleException::m_bIsSaveDumpFlag = true;
bool		CUnhandleException::m_bInitSuccess = false;

//global variable define
CUnhandleException g_UnhandleException;  

//CUnhandleException class funcation define
CUnhandleException::CUnhandleException()
{
	m_bInitSuccess = Init();
}

CUnhandleException::~CUnhandleException()
{

}

bool CUnhandleException::GenerateDefSaveDumpPath()
{
	bool bResult = false;
	do 
	{
		char path[MAX_PATH] = {0};
		if( 0 == GetModuleFileNameA(NULL, path, MAX_PATH) )
			break;

		char *pos = strrchr(path, '\\');
		if( NULL == pos )
			break;

		m_strDumpPath.insert(0, path, pos - path + 1);   //有'\' 结尾的存储目录
		SYSTEMTIME ST = {0}; 
		GetLocalTime(&ST);
		CStringMethod::StringCat(m_strDumpPath, "%04u%02u%02u_%02u%02u%02u%03u.dmp", ST.wYear, ST.wMonth, ST.wDay, ST.wHour, ST.wMinute, ST.wSecond, ST.wMilliseconds);

		bResult = true;
	} while (false);

	return bResult;
}

bool CUnhandleException::Init()
{
	bool bResult = false;
	do 
	{
		if( false == GenerateDefSaveDumpPath() )
			break;

		SetUnhandledExceptionFilter(StackTraceExceptionFilter);   //使应用程序能够取代每个线程的顶级异常处理程序和过程

		bResult = true;
	} while (false);

	return bResult;
}

long CALLBACK CUnhandleException::StackTraceExceptionFilter(LPEXCEPTION_POINTERS pException)
{
	if (NULL != pException && m_bInitSuccess && m_bIsSaveDumpFlag)
	{
		return WriteMiniDump(pException);
	}

	return 0;
}

long CUnhandleException::WriteMiniDump(LPEXCEPTION_POINTERS pException)
{
	HANDLE hMiniDumpFile = CreateFileA(
		m_strDumpPath.c_str(),
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	if (hMiniDumpFile != INVALID_HANDLE_VALUE)
	{
		DumpMiniDump(hMiniDumpFile, pException);
		CloseHandle(hMiniDumpFile);
	}
	
	return 0;
}

long CUnhandleException::DumpMiniDump(HANDLE hFile, LPEXCEPTION_POINTERS pException)
{
	if(hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = pException;
		eInfo.ClientPointers = FALSE;
        
		//写dump文件
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpNormal,//MiniDumpWithFullMemory,//MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpScanMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo | MiniDumpWithCodeSegs,
			pException ? &eInfo : NULL,
			NULL,
			NULL);
	}

	return 0;
}