//per-IO
#ifndef COMPLEXCOMPLETIONPORT_DATASTRUCT_HEAD
#define COMPLEXCOMPLETIONPORT_DATASTRUCT_HEAD 
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>

class CIOCPSer;

enum ESocketStatu
{
	InitSocke = 1,     //第一次malloc的状态
	CloseSock     = 2,
	OpenSock     = 3   //已经连接成功后socket的状态
};

//I/O-per
typedef struct _tagCIOCPBuffer
{
	friend CIOCPSer;

private:
	WSAOVERLAPPED ol;

	SOCKET sClient;   //AcceptEx recv client socket

	ULONG  nSequenceNum;  //(暂时没有使用)

	char *buff;   //这个buff所指的内存区必须紧跟此结构体之后
	int  buffLen;

	int nOperation;
#define OP_UNKNOW   0
#define OP_ACCEPT	1
#define OP_WRITE	2
#define OP_READ		3

	_tagCIOCPBuffer *pBuffer;

	void InitStruct()
	{
		sClient		 = INVALID_SOCKET;
		nSequenceNum = 0;
		buff         = NULL;
		buffLen      = 0;
		nOperation   = OP_UNKNOW;
		pBuffer      = NULL;
	}

	void ReturnIniStatus()
	{
		sClient		 = INVALID_SOCKET;
		nSequenceNum = 0;
		buff         = NULL;
		//buffLen      = 0;
		nOperation   = OP_UNKNOW;
		pBuffer      = NULL;
	}

	void ReleaseStruct()      
	{
	/*	if (buff)
		{
			free(buff);
			buff = NULL;
		}*/
		//if (NULL != sClient && INVALID_SOCKET != sClient)
		//{
		//	closesocket(sClient);
		//	sClient = NULL;
		//}
	}

private:
	_tagCIOCPBuffer(){}
	_tagCIOCPBuffer(const _tagCIOCPBuffer& );
	_tagCIOCPBuffer& operator=(const _tagCIOCPBuffer& ); 

}CIOCPBuffer, *PCIOCPBuffer;



//Handle-per数据。它包含了一个套节字的信息
typedef struct _tagCIOCPContext
{
	friend CIOCPSer;

private:
	SOCKET s;						// 套节字句柄

	SOCKADDR_IN addrLocal;			// 连接的本地地址
	SOCKADDR_IN addrRemote;			// 连接的远程地址

	ESocketStatu eSockState;			     // 套节字关闭开启状态

	unsigned int nOutstandingRecv;			// 此套节字上抛出的重叠操作的数量
	unsigned int nOutstandingSend;

	unsigned int nMaxAsynRecvCount;         //最大的异步操作数
	unsigned int nMaxAsynSendCount;

	ULONG nReadSequence;			// 安排给接收的下一个序列号    （暂时未使用）
	ULONG nCurrentReadSequence;		// 当前要读的序列号             （暂时未使用）
	PCIOCPBuffer pOutOfOrderReads;	// 记录没有按顺序完成的读I/O

	CRITICAL_SECTION Lock;			// 保护这个结构，此结构可能被多个线程访问

	_tagCIOCPContext *pNext;

public:
	void InitStruct(unsigned int MaxRecvCount = 200, unsigned int MaxSendCount = 200)
	{
		memset(&addrLocal, 0x0, sizeof(SOCKADDR_IN));
		memset(&addrRemote, 0x0, sizeof(SOCKADDR_IN));

		s = INVALID_SOCKET;
		eSockState = InitSocke;
		nOutstandingRecv     = 0;
		nOutstandingSend     = 0;
		nReadSequence        = 0;
		nCurrentReadSequence = 0;
		pOutOfOrderReads     = NULL;
		nMaxAsynRecvCount    = MaxRecvCount;
		nMaxAsynSendCount    = MaxSendCount;

		pNext                = NULL;

		InitializeCriticalSection(&Lock);
	}

	

	void ReleaseStruct()
	{
		if(NULL != s && INVALID_SOCKET != s)
		{
			closesocket(s);
			s = NULL;
		}

		assert(pNext != NULL);
		DeleteCriticalSection(&Lock);
	}

	void ReturnInitStatus()
	{
		memset(&addrLocal, 0x0, sizeof(SOCKADDR_IN));
		memset(&addrRemote, 0x0, sizeof(SOCKADDR_IN));

		s = INVALID_SOCKET;
		eSockState = InitSocke;
		nOutstandingRecv     = 0;
		nOutstandingSend     = 0;
		nReadSequence        = 0;
		nCurrentReadSequence = 0;
		pOutOfOrderReads     = NULL;
		nMaxAsynRecvCount    = 200;
		nMaxAsynSendCount    = 200;

		pNext                = NULL;
	}


	void SetMaxAsynSendCnt(unsigned int count){nMaxAsynSendCount = count;}
	void SetMaxAsynRecvCnt(unsigned int count){nMaxAsynRecvCount = count;}

//	unsigned GetOutStandingRecvCnt() const{return nOutstandingRecv;}
//	unsigned GetOutStandingSendCnt() const{return nOutstandingSend;}

private:
	//确保此类只能通过我的创建函数来创建，不能直接定义对象
	_tagCIOCPContext(){}   
	_tagCIOCPContext(const _tagCIOCPContext& );
	_tagCIOCPContext& operator=(const _tagCIOCPContext& );

}CIOCPContext, *PCIOCPContext;


#endif