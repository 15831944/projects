#pragma once
#include <string>
#include "basictypes.h"

#ifdef OS_WIN
#include <Windows.h>
#endif

/*************************************************
函数使用说明
MultiByteToWideChar(...)
1.第一个参数指定了与多字节字符串关联的一个代码页值。
1.第四个参数可以为两种值：如果这个参数是-1,函数处理整个输入字符串,包括空终结者。或者又外部自己指定长度。
2.第六个参数如果为0，此函数返回转换后的字符数。（注意这里指的是转换第四个参数指定的长度后的字符数）
3.此函数只有在缓存空间足够的情况下才能成功返回。
*************************************************/


/*************************************************
函数使用说明
WideCharToMultiByte(...)
1.第一个参数指定了要转换成的多字节字符串的一个代码页值。
1.第四个参数可以为两种值：如果这个参数是-1,函数处理整个输入字符串,包括空终结者。或者由外部自己指定长度（字符长度）。
2.第六个参数如果为0，此函数返回转换后的字符数。（注意这里指的是转换第四个参数指定的长度后的字符数）
3.此函数只有在缓存空间足够的情况下才能成功返回。
*************************************************/


/*************************************************
函数使用说明
sizeof()：获取字符串的字节数。
_countof()：获取字符串的字符数。（注意不能传入new出来的数据指针，这样是不行的）
*************************************************/



/*************************************************
CConvertCode:
此类主要完成宽字符与多字节之间的转换工作。
注意：
1.这里的unicode 指的是UTF-16即每个字符采用两个字节来表示
2.几种编码集之间的转换的重要桥梁是ASNI到Unicode的转换
3.vc下的ASNI其实就是GBK字符集。
4.一般情况下未直接说明Unicode字符集的话,Unicode字符集指的就是UTF-16（所有字符都用两个字节来表示）。
5.对于WideCharToMultiByte与MultiByteToWideChar函数里面的长度都指的是字符长度。
6.GBK、GB2312等与UTF8之间都必须通过Unicode编码才能相互转换： 

  GBK、GB2312－－Unicode－－UTF8 

  UTF8－－Unicode－－GBK、GB2312 
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