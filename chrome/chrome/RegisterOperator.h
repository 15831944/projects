#pragma once

#include <Windows.h>
#include <string>


/*************************************************
// <Summary></Summary>
������Ҫ��������ɶ���ע���ļ򵥲��������в�����Ȩ���밲ȫ���ж��������
���Ҫ�����ϸ�ĵط�����Ҫ���밲ȫ��Ȩ�޷���Ķ���
*************************************************/
namespace BaseClassLibrary
{
	class CRegisterOper
	{
	public:
		static bool GetRegValue(HKEY hRootKey, std::string strSubKey, std::string strValueNam, std::string &strvalue, DWORD &dwType);
		static bool SetRegValue(HKEY hRootKey, std::string strSubKey, std::string strValueNam, std::string strvalue, DWORD dwType);
	};
}