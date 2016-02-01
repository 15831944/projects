// chrome.cpp : 定义控制台应用程序的入口点。
//

// #include "stdafx.h"
// #include <iostream>
// #include "macro_function.h"
// #include "scoped_ptr.h"
// 
// class CTest
// {
// public:
// 	CTest():v(0), f(0.1){}
// 	int v;
// 	double f;
// };
// int _tmain(int argc, _TCHAR* argv[])
// {
// 	CTest *p = new CTest();
// 
// 	scoped_ptr<CTest> CTest_ptr(new CTest());
// 	scoped_array<int> CTestArr_ptr(new int[100]);
// CTestArr_ptr[1] = 3;
// 
// std::cout << CTestArr_ptr[1] << "  " << CTestArr_ptr[0] << std::endl;
// 
// 	//bassclass::scoped_ptr<int> int_ptr(bassclass::scoped_ptr<CTest>(p));
// 
// 	if (CTest_ptr)
// 	{
// 		std::cout << "ptr null" << std::endl;
// 	}
// 
// 	short a[10] = {0};
// 
// 	//testing
// 	std::cout << ArrayElemNum(a) << std::endl;
// 	std::cout << ArrayMemSize(a) << std::endl;
// 	std::cout << CTest_ptr->f << std::endl;
// }

#include <stdio.h>
#include <iostream>
#include "singleton.h"
//#include "vld.h"
#include "UnhandleException.h"
#include "PublicMethod.h"
#include "compiler_specific.h"
#include "macro_function.h"
#include "basictypes.h"
#include "convertcode.h"
#include <stdlib.h>
#include "StunServer.h"
#include <string>
#include "RegisterOperator.h"
#include "rand.h"

#include "inet_pton.h"

#pragma pack(2)
struct abc
{
	long b;
	short a;
	char  c;
};
#pragma pack()

class CTest: public Singleton<CTest>
{
	friend class Singleton<CTest>;
public:
	int GetData() {return a;}
	void SetData(int b){a = b;}
private:
	CTest(): a(4){}
	~CTest(){std::cout << "CC" << std::endl;}
	CTest& operator=(const CTest&);
	CTest(const CTest&);
	int a;


};


class B;
class A
{
public:
	A(B *b):m_b(b){}
private:
	B *m_b;
};

class B
{
public:
	B():ALLOW_THIS_IN_INITIALIZER_LIST(m_a(this)){}
private:
	A m_a;
};

using namespace BaseClassLibrary;


int main()
{
	char dd[100] = {0};


	Curl_inet_pton(AF_INET, "10.0.0.1", dd);
	//redisSrand48(GetTickCount());
	//int gg = redisLrand48();

	//CStunServer ser;
	//bool ff= ser.Start("10.0.0.1", "10.0.0.2");

// 	HKEY rootKey= HKEY_CURRENT_USER;
// 	//std::string subKey = "PPStream.FDS";
// 	std::string subKey = "zhonghao";
// 
// 	std::string strValue;
// 	DWORD dwType = 0;
// 
// 	//CRegisterOper::GetRegValue(rootKey, subKey, "ipt", strValue, dwType);
// 	strValue = "23456";
// 	CRegisterOper::SetRegValue(rootKey, subKey, "ipt", strValue, REG_SZ);





	unsigned long dfdf[4] = {0};
	CExeRef::GetCurrentExeVer("C:\\Program Files (x86)\\IQIYI_PPS\\iQIYI\\QiyiClient.exe", dfdf, 4);


	CConvertCode::UTF8ToUnicode("sdf", 4);
	std::wstring strUnicode = L"asdf";

	std::wstring strUnicode2 = L"asdf";
	CConvertCode::UnicodeToUTF8(strUnicode2, strUnicode2.length());

	//int g = 3;
	//int gg = g / (g - 3);

	int64 fsf = 100000000i64;
	//std::string ff = "    ffffffDds     ";
	//bool g = CStringMethod::IsAllLower(CStringMethod::StringTrim(ff));

	CTest &t = CTest::Instance();
	CTest *pt1 = CTest::PInstance();
	CTest *pt2 = CTest::PInstance();
	std::cout << t.GetData() << std::endl;
	printf("0x%p\n", &t);
	printf("0x%p\n", pt1);
	printf("0x%p\n", pt2);

	pt1->SetData(3);
	std::cout << t.GetData() << std::endl;



//printf("%d, %d\n", __alignof(struct abc), __alignof(long long));
	return 0;
 }

