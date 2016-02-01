#ifndef GETLOCALDNS_H
#define GETLOCALDNS_H
#include <string>
#include <vector>


#include "basictypes.h"


#if defined(OS_WIN)
#include "GetLocalDNSImp_Win.h"
#elif defined(OS_ANDROID)
#include "GetLocalDNSImp_Android.h"
#elif defined(OS_IOS)
#include "GetLocalDNSImp_IOS.h"
#else
#error This code is not tested on this platform.
#endif

bool GetLocalDNS(std::vector<std::string> &vecLocalDnsIp);


#endif  //GETLOCALDNS_H