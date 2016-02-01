#include "GetLocalDNSImp_Win.h"
#include <WinSock2.h>
#include <Windows.h>
#include <IPHlpApi.h>
#pragma comment(lib, "Iphlpapi.lib")

eWinPlatform Getplatform(void)
{
	OSVERSIONINFOA Osv;

	memset(&Osv, 0, sizeof(Osv));
	Osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if (!GetVersionExA(&Osv))
	{
		memset(&Osv, 0, sizeof(Osv));
		Osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
		if (!GetVersionExA(&Osv))
			return WIN_UNKNOWN;
	}

	switch(Osv.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
		return WIN_3X;

	case VER_PLATFORM_WIN32_WINDOWS:
		return WIN_9X;

	case VER_PLATFORM_WIN32_NT:
		return WIN_NT;

	default:
		return WIN_UNKNOWN;
	}
}

bool GetREGSZ(HKEY hKey, const std::string strLeafKeyName, std::string &strOutValue)
{
	bool   bResult     = false;
	int    res         = 0;
	char   buffer[257] = {0};
	DWORD  dwLen       = 256;
	DWORD  dwType      = 0;

	do 
	{
		res = RegQueryValueExA(hKey, strLeafKeyName.c_str(), 0, &dwType, 
			(BYTE *)buffer, &dwLen);
		if(ERROR_SUCCESS != res)
			break;

		strOutValue = buffer;

		bResult = true;
	} while (false);

	return bResult;
}

bool GetEnumREGSZ(HKEY hKeyParent, const std::string strLeafKeyName, std::string &strOutValue)
{
	bool  bResult = false;
	char  enumKeyName[256];
	DWORD enumKeyNameBuffSize;
	DWORD enumKeyIdx = 0;
	HKEY  hKeyEnum = NULL;
	int   res = 0;

	do 
	{
		while(true)
		{
			enumKeyNameBuffSize = sizeof(enumKeyName);
			res = RegEnumKeyExA(hKeyParent, enumKeyIdx++, enumKeyName,
				&enumKeyNameBuffSize, 0, NULL, NULL, NULL);

			if (ERROR_SUCCESS != res)
				break;

			res = RegOpenKeyExA(hKeyParent, enumKeyName, 0, KEY_QUERY_VALUE,
				&hKeyEnum);
			if (ERROR_SUCCESS != res)
				continue;

			GetREGSZ(hKeyEnum, strLeafKeyName, strOutValue);

			if(hKeyEnum)
			{
				RegCloseKey(hKeyEnum);
				hKeyEnum = NULL;
			}

			if(!strOutValue.empty())
			{
				bResult = true;
				break;
			}
		}

	} while (false);


	if(hKeyEnum)
	{
		RegCloseKey(hKeyEnum);
		hKeyEnum = NULL;
	}

	return true;
}

/*
* Implementation supports Windows 95, 98 and ME.
*/
bool GetDNSRegistry9X(std::vector<std::string> &vecLocalDnsIp)
{
	bool bResult = false;
	HKEY hKey_VxD_MStcp = NULL;
	int  res;

	do 
	{
		res = RegOpenKeyExA(HKEY_LOCAL_MACHINE, WIN_NS_9X, 0, KEY_READ,
			&hKey_VxD_MStcp);
		if (ERROR_SUCCESS != res)
			break;

		std::string strValue;
		if(false == GetREGSZ(hKey_VxD_MStcp, NAMESERVER, strValue))
			break;

		CStringMethod::StringSplite(strValue, ",", vecLocalDnsIp);

		if(!vecLocalDnsIp.empty())
			bResult = true;

	} while (false);

	if(NULL != hKey_VxD_MStcp)
	{ 
		RegCloseKey(hKey_VxD_MStcp);
		hKey_VxD_MStcp = NULL;
	}

	return bResult;
}

/*
* Implementation supports Windows NT 3.5 and newer.
*/
bool GetDNSRegistryNT(std::vector<std::string> &vecLocalDnsIp)
{
	bool bResult               = false;
	HKEY hKey_Interfaces       = NULL;
	HKEY hKey_Tcpip_Parameters = NULL;
	int  res = 0;
	std::string strValue;

	do 
	{
		res = RegOpenKeyExA(HKEY_LOCAL_MACHINE, WIN_NS_NT_KEY, 0, KEY_READ,
			&hKey_Tcpip_Parameters);
		if (res != ERROR_SUCCESS)
			break;

		/*
		** Global DNS settings override adapter specific parameters when both
		** are set. Additionally static DNS settings override DHCP-configured
		** parameters when both are set.
		*/

		/* Global DNS static parameters */

		GetREGSZ(hKey_Tcpip_Parameters, NAMESERVER, strValue);
		if(!strValue.empty())
		{
			bResult = true;
			break;
		}

		/* Global DNS DHCP-configured parameters */
		GetREGSZ(hKey_Tcpip_Parameters, DHCPNAMESERVER, strValue);
		if(!strValue.empty())
		{
			bResult = true;
			break;
		}

		/* Try adapter specific parameters */
		res = RegOpenKeyExA(hKey_Tcpip_Parameters, "Interfaces", 0,
			KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS,
			&hKey_Interfaces);
		if (ERROR_SUCCESS != res)
			break;

		/* Adapter specific DNS static parameters */
		GetEnumREGSZ(hKey_Interfaces, NAMESERVER, strValue);
		if(!strValue.empty())
		{
			bResult = true;
			break;
		}

		/* Adapter specific DNS DHCP-configured parameters */
		GetEnumREGSZ(hKey_Interfaces, DHCPNAMESERVER, strValue);
		if(!strValue.empty())
		{
			bResult = true;
			break;
		}

	} while (false);

	if(bResult)
	{
		CStringMethod::StringSplite(strValue, ",", vecLocalDnsIp);
	}

	if(NULL != hKey_Tcpip_Parameters)
	{
		RegCloseKey(hKey_Tcpip_Parameters);
		hKey_Tcpip_Parameters = NULL;
	}

	if(NULL != hKey_Interfaces)
	{
		RegCloseKey(hKey_Interfaces);
		hKey_Interfaces = NULL;
	}

	return bResult;
}

bool GetDNSRegistry(std::vector<std::string> &vecLocalDnsIp)
{
	eWinPlatform platform;

	platform = Getplatform();

	if (platform == WIN_NT)
		return GetDNSRegistryNT(vecLocalDnsIp);
	else if (platform == WIN_9X)
		return GetDNSRegistry9X(vecLocalDnsIp);

	return false;
}

/*
Implementation supports Windows XP and newer.
*/
bool GetDNSAdaptersAddresses(std::vector<std::string> &vecLocalDnsIp)
{
	bool bResult = false;
	IP_ADAPTER_DNS_SERVER_ADDRESS *pIPAdaDNSAddr = NULL;
	IP_ADAPTER_ADDRESSES *pIPAdaAdd = NULL;
	ULONG ulOutBufferLen = 0;
	ULONG ulRetVal = 0;


	for(int i= 0; i < 5; ++i)
	{
		ulRetVal = GetAdaptersAddresses(AF_INET, 0, NULL, pIPAdaAdd, &ulOutBufferLen);
		if(ERROR_BUFFER_OVERFLOW != ulRetVal && ERROR_SUCCESS == ulRetVal)
			break;

		if(NULL != pIPAdaAdd)
		{
			free(pIPAdaAdd);
			pIPAdaAdd = NULL;
		}

		pIPAdaAdd = (PIP_ADAPTER_ADDRESSES)malloc(ulOutBufferLen);
		if(NULL == pIPAdaAdd)
			break;
	}

	if(ERROR_SUCCESS == ulRetVal)
	{
		sockaddr *sa = NULL;
		sockaddr_in *sin = NULL;
		IP_ADAPTER_ADDRESSES *pIPAdaAddrList = pIPAdaAdd;
		for (; pIPAdaAddrList; pIPAdaAddrList = pIPAdaAddrList->Next)
		{
			for (pIPAdaDNSAddr = pIPAdaAddrList->FirstDnsServerAddress; 
				pIPAdaDNSAddr; pIPAdaDNSAddr = pIPAdaDNSAddr->Next)
			{
				sockaddr *sa = pIPAdaDNSAddr->Address.lpSockaddr;
				if(AF_INET == sa->sa_family)
				{
					sin = (sockaddr_in *)sa;
					if(INADDR_NONE == sin->sin_addr.S_un.S_addr || INADDR_ANY == sin->sin_addr.S_un.S_addr)
						continue;
					vecLocalDnsIp.push_back(inet_ntoa(sin->sin_addr));
				}
			}
		}

		if(!vecLocalDnsIp.empty())
			bResult = true;
	}

	if(NULL != pIPAdaAdd)
	{
		free(pIPAdaAdd);
		pIPAdaAdd = NULL;
	}

	return bResult;
}

/*
Note: Ancient PSDK required in order to build a W98 target.
*/
bool GetDNSNetworkParams(std::vector<std::string> &vecLocalDnsIp)
{
	bool bResult = false;
	FIXED_INFO *pFixdInfo = NULL;
	ULONG ulOutBufLen;
	DWORD dwRetVal;
	IP_ADDR_STRING *pIPAddr = NULL;
	do 
	{
		dwRetVal = GetNetworkParams(pFixdInfo, &ulOutBufLen);
		if(ERROR_BUFFER_OVERFLOW != dwRetVal && ERROR_SUCCESS != dwRetVal)
			break;

		if(ERROR_BUFFER_OVERFLOW == dwRetVal)
		{
			pFixdInfo = (PFIXED_INFO)malloc(ulOutBufLen);
			if(NULL == pFixdInfo)
				break;
		}

		dwRetVal = GetNetworkParams(pFixdInfo, &ulOutBufLen);

		if(ERROR_SUCCESS == dwRetVal)
		{
			for(pIPAddr = &pFixdInfo->DnsServerList; pIPAddr; pIPAddr = pIPAddr->Next)
			{
				vecLocalDnsIp.push_back(pIPAddr->IpAddress.String);
			}
		}

		if(!vecLocalDnsIp.empty())
			bResult = true;
	} while (false);

	if(NULL != pFixdInfo)
	{
		free(pFixdInfo);
		pFixdInfo = NULL;
	}

	return bResult;
}

bool GetDNSWindows(std::vector<std::string> &vecLocalDnsIp)
{
	/* Try using IP helper API GetAdaptersAddresses() */
	if (GetDNSAdaptersAddresses(vecLocalDnsIp))
		return 1;

	/* Try using IP helper API GetNetworkParams() */
	if (GetDNSNetworkParams(vecLocalDnsIp))
		return 1;

	/* Fall-back to registry information */
	return GetDNSRegistry(vecLocalDnsIp);

	return 0;
}

bool GetLocalDNSPlatformWin(std::vector<std::string> &vecLocalDnsIp)
{
	return GetDNSWindows(vecLocalDnsIp);
}