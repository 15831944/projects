#include "convertcode.h"
#include <new>

#define CP_GB2312   936
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//class funcation define

/*************************************************
// <Summary>ת��utf-8�ַ���Ϊunicode�ַ���</Summary>
// <DateTime>2015/01/14</DateTime>
// <Parameter name="strUTF8" type="IN">��utf-8�ַ���</Parameter>
// <Parameter name="charLength" type="IN">utf-8�ַ������ȣ��ֽ�������ʵҲ���ַ����������Բ�������ֹ�ַ�"\0"�������ڲ��Ѿ������ˣ� Ϊ-1�ͱ�ʾ�����ַ�������'\0'</Parameter>
// <Returns>����ת���õ�unicode�ַ����������ؿմ�</Returns>
*************************************************/
std::wstring CConvertCode::UTF8ToUnicode(const std::string &strUTF8, int charLength)
{
	std::wstring strUnicode;
	do 
	{
		if(strUTF8.empty() || 0 == charLength)
			break;
#ifdef OS_WIN
		//���ؿ��ַ��������length = -1�Ͱ�����ֹ�ַ�"\0"��
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
// <Summary>ת��unicode�ַ���Ϊutf-8�ַ���</Summary>
// <DateTime>2015/01/14</DateTime>
// <Parameter name="wstrUnicode" type="IN">��ת����unicode�ַ���</Parameter>
// <Parameter name="charLength" type="IN">unicode�ַ������ַ���</Parameter>
// <Returns>����ת���õ�utf-8�ַ����������ؿմ�</Returns>
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
// <Summary>ת��gb2312��Unicode</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strGB2312" type="IN">��ת����GB2312�ַ���</Parameter>
// <Parameter name="charLength" type="IN">GB2312�ַ������ַ���</Parameter>
// <Returns>����ת���õ�Unicode�ַ����������ؿմ�</Returns>
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

		//��gb2312תΪunicode
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
// <Summary>ת��Unicode��gb2312</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strUnicode" type="IN">��ת����Unicode�ַ���</Parameter>
// <Parameter name="charLength" type="IN">Unicode�ַ������ַ���</Parameter>
// <Returns>����ת���õ�gb2312�ַ����������ؿմ�</Returns>
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
// <Summary>ת��gb2312��utf-8</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strGB2312" type="IN">��ת����GB2312�ַ���</Parameter>
// <Parameter name="charLength" type="IN">GB2312�ַ������ַ���</Parameter>
// <Returns>����ת���õ�utf-8�ַ����������ؿմ�</Returns>
*************************************************/
std::string CConvertCode::GB2312ToUTF8(const std::string &strGB2312, int charLength)
{
	std::string strUTF8;
	do 
	{
		if(strGB2312.empty() || 0 == charLength)
			break;

		//��GB2312ת��ΪUnicode
		std::wstring strUnicode = GB2312ToUnicode(strGB2312, charLength);

		if(strUnicode.empty())
			break;

		//��unicodeתΪutf-8
		strUTF8 = UnicodeToUTF8( strUnicode, strUnicode.length() );

	} while (false);

	return strUTF8;
}

/*************************************************
// <Summary>ת��utf-8��gb2312</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strUTF8" type="IN">��ת����utf-8�ַ���</Parameter>
// <Parameter name="charLength" type="IN">utf-8�ַ������ַ���</Parameter>
// <Returns>����ת���õ�gb2312�ַ����������ؿմ�</Returns>
*************************************************/
std::string CConvertCode::UTF8ToGB2312(const std::string &strUTF8, int charLength)
{
	std::string strGB2312;
	do 
	{
		if(strUTF8.empty() || 0 == charLength)
			break;

		//��utf-8ת��Ϊunicode
		std::wstring strUnicode = UTF8ToUnicode(strUTF8, charLength);
		if(strUnicode.empty())
			break;

		//��unicodeת��Ϊgb2312
		strGB2312 = UnicodeToGB2312( strUnicode, strUnicode.length() );
	} while (false);

	return strGB2312;
}

/*************************************************
// <Summary>ת��GBK��UTF-8</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strGBK" type="IN">��ת����GBK�ַ���</Parameter>
// <Parameter name="charLength" type="IN">GBK�ַ������ַ���</Parameter>
// <Returns>����ת���õ�UTF-8�ַ����������ؿմ�</Returns>
*************************************************/
std::string CConvertCode::GBKToUTF8(const std::string &strGBK, int charLength)
{
	std::string strUTF8;
	do 
	{
		if(strGBK.empty() || 0 == charLength)
			break;

		//��GBKתΪUnicode
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

		//��UnicodeתΪUTF-8
		strUTF8 = UnicodeToUTF8(wBuffer, charCount);
		delete[] wBuffer;
	} while (false);

	return strUTF8;	
}

/*************************************************
// <Summary>ת��UTF-8��GBK</Summary>
// <DateTime>2015/01/15</DateTime>
// <Parameter name="strUTF8" type="IN">��ת����UTF-8�ַ���</Parameter>
// <Parameter name="charLength" type="IN">UTF-8�ַ������ַ���</Parameter>
// <Returns>����ת���õ�GBK�ַ����������ؿմ�</Returns>
*************************************************/
std::string CConvertCode::UTF8ToGBK(const std::string &strUTF8, int charLength)
{
	std::string strGBK;
	do 
	{
		if(strUTF8.empty() || 0 == charLength)
			break;

		//��UTF-8ת��ΪUnicode
		std::wstring strUnicode = UTF8ToUnicode(strUTF8, charLength);
		if(strUnicode.empty())
			break;

		//��Unicodeת��ΪGBK
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

