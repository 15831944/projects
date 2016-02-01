#include "stdafx.h"
#include <sstream>
#include "http_client.h" 
#include <new>
#include "GZipHelper.h"

//SSockInfo struct

void _tagSSockInfo::RestoreInitialState()
{
	s_bIsHasRecvHttpHead      = false;
	s_bIsHasRecvContentData   = false;
	s_bHttpDowning            = false;
	s_bIsContentGzip          = false;
	s_bIsChunkedRecvLen       = false;
	s_bJumpStart              = false;
	s_bDownloadingFin         = false;
	s_unHasHeadLen            = 0;
	s_unContentLen            = 0;
	s_unHasRecvContentData    = 0;
	s_unCacheHasLen           = 0;
	s_unThisRemainChunkedLen  = 0;
	s_unCBCurrentPos          = 0;
	s_dm                      = e_UNKNOW;
	s_strOrigUrl.clear();
	s_strJumpUrl.clear();
	s_strHeadExpand.clear();

	memset(s_cHeadBuf, 0x0, 2049);
	memset(s_strCacheBuf, 0x0, 2048);
	if (s_hSocket)
	{
		closesocket(s_hSocket);
		s_hSocket = INVALID_SOCKET;
	}

	if (s_cDataBuf)
	{
		free(s_cDataBuf);
		s_cDataBuf = NULL;
	}
}

void _tagSSockInfo::RedirectSetUp()
{
	s_bIsHasRecvHttpHead      = false;
	s_bIsHasRecvContentData   = false;
	s_bIsContentGzip          = false;
	s_bIsChunkedRecvLen       = false;
	s_bJumpStart              = false;
	s_bDownloadingFin         = false;
	s_dm                      = e_UNKNOW;
	s_unHasHeadLen            = 0;
	s_unContentLen            = 0;
	s_unHasRecvContentData    = 0;
	s_unCacheHasLen           = 0;
	s_unThisRemainChunkedLen  = 0;
	s_unCBCurrentPos          = 0;

	memset(s_cHeadBuf, 0x0, 2049);
	memset(s_strCacheBuf, 0x0, 2048);


	if (s_hSocket)
	{
		closesocket(s_hSocket);
		s_hSocket = INVALID_SOCKET;
	}

	if (s_cDataBuf)
	{
		free(s_cDataBuf);
		s_cDataBuf = NULL;
	}
}
/*
example: strAttr = "Content-Length"、"Location"
*/
bool _tagSSockInfo::GetAttributeSection(std::string strAttr, std::string &strValue)
{
	bool bResult = false;
	do 
	{
		if (!s_bIsHasRecvHttpHead)
			break;

		char *pos1 = strstr(s_cHeadBuf, strAttr.c_str());
		if (NULL == pos1)
			break;

		char *pos2 = strstr(pos1, "\r\n");
		if (NULL == pos2)
			break;

		strValue.insert(strValue.begin(), pos1, pos2);
		bResult = true;
	} while (false);

	return bResult;
}

bool _tagSSockInfo::GetContentLength(size_t &len)
{
	bool bResult = false;
	do 
	{
		std::string strAttrValue;
		if(GetAttributeSection("Content-Length", strAttrValue))
		{
			std::string::size_type pos1 = strAttrValue.find(" ");
			if (pos1 == std::string::npos)
				break;

			len = atoi(strAttrValue.substr(pos1 + 1).c_str());
		}
		else
			break;

		bResult = true;
	} while (false);

	return bResult;
}

bool _tagSSockInfo::GetContentEncodeIsGzip()
{
	bool bResult = false;
	do 
	{
		std::string strAttrValue;
		if(GetAttributeSection("Content-Encoding", strAttrValue))
		{
			std::string::size_type pos1 = strAttrValue.find(" ");
			if (pos1 == std::string::npos)
				break;

			if(strAttrValue.substr(pos1 +1) == std::string("gzip"))
				bResult = true;
		}
	} while (false);

	return bResult;
}

bool _tagSSockInfo::GetContentIsTransferEncoding()
{
	bool bResult = false;
	do 
	{
		std::string strAttrValue;
		if(GetAttributeSection("Transfer-Encoding", strAttrValue))
		{
			std::string::size_type pos1 = strAttrValue.find(" ");
			if (pos1 == std::string::npos)
				break;

			if(strAttrValue.substr(pos1 +1) == std::string("chunked"))
				bResult = true;
		}
	} while (false);

	return bResult;
}

bool _tagSSockInfo::GetLocationUrl()
{
	bool bResult = false;
	do 
	{
		std::string strAttrValue;
		if(GetAttributeSection("Location", strAttrValue))
		{
			std::string::size_type pos1 = strAttrValue.find(" ");
			if (pos1 == std::string::npos)
				break;

			s_strJumpUrl = strAttrValue.substr(pos1 + 1);
		}
		else
			break;

		bResult = true;
	} while (false);

	return bResult;
}

unsigned long _tagSSockInfo::GetHttpState()
{
	unsigned long httpState = 0;
	do 
	{
		if (!s_bIsHasRecvHttpHead)
			break;

		std::string strHttpHead(s_cHeadBuf);

		std::string::size_type pos1 = strHttpHead.find(" ");
		if (std::string::npos == pos1)
			break;

		httpState = strtoul(strHttpHead.c_str() + pos1 + 1, NULL, 10);
	} while (false);

	return httpState;
}

bool _tagSSockInfo::OnDealWithHttpHead(int &errorCode)
{
	bool bResult = false;
	do 
	{
		if(!s_bIsHasRecvHttpHead)
		{
			errorCode = HTTPHEADDONOTFIN_INFO;
			break;
		}

		unsigned long StateCode = GetHttpState();
		if(200 == StateCode)
		{
			size_t contentLen = 0;
			if(GetContentLength(contentLen))
			{
				s_unContentLen = contentLen;
				s_dm           = e_CONTENTLENG;
				if (NULL == s_cDataBuf)
				{
					s_cDataBuf = (char *)malloc(sizeof(char) * s_unContentLen);
					if (NULL == s_cDataBuf)
					{
						errorCode = NEWMEMORY_ERROR;
						break;
					}

					memset(s_cDataBuf, 0x0, s_unContentLen);
				}
			}
			else if(GetContentIsTransferEncoding())
			{ 
				s_dm = e_CHUNKED;
			}
			else
			{
				errorCode = GETCONTENTLENGTH_ERROR;
				break;
			}	

			if(GetContentEncodeIsGzip())
				s_bIsContentGzip = true;
		}
		else if(302 == StateCode)
		{
			if(GetLocationUrl())
			{
				errorCode = HTTPDOWNINGREDIRECT_INFO;
			}
			else
			{
				errorCode = PARSELOACTION_ERROR;
			}
			break;
		}
		else
		{
			errorCode = StateCode;
			break;
		}

		bResult = true;
	} while (false);

	return bResult;
}

bool _tagSSockInfo::OndealWithChunckData()
{
	while(0 < s_unCacheHasLen)
	{
		if(!s_bIsChunkedRecvLen)
		{
			std::string strCache;
			if(s_bJumpStart)
			{
				if (s_unCacheHasLen <= 2)
					break;

				s_unCBCurrentPos += 2;
				strCache = s_strCacheBuf + s_unCBCurrentPos;
				s_unCacheHasLen -= 2;  
			}
			else
			{
				strCache = s_strCacheBuf;
				s_bJumpStart = true;
			}

			std::string::size_type pos1 = strCache.find("\r\n");
			if (std::string::npos != pos1)
			{
				s_unThisRemainChunkedLen = strtoul(strCache.substr(0, pos1).c_str(), 0, 16);
				s_bIsChunkedRecvLen = true;
				if (0 == s_unThisRemainChunkedLen)
				{
					s_bDownloadingFin = true;
					break;
				}

				if (NULL == s_cDataBuf)
				{
					s_cDataBuf = (char *)malloc(sizeof(char) * s_unThisRemainChunkedLen);
					memset(s_cDataBuf, 0x0, s_unThisRemainChunkedLen);
					s_unContentLen += s_unThisRemainChunkedLen;
				}
				else
				{
					s_cDataBuf = (char *)realloc(s_cDataBuf, s_unContentLen + s_unThisRemainChunkedLen);
					s_unContentLen += s_unThisRemainChunkedLen;
				}

				s_unCacheHasLen -= pos1 + 2;
				s_unCBCurrentPos += pos1 + 2;
				assert(s_unCacheHasLen >= 0);
			}
			else      //not complete length
			{
				if(s_bJumpStart)
				{
					s_unCBCurrentPos -= 2;
					s_unCacheHasLen  += 2;  
				}
				break;
			}
		}
		else   //s_bIsChunkedRecvLen = true
		{
				int cpyLen = min(s_unThisRemainChunkedLen, s_unCacheHasLen);
				memcpy(s_cDataBuf + s_unHasRecvContentData, s_strCacheBuf + s_unCBCurrentPos, cpyLen);
				s_unCacheHasLen -= cpyLen;
				s_unCBCurrentPos += cpyLen;

				s_unThisRemainChunkedLen -= cpyLen;
				s_unHasRecvContentData += cpyLen;

				assert(s_unThisRemainChunkedLen >= 0);
				if (0 == s_unThisRemainChunkedLen)
					s_bIsChunkedRecvLen = false;
		}
	}

	//完成移位操作
	if (!s_bDownloadingFin && !s_bDownloadingFin)
	{
		if (0 == s_unCacheHasLen)
		{
			s_unCBCurrentPos = 0;
			s_unCacheHasLen  = 0;
		}
		else
		{
			memcpy(s_strCacheBuf, s_strCacheBuf + s_unCBCurrentPos, s_unCacheHasLen);
			s_unCBCurrentPos = 0;
		}
	}
	return true;
}

bool _tagSSockInfo::RecvHeadData(int &errorCode)
{
	bool bResult = false;
	errorCode = 0;
	do 
	{
		char recvBuf[2049] = {0};
		int recvResult = recv(s_hSocket, recvBuf, 2048, 0);

		if (recvResult < 0)
		{
			errorCode = GetLastError();
			break;
		}
		else if(0 == recvResult)
		{
			errorCode = SOCKCLOSE;
			break;
		}
		else
		{
			std::string strRecv(recvBuf);
			std::string::size_type pos = strRecv.find("\r\n\r\n");
			if(std::string::npos == pos)
			{
				memcpy(s_cHeadBuf + s_unHasHeadLen, recvBuf, recvResult);
				s_unHasHeadLen += recvResult;
				bResult = true;
			}
			else
			{
				memcpy(s_cHeadBuf + s_unHasHeadLen, recvBuf, pos + 4);
				s_unHasHeadLen += (pos + 4);
				s_bIsHasRecvHttpHead = true;

				if(!OnDealWithHttpHead(errorCode))
				{
					if (HTTPHEADDONOTFIN_INFO == errorCode)
					{
						assert(false);
					}
					break;
				}

				if (e_CONTENTLENG == s_dm)
				{
					memcpy(s_cDataBuf + s_unHasRecvContentData,  recvBuf + pos + 4, recvResult - (pos + 4));
					s_unHasRecvContentData += recvResult - (pos + 4);
				}
				else if(e_CHUNKED == s_dm)
				{
					memcpy(s_strCacheBuf + s_unCBCurrentPos + s_unCacheHasLen,  recvBuf + pos + 4, recvResult - (pos + 4));
					s_unCacheHasLen += recvResult - (pos + 4);
				}
				else
					assert(false);

				bResult = true;
			}
		}
	} while (false);

	return bResult;
}

bool _tagSSockInfo::RecvContentData(int &errorCode)
{
	bool bResult = false;
	do 
	{
		if (!s_bIsHasRecvHttpHead)
		{
			errorCode = NOTRECVFINFSHHEAD;
			break;
		}

		if(s_hSocket == INVALID_SOCKET)
		{
			errorCode = NOTINISOCKET;
			break;
		}

		int recvLen = 0;
		if (e_CONTENTLENG == s_dm)
		{
			recvLen = recv(s_hSocket, s_cDataBuf + s_unHasRecvContentData, s_unContentLen- s_unHasRecvContentData, 0);
			s_unHasRecvContentData += recvLen;
		}
		else if (e_CHUNKED == s_dm)
		{
			OndealWithChunckData();
			recvLen = recv(s_hSocket, s_strCacheBuf + s_unCBCurrentPos + s_unCacheHasLen, 2048- s_unCacheHasLen, 0);
			s_unCacheHasLen += recvLen;
			OndealWithChunckData();
		}
		else
			assert(false);

		if (recvLen < 0)
		{
			errorCode = GetLastError();
			break;
		}
		else if(0 == recvLen)
		{
			if ((s_dm == e_CONTENTLENG && s_unHasRecvContentData == s_unContentLen)
				|| (s_dm == e_CHUNKED && true == s_bDownloadingFin))
			{
				s_bIsHasRecvContentData = true;
				errorCode = SUCCSEE;
			}
			else
			{
				errorCode = SOCKCLOSE;
			}
			break;
		}


		if ((0 != s_unContentLen && s_dm == e_CONTENTLENG && s_unHasRecvContentData == s_unContentLen)
			|| (0 != s_unContentLen && s_dm == e_CHUNKED && true == s_bDownloadingFin))
		{
			s_bIsHasRecvContentData = true;
			errorCode = SUCCSEE;
			break;
		}
		bResult = true;

	} while (false);
	return bResult;
}

//CHttpClient class
CHttpClient::CHttpClient()
: m_bIsRuning(false)
, m_hRecvThread(NULL)
, m_unThreadID(0)
, m_bShutDown(false)
, m_bRedirect(false)
{
	FD_ZERO(&m_readSockSet);
}

CHttpClient::~CHttpClient()
{
	if (m_hRecvThread)
	{
		if(m_hRecvThread)
		{
			m_bShutDown = true;
			WaitForSingleObject(m_hRecvThread, 1000);
		}
		CloseHandle(m_hRecvThread);
		m_hRecvThread = NULL;
	}
}

bool CHttpClient::StartSer()
{
	do 
	{
		if (!m_bIsRuning)
		{
			m_bShutDown = false;
			m_hRecvThread = (HANDLE)_beginthreadex(NULL, 0, RecvThread, (void *)this, 0, &m_unThreadID);
			if(NULL == m_hRecvThread)
				break;
			m_bIsRuning = true;
		}
	} while (false);

	return m_bIsRuning;
}

bool CHttpClient::StopSer()
{
	m_bShutDown = true;
	if (m_hRecvThread)
	{
		WaitForSingleObject(m_hRecvThread, 1000);
	}

	CloseHandle(m_hRecvThread);
	m_hRecvThread = NULL;
	m_unThreadID  = 0;

	FD_ZERO(&m_readSockSet);
	m_taskidToSockInfo.clear();
	m_bIsRuning = false;

	return true;
}

void CHttpClient::StopSerOnOwn()
{
	CloseHandle(m_hRecvThread);
	m_hRecvThread = NULL;
	m_unThreadID  = 0;

	FD_ZERO(&m_readSockSet);
	m_taskidToSockInfo.clear();
	m_bIsRuning = false;
}

bool CHttpClient::RegisterTask(unsigned taskID, IRecvInterface *recvSink)
{
	bool bResult = false;
	do 
	{
		if (NULL == recvSink || INVALID_TASK_ID == taskID)
			break;

		AutoLock lockGuide(m_mapLock);
		if (m_taskidToSockInfo.find(taskID) == m_taskidToSockInfo.end())
		{
			boost::shared_ptr<SSockInfo> newSockInfo(new SSockInfo(taskID, recvSink));
			if (newSockInfo)
			{
				m_taskidToSockInfo.insert(std::make_pair(taskID, newSockInfo));
				bResult = true;
			}
		}
		else
			bResult = true;
	} while (false);

	return bResult;
}

bool CHttpClient::cancelTask(unsigned taskID)
{
	bool bResult = false;
	do 
	{
		AutoLock lockGuide(m_mapLock);
		if (m_taskidToSockInfo.find(taskID) == m_taskidToSockInfo.end())
			break;

		m_taskidToSockInfo.erase(taskID);
		bResult = true;

	} while (false);
	return bResult;
}

//example: strUrl = http://list3.ppstream.com:8080/cfg/ClientPolicies.xml
//strHostObj  = /cfg/ClientPolicies.xml
//strHostPort = 8080
//strHostName = list3.ppstream.com
bool CHttpClient::ParseHttpUrl(std::string strUrl, std::string &strHostName, unsigned &strHostPort, std::string &strHostObj)
{
	bool bResult = false;
	do 
	{
		if (strUrl.empty())
			break;

		std::string::size_type pos1 = strUrl.find("http://");
		if(pos1 != std::string::npos)
			strUrl = strUrl.substr(7);

		std::string strHostNameAndPort;
		pos1 = strUrl.find("/");
		if (pos1 == std::string::npos)
		{
			strHostNameAndPort = strUrl;
			strHostObj = "";
		}
		else
		{
			strHostNameAndPort = strUrl.substr(0, pos1);
			strHostObj = strUrl.substr(pos1);
		}

		pos1 = strHostNameAndPort.find(":");
		if (pos1 == std::string::npos)
		{
			strHostName = strHostNameAndPort;
			strHostPort = 80;
		}
		else
		{
			strHostName = strHostNameAndPort.substr(0, pos1);
			strHostPort = strtoul((strHostNameAndPort.substr(pos1 + 1)).c_str(), 0, 10);
		}

		bResult = true;
	} while (false);

	return bResult;
}

/*
strHostName = "www.baidu.com"  or  "www.baidu.com/" or "127.0.0.1"
*/
bool CHttpClient::Connect(SSockInfo_PTR sockInfo_ptr, std::string strHostName, unsigned strHostPort, unsigned outSec/* = 5*/, unsigned MaxCount/* = MAXCONCOUNT*/)
{
	bool bResult = false;
	do 
	{
		if (strHostName.empty() || !sockInfo_ptr)
			break;

		//must be url redirect
		if ((INVALID_SOCKET != sockInfo_ptr->s_hSocket))
		{
			closesocket(sockInfo_ptr->s_hSocket);
			sockInfo_ptr->s_hSocket = INVALID_SOCKET;
		}

		//std::string strSer(strHostName);

		//delete '/' in the tail
		std::string::size_type pos1 = strHostName.rfind("/");
		if(pos1 != std::string::npos)
			strHostName = strHostName.substr(0, pos1);

		struct hostent *pHost = gethostbyname(strHostName.c_str());
		if (NULL == pHost)
			break;

		fd_set ws;
		SOCKET fd[FD_SETSIZE];
		FD_ZERO(&ws);

		int i = 0;
		unsigned int hasConCount = 0;
		char *pIP = pHost->h_addr_list[0];
		while(pIP)
		{
			if (4 == pHost->h_length)
			{
				bool bSkip = true;
				sockaddr_in conAddr;
				memset(&conAddr, 0x0, sizeof(conAddr));
				conAddr.sin_addr.S_un.S_addr = *((ULONG *)pIP);
				conAddr.sin_port             = htons(strHostPort);
				conAddr.sin_family           = AF_INET;

				SOCKET conSock = socket(AF_INET, SOCK_STREAM, 0);
				if(INVALID_SOCKET != conSock)
				{
					u_long nbio_on = 1;
					if(SOCKET_ERROR != ioctlsocket(conSock, FIONBIO, &nbio_on))
					{
						if(SOCKET_ERROR != connect(conSock, (sockaddr *)&conAddr, sizeof(sockaddr)) || WSAEWOULDBLOCK == WSAGetLastError())
						{
							bSkip = false;
							++hasConCount;
						}	
					}
				}

				if (bSkip)
				{
					if (INVALID_SOCKET != conSock)
					{
						closesocket(conSock);
						conSock = INVALID_SOCKET;
					}
				}
				else
				{
					FD_SET(conSock, &ws);
					fd[hasConCount - 1] = conSock;
				}
			}

			if (hasConCount >= MaxCount)
			{
				break;
			}
			++i;
			pIP = pHost->h_addr_list[i];
		}

		if (hasConCount > 0)
		{
			struct timeval outTime = {outSec, 0};
			int nselectResult = select(0, NULL, &ws, NULL, &outTime);
			if (0 == nselectResult)
			{
				for(unsigned int i = 0; i < hasConCount; ++i)
				{
					if (INVALID_SOCKET != fd[i])
					{
						closesocket(fd[i]);
						fd[i] = INVALID_SOCKET;
					}
				}
				break;
			}
			else if(SOCKET_ERROR == nselectResult)
			{
				for(unsigned int i = 0; i < hasConCount; ++i)
				{
					if (INVALID_SOCKET != fd[i])
					{
						closesocket(fd[i]);
						fd[i] = INVALID_SOCKET;
					}
				}
				break;
			}
			else
			{
				for(unsigned int i = 0; i < hasConCount; ++i)
				{
					if(FD_ISSET(fd[i], &ws) && !bResult)
					{
						u_long bio_off = 0;
						if(SOCKET_ERROR != ioctlsocket(fd[i], FIONBIO, &bio_off))
						{
							int ms = 5000; //5s
							setsockopt(fd[i], SOL_SOCKET,SO_RCVTIMEO, (char *)&ms, sizeof(ms));
							setsockopt(fd[i], SOL_SOCKET, SO_SNDTIMEO, (char *)&ms, sizeof(ms));

							if(sockInfo_ptr->s_hSocket != INVALID_SOCKET) //must be url redirect
							{
								closesocket(sockInfo_ptr->s_hSocket);
								sockInfo_ptr->s_hSocket = INVALID_SOCKET;
							}

							sockInfo_ptr->s_hSocket = fd[i];
							bResult = true;
						}
					}
					else
					{
						if (INVALID_SOCKET != fd[i])
						{
							closesocket(fd[i]);
							fd[i] = INVALID_SOCKET;
						}
					}
				}
			}
		}
	} while (false);
	return bResult;
}

/*
example: strHeadExpand = "Connection: close\r\n"
*/

bool CHttpClient::BuildSendData(std::ostringstream &httpHead, std::string strHostName, std::string strHostObj, std::string strHeadExpand)
{
	bool bResult = false;
	httpHead.str("");
	do 
	{
		if (strHostName.empty())
			break;

		if (!strHostObj.empty())
		{
			httpHead << "GET " << strHostObj.c_str() << " " << "HTTP/1.1\r\n";
		}
		else
		{
			httpHead << "GET " << "HTTP/1.1\r\n";
		}

		if (strHeadExpand.empty())
		{
			httpHead << "Host: " << strHostName.c_str() << "\r\n\r\n";
		}
		else
		{
			httpHead << "Host: " << strHostName.c_str() << "\r\n" << strHeadExpand.c_str() << "\r\n";
		}

		bResult = true;
	} while (false);

	return bResult;
}

bool CHttpClient::Send(SSockInfo_PTR sockInfo, const char *sendBuf, unsigned sendLen, int &errorCode)
{
	bool bResult = false;
	do 
	{
		if (!sockInfo || NULL == sendBuf)
			break;
		if (INVALID_SOCKET == sockInfo->s_hSocket)
			break;

		unsigned hasSendLen = 0;
		while(hasSendLen < sendLen)
		{
			int sndResult = send(sockInfo->s_hSocket, sendBuf + hasSendLen, sendLen - hasSendLen, 0);
			if (sndResult <= 0)
			{
				errorCode = WSAGetLastError();
				break;
			}
			hasSendLen += sndResult;
		}

		assert(sendLen - hasSendLen == 0);

		if (hasSendLen == sendLen)
			bResult = true;
	} while (false);

	return bResult;
}

bool  CHttpClient::Get(const unsigned unTaskID, const std::string &strUrl, int &errorCode, const std::string &strHeadExpand/* = ""*/, bool bJump/* = false*/)
{
	bool bResult = false;
	do 
	{
		SSockInfo_PTR taskSockInfo;
		{
			AutoLock lockGuide(m_mapLock);
			if (strUrl.empty())
			{
				errorCode = PARARM_URL_EMPTYERROR;
				break;
			}

			if (m_taskidToSockInfo.end() == m_taskidToSockInfo.find(unTaskID))
			{
				errorCode = TASK_NO_REGISTER_ERROR;
				break;
			}

			taskSockInfo = m_taskidToSockInfo.find(unTaskID)->second;
		}

		if (!taskSockInfo)
		{
			errorCode = INVALUE_SOCKINFOPTR_ERROR;
			break;
		}

		if(taskSockInfo->s_bHttpDowning)
		{
			errorCode = HTTPDOWNING_INFO;
			break;
		}

		if(!bJump)
		{
			taskSockInfo->s_strOrigUrl    = strUrl;
			taskSockInfo->s_strHeadExpand = strHeadExpand;
		}


		std::string strHostName;
		unsigned    strHostPort;
		std::string strHostObj;
		if(!ParseHttpUrl(strUrl, strHostName, strHostPort, strHostObj))
		{
			errorCode = PARESHTTPURL_ERROR;		
			break;
		}

		if(!Connect(taskSockInfo, strHostName, strHostPort))
		{
			errorCode = CONNECTHTTPSER_ERROR;
			break;
		}

		std::ostringstream httpHead;
		if (!BuildSendData(httpHead, strHostName, strHostObj, strHeadExpand))
		{
			RecycleSockInfo(taskSockInfo);
			errorCode = BUILDHTTPHEAD_ERROR;
			break;
		}

		errorCode = 0;
		bResult = Send(taskSockInfo, httpHead.str().c_str(), httpHead.str().size(), errorCode);

		if (bResult)
		{
			taskSockInfo->s_bHttpDowning = true;
			AutoLock lockGuide(m_readSetLock);
			FD_SET(taskSockInfo->s_hSocket, &m_readSockSet);
		}
		else
			RecycleSockInfo(taskSockInfo);

	} while (false);

	return bResult;
}

unsigned CHttpClient::GetHttpDownloadingState(unsigned unTaskID)
{
	AutoLock lockGuide(m_mapLock);
	if (m_taskidToSockInfo.find(unTaskID) == m_taskidToSockInfo.end())
	{
		return -1;
	}

	SSockInfo_PTR sockInfo = m_taskidToSockInfo[unTaskID];
	return sockInfo->s_bHttpDowning;
}


SSockInfo_PTR CHttpClient::GetSockInfoBySock(const SOCKET readSock)
{
	AutoLock lockGuide(m_mapLock);
	for (std::map<unsigned, SSockInfo_PTR>::iterator ite = m_taskidToSockInfo.begin(); ite != m_taskidToSockInfo.end(); ++ite)
	{
		if (readSock == ite->second->s_hSocket)
		{
			return ite->second;
		}
	}

	return SSockInfo_PTR();
}

void CHttpClient::RecycleSockInfo(SSockInfo_PTR sockInfo_ptr)
{
	if (!sockInfo_ptr)
		return;

	sockInfo_ptr->RestoreInitialState();

}

unsigned int __stdcall CHttpClient::RecvThread(void *param)
{
	do 
	{
		CHttpClient *pThis = reinterpret_cast<CHttpClient *>(param);
		if (NULL ==pThis)
			break;
		return pThis->RecvThread();

	} while(false);

	return 0;
}

unsigned int __stdcall CHttpClient::RecvThread()
{
	fd_set tmpReadSockSet;
	struct timeval tm = {0, 500000};   //500ms
	while(true)
	{
		FD_ZERO(&tmpReadSockSet);
		{
			AutoLock lockGuide(m_readSetLock);
			tmpReadSockSet = m_readSockSet;
		}


		int selResult = select(0, &tmpReadSockSet, NULL, NULL, &tm);
		if (selResult < 0)
		{ 
			if (true == m_bShutDown)
				break;

			if (10022 == GetLastError())
				continue;

			StopSerOnOwn(); 
			break;
		}
		else if(0 == selResult)
		{
			if (true == m_bShutDown)
				break;
		}
		else
		{
			for (unsigned i = 0; i < tmpReadSockSet.fd_count; ++i)
			{
				if (true == m_bShutDown)
					break;

				SSockInfo_PTR sockInfo_ptr = GetSockInfoBySock(tmpReadSockSet.fd_array[i]);
				if (sockInfo_ptr)
				{
					int errorCode = 0;
					bool re = false;
					if (!(sockInfo_ptr->s_bIsHasRecvHttpHead))
					{
						re = sockInfo_ptr->RecvHeadData(errorCode);
						if (false == re)
						{
							if (m_bRedirect && errorCode == HTTPDOWNINGREDIRECT_INFO)
							{
								{
									AutoLock lockGudie(m_readSetLock);
									if(FD_ISSET(sockInfo_ptr->s_hSocket, &m_readSockSet))
									{
										FD_CLR(sockInfo_ptr->s_hSocket, &m_readSockSet);
									}
								}

								sockInfo_ptr->RedirectSetUp();
								int errorCode = 0;
								bool bGetRes = Get(sockInfo_ptr->s_unTaskID, sockInfo_ptr->s_strJumpUrl, errorCode, sockInfo_ptr->s_strHeadExpand, true);
								if (!bGetRes)
								{
									{
										AutoLock lockGudie(m_readSetLock);
										if(FD_ISSET(sockInfo_ptr->s_hSocket, &m_readSockSet))
										{
											FD_CLR(sockInfo_ptr->s_hSocket, &m_readSockSet);
										}
									}

									assert(sockInfo_ptr->s_iSink != NULL);
									if (sockInfo_ptr->s_iSink)
									{
										(sockInfo_ptr->s_iSink)->OnDownLoadFinish(false, errorCode);
									}

									RecycleSockInfo(sockInfo_ptr);
								}
							}
							else
							{
								{
									AutoLock lockGudie(m_readSetLock);
									if(FD_ISSET(sockInfo_ptr->s_hSocket, &m_readSockSet))
									{
										FD_CLR(sockInfo_ptr->s_hSocket, &m_readSockSet);
									}
								}

								//通知上层,关闭任务
								if (sockInfo_ptr->s_iSink)
								{
									(sockInfo_ptr->s_iSink)->OnDownLoadFinish(false, errorCode);
								}

								RecycleSockInfo(sockInfo_ptr);
							}
						}
					}
					else
					{
						re = sockInfo_ptr->RecvContentData(errorCode);
						if (false == re)
						{
							bool bResult = false;
							{
								AutoLock lockGudie(m_readSetLock);
								if(FD_ISSET(sockInfo_ptr->s_hSocket, &m_readSockSet))
								{
									FD_CLR(sockInfo_ptr->s_hSocket, &m_readSockSet);
								}
							}
							//通知上层,关闭任务
							if (sockInfo_ptr->s_iSink)
							{
								if (errorCode == SUCCSEE)
								{
									bResult = true;
									if(sockInfo_ptr->s_bIsContentGzip)
									{
										//解压数据;
										CGZIP2A * pGZip = NULL;
										pGZip = new(std::nothrow) CGZIP2A((LPGZIP)sockInfo_ptr->s_cDataBuf,sockInfo_ptr->s_unContentLen);
										if(NULL != pGZip)
										{
											//char *unComData = pGZip->psz;
											int unComLen = pGZip->Length;
											free(sockInfo_ptr->s_cDataBuf);
											sockInfo_ptr->s_cDataBuf = NULL;
											sockInfo_ptr->s_cDataBuf = (char *)malloc(sizeof(char) * unComLen);
											memset(sockInfo_ptr->s_cDataBuf, 0x0, unComLen);
											memcpy(sockInfo_ptr->s_cDataBuf, pGZip->psz, unComLen);
											sockInfo_ptr->s_unContentLen = unComLen;
										}
										else
										{
											errorCode = COMPRESSIONDATA_ERROR;
											bResult = false;
										}
										if(NULL != pGZip)
											delete pGZip;
									}

									(sockInfo_ptr->s_iSink)->OnDownLoadFinish(bResult, errorCode);
								}	
								else
									(sockInfo_ptr->s_iSink)->OnDownLoadFinish(bResult, errorCode);
							}

							RecycleSockInfo(sockInfo_ptr);
						}
					}
				}
				else   //read set Data is not synchronized
				{
					{
						AutoLock loskGuide(m_readSetLock);
						if(FD_ISSET(tmpReadSockSet.fd_array[i], &m_readSetLock))
						{
							FD_CLR(tmpReadSockSet.fd_array[i], &m_readSetLock);
							closesocket(tmpReadSockSet.fd_array[i]);
						}
					}
				}

				if (true == m_bShutDown)
					break;
			}

			if (true == m_bShutDown)
				break;
		}
	}
	return 0;
}



bool CHttpClient::GetDataBuf(const unsigned unTaskID, std::string &strData)
{
	bool bResult = false;
	do 
	{
		SSockInfo_PTR sockInfo_ptr;
		{
			AutoLock lockGuide(m_mapLock);
			if(m_taskidToSockInfo.find(unTaskID) == m_taskidToSockInfo.end())
				break;

			sockInfo_ptr = m_taskidToSockInfo[unTaskID];
		}

		if (sockInfo_ptr)
		{
			if (!sockInfo_ptr->s_bIsHasRecvContentData)
				break;		
			strData = sockInfo_ptr->s_cDataBuf;
		}

		bResult = true;
	} while (false);

	return bResult;
}


bool CHttpClient::GetDataLen(const unsigned unTaskID, unsigned int &unDataLen)
{
	bool bResult = false;
	do 
	{
		SSockInfo_PTR sockInfo_ptr;
		{
			AutoLock lockGuide(m_mapLock);
			if(m_taskidToSockInfo.find(unTaskID) == m_taskidToSockInfo.end())
				break;

			sockInfo_ptr = m_taskidToSockInfo[unTaskID];
		}

		if (sockInfo_ptr)
		{
			if (!sockInfo_ptr->s_bIsHasRecvContentData)
				break;	
			unDataLen = sockInfo_ptr->s_unContentLen;
		}

		bResult = true;
	} while (false);

	return bResult;
}


