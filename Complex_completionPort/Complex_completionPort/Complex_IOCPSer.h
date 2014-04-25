#include "DataStruct.h"
#include <WinSock2.h>
#include <Windows.h>
#include <Mswsock.h>
#include <process.h>


#define MAXCONNECTEDCOUNT   2000
#define MAXFREEBUFFER       200
#define MAXIOBUFFERSIZE     1024
#define COMPLLEXIOCPSERLOG  "complexIOCPLog"

#define LISTENPORT 4567
class CIOCPSer
{
public:
	CIOCPSer();
	~CIOCPSer();

	/**********************************
	函 数 功 能：启动服务
	参 数 说 明：
	参数名                   参数意义
	wPort                   监听端口号
	nMaxConnections         允许的最大连接数
	nMaxFreeBuffers         允许的最大不用真正释放的IO唯一数据结构个数
	nMaxFreeContexts		允许的最大不用真正释放的句柄唯一数据结构个数

	*************************************/
	bool StartSer(WORD wPort = 4567, unsigned int nMaxConnections = MAXCONNECTEDCOUNT, 
				  unsigned int nMaxFreeBuffers = 200, unsigned int nMaxFreeContexts = 100, unsigned int nInitialReads = 4);
private:

	//监听线程处理函数
	static unsigned int _stdcall ListenWorkThread(LPVOID parame);
	unsigned int _stdcall ListenWorkThread();

	PCIOCPBuffer AllocIOBuffer(int bufLen = MAXIOBUFFERSIZE);
	void FreeIOBuffer(PCIOCPBuffer pIOBuf);
	void FreeAllIOBuffer();

	PCIOCPContext AllocContextPer(SOCKET s);
	void FreeContextPer(PCIOCPContext pContextPer);
	void FreeAllContextPer();



private:
	 bool m_bServerStarted;
	 bool m_bShutDown;

	 WORD m_wPort;
	 unsigned int m_nMaxConnections;    //本服务器允许的最大联入数
	 unsigned int m_nMaxFreeBuffers;    //本服务器运行的最大IO唯一数据的个数
	 unsigned int m_nMaxFreeContexts;
	 unsigned int m_nInitialReads;

	 SOCKET m_sListen;
	 HANDLE m_hCompletion;

	 HANDLE m_hAcceptEvent;  


	 PCIOCPBuffer  m_pFreeBufferList;
	 PCIOCPContext m_pFreeContextList;
	 

	HANDLE m_hListenThread;

	 LPFN_ACCEPTEX m_lpfnAcceptEx;	
	 LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockaddrs; 
};