#include "GetLocalDNS.h"
#include "basictypes.h"

bool GetLocalDNS(std::vector<std::string> &vecLocalDnsIp)
{
#if defined(OS_WIN)
return GetLocalDNSPlatformWin(vecLocalDnsIp);
#elif defined(OS_ANDROID)
return GetLocalDNSPlatformAndroid(vecLocalDnsIp);
#elif defined(OS_IOS)
return GetLocalDNSPlatformIOS(vecLocalDnsIp);
#else
#error This code is not tested on this platform.
#endif
	
}