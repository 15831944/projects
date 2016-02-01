#ifndef PUBLICMETHOD_H
#define PUBLICMETHOD_H
#include <string>
#include <cctype>
#include <vector>
#include <Windows.h>

//Functions related to string
class CStringMethod
{
public:
	//字符串操作相关函数
	static std::string& StringCat(std::string &str, const char *formatString, ...);
	static std::string& StringFormat(std::string &srcString, const char *pFormatString, ...);
	static std::string& StringLeftTrim(std::string &str, const char ch = ' ');
	static std::string& StringRightTrim(std::string &str, const char ch = ' ');
	static std::string& StringTrim(std::string &str, const char ch = ' ');
	static void         StringSplite(const std::string &strInput, const std::string &spliter, std::vector<std::string> &vecResult);
	//static const std::string& StringMakeUpper(std::string &str);
	//static const std::string& StringMakeLower(std::string &str);


	//用于判断string内的字符的一些相关函数
	static bool IsAllNum(const std::string &str);
	static bool IsAllAlpha(const std::string &str);
	static bool IsAllUpper(const std::string &str);
	static bool IsAllLower(const std::string &str);
};


//执行文件相关的函数类
class CExeRef
{
public:
	static std::string GetCurrentExeDir();
	static bool        GetCurrentExeVer(const std::string& strExeFileName, unsigned long *arrayVer, unsigned arraySize);

};

#endif //PUBLICMETHOD_H