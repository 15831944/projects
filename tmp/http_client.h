#ifndef HTTPCLIENT_HEAD
#define HTTPCLIENT_HEAD

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <WinSock2.h>
#include <process.h>
#include <list>
#include <vector>
#include "Interface.h"
#include "lock.h"



#define INVALID_TASK_ID 0xFFFFFFFF
#define MAXCONCOUNT     4

#define SUCCSEE                        0


#define COMPRESSIONDATA_ERROR          -88
#define TASK_NO_REGISTER_ERROR         -89
#define PAUSELOACTION_ERROR            -90
#define BUILDHTTPHEAD_ERROR            -91
#define CONNECTHTTPSER_ERROR           -92
#define PARESHTTPURL_ERROR             -93
#define INVALUE_SOCKINFOPTR_ERROR      -94
#define PARARM_URL_EMPTYERROR          -95
#define SOCKCLOSE                      -96
#define NOTINISOCKET                   -97
#define NOTRECVFINFSHHEAD              -98
#define NEWMEMORY_ERROR                -99
#define GETCONTENTLENGTH_ERROR         -100


#define HTTPDOWNING_INFO              -200
#define HTTPDOWNINGREDIRECT_INFO      -201
#define HTTPHEADDONOTFIN_INFO         -202



/*
版 本 号：
（1）1.0.0.1：
   0.采用的是阻塞模式+select的结构；
   1.仅支持简单的发送与接收功能；
   2.支持多任务的下载；
   3.仅支持get方法；
   4.不支持内部跳转功能；
   5.不支持同一任务的多次http下载任务
   6.不支持gzip格式 
   7.不支持chucked模式；
   8.不支持并行下载；
（2）1.0.0.2：
   1.支持内部跳转开关；(默认不支持)
   2.支持同一任务的多次http下载任务；（这里不是并行下载的意思，在同一时刻只允许一个下载（串行下载））
（3）1.0.0.3：
    1.支持gzip模式数据的传输
	2.支持chucked模式
创建时间：2014.10.29

*/

class CHttpClient;

typedef struct _tagSSockInfo
{
public:
	friend class CHttpClient;

	_tagSSockInfo(unsigned taskID, IRecvInterface *recvSink)
		: s_iSink(recvSink)
		, s_unTaskID(taskID)
		, s_hSocket(INVALID_SOCKET)
		, s_cDataBuf(NULL)
		, s_bIsHasRecvHttpHead(false)
		, s_bIsHasRecvContentData(false)
		, s_bHttpDowning(false)
		, s_bIsContentGzip(false)
		, s_unHasHeadLen(0)
		, s_unContentLen(0)
		, s_unHasRecvContentData(0)
		, s_dm(e_UNKNOW)
		, s_bIsChunkedRecvLen(false)
		, s_unCacheHasLen(0)
		, s_bJumpStart(false)
	{
		memset(s_cHeadBuf, 0x0, 2049);
		memset(s_strCacheBuf, 0x0, 2048);
	}

	~_tagSSockInfo()
	{
		if (s_hSocket)
		{
			closesocket(s_hSocket);
			s_hSocket = INVALID_SOCKET;
		}

		if (s_cDataBuf)
		{
			delete[] s_cDataBuf;
			s_cDataBuf = NULL;
		}
	}
private:

	bool GetAttributeSection(std::string strAttr, std::string &strValue);
	bool GetContentLength(size_t &len);
	bool GetContentEncodeIsGzip();
	bool GetContentIsTransferEncoding();
	bool GetLocationUrl();
	unsigned long GetHttpState();
	void RestoreInitialState();
	void RedirectSetUp();

	bool RecvHeadData(int &errorCode);
	bool RecvContentData(int &errorCode);
	bool OnDealWithHttpHead(int &errorCode);
	bool OndealWithChunckData();

	//bool GetContentRange(size_t &pos, size_t &len, size_t &total);
	

private:
	enum DONEMETHODS
	{
		e_UNKNOW,
		e_CONTENTLENG,
		e_CHUNKED
	};

private:



	SOCKET            s_hSocket;
	IRecvInterface    *s_iSink;
	enum DONEMETHODS  s_dm;
	unsigned          s_unTaskID;
	bool              s_bHttpDowning;
	bool              s_bIsContentGzip;
	CLock             s_owerLock;    //Temporary unused


	std::string s_strOrigUrl;   //The original url
	std::string s_strJumpUrl;	//jump url  
	std::string s_strHeadExpand;

	//{content data param
	char      *s_cDataBuf;  
	unsigned  s_unContentLen;
	unsigned  s_unHasRecvContentData;
	bool      s_bIsHasRecvContentData;
	//}

	//{http response head data param
	char      s_cHeadBuf[2049];     ////add '\0' in the string tail, s_cHeadBuf[2048] = '\0'
	unsigned  s_unHasHeadLen;          //include /r/n/r/n
	bool      s_bIsHasRecvHttpHead;   //include /r/n/r/n
	//}

	//{chuncked model
	char                     s_strCacheBuf[2048];
	unsigned                 s_unCacheHasLen;
	unsigned                 s_unCacheBufStartPos;
	bool                     s_bIsChunkedRecvLen;
	unsigned                 s_unThisRemainChunkedLen;
	bool                     s_bJumpStart;
    
	//}
}SSockInfo, *PSSockInfo;

typedef boost::shared_ptr<SSockInfo> SSockInfo_PTR;


class CHttpClient
{
public:
	CHttpClient();
	~CHttpClient();

	//start http server
	bool StartSer();
	//stop http server
	bool StopSer();


	void SetRedirectIntenel(bool bON = false){m_bRedirect = bON;}
	unsigned GetHttpDownloadingState(unsigned unTaskID);

	bool RegisterTask(unsigned taskID, IRecvInterface *recvSink);
	bool cancelTask(unsigned taskID);
//	bool StopHttpdownloading(unsigned taskID);
	bool Get(const unsigned unTaskID, const std::string &strUrl, int &errorCode, const std::string &strHeadExpand = "", bool bJump = false);
	bool GetDataBuf(const unsigned unTaskID, std::string &strData);
	bool GetDataLen(const unsigned unTaskID, unsigned int &unDataLen);

private:

	//{related to send http requset function
	bool ParseHttpUrl(std::string strUrl, std::string &strHostName, unsigned &strHostPort, std::string &strHostObj);
	bool Connect(SSockInfo_PTR sockInfo_ptr, std::string strHostName, unsigned strHostPort, unsigned outSec = 5, unsigned MaxCount = MAXCONCOUNT);
	bool BuildSendData(std::ostringstream &httpHead, std::string strHostName, std::string strHostObj, std::string strHeadExpand);
	bool Send(SSockInfo_PTR sockInfo, const char *sendBuf, unsigned sendLen, int &errorCode);
	//}

	//{receive data thread 
	static unsigned int __stdcall RecvThread(void *param);
	unsigned int __stdcall RecvThread();
	//}

	//{help function
	SSockInfo_PTR GetSockInfoBySock(const SOCKET readSock);
    void StopSerOnOwn();
	void RecycleSockInfo(SSockInfo_PTR sockInfo_ptr);
	//}

private:
	bool     m_bIsRuning;
	HANDLE   m_hRecvThread;
	unsigned m_unThreadID;
	CLock    m_mapLock;
	CLock    m_readSetLock;
	bool     m_bShutDown;
	bool     m_bRedirect;

	fd_set   m_readSockSet;
	std::map<unsigned, SSockInfo_PTR> m_taskidToSockInfo;
};
#endif