#include "RapidXmlHelp.h"

int main()
{
	CRepidXmlHelp help("D:\\policy.hcdnclient.pc.3.xml");
	help.ParseXmlAndCreateDOM();
	return 0;
}