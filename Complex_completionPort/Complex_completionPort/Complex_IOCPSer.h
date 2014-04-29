#include "DataStruct.h"
#include <WinSock2.h>
#include <Windows.h>
#include <Mswsock.h>
#include <process.h>


#define MAXCONNECTEDCOUNT   2000
#define MAXFREEBUFFER       200
#define MAXFREECONTEXT      200
#define MAXIOBUFFERSIZE     1024
#define MAXWORKTHREADNUM    10
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

	void SetInitPostAsynAcceptCnt(unsigned int count){m_unInitAsynAcceptCnt = count;}
	void SetInitWorkThreadCnt(unsigned int count){m_unInitWorkThreadCnt = (count > MAXWORKTHREADNUM ? MAXWORKTHREADNUM : count);}
private:

	BOOL CheckAndGoAwayViciousLink();

	//监听线程处理函数
	static unsigned int _stdcall ListenWorkThread(LPVOID param);
	unsigned int _stdcall ListenWorkThread();

	//工作线程处理函数
	static unsigned int _stdcall WorkThread(LPVOID param);
	unsigned int _stdcall WorkThread();


	//I/O)_per内存管理相关函数
	PCIOCPBuffer AllocIOBuffer(int bufLen = MAXIOBUFFERSIZE);
	void FreeIOBuffer(PCIOCPBuffer pIOBuf);
	void FreeAllIOBuffer();

	//handle_per内存管理相关函数
	PCIOCPContext AllocContextPer(SOCKET s);
	void FreeContextPer(PCIOCPContext pContextPer);
	void FreeAllContextPer();


	//连接链表管理相关函数
	BOOL AddConnected(PCIOCPContext pContext);
	void CloseConnected(PCIOCPContext pContext);
	void CloseAllConnected();


	//投递的异步accept请求的链表管理相关函数
	BOOL AddPendingAccept(PCIOCPBuffer pIOBuffer);
	BOOL RemovePendingAccept(PCIOCPBuffer pIOBuffer);

	//异步I/O投递相关函数
	BOOL PostAccept(PCIOCPBuffer pBuffer);
	BOOL PostSend(PCIOCPContext pContext, PCIOCPBuffer pBuffer);
	BOOL PostRecv(PCIOCPContext pContext, PCIOCPBuffer pBuffer);



private:
	 bool m_bServerStarted;
	 bool m_bShutDown;

	 WORD m_wPort;
	 unsigned int m_nMaxConnections;    //本服务器允许的最大联入数
	 unsigned int m_nMaxFreeBuffers;    //本服务器运行的最大IO唯一数据的个数
	 unsigned int m_nMaxFreeContexts;   //本服务器运行的最大handle唯一数据的个数
	 unsigned int m_nInitialReads;

	 SOCKET m_sListen;
	 HANDLE m_hCompletion;

	 HANDLE m_hAcceptEvent;  


	 //I/O-per内存管理链表相关变量
	 PCIOCPBuffer  m_pFreeBufferList;
	 unsigned int  m_unFreeBufferCount;
	 CRITICAL_SECTION m_csFreeBufferListLock;
	
	 //handle-per内存管理链表相关变量
	 PCIOCPContext m_pFreeContextList;
	 unsigned int  m_unFreeContextCount;
	 CRITICAL_SECTION m_csFreeContextListLock;

	 //连接链表管理相关变量
	 PCIOCPContext m_pConnectedList;    // 已经连接的句柄对象链表
	 unsigned int  m_unCurrentConnectedCount;
	 CRITICAL_SECTION m_csConnectedListLock;

	 //投递的异步accept请求管理的I/O-per数据结构的链表管理相关变量
	 PCIOCPBuffer m_pPostAcceptBufList;
	 unsigned int m_unCuttentPostAcceptBufCount;
	 CRITICAL_SECTION m_csPostAcceptBufListLock;

	 

	HANDLE m_hListenThread;   //监听线程的句柄


	unsigned int m_unInitAsynAcceptCnt;
	unsigned int m_unInitWorkThreadCnt;

	 LPFN_ACCEPTEX m_lpfnAcceptEx;	
	 LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockaddrs; 
};