#include "PublicMethod.h"
#include <stdarg.h>

#pragma message("automatic link to VERSION.LIB")
#pragma comment(lib, "version.lib")


/*************************************************
// <Summary>�ַ���ƴ�Ӳ���</Summary>
// <DateTime>2013/9/12</DateTime>
// <Parameter name="str" type="INOUT">ԭ�ַ���</Parameter>
// <Parameter name="formatString" type="IN">��ʽ���ַ�����ʽ</Parameter>
// <Returns>���Ӻ���ַ��� ���ӷ���ֵ����ֱ��ʹ�øú������б��ʽ����</Returns>
*************************************************/
std::string& CStringMethod::StringCat(std::string &str, const char *formatString, ...)
{
	char buf[2048] = {0};
	va_list args;
	va_start(args, formatString);
	int result = vsnprintf_s(buf, sizeof(buf), 2048, formatString, args);
	if(result < 0)
		return str;
	str.append(buf);
	va_end(args);
	return str;
}

/*************************************************
// <Summary>��ʽ���ַ���</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="INOUT">ԭ�ַ���</Parameter>
// <Parameter name="formatString" type="IN">��ʽ���ַ�����ʽ</Parameter>
// <Returns>��ʽ������ַ��� ���ӷ���ֵ����ֱ��ʹ�øú������б��ʽ����</Returns>
*************************************************/
std::string& CStringMethod::StringFormat(std::string &srcString, const char *pFormatString, ...)
{
	srcString = "";
	va_list args;
	char buf[2048] = {0};
	va_start(args, pFormatString);
	int result = vsnprintf_s(buf, sizeof(buf), 2048, pFormatString, args);
	if (result < 0)
		srcString = "";
	else
		srcString = buf;

	return srcString;
}


/*************************************************
// <Summary>�ַ�����ü�����</Summary>
// <DateTime>2015/01/12</DateTime>
// <Parameter name="str" type="IN/OUT">ԭʼ�ַ���</Parameter>
// <Parameter name="ch" type="IN">�ü����ַ�</Parameter>
// <Returns>�ü�����ַ���</Returns>
*************************************************/
std::string& CStringMethod::StringLeftTrim(std::string &str, const char ch /* = ' ' */)
{
	std::string::iterator it = str.begin();
	for(; it != str.end(); ++it)
	{
		if (*it != ch)
			break;
	}

	if(it == str.end())
		return str;

	return str.assign(it, str.end());
}


/*************************************************
// <Summary>�ַ����Ҳü�����</Summary>
// <DateTime>2015/01/12</DateTime>
// <Parameter name="str" type="IN/OUT">ԭʼ�ַ���</Parameter>
// <Parameter name="ch" type="IN">�ü����ַ�</Parameter>
// <Returns>�ü�����ַ���</Returns>
*************************************************/
std::string& CStringMethod::StringRightTrim(std::string &str, const char ch /* = ' ' */)
{
	std::string::reverse_iterator it = str.rbegin();
	unsigned int count = 0;
	for(; it != str.rend(); ++it)
	{
		if (*it != ch)
			break;
		++count;
	}

	if(it == str.rend())
		return str;

	return str.assign(str, 0, str.size() - count);
}


/*************************************************
// <Summary>�ַ����ü�����</Summary>
// <DateTime>2015/01/12</DateTime>
// <Parameter name="str" type="IN/OUT">ԭʼ�ַ���</Parameter>
// <Parameter name="ch" type="IN">�ü����ַ�</Parameter>
// <Returns>�ü�����ַ���</Returns>
*************************************************/
std::string& CStringMethod::StringTrim(std::string &str, const char ch/* = ' '*/)
{
	StringLeftTrim(str, ch);
	StringRightTrim(str, ch);
	return str;
}

//const std::string& CStringMethod::StringMakeUpper(std::string &str)
//{
//	return str;
//}
//const std::string& CStringMethod::StringMakeLower(std::string &str)
//{
//	return str;
//}

/*************************************************
// <Summary>�ж��ַ����е��ַ��Ƿ�ȫ��Ϊ����</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">ԭ��</Parameter>
// <Returns>ȫΪ���ַ���true</Returns>
*************************************************/
bool CStringMethod::IsAllNum(const std::string &str)
{
	bool bResult = false;
	do 
	{
		std::string::const_iterator it = str.begin();
		for( ; it != str.end(); ++it )
		{
			if( !isdigit(*it) )
				break;
		}

		if( it != str.end() )
			break;

		bResult = true;
	} while (false);

	return bResult;
}

/*************************************************
// <Summary>�ж��ַ����е��ַ��Ƿ�ȫ��Ϊ��ĸ</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">ԭ��</Parameter>
// <Returns>ȫΪ��ĸ����true</Returns>
*************************************************/
bool CStringMethod::IsAllAlpha(const std::string &str)
{
	bool bResult = false;
	do 
	{
		std::string::const_iterator it = str.begin();
		for( ; it != str.end(); ++it )
		{
			if( !isalpha(*it) )
				break;
		}

		if ( it != str.end() )
			break;

		bResult = true;
	} while (false);

	return bResult;
}

/*************************************************
// <Summary>��ָ�����ַ����ָ����봮</Summary>
// <DateTime> 2015/09/24 </DateTime>
// <Parameter name="strInput" type="IN">ԭ�ַ���</Parameter>
// <Parameter name="spliter" type="IN">�ָ��ַ���</Parameter>
// <Parameter name="vecResult" type="OUT">������</Parameter>
// <Returns>void</Returns>
// @author  zhonghao
*************************************************/
void CStringMethod::StringSplite(const std::string &strInput, const std::string &spliter, std::vector<std::string> &vecResult)
{
	std::string::size_type unLastPos = 0;
	std::string::size_type unSpliterSize = spliter.size();

	while(unLastPos < strInput.length())
	{
		std::string::size_type pos = strInput.find(spliter, unLastPos);
		if(std::string::npos == pos)
		{
			vecResult.push_back(strInput.substr(unLastPos));
			return;
		}

		vecResult.push_back(strInput.substr(unLastPos, pos - unLastPos));
		unLastPos = pos + unSpliterSize;
	}
}

/*************************************************
// <Summary>�ж��ַ����е��ַ��Ƿ�ȫ��Ϊ��д</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">ԭ��</Parameter>
// <Returns>��ĸȫΪ��д����true</Returns>
*************************************************/
bool CStringMethod::IsAllUpper(const std::string &str)
{
	bool bResult = false;
	do 
	{
		std::string::const_iterator it = str.begin();
		for( ; it != str.end(); ++it )
		{
			if( !isupper(*it) )
				break;
		}

		if ( it != str.end() )
			break;

		bResult = true;
	} while (false);

	return bResult;
}

/*************************************************
// <Summary>�ж��ַ����е��ַ��Ƿ�ȫ��ΪСд</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">ԭ��</Parameter>
// <Returns>��ĸȫΪСд����true</Returns>
*************************************************/
bool CStringMethod::IsAllLower(const std::string &str)
{
	bool bResult = false;
	do 
	{
		std::string::const_iterator it = str.begin();
		for( ; it != str.end(); ++it )
		{
			if( !islower(*it) )
				break;
		}

		if ( it != str.end() )
			break;

		bResult = true;
	} while (false);

	return bResult;
}

/*************************************************
// <Summary>��ȡ��ǰexe�ļ���Ŀ¼ ����������һ��'\'��</Summary>
// <DateTime>2015/01/20</DateTime>
// <Returns>���ص�ǰexe�ļ���Ŀ¼ ����������һ��'\'��</Returns>
*************************************************/
std::string CExeRef::GetCurrentExeDir()
{
	std::string exeDir;
	char moduleName[MAX_PATH] = {0};
	unsigned long dwLen = GetModuleFileNameA(NULL, moduleName, MAX_PATH);
	if(dwLen > 0)
	{
		std::string moduleExe(moduleName);
		std::string::size_type pos1 = moduleExe.rfind('\\');
		if(pos1 != std::string::npos)
			exeDir = moduleExe.substr(0, pos1 + 1);
	}

	return exeDir;
}


/*************************************************
// <Summary>��ȡָ���ļ��İ汾��</Summary>
// <DateTime>2015/01/20</DateTime>
// <Parameter name="strExeFileName" type="IN">�ļ���ȫ·��</Parameter>
// <Parameter name="arrayVer" type="IN/OUT">��¼���صİ汾�ŵ�����</Parameter>
// <Parameter name="arraySize" type="IN">����ĳ���</Parameter>
// <Returns>��ȡ�ɹ�����ture, ���򷵻�false</Returns>
*************************************************/
bool CExeRef::GetCurrentExeVer(const std::string& strExeFileName, unsigned long *arrayVer, unsigned arraySize)
{
	bool bResult = false;
	unsigned long deHandle;
	unsigned long dwSize = 0;
	do 
	{
		dwSize = GetFileVersionInfoSizeA(strExeFileName.c_str(), &deHandle);
		if(0 == dwSize)
			break;

		char *pData = new(std::nothrow) char[dwSize + 1];
		if(NULL == pData)
			break;
		memset(pData, 0x0, dwSize + 1);

		if (FALSE == GetFileVersionInfoA(strExeFileName.c_str(), deHandle, dwSize, pData))
		{
			if(pData)
				delete[] pData;
			break;
		}

		VS_FIXEDFILEINFO FixedInfo;


		UINT nLength;
		VS_FIXEDFILEINFO *pFixedInfo = NULL;

	    if(FALSE == VerQueryValueA(pData, "\\", (void **) &pFixedInfo, &nLength))
		{
			if(pData)
				delete[] pData;
			break;
		}

		memcpy(&FixedInfo, pFixedInfo, sizeof(FixedInfo));

		if(!arraySize || arraySize < 4)
			break;

		arrayVer[0] = HIWORD(FixedInfo.dwFileVersionMS);
		arrayVer[1] = LOWORD(FixedInfo.dwFileVersionMS);
		arrayVer[2] = HIWORD(FixedInfo.dwFileVersionLS);
		arrayVer[3] = LOWORD(FixedInfo.dwFileVersionLS);
		bResult = true;
		if(pData)
			delete[] pData;
	} while (false);    

	return bResult;
}

