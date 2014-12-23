#include "PublicMethod.h"
#include <stdarg.h>


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




