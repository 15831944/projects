#include "singleton.h"

class TestSingleClass: public Singleton<TestSingleClass>
{
};


int main()
{
	TestSingleClass &Instance = TestSingleClass::Instance();
}
