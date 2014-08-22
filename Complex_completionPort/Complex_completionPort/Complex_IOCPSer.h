#ifndef COMPLEXCOMPLETIONPORT_COMPLEXIOCPSER_HEAD
#define COMPLEXCOMPLETIONPORT_COMPLEXIOCPSER_HEAD
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
	nMaxFreeBuffers         允许的最大不用真正释放的IO唯一数据结构个数(即最大的空闲IO唯一数据结构个数)
	nMaxFreeContexts		允许的最大不用真正释放的句柄唯一数据结构个数(即最大的空闲句柄唯一数据结构个数)

	*************************************/
	bool StartSer(unsigned long &errorCode, WORD wPort = LISTENPORT, unsigned int nMaxConnections = MAXCONNECTEDCOUNT, 
				  unsigned int nMaxFreeBuffers = MAXFREEBUFFER, unsigned int nMaxFreeContexts = MAXFREECONTEXT, unsigned int nInitialReads = 4);

	/**********************************
	函 数 功 能：停止服务
	***********************************/
	void StopSer();

	/*等待这个服务退出，如果主线程不会立即退出的话可以不用调用*/
	void join();

	void SetInitPostAsynAcceptCnt(unsigned int count){m_unInitAsynAcceptCnt = count;}    //此函数在StartSer前调用
	void SetInitWorkThreadCnt(unsigned int count){m_unInitWorkThreadCnt = (count > MAXWORKTHREADNUM ? MAXWORKTHREADNUM : count);}
private:

	//监听线程处理函数
	static unsigned int _stdcall ListenWorkThread(LPVOID param);
	unsigned int _stdcall ListenWorkThread();

	//工作线程处理函数
	static unsigned int _stdcall WorkThread(LPVOID param);
	unsigned int _stdcall WorkThread();


	//I/O-per内存管理相关函数
	PCIOCPBuffer AllocIOBuffer(int bufLen = MAXIOBUFFERSIZE);
	void FreeIOBuffer(PCIOCPBuffer pIOBuf, bool bForce = false);
	void FreeAllIOBuffer();

	//handle-per内存管理相关函数
	PCIOCPContext AllocContextPer(SOCKET s, unsigned int MaxRecvCnt = 200, unsigned MaxSendCnt = 200);
	void FreeContextPer(PCIOCPContext pContextPer,  bool force = false);
	void FreeAllContextPer();


	//连接链表管理相关函数(相当于session管理), 这里可以单独的写一个session管理类
	BOOL AddConnectedList(PCIOCPContext pContext);
	void RemoveAndCloseConnected(PCIOCPContext pContext);
	void RemoveAllConnected();


	//投递的异步accept请求的链表管理相关函数
	BOOL AddPendingAccept(PCIOCPBuffer pIOBuffer);
	BOOL RemovePendingAccept(PCIOCPBuffer pIOBuffer);

	//异步I/O投递相关函数
	BOOL PostAccept(PCIOCPBuffer pBuffer);
	BOOL PostSend(PCIOCPContext pContext, PCIOCPBuffer pBuffer);
	BOOL PostRecv(PCIOCPContext pContext, PCIOCPBuffer pBuffer);

	//一些处理函数
	BOOL CheckAndGoAwayViciousLink();

	//异步I/O事件处理函数
	void DealFunction(PCIOCPContext pContext, PCIOCPBuffer pBuffer, DWORD dwTransBytes);



private:
	 bool m_bServerStarted;          //服务启动标记
	 bool m_bShutDown;               //服务关闭/停止标记

	 WORD m_wPort;                      //监听端口
	 unsigned int m_nMaxConnections;    //本服务器允许的最大联入数
	 unsigned int m_nMaxFreeBuffers;    //本服务器运行的最大空闲IO唯一数据的个数
	 unsigned int m_nMaxFreeContexts;   //本服务器运行的最大空闲handle唯一数据的个数
	 unsigned int m_nInitialReads;

	 SOCKET m_sListen;
	 HANDLE m_hCompletion;

	 HANDLE m_hAcceptEvent;        //受信时表示处理acceptex的异步IO后已经用完，不够了
	 HANDLE m_hRepostEvent;        //受信时表示接收了一个acceptex
	 HANDLE m_hShutDownEvent;      //受信时表示停止服务，进行停止处理


	 //I/O-per内存管理链表相关变量
	 PCIOCPBuffer  m_pFreeBufferList;
	 unsigned int  m_unFreeBufferCount;
	 CRITICAL_SECTION m_csFreeBufferListLock;
	
	 //handle-per内存管理链表相关变量
	 PCIOCPContext m_pFreeContextList;
	 unsigned int  m_unFreeContextCount;
	 CRITICAL_SECTION m_csFreeContextListLock;

	 //连接链表管理相关变量
	 PCIOCPContext m_pConnectedList;    // 已经连接上的句柄对象链表
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

	 long m_errorCode;
};

#endif