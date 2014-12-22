#ifndef UNHANDLEEXCEPTION_H
#define UNHANDLEEXCEPTION_H

#pragma comment(lib, "dbghelp.lib")
#include <Windows.h>
#include <string>
//#include <imagehlp.h>


class CUnhandleException
{
public:
	CUnhandleException();
	~CUnhandleException();

protected:
private:
	void Init();

	static long WriteMiniDump(LPEXCEPTION_POINTERS e);
	static long CALLBACK StackTraceExceptionFilter(LPEXCEPTION_POINTERS e);
	static long DumpMiniDump(HANDLE hFile, LPEXCEPTION_POINTERS e);
private:
	LPTOP_LEVEL_EXCEPTION_FILTER m_pTPEF;
};





#endif //UNHANDLEEXCEPTION_H