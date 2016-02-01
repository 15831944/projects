#include "GetLocalDNSImp_Android.h"

bool GetLocalDNSPlatformAndroid(std::vector<std::string> &vecLocalDnsIp)
{
	unsigned int i;
	char propname[PROP_NAME_MAX];
	char propvalue[PROP_VALUE_MAX]="";

	for (i = 1; i <= MAX_DNS_PROPERTIES; i++) 
	{
		snprintf(propname, sizeof(propname), "%s%u", DNS_PROP_NAME_PREFIX, i);
		if (__system_property_get(propname, propvalue) < 1)
		{
			break;
		}

		vecLocalDnsIp.push_back(propvalue);
	}

	if(!vecLocalDnsIp.empty())
		return true;

	return false;
}


