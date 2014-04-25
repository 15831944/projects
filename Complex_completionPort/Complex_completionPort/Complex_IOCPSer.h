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



private:
	 bool m_bServerStarted;
	 bool m_bShutDown;

	 WORD m_wPort;
	 unsigned int m_nMaxConnections;    //����������������������
	 unsigned int m_nMaxFreeBuffers;    //�����������е����IOΨһ���ݵĸ���
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