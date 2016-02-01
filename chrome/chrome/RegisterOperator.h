#pragma once

#include <Windows.h>
#include <string>


/*************************************************
// <Summary></Summary>
此类主要是用来完成对于注册表的简单操作，其中不包括权限与安全的判断与操作，
如果要用在严格的地方，需要加入安全与权限方面的东东
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