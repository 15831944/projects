#ifndef GETLOCALDNSIMP_ANDROID_H
#define GETLOCALDNSIMP_ANDROID_H
#include <string>
#include <vector>
#include <sys/system_properties.h>

/* From the Bionic sources */
#define DNS_PROP_NAME_PREFIX  "net.dns"
#define MAX_DNS_PROPERTIES    8


bool GetLocalDNSPlatformAndroid(std::vector<std::string> &vecLocalDnsIp);

#endif //GETLOCALDNSIMP_ANDROID_H