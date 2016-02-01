#ifndef GETLOCALDNSIMP_WIN_H
#define GETLOCALDNSIMP_WIN_H
#include <string>
#include <vector>
#include "PublicMethod.h"

#define WIN_NS_9X      "System\\CurrentControlSet\\Services\\VxD\\MSTCP"
#define WIN_NS_NT_KEY  "System\\CurrentControlSet\\Services\\Tcpip\\Parameters"
#define NAMESERVER     "NameServer"
#define DHCPNAMESERVER "DhcpNameServer"

typedef enum {
	WIN_UNKNOWN,
	WIN_3X,
	WIN_9X,
	WIN_NT,
}eWinPlatform;

bool GetLocalDNSPlatformWin(std::vector<std::string> &vecLocalDnsIp);

bool GetDNSWindows(std::vector<std::string> &vecLocalDnsIp);

bool GetDNSAdaptersAddresses(std::vector<std::string> &vecLocalDnsIp);

bool GetDNSNetworkParams(std::vector<std::string> &vecLocalDnsIp);

bool GetDNSRegistry(std::vector<std::string> &vecLocalDnsIp);

bool GetDNSRegistry9X(std::vector<std::string> &vecLocalDnsIp);

bool GetDNSRegistryNT(std::vector<std::string> &vecLocalDnsIp);

eWinPlatform Getplatform(void);

bool GetREGSZ(HKEY hKey, const std::string strLeafKeyName, std::string &strOutValue);

bool GetEnumREGSZ(HKEY hKeyParent, const std::string strLeafKeyName, std::string &strOutValue);

#endif //GETLOCALDNSIMP_WIN_H


