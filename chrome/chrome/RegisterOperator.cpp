#include "RegisterOperator.h"
#include "PublicMethod.h"

namespace BaseClassLibrary
{
	bool CRegisterOper::GetRegValue(HKEY hRootKey, std::string strSubKey, std::string strValueNam, std::string &strvalue, DWORD &dwType)
	{
		bool bResult = false;
		HKEY key;
		do 
		{

			if ( ERROR_SUCCESS !=  RegOpenKeyA(hRootKey, strSubKey.c_str(),&key) )
				break;

			char  buffer[256] = {0};
			DWORD dwLen = 256;
			if(  ERROR_SUCCESS != RegQueryValueExA(key, strValueNam.c_str(), 0, &dwType, (BYTE *)buffer, &dwLen) )
				break;

			if( REG_DWORD == dwType )
			{
				DWORD dwValue = *(reinterpret_cast<DWORD *>(buffer));
				CStringMethod::StringFormat(strvalue, "%u", dwValue);
			}
			else if( REG_EXPAND_SZ == dwType || REG_SZ == dwType)
			{
				strvalue = buffer;
			}

			bResult = true;
		} while (false);

		RegCloseKey(key);

		return bResult;
	}

	bool CRegisterOper::SetRegValue(HKEY hRootKey, std::string strSubKey, std::string strValueNam, std::string strvalue, DWORD dwType)
	{
		bool bResult = false;
		HKEY key;
		do 
		{
			if( ERROR_SUCCESS != RegOpenKeyA(hRootKey, strSubKey.c_str(), &key) )
			{
				if( ERROR_SUCCESS != RegCreateKeyA(hRootKey, strSubKey.c_str(), &key))
					break;
			}


			if( REG_DWORD == dwType )
			{
				unsigned long dwValut = static_cast<unsigned long>(atol(strvalue .c_str()));
				if( ERROR_SUCCESS != RegSetValueExA(key, strValueNam.c_str(), 0, dwType, (BYTE *)&dwValut, sizeof(unsigned long)) )
					break;
			}

			if( REG_EXPAND_SZ == dwType || REG_SZ == dwType)
			{
				if( ERROR_SUCCESS != RegSetValueExA(key, strValueNam.c_str(), 0, dwType, (BYTE *)strvalue.c_str(), strvalue.length()) )
					break;

			}

			bResult = true;
		} while (false);

		RegCloseKey(key);
		return bResult;
	}
}