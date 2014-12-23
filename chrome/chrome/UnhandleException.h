#ifndef UNHANDLEEXCEPTION_H
#define UNHANDLEEXCEPTION_H

#include <Windows.h>
#include <string>
#pragma comment(lib, "dbghelp.lib")



/*************************************************
// <Summary>在程序崩溃时，打印出堆栈信息(转存文件)。
	1.打印地址默认与exe同一目录 
	2.默认是打印dump文件的功能是自动打开的
	3.定义了CUnhandleException类的全局变量
   </Summary>
// <DateTime>2014/12/23</DateTime>
*************************************************/
class CUnhandleException
{
public:
	CUnhandleException();
	~CUnhandleException();

public:
	void SetSavePath(std::string strSavePath);
	std::string GetSavePath(){return m_strDumpPath;}
	void OpenSaveDumpFun(bool bFlag = true){m_bIsSaveDumpFlag = bFlag;}
protected:
private:
	bool Init();
	bool GenerateDefSaveDumpPath();

	static long WriteMiniDump(LPEXCEPTION_POINTERS pException);
	static long CALLBACK StackTraceExceptionFilter(LPEXCEPTION_POINTERS pException);
	static long DumpMiniDump(HANDLE hFile, LPEXCEPTION_POINTERS pException);
private:


	static std::string m_strDumpPath;        //保存dump文件的路径
	static bool        m_bIsSaveDumpFlag;    //是否保存dump文件的标志
	static bool        m_bInitSuccess;       //初始化环境成功的标记
};

extern CUnhandleException g_UnhandleException;   //declaration



#endif //UNHANDLEEXCEPTION_H