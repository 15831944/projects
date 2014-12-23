#ifndef PUBLICMETHOD_H
#define PUBLICMETHOD_H
#include <string>
#include <cctype>

//Functions related to string
class CStringMethod
{
public:
	//�ַ���������غ���
	static std::string& StringCat(std::string &str, const char *formatString, ...);
	static std::string& StringFormat(std::string &srcString, const char *pFormatString, ...);
	static std::string& StringLeftTrim(std::string &str, const char ch = ' ');
	static std::string& StringRightTrim(std::string &str, const char ch = ' ');
	static std::string& StringTrim(std::string &str, const char ch = ' ');
	//static const std::string& StringMakeUpper(std::string &str);
	//static const std::string& StringMakeLower(std::string &str);


	//�����ж�string�ڵ��ַ���һЩ��غ���
	static bool IsAllNum(const std::string &str);
	static bool IsAllAlpha(const std::string &str);
	static bool IsAllUpper(const std::string &str);
	static bool IsAllLower(const std::string &str);
};

#endif //PUBLICMETHOD_H