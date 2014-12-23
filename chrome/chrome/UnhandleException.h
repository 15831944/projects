#ifndef UNHANDLEEXCEPTION_H
#define UNHANDLEEXCEPTION_H

#include <Windows.h>
#include <string>
#pragma comment(lib, "dbghelp.lib")



/*************************************************
// <Summary>�ڳ������ʱ����ӡ����ջ��Ϣ(ת���ļ�)��
	1.��ӡ��ַĬ����exeͬһĿ¼ 
	2.Ĭ���Ǵ�ӡdump�ļ��Ĺ������Զ��򿪵�
	3.������CUnhandleException���ȫ�ֱ���
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


	static std::string m_strDumpPath;        //����dump�ļ���·��
	static bool        m_bIsSaveDumpFlag;    //�Ƿ񱣴�dump�ļ��ı�־
	static bool        m_bInitSuccess;       //��ʼ�������ɹ��ı��
};

extern CUnhandleException g_UnhandleException;   //declaration



#endif //UNHANDLEEXCEPTION_H