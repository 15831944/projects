#include "convertcode.h"
#include <new>

#define CP_GB2312   936
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//class funcation define

/*************************************************
// <Summary>转换utf-8字符串为unicode字符串</Summary>
// <DateTime>2015/01/14</DateTime>
// <Parameter name="strUTF8" type="IN">待utf-8字符串</Parameter>
// <Parameter name="charLength" type="IN">utf-8字符串长度（字节数，其实也是字符数）（可以不包括终止字符"\0"，函数内部已经处理了） 为-1就表示整个字符串包括'\0'</Parameter>
// <Returns>返回转换好的unicode字符串，出错返回空串</Returns>
*************************************************/
std::wstring CConvertCode::UTF8ToUnicode(const std::string &strUTF8, int charLength)
{
	std::wstring strUnicode;
	do 
	{
		if(strUTF8.empty() || 0 == charLength)
			break;
#ifdef OS_WIN
		//返回宽字符数（如果length = -1就包括终止字符"\0"）
		int charCount = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), charLength, NULL, 0);
		if(0 == charCount)
			break;

		wchar_t *wbuffer = new(std::nothrow) wchar_t[charCount + 1];
		if (NULL == *wbuffer)
			break;

		memset( wbuffer, 0, (charCount + 1) * sizeof(wchar_t) );
		if( 0 == MultiByteToWideChar( CP_UTF8, 0 , strUTF8.c_str(), charLength, wbuffer, charCount) )
		{
			delete[] wbuffer;
			break;
		}

		strUnicode = wbuffer;
		delete[] wbuffer;
#endif

	} while (false);

	return strUnicode;
}


/*************************************************
// <Summary>转换unicode字符串为utf-8字符串</Summary>
// <DateTime>2015/01/14</DateTime>
// <Parameter name="wstrUnicode" type="IN">待转换的unicode字符串</Parameter>
// <Parameter name="charLength" type="IN">unicode字符串的字符数</Parameter>
// <Returns>返回转换好的utf-8字符串，出错返回空串</Returns>
*************************************************/
std::string  CConvertCode::UnicodeToUTF8(const std::wstring &wstrUnicode, int charLength)
{
	std::string strUTF8;
	do 
	{
		if (wstrUnicode.empty() || 0 == charLength)
			break;

		int charCount = WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), charLength, NULL, 0, NULL, NULL);
		if (0 == charCount)
			break;

		char *buffer = new(std::nothrow) char[charCount + 1];
		if (NULL == *buffer)
			break;

		memset( buffer, 0, (charCount + 1) * sizeof(char) );
		if(0 == WideCharToMultiByte(CP_UTF8, 0, wstrUnicode.c_str(), charLength, buffer, charCount, NULL, NULL) )
		{
			delete[] buffer;
			break;
		}

		strUTF8 = buffer;
		delete[] buffer;

	} while (false);

	return strUTF8;
}

/*************************************************
// <Summary>转换gb2312到Unicode</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strGB2312" type="IN">待转换的GB2312字符串</Parameter>
// <Parameter name="charLength" type="IN">GB2312字符串的字符数</Parameter>
// <Returns>返回转换好的Unicode字符串，出错返回空串</Returns>
*************************************************/
std::wstring CConvertCode::GB2312ToUnicode(const std::string &strGB2312, int charLength)
{
	std::wstring strUnicode;
	do 
	{
		if(strGB2312.empty() || 0 == charLength)
			break;

		int charCount = MultiByteToWideChar(CP_GB2312, 0, strGB2312.c_str(), charLength, NULL, 0);
		if(0 == charCount)
			break;

		wchar_t *wBuffer = new(std::nothrow) wchar_t[charCount + 1];
		if(NULL == wBuffer)
			break;

		memset(wBuffer, 0x0, (charCount + 1) * sizeof(wchar_t));

		//将gb2312转为unicode
		if( 0 == MultiByteToWideChar(CP_GB2312, 0, strGB2312.c_str(), charLength, wBuffer, charCount) )
		{
			delete[] wBuffer;
			break;
		}

		strUnicode = wBuffer;
		delete[] wBuffer;

	} while (false);

	return strUnicode;
}

/*************************************************
// <Summary>转换Unicode到gb2312</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strUnicode" type="IN">待转换的Unicode字符串</Parameter>
// <Parameter name="charLength" type="IN">Unicode字符串的字符数</Parameter>
// <Returns>返回转换好的gb2312字符串，出错返回空串</Returns>
*************************************************/
std::string  CConvertCode::UnicodeToGB2312(const std::wstring &strUnicode, int charLength)
{
	std::string strGB2312;
	do 
	{
		if(strUnicode.empty() || 0 == charLength)
			break;

		int charCount = WideCharToMultiByte(CP_GB2312, 0, strUnicode.c_str(), charLength, NULL, 0, NULL, NULL);
		if(0 == charCount)
			break;

		char *buffer = new(std::nothrow) char[charCount + 1];
		if(NULL == buffer)
			break;

		memset(buffer, 0x0, charCount + 1);

		if( 0 == WideCharToMultiByte(CP_GB2312, 0, strUnicode.c_str(), charLength, buffer, charCount, NULL, NULL) )
		{
			delete[] buffer;
			break;
		}

		strGB2312 = buffer;
		delete[] buffer;

	} while (false);

	return strGB2312;
}

/*************************************************
// <Summary>转换gb2312到utf-8</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strGB2312" type="IN">待转换的GB2312字符串</Parameter>
// <Parameter name="charLength" type="IN">GB2312字符串的字符数</Parameter>
// <Returns>返回转换好的utf-8字符串，出错返回空串</Returns>
*************************************************/
std::string CConvertCode::GB2312ToUTF8(const std::string &strGB2312, int charLength)
{
	std::string strUTF8;
	do 
	{
		if(strGB2312.empty() || 0 == charLength)
			break;

		//将GB2312转换为Unicode
		std::wstring strUnicode = GB2312ToUnicode(strGB2312, charLength);

		if(strUnicode.empty())
			break;

		//将unicode转为utf-8
		strUTF8 = UnicodeToUTF8( strUnicode, strUnicode.length() );

	} while (false);

	return strUTF8;
}

/*************************************************
// <Summary>转换utf-8到gb2312</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strUTF8" type="IN">待转换的utf-8字符串</Parameter>
// <Parameter name="charLength" type="IN">utf-8字符串的字符数</Parameter>
// <Returns>返回转换好的gb2312字符串，出错返回空串</Returns>
*************************************************/
std::string CConvertCode::UTF8ToGB2312(const std::string &strUTF8, int charLength)
{
	std::string strGB2312;
	do 
	{
		if(strUTF8.empty() || 0 == charLength)
			break;

		//将utf-8转换为unicode
		std::wstring strUnicode = UTF8ToUnicode(strUTF8, charLength);
		if(strUnicode.empty())
			break;

		//将unicode转换为gb2312
		strGB2312 = UnicodeToGB2312( strUnicode, strUnicode.length() );
	} while (false);

	return strGB2312;
}

/*************************************************
// <Summary>转换GBK到UTF-8</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strGBK" type="IN">待转换的GBK字符串</Parameter>
// <Parameter name="charLength" type="IN">GBK字符串的字符数</Parameter>
// <Returns>返回转换好的UTF-8字符串，出错返回空串</Returns>
*************************************************/
std::string CConvertCode::GBKToUTF8(const std::string &strGBK, int charLength)
{
	std::string strUTF8;
	do 
	{
		if(strGBK.empty() || 0 == charLength)
			break;

		//将GBK转为Unicode
		int charCount = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), charLength, NULL, 0);
		if(0 == charCount)
			break;

		wchar_t *wBuffer = new(std::nothrow) wchar_t[charCount + 1];
		if(NULL == wBuffer)
			break;

		memset( wBuffer, 0x0, sizeof(wchar_t) * (charCount + 1) );

		if( 0 == MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), charLength, wBuffer, charCount) )
		{
			delete[] wBuffer;
			break;
		}

		//将Unicode转为UTF-8
		strUTF8 = UnicodeToUTF8(wBuffer, charCount);
		delete[] wBuffer;
	} while (false);

	return strUTF8;	
}

/*************************************************
// <Summary>转换UTF-8到GBK</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strUTF8" type="IN">待转换的UTF-8字符串</Parameter>
// <Parameter name="charLength" type="IN">UTF-8字符串的字符数</Parameter>
// <Returns>返回转换好的GBK字符串，出错返回空串</Returns>
*************************************************/
std::string CConvertCode::UTF8ToGBK(const std::string &strUTF8, int charLength)
{
	std::string strGBK;
	do 
	{
		if(strUTF8.empty() || 0 == charLength)
			break;

		//将UTF-8转换为Unicode
		std::wstring strUnicode = UTF8ToUnicode(strUTF8, charLength);
		if(strUnicode.empty())
			break;

		//将Unicode转换为GBK
		int charCount = WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), strUnicode.length(), NULL, 0, NULL, NULL);
		if(0 == charCount)
			break;

		char *buffer = new(std::nothrow) char[charCount + 1];
		if(NULL == buffer)
			break;
		memset(buffer, 0x0, charCount + 1);

		if( 0 == WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), strUnicode.length(), buffer, charCount, NULL, NULL) )
		{
			delete[] buffer;
			break;
		}

		strGBK = buffer;
		delete[] buffer;
	} while (false);

	return strGBK;	
}

