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
	�� �� �� �ܣ���������
	�� �� ˵ ����
	������                   ��������
	wPort                   �����˿ں�
	nMaxConnections         ��������������
	nMaxFreeBuffers         ���������������ͷŵ�IOΨһ���ݽṹ����(�����Ŀ���IOΨһ���ݽṹ����)
	nMaxFreeContexts		���������������ͷŵľ��Ψһ���ݽṹ����(�����Ŀ��о��Ψһ���ݽṹ����)

	*************************************/
	bool StartSer(unsigned long &errorCode, WORD wPort = LISTENPORT, unsigned int nMaxConnections = MAXCONNECTEDCOUNT, 
				  unsigned int nMaxFreeBuffers = MAXFREEBUFFER, unsigned int nMaxFreeContexts = MAXFREECONTEXT, unsigned int nInitialReads = 4);

	/**********************************
	�� �� �� �ܣ�ֹͣ����
	***********************************/
	void StopSer();

	/*�ȴ���������˳���������̲߳��������˳��Ļ����Բ��õ���*/
	void join();

	void SetInitPostAsynAcceptCnt(unsigned int count){m_unInitAsynAcceptCnt = count;}    //�˺�����StartSerǰ����
	void SetInitWorkThreadCnt(unsigned int count){m_unInitWorkThreadCnt = (count > MAXWORKTHREADNUM ? MAXWORKTHREADNUM : count);}
private:

	//�����̴߳�����
	static unsigned int _stdcall ListenWorkThread(LPVOID param);
	unsigned int _stdcall ListenWorkThread();

	//�����̴߳�����
	static unsigned int _stdcall WorkThread(LPVOID param);
	unsigned int _stdcall WorkThread();


	//I/O-per�ڴ������غ���
	PCIOCPBuffer AllocIOBuffer(int bufLen = MAXIOBUFFERSIZE);
	void FreeIOBuffer(PCIOCPBuffer pIOBuf, bool bForce = false);
	void FreeAllIOBuffer();

	//handle-per�ڴ������غ���
	PCIOCPContext AllocContextPer(SOCKET s, unsigned int MaxRecvCnt = 200, unsigned MaxSendCnt = 200);
	void FreeContextPer(PCIOCPContext pContextPer,  bool force = false);
	void FreeAllContextPer();


	//�������������غ���(�൱��session����), ������Ե�����дһ��session������
	BOOL AddConnectedList(PCIOCPContext pContext);
	void RemoveAndCloseConnected(PCIOCPContext pContext);
	void RemoveAllConnected();


	//Ͷ�ݵ��첽accept��������������غ���
	BOOL AddPendingAccept(PCIOCPBuffer pIOBuffer);
	BOOL RemovePendingAccept(PCIOCPBuffer pIOBuffer);

	//�첽I/OͶ����غ���
	BOOL PostAccept(PCIOCPBuffer pBuffer);
	BOOL PostSend(PCIOCPContext pContext, PCIOCPBuffer pBuffer);
	BOOL PostRecv(PCIOCPContext pContext, PCIOCPBuffer pBuffer);

	//һЩ������
	BOOL CheckAndGoAwayViciousLink();

	//�첽I/O�¼�������
	void DealFunction(PCIOCPContext pContext, PCIOCPBuffer pBuffer, DWORD dwTransBytes);



private:
	 bool m_bServerStarted;          //�����������
	 bool m_bShutDown;               //����ر�/ֹͣ���

	 WORD m_wPort;                      //�����˿�
	 unsigned int m_nMaxConnections;    //����������������������
	 unsigned int m_nMaxFreeBuffers;    //�����������е�������IOΨһ���ݵĸ���
	 unsigned int m_nMaxFreeContexts;   //�����������е�������handleΨһ���ݵĸ���
	 unsigned int m_nInitialReads;

	 SOCKET m_sListen;
	 HANDLE m_hCompletion;

	 HANDLE m_hAcceptEvent;        //����ʱ��ʾ����acceptex���첽IO���Ѿ����꣬������
	 HANDLE m_hRepostEvent;        //����ʱ��ʾ������һ��acceptex
	 HANDLE m_hShutDownEvent;      //����ʱ��ʾֹͣ���񣬽���ֹͣ����


	 //I/O-per�ڴ����������ر���
	 PCIOCPBuffer  m_pFreeBufferList;
	 unsigned int  m_unFreeBufferCount;
	 CRITICAL_SECTION m_csFreeBufferListLock;
	
	 //handle-per�ڴ����������ر���
	 PCIOCPContext m_pFreeContextList;
	 unsigned int  m_unFreeContextCount;
	 CRITICAL_SECTION m_csFreeContextListLock;

	 //�������������ر���
	 PCIOCPContext m_pConnectedList;    // �Ѿ������ϵľ����������
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

	 long m_errorCode;
};

#endif