#include "RegisterOperator.h"


using namespace BaseClassLibrary;

int main()
{
	HKEY rootKey= HKEY_CURRENT_USER;
	std::string subKey = "PPStream.FDS";
	std::string strValue;
	DWORD dwType = 0;

	CRegisterOper::GetRegValue(rootKey, subKey, "ip", strValue, dwType);
	strValue = "23456";
	CRegisterOper::SetRegValue(rootKey, subKey, "ip", strValue, dwType);

	return 0;
}