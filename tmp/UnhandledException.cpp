#include <Windows.h>
#include <DbgHelp.h>
#include "UnHandledException.h"
#include "../utility/PublicMethod.h"
#include <iostream>

#pragma comment(lib, "DbgHelp.lib")

using namespace why::win;
using namespace why::utility;
using std::string;

string UnhandledException::m_strDumpFileDir;
string UnhandledException::m_strDumpFileName;

// <Summary>MiniDumpWriteDump函数设置的回调函数，用于指定哪些信息被包含在minidump文件中
// 详细参考msdn
// </Summary>
// <DateTime>2014-12-16</DateTime>
BOOL CALLBACK MyMiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
    BOOL bResult = FALSE;

    do 
    {
        if (NULL == pInput || NULL == pOutput)   // 参数检测
        {
            break;
        }

        // 决定哪些数据应该保存在minidump文件中
        switch (pInput->CallbackType)            
        {
        case IncludeModuleCallback:     
        case IncludeThreadCallback:     
        case ModuleCallback:           
        case ThreadCallback:
        case ThreadExCallback:
        case MemoryCallback:
            bResult = TRUE;
        	break;
        }
    } while (false);

    return bResult;
}

// <Summary>发生未捕获异常后的执行函数</Summary>
// <DateTime>2014-12-16</DateTime>
// <Parameter name="pExceptionInfo" type="IN"></Parameter>
// <Returns>参考msdn</Returns>
LONG WINAPI MyTopLevelExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    std::cout << "detect an unhandled exception." <<std::endl;
    // 创建文件内核对象
    string strFilePath = UnhandledException::GetDumpFileDir();  // 文件保存路径
    if ("" == strFilePath)    // 如果dump文件的保存目录为空，那么将该文件保存在当前的可执行目录下
    {
        strFilePath = PublicMethod::GetExePath();
        strFilePath += "\\";
    }
    strFilePath += UnhandledException::GetDumpFileName();       // 加上dump文件名
    std::cout << "dump文件路径：" << strFilePath << std::endl;
    HANDLE hFile = CreateFileA(strFilePath.c_str(), GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  // 创建文件
    if ((NULL == hFile) || (INVALID_HANDLE_VALUE == hFile))
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // 获取创建dump文件需要的信息
    HANDLE hProcess = GetCurrentProcess();      // 当前进程句柄
    DWORD dwProcessId = GetCurrentProcessId();  // 当前进程id
    DWORD dwThreadId = GetCurrentThreadId();    // 当前线程id
    MINIDUMP_EXCEPTION_INFORMATION mei = {dwThreadId, pExceptionInfo, TRUE};    // minidump异常信息 设置该参数可以将崩溃的异常信息写入到dump文件中
    MINIDUMP_CALLBACK_INFORMATION mci = {MyMiniDumpCallback, NULL};             // minidump回调函数信息 通过回调函数指定哪些内容包含在dump文件中
    MiniDumpWriteDump(hProcess, dwProcessId, hFile, MiniDumpNormal, &mei, NULL, &mci);   // 写minidump文件
    CloseHandle(hFile);         // 关闭文件内核对象句柄

    return EXCEPTION_EXECUTE_HANDLER;
}

// <Summary>是否在程序崩溃时 生成dump文件</Summary>
// <DateTime>2014-12-10</DateTime>
// <Parameter name="enable" type="IN">true-在程序崩溃时生成dump文件，否则生成文件</Parameter>
void UnhandledException::EnableProduceDumpFile(bool bEnable)
{
    LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter = bEnable ? MyTopLevelExceptionFilter : NULL;
    SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
}

// <Summary>得到dump文件名</Summary>
// <DateTime>2014-12-11</DateTime>
// <Returns>dump文件名 文件名格式为：用户名_年月日_时分秒.dmp</Returns>
string UnhandledException::GetDumpFileName()
{
    char szUserName[100] = {0};
    DWORD dwSize = 100;
    
    if (GetUserNameA(szUserName, &dwSize) != 0)   // 获得用户名成功
    {
        m_strDumpFileName = szUserName;
        m_strDumpFileName += "_";
    }

    // 获取本地时间
    SYSTEMTIME systemTime = {0};
    GetLocalTime(&systemTime);
    m_strDumpFileName = PublicMethod::StringCat(m_strDumpFileName, "%04u%02u%02u_%02u%02u%02u.dmp", 
        systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

    return m_strDumpFileName;
}
