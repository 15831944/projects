//per-IO
#ifndef COMPLEXCOMPLETIONPORT_DATASTRUCT_HEAD
#define COMPLEXCOMPLETIONPORT_DATASTRUCT_HEAD 
#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>

class CIOCPSer;

enum ESocketStatu
{
	InitSocke = 1,     //��һ��malloc��״̬
	CloseSock     = 2,
	OpenSock     = 3   //�Ѿ����ӳɹ���socket��״̬
};

//I/O-per
typedef struct _tagCIOCPBuffer
{
	friend CIOCPSer;

private:
	WSAOVERLAPPED ol;

	SOCKET sClient;   //AcceptEx recv client socket

	ULONG  nSequenceNum;  //(��ʱû��ʹ��)

	char *buff;   //���buff��ָ���ڴ�����������˽ṹ��֮��
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



//Handle-per���ݡ���������һ���׽��ֵ���Ϣ
typedef struct _tagCIOCPContext
{
	friend CIOCPSer;

private:
	SOCKET s;						// �׽��־��

	SOCKADDR_IN addrLocal;			// ���ӵı��ص�ַ
	SOCKADDR_IN addrRemote;			// ���ӵ�Զ�̵�ַ

	ESocketStatu eSockState;			     // �׽��ֹرտ���״̬

	unsigned int nOutstandingRecv;			// ���׽������׳����ص�����������
	unsigned int nOutstandingSend;

	unsigned int nMaxAsynRecvCount;         //�����첽������
	unsigned int nMaxAsynSendCount;

	ULONG nReadSequence;			// ���Ÿ����յ���һ�����к�    ����ʱδʹ�ã�
	ULONG nCurrentReadSequence;		// ��ǰҪ�������к�             ����ʱδʹ�ã�
	PCIOCPBuffer pOutOfOrderReads;	// ��¼û�а�˳����ɵĶ�I/O

	CRITICAL_SECTION Lock;			// ��������ṹ���˽ṹ���ܱ�����̷߳���

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
	//ȷ������ֻ��ͨ���ҵĴ�������������������ֱ�Ӷ������
	_tagCIOCPContext(){}   
	_tagCIOCPContext(const _tagCIOCPContext& );
	_tagCIOCPContext& operator=(const _tagCIOCPContext& );

}CIOCPContext, *PCIOCPContext;


#endif