//per-IO
#include <WinSock2.h>
#include <Windows.h>

enum ESocketStatu
{
	InitSocke = 1,     //��һ��malloc��״̬
	CloseSock     = 2,
	OpenSock     = 3   //�Ѿ����ӳɹ���socket��״̬
};

//I/O-per
typedef struct _tagCIOCPBuffer
{
	WSAOVERLAPPED ol;

	SOCKET sClient;   //AcceptEx recv client socket

	ULONG  nSequenceNum;  //???????????(��ʱû��ʹ��)

	char *buff;   //���buff��ָ���ڴ�����������˽ṹ��֮��
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



//Handle-per���ݡ���������һ���׽��ֵ���Ϣ
typedef struct _tagCIOCPContext
{
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

	//CRITICAL_SECTION Lock;			// ��������ṹ  �����������濼�ǣ���������϶����еģ�

	_tagCIOCPContext *pNext;

	_tagCIOCPContext(unsigned int MaxRecvCount = 200, unsigned int MaxSendCount = 200)
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

	unsigned GetOutStandingRecvCnt() const{return nOutstandingRecv;}
	unsigned GetOutStandingSendCnt() const{return nOutstandingSend;}

private:
	_tagCIOCPContext(const _tagCIOCPContext& );
	_tagCIOCPContext& operator=(const _tagCIOCPContext& );

}CIOCPContext, *PCIOCPContext;