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

	void SetInitPostAsynAcceptCnt(unsigned int count){m_unInitAsynAcceptCnt = count;}
	void SetInitWorkThreadCnt(unsigned int count){m_unInitWorkThreadCnt = (count > MAXWORKTHREADNUM ? MAXWORKTHREADNUM : count);}
private:

	BOOL CheckAndGoAwayViciousLink();

	//�����̴߳�����
	static unsigned int _stdcall ListenWorkThread(LPVOID param);
	unsigned int _stdcall ListenWorkThread();

	//�����̴߳�����
	static unsigned int _stdcall WorkThread(LPVOID param);
	unsigned int _stdcall WorkThread();


	//I/O)_per�ڴ������غ���
	PCIOCPBuffer AllocIOBuffer(int bufLen = MAXIOBUFFERSIZE);
	void FreeIOBuffer(PCIOCPBuffer pIOBuf);
	void FreeAllIOBuffer();

	//handle_per�ڴ������غ���
	PCIOCPContext AllocContextPer(SOCKET s);
	void FreeContextPer(PCIOCPContext pContextPer);
	void FreeAllContextPer();


	//�������������غ���
	BOOL AddConnected(PCIOCPContext pContext);
	void CloseConnected(PCIOCPContext pContext);
	void CloseAllConnected();


	//Ͷ�ݵ��첽accept��������������غ���
	BOOL AddPendingAccept(PCIOCPBuffer pIOBuffer);
	BOOL RemovePendingAccept(PCIOCPBuffer pIOBuffer);

	//�첽I/OͶ����غ���
	BOOL PostAccept(PCIOCPBuffer pBuffer);
	BOOL PostSend(PCIOCPContext pContext, PCIOCPBuffer pBuffer);
	BOOL PostRecv(PCIOCPContext pContext, PCIOCPBuffer pBuffer);



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


	 //I/O-per�ڴ����������ر���
	 PCIOCPBuffer  m_pFreeBufferList;
	 unsigned int  m_unFreeBufferCount;
	 CRITICAL_SECTION m_csFreeBufferListLock;
	
	 //handle-per�ڴ����������ر���
	 PCIOCPContext m_pFreeContextList;
	 unsigned int  m_unFreeContextCount;
	 CRITICAL_SECTION m_csFreeContextListLock;

	 //�������������ر���
	 PCIOCPContext m_pConnectedList;    // �Ѿ����ӵľ����������
	 unsigned int  m_unCurrentConnectedCount;
	 CRITICAL_SECTION m_csConnectedListLock;

	 //Ͷ�ݵ��첽accept��������I/O-per���ݽṹ�����������ر���
	 PCIOCPBuffer m_pPostAcceptBufList;
	 unsigned int m_unCuttentPostAcceptBufCount;
	 CRITICAL_SECTION m_csPostAcceptBufListLock;

	 

	HANDLE m_hListenThread;   //�����̵߳ľ��


	unsigned int m_unInitAsynAcceptCnt;
	unsigned int m_unInitWorkThreadCnt;

	 LPFN_ACCEPTEX m_lpfnAcceptEx;	
	 LPFN_GETACCEPTEXSOCKADDRS m_lpfnGetAcceptExSockaddrs; 
};