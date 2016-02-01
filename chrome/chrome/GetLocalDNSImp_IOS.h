#ifndef GETLOCALDNSIMP_IOS_H
#define GETLOCALDNSIMP_IOS_H
#include <string>
#include <vector>
#include <resolv.h>
#include <arpa/inet.h>

bool GetLocalDNSPlatformIOS(std::vector<std::string> &vecLocalDnsIp);

#endif //GETLOCALDNSIMP_IOS_H