#ifndef SINGLETON_H
#define SINGLETON_H
#pragma once


/*************************************************
<example>
//class define
class CTest: public CSingleton<CTest>
{
	friend class CSingleton<CTest>;
public:
	int GetData() {return a;}
private:
	CTest(): a(4){}
	CTest& operator=(const CTest&);
	CTest(const CTest&);
	~CTest(){std::cout << "CC" << std::endl;}
private:
	int a;
};

//class use
CTest &t = CTest::RefInstance();
std::cout << t.GetData() << std::endl;

*************************************************/


/*************************************************
// <Summary>单例模式的模板类</Summary>
// <DateTime>2014/12/18</DateTime>
*************************************************/
template <class T>
class CSingleton
{
public:
	typedef T element_type;

	static element_type& RefInstance()
	{
		static element_type instance;
		return instance;
	}

	static element_type* PInstance()
	{
		return &RefInstance();
	}
protected:
	CSingleton(){}
	virtual ~CSingleton(){}
	CSingleton(const CSingleton&);
	CSingleton& operator=(const CSingleton&);
};

#endif