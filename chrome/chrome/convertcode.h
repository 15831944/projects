#pragma once
#include <string>
#include "basictypes.h"

#ifdef OS_WIN
#include <Windows.h>
#endif

/*************************************************
����ʹ��˵��
MultiByteToWideChar(...)
1.��һ������ָ��������ֽ��ַ���������һ������ҳֵ��
1.���ĸ���������Ϊ����ֵ��������������-1,�����������������ַ���,�������ս��ߡ��������ⲿ�Լ�ָ�����ȡ�
2.�������������Ϊ0���˺�������ת������ַ�������ע������ָ����ת�����ĸ�����ָ���ĳ��Ⱥ���ַ�����
3.�˺���ֻ���ڻ���ռ��㹻������²��ܳɹ����ء�
*************************************************/


/*************************************************
����ʹ��˵��
WideCharToMultiByte(...)
1.��һ������ָ����Ҫת���ɵĶ��ֽ��ַ�����һ������ҳֵ��
1.���ĸ���������Ϊ����ֵ��������������-1,�����������������ַ���,�������ս��ߡ��������ⲿ�Լ�ָ�����ȣ��ַ����ȣ���
2.�������������Ϊ0���˺�������ת������ַ�������ע������ָ����ת�����ĸ�����ָ���ĳ��Ⱥ���ַ�����
3.�˺���ֻ���ڻ���ռ��㹻������²��ܳɹ����ء�
*************************************************/


/*************************************************
����ʹ��˵��
sizeof()����ȡ�ַ������ֽ�����
_countof()����ȡ�ַ������ַ�������ע�ⲻ�ܴ���new����������ָ�룬�����ǲ��еģ�
*************************************************/



/*************************************************
CConvertCode:
������Ҫ��ɿ��ַ�����ֽ�֮���ת��������
ע�⣺
1.�����unicode ָ����UTF-16��ÿ���ַ����������ֽ�����ʾ
2.���ֱ��뼯֮���ת������Ҫ������ASNI��Unicode��ת��
3.vc�µ�ASNI��ʵ����GBK�ַ�����
4.һ�������δֱ��˵��Unicode�ַ����Ļ�,Unicode�ַ���ָ�ľ���UTF-16�������ַ����������ֽ�����ʾ����
5.����WideCharToMultiByte��MultiByteToWideChar��������ĳ��ȶ�ָ�����ַ����ȡ�
6.GBK��GB2312����UTF8֮�䶼����ͨ��Unicode��������໥ת���� 

  GBK��GB2312����Unicode����UTF8 

  UTF8����Unicode����GBK��GB2312 
*************************************************/
class CConvertCode
{
public:
	static std::wstring UTF8ToUnicode(const std::string &strUTF8, int charLength);
	static std::string  UnicodeToUTF8(const std::wstring &wstrUnicode, int charLength);

	static std::wstring GB2312ToUnicode(const std::string &strGB2312, int charLength);
	static std::string  UnicodeToGB2312(const std::wstring &strUnicode, int charLength);

	static std::string GB2312ToUTF8(const std::string &strGB2312, int charLength);
	static std::string UTF8ToGB2312(const std::string &strUTF8, int charLength);

	static std::string GBKToUTF8(const std::string &strGBK, int charLength);
	static std::string UTF8ToGBK(const std::string &strUTF8, int charLength);
};