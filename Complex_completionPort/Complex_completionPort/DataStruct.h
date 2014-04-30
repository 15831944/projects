//per-IO
#include <WinSock2.h>
#include <Windows.h>

enum ESocketStatu
{
	InitSocke = 1,
	CloseSock     = 2,
	OpenSock     = 3
};

//I/O-per
typedef struct _tagCIOCPBuffer
{
	WSAOVERLAPPED ol;

	SOCKET sClient;   //AcceptEx recv client socket

	ULONG  nSequenceNum;  //???????????(暂时没有使用)

	char *buff;   //这个buff所指的内存区必须紧跟此结构体之后
	int  buffLen;

	int nOperation;
#define OP_UNKNOW   0
#define OP_ACCEPT	1
#define OP_WRITE	2
#define OP_READ		3

	_tagCIOCPBuffer *pBuffer;

	_tagCIOCPBuffer()
	{
		sClient		 = INVALID_SOCKET;
		nSequenceNum = 0;
		buff         = NULL;
		buffLen      = 0;
		nOperation   = OP_UNKNOW;
		pBuffer      = NULL;
	}

	~_tagCIOCPBuffer()      
	{
	/*	if (buff)
		{
			free(buff);
			buff = NULL;
		}*/
	}

private:
	_tagCIOCPBuffer(const _tagCIOCPBuffer& );
	_tagCIOCPBuffer& operator=(const _tagCIOCPBuffer& ); 

}CIOCPBuffer, *PCIOCPBuffer;



//Handle-per数据。它包含了一个套节字的信息
typedef struct _tagCIOCPContext
{
	SOCKET s;						// 套节字句柄

	SOCKADDR_IN addrLocal;			// 连接的本地地址
	SOCKADDR_IN addrRemote;			// 连接的远程地址

	ESocketStatu eSockState;					// 套节字关闭开启状态

	int nOutstandingRecv;			// 此套节字上抛出的重叠操作的数量
	int nOutstandingSend;

	int nMaxAsynRecvCount;
	int nMaxAsynSendCount;

	ULONG nReadSequence;			// 安排给接收的下一个序列号
	ULONG nCurrentReadSequence;		// 当前要读的序列号
	PCIOCPBuffer pOutOfOrderReads;	// 记录没有按顺序完成的读I/O

	//CRITICAL_SECTION Lock;			// 保护这个结构

	_tagCIOCPContext *pNext;

	_tagCIOCPContext()
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

	void setMaxAsynSendCnt(int count){nMaxAsynSendCount = count;}
	void setMaxAsynRecvCnt(int count){nMaxAsynRecvCount = count;}

private:
	_tagCIOCPContext(const _tagCIOCPContext& );
	_tagCIOCPContext& operator=(const _tagCIOCPContext& );

}CIOCPContext, *PCIOCPContext;