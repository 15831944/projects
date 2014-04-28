#include "DataStruct.h"
#include <WinSock2.h>
#include <Windows.h>
#include <Mswsock.h>
#include <process.h>


#define MAXCONNECTEDCOUNT   2000
#define MAXFREEBUFFER       200
#define MAXFREECONTEXT      200
#define MAXIOBUFFERSIZE     1024
#define COMPLLEXIOCPSERLOG  "complexIOCPLog"

#define LISTENPORT 4567
class CIOCPSer
{
public:
	CIOCPSer();
	~CIOCPSer();

	/**********************************
	�� �� �� �ܣ���������
	�� �� ˵ ����
	������                   ��������
	wPort                   �����˿ں�
	nMaxConnections         ��������������
	nMaxFreeBuffers         ���������������ͷŵ�IOΨһ���ݽṹ����
	nMaxFreeContexts		���������������ͷŵľ��Ψһ���ݽṹ����

	*************************************/
	bool StartSer(WORD wPort = 4567, unsigned int nMaxConnections = MAXCONNECTEDCOUNT, 
				  unsigned int nMaxFreeBuffers = 200, unsigned int nMaxFreeContexts = 100, unsigned int nInitialReads = 4);
private:

	//�����̴߳�����
	static unsigned int _stdcall ListenWorkThread(LPVOID parame);
	unsigned int _stdcall ListenWorkThread();

	PCIOCPBuffer AllocIOBuffer(int bufLen = MAXIOBUFFERSIZE);
	void FreeIOBuffer(PCIOCPBuffer pIOBuf);
	void FreeAllIOBuffer();

	PCIOCPContext AllocContextPer(SOCKET s);
	void FreeContextPer(PCIOCPContext pContextPer);
	void FreeAllContextPer();

	BOOL AddConnected(PCIOCPContext pContext);
	void CloseConnected(PCIOCPContext pContext);
	void CloseAllConnected();



private:
	 bool m_bServerStarted;
	 bool m_bShutDown;

	 WORD m_wPort;
	 unsigned int m_nMaxConnections;    //����������������������
	 unsigned int m_nMaxFreeBuffers;    //�����������е����IOΨһ���ݵĸ���
	 unsigned int m_nMaxFreeContexts;   //�����������е����handleΨһ���ݵĸ���
	 unsigned int m_nInitialReads;

	 SOCKET m_sListen;
	 HANDLE m_hCompletion;

	 HANDLE m_hAcceptEvent;  


	 //handle-per and I/O-per �ڴ����������ر���
	 PCIOCPBuffer  m_pFreeBufferList;
	 PCIOCPContext m_pFreeContextList;
	 unsigned int  m_unFreeBufferCount;
	 unsigned int  m_unFreeContextCount;
	 CRITICAL_SECTION m_csFreeBufferListLock;
	 CRITICAL_SECTION m_csFreeContextListLock;

	 //��������
	 PCIOCPContext m_pConnectedList;    // �Ѿ����ӵľ����������
	 unsigned int  m_unCurrentConnectedCount;
	 CRITICAL_SECTION m_csConnectedListLock;

	 

	HANDLE m_hListenThread;   //�����̵߳ľ��

	 LPFN_ACCEPTEX m_lpfnAcceptEx;	
	 LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockaddrs; 
};