#include "LogInterface.h"

int main()
{
	CLogInter::StartLog("e:\\MyLog\\");
	char data[10] = "hello log";
	CLogInter::PrintLog("log1", _Debug, "%s", data);
//	CLogInter::SetLogLevel(_Warn);
	CLogInter::PrintLog("log2", _Debug, "%s", data);
	CLogInter::StopLog();
	return 0;
}