#include "GetLocalDNSImp_IOS.h"

bool GetLocalDNSPlatformIOS(std::vector<std::string> &vecLocalDnsIp)
{
	bool iErrorCode = false;
	iErrorCode = res_init();
	if(iErrorCode != 0)
	{
		return iErrorCode ;
	}
	else
	{
		std::string strTmp;
		for (int i=0;i<MAXNS;i++)
		{
			strTmp = inet_ntoa(_res.nsaddr_list[i].sin_addr);
			if(strTmp == "0.0.0.0")
				break;
			vecLocalDnsIp.push_back(strTmp);
		}

		if(!vecLocalDnsIp.empty())
			iErrorCode = true;
	}
	return iErrorCode;
}