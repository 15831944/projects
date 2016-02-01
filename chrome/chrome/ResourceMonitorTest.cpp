#include "ResourceMonitor.h"
int main()
{

	CResourceMonitor mon;
	std::map<std::string, double> m;
	int error = 0;
	bool flag = mon.InitGlobalData("QyKernel");
	if(flag == false)
		printf("init global data is error!\n");
	while(true)
	{
		//注意在调用InitGlobalData与GetGlobalAndSpeciValueByPDH之间
		//和GetGlobalAndSpeciValueByPDH与再次GetGlobalAndSpeciValueByPDH之间一定要sleep一会儿
		//要不然在采集数据时容易报错误
		Sleep(1000);  
		mon.GetGlobalAndSpeciValueByPDH(m,error);
		for (std::map<std::string, double>::iterator itr = m.begin(); itr != m.end(); ++itr)
		{
			printf("%s:%f  ",(itr->first).c_str(), itr->second);
		}
		printf("\n");	
	}

	return 0;
}