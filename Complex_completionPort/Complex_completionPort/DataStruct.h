//per-IO
#include <WinSock2.h>
#include <Windows.h>
typedef struct _tagCIOCPBuffer
{
	WSAOVERLAPPED ol;

	SOCKET sClient;   //AcceptEx recv client socket

	ULONG  nSequenceNum;  //???????????

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



// ����per-Handle���ݡ���������һ���׽��ֵ���Ϣ
typedef struct _tagCIOCPContext
{
	SOCKET s;						// �׽��־��

	SOCKADDR_IN addrLocal;			// ���ӵı��ص�ַ
	SOCKADDR_IN addrRemote;			// ���ӵ�Զ�̵�ַ

	BOOL bClosing;					// �׽����Ƿ�ر�

	int nOutstandingRecv;			// ���׽������׳����ص�����������
	int nOutstandingSend;


	ULONG nReadSequence;			// ���Ÿ����յ���һ�����к�
	ULONG nCurrentReadSequence;		// ��ǰҪ�������к�
	PCIOCPBuffer pOutOfOrderReads;	// ��¼û�а�˳����ɵĶ�I/O

	CRITICAL_SECTION Lock;			// ��������ṹ

	_tagCIOCPContext *pNext;

	_tagCIOCPContext()
	{
		memset(&addrLocal, 0x0, sizeof(SOCKADDR_IN));
		memset(&addrRemote, 0x0, sizeof(SOCKADDR_IN));

		s = INVALID_SOCKET;
		bClosing = TRUE;
		nOutstandingRecv     = 0;
		nOutstandingSend     = 0;
		nReadSequence        = 0;
		nCurrentReadSequence = 0;
		pOutOfOrderReads     = NULL;

		pNext                = NULL;
	}

private:
	_tagCIOCPContext(const _tagCIOCPContext& );
	_tagCIOCPContext& operator=(const _tagCIOCPContext& );

}CIOCPContext, *PCIOCPContext;