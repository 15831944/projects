#include "stdafx.h"
#include "ShareMemory.h"


//server
int main()
{
	BaseClassLibrary::CShareMem mm;
	bool gg = mm.Create("zhonghao", 1024);
	printf("%d", gg);
	char buf[100] = "share memory test";
	mm.Send(buf, strlen(buf));
	getchar();
	char buf2[100] = "share memory test2";
	mm.Send(buf2, strlen(buf2));
	getchar();
	return 0;
}

//client
/*
#include "stdafx.h"
#include "../../chrome/chrome/ShareMemory.h"

class CRecv:public BaseClassLibrary::IDataReceiver
{
	virtual void OnRecv(const char*name,char* pData,int len); 
};

void CRecv::OnRecv(const char*name,char* pData,int len)
{
	char buf[100] = {0};
	memcpy(buf, pData, len);
	int ff=0;
	++ff;
}



int _tmain(int argc, _TCHAR* argv[])
{	
	BaseClassLibrary::CShareMem mm;
	CRecv *recvInter = new CRecv;;
	bool ff = mm.Open("zhonghao");
	char buf[100] = "share memory test";
	mm.Read(recvInter);
	return 0;
}
*/