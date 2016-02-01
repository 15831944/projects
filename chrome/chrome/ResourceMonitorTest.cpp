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
		//ע���ڵ���InitGlobalData��GetGlobalAndSpeciValueByPDH֮��
		//��GetGlobalAndSpeciValueByPDH���ٴ�GetGlobalAndSpeciValueByPDH֮��һ��Ҫsleepһ���
		//Ҫ��Ȼ�ڲɼ�����ʱ���ױ�����
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