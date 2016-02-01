#include "PublicMethod.h"
#include <stdarg.h>

#pragma message("automatic link to VERSION.LIB")
#pragma comment(lib, "version.lib")


/*************************************************
// <Summary>字符串拼接操作</Summary>
// <DateTime>2013/9/12</DateTime>
// <Parameter name="str" type="INOUT">原字符串</Parameter>
// <Parameter name="formatString" type="IN">格式化字符串格式</Parameter>
// <Returns>连接后的字符串 增加返回值便于直接使用该函数进行表达式运算</Returns>
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
// <Summary>格式化字符串</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="INOUT">原字符串</Parameter>
// <Parameter name="formatString" type="IN">格式化字符串格式</Parameter>
// <Returns>格式化后的字符串 增加返回值便于直接使用该函数进行表达式运算</Returns>
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
// <Summary>字符串左裁剪操作</Summary>
// <DateTime>2015/01/12</DateTime>
// <Parameter name="str" type="IN/OUT">原始字符串</Parameter>
// <Parameter name="ch" type="IN">裁剪的字符</Parameter>
// <Returns>裁剪后的字符串</Returns>
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
// <Summary>字符串右裁剪操作</Summary>
// <DateTime>2015/01/12</DateTime>
// <Parameter name="str" type="IN/OUT">原始字符串</Parameter>
// <Parameter name="ch" type="IN">裁剪的字符</Parameter>
// <Returns>裁剪后的字符串</Returns>
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
// <Summary>字符串裁剪操作</Summary>
// <DateTime>2015/01/12</DateTime>
// <Parameter name="str" type="IN/OUT">原始字符串</Parameter>
// <Parameter name="ch" type="IN">裁剪的字符</Parameter>
// <Returns>裁剪后的字符串</Returns>
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
// <Summary>判断字符串中的字符是否全部为数字</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">原串</Parameter>
// <Returns>全为数字返回true</Returns>
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
// <Summary>判断字符串中的字符是否全部为字母</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">原串</Parameter>
// <Returns>全为字母返回true</Returns>
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
// <Summary>用指定的字符串分割输入串</Summary>
// <DateTime> 2015/09/24 </DateTime>
// <Parameter name="strInput" type="IN">原字符串</Parameter>
// <Parameter name="spliter" type="IN">分割字符串</Parameter>
// <Parameter name="vecResult" type="OUT">结果输出</Parameter>
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
// <Summary>判断字符串中的字符是否全部为大写</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">原串</Parameter>
// <Returns>字母全为大写返回true</Returns>
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
// <Summary>判断字符串中的字符是否全部为小写</Summary>
// <DateTime>2014/12/23</DateTime>
// <Parameter name="str" type="IN">原串</Parameter>
// <Returns>字母全为小写返回true</Returns>
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
// <Summary>获取当前exe文件的目录 （包括最后的一个'\'）</Summary>
// <DateTime>2015/01/20</DateTime>
// <Returns>返回当前exe文件的目录 （包括最后的一个'\'）</Returns>
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
// <Summary>获取指定文件的版本号</Summary>
// <DateTime>2015/01/20</DateTime>
// <Parameter name="strExeFileName" type="IN">文件的全路径</Parameter>
// <Parameter name="arrayVer" type="IN/OUT">记录返回的版本号的数组</Parameter>
// <Parameter name="arraySize" type="IN">数组的长度</Parameter>
// <Returns>获取成功返回ture, 否则返回false</Returns>
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

