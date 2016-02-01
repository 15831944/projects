#include "FuncBindThread.h"
#include "iostream"
using namespace BaseClassLibrary;

class Test
{
public:
	void Start(){bindThread.StartWithOptions();}
	void TestFun(int a){std::cout << a <<std::endl;}
//��ʱ����
	void LanuchFun()
	{
		bindThread.PostFuncTask(Bind(&Test::TestFun, this, 5),1000);
	}
//��ʱ����
	void LanuchFun1()
	{
		bindThread.PostFuncTask(Bind(&Test::TestFun, this, 10));
	}

private:
	Thread bindThread;
};

int main()
{
	Test ff;
	ff.Start();
	ff.LanuchFun();
	ff.LanuchFun1();
	Sleep(10000);
	return 0;
}