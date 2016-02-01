#include <iostream>
#include <string>
#include <vector>


#include "GetLocalDNS.h"

using namespace std;

int main(int argc, char *argv[])
{
	std::vector<std::string> dnsip;
	bool bResult = GetLocalDNS(dnsip);
	return 0;
}