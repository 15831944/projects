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

// <Summary>MiniDumpWriteDump�������õĻص�����������ָ����Щ��Ϣ��������minidump�ļ���
// ��ϸ�ο�msdn
// </Summary>
// <DateTime>2014-12-16</DateTime>
BOOL CALLBACK MyMiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput)
{
    BOOL bResult = FALSE;

    do 
    {
        if (NULL == pInput || NULL == pOutput)   // �������
        {
            break;
        }

        // ������Щ����Ӧ�ñ�����minidump�ļ���
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

// <Summary>����δ�����쳣���ִ�к���</Summary>
// <DateTime>2014-12-16</DateTime>
// <Parameter name="pExceptionInfo" type="IN"></Parameter>
// <Returns>�ο�msdn</Returns>
LONG WINAPI MyTopLevelExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    std::cout << "detect an unhandled exception." <<std::endl;
    // �����ļ��ں˶���
    string strFilePath = UnhandledException::GetDumpFileDir();  // �ļ�����·��
    if ("" == strFilePath)    // ���dump�ļ��ı���Ŀ¼Ϊ�գ���ô�����ļ������ڵ�ǰ�Ŀ�ִ��Ŀ¼��
    {
        strFilePath = PublicMethod::GetExePath();
        strFilePath += "\\";
    }
    strFilePath += UnhandledException::GetDumpFileName();       // ����dump�ļ���
    std::cout << "dump�ļ�·����" << strFilePath << std::endl;
    HANDLE hFile = CreateFileA(strFilePath.c_str(), GENERIC_ALL, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  // �����ļ�
    if ((NULL == hFile) || (INVALID_HANDLE_VALUE == hFile))
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // ��ȡ����dump�ļ���Ҫ����Ϣ
    HANDLE hProcess = GetCurrentProcess();      // ��ǰ���̾��
    DWORD dwProcessId = GetCurrentProcessId();  // ��ǰ����id
    DWORD dwThreadId = GetCurrentThreadId();    // ��ǰ�߳�id
    MINIDUMP_EXCEPTION_INFORMATION mei = {dwThreadId, pExceptionInfo, TRUE};    // minidump�쳣��Ϣ ���øò������Խ��������쳣��Ϣд�뵽dump�ļ���
    MINIDUMP_CALLBACK_INFORMATION mci = {MyMiniDumpCallback, NULL};             // minidump�ص�������Ϣ ͨ���ص�����ָ����Щ���ݰ�����dump�ļ���
    MiniDumpWriteDump(hProcess, dwProcessId, hFile, MiniDumpNormal, &mei, NULL, &mci);   // дminidump�ļ�
    CloseHandle(hFile);         // �ر��ļ��ں˶�����

    return EXCEPTION_EXECUTE_HANDLER;
}

// <Summary>�Ƿ��ڳ������ʱ ����dump�ļ�</Summary>
// <DateTime>2014-12-10</DateTime>
// <Parameter name="enable" type="IN">true-�ڳ������ʱ����dump�ļ������������ļ�</Parameter>
void UnhandledException::EnableProduceDumpFile(bool bEnable)
{
    LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter = bEnable ? MyTopLevelExceptionFilter : NULL;
    SetUnhandledExceptionFilter(lpTopLevelExceptionFilter);
}

// <Summary>�õ�dump�ļ���</Summary>
// <DateTime>2014-12-11</DateTime>
// <Returns>dump�ļ��� �ļ�����ʽΪ���û���_������_ʱ����.dmp</Returns>
string UnhandledException::GetDumpFileName()
{
    char szUserName[100] = {0};
    DWORD dwSize = 100;
    
    if (GetUserNameA(szUserName, &dwSize) != 0)   // ����û����ɹ�
    {
        m_strDumpFileName = szUserName;
        m_strDumpFileName += "_";
    }

    // ��ȡ����ʱ��
    SYSTEMTIME systemTime = {0};
    GetLocalTime(&systemTime);
    m_strDumpFileName = PublicMethod::StringCat(m_strDumpFileName, "%04u%02u%02u_%02u%02u%02u.dmp", 
        systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

    return m_strDumpFileName;
}
