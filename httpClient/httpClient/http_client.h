#ifndef HTTPCLIENT_HEAD
#define HTTPCLIENT_HEAD

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <WinSock2.h>
#include <process.h>
#include "Interface.h"
#include "lock.h"



#define INVALID_TASK_ID 0xFFFFFFFF
#define MAXCONCOUNT     4


#define BUILDHTTPHEAD_ERROR            -91
#define CONNECTHTTPSER_ERROR           -92
#define PARESHTTPURL_ERROR             -93
#define INVALUE_SOCKINFOPTR_ERROR      -94
#define PARARM_ERROR                   -95
#define SOCKCLOSE                      -96
#define NOTINISOCKET                   -97
#define NOTRECVFINFSHHEAD              -98
#define NEWMEMORY_ERROR                -99
#define GETCONTENTLENGTH_ERROR         -100


#define HTTPDOWNING_INFO              -200



/*
版 本 号：
（1）1.0.0.1：
   0.采用的是阻塞模式+select的结构；
   1.仅支持简单的发送与接收功能；
   2.支持多任务的下载；
   3.仅支持get方法；
   4.不支持内部跳转功能；
   5.不支持chucked模式；
   6.不支持gzip格式 
   7.不支持同一任务的多次http下载任务

创建时间：2014.10.29
*/

typedef struct _tagSSockInfo
{
	SOCKET    s_hSocket;
	bool      s_bHttpDowning;
	
	
	int       s_DataLen;
	unsigned  s_unTaskID;
	CLock     s_owerLock;    //暂时未使用

	

	std::string s_strOrigUrl;
	std::string s_strJumpUrl;	   



	char      *s_cDataBuf;   //add '\0' in the string tail
	unsigned  s_unContentLen;
	unsigned  s_unHasRecvContentData;
	bool      s_bIsHasRecvContentData;

	char      s_cHeadBuf[2049];     ////add '\0' in the string tail, s_cHeadBuf[2048] = '\0'
	unsigned  s_unHasHeadLen;  
	bool      s_bIshasRecvHttpHead;


	IRecvInterface *s_iSink;
	          


	bool RecvHeadData(int &errorCode);
	bool RecvContentData(int &errorCode);
	unsigned long GetHttpState();

	//bool GetContentRange(size_t &pos, size_t &len, size_t &total);
	bool GetContentLength(size_t &len);
	bool GetAttributeSection(std::string strAttr, std::string &strValue);

	_tagSSockInfo(unsigned taskID, IRecvInterface *recvSink)
	: s_iSink(recvSink)
	, s_unTaskID(taskID)
	, s_hSocket(INVALID_SOCKET)
	, s_cDataBuf(NULL)
	, s_DataLen(0)
	, s_bIshasRecvHttpHead(false)
	, s_bHttpDowning(false)
	, s_unHasHeadLen(0)
	, s_unContentLen(0)
	, s_unHasRecvContentData(0)
	{
		memset(s_cHeadBuf, 0x0, 2049);
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
			delete(s_cDataBuf);
			s_cDataBuf = NULL;
		}
	}
}SSockInfo, *PSSockInfo;

typedef boost::shared_ptr<SSockInfo> SSockInfo_PTR;


class CHttpClient
{
public:
	CHttpClient();
	~CHttpClient();

	bool StartSer();
	bool StopSer();

	bool RegisterTask(unsigned taskID, IRecvInterface *recvSink);
	bool Get(const unsigned unTaskID, const std::string &strUrl, int &errorCode, const std::string &strHeadExpand = "");
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
	// void StopSerOnOwn();

	//}
	
   
	
private:
	bool     m_bIsRuning;
	HANDLE   m_hRecvThread;
	unsigned m_unThreadID;
	CLock    m_mapLock;
	CLock    m_readSetLock;
	bool     m_bShutDown;

	fd_set   m_readSockSet;
	std::map<unsigned, SSockInfo_PTR> m_taskidToSockInfo;
};
#endif