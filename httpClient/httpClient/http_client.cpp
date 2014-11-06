#include "stdafx.h"
#include <sstream>
#include "http_client.h" 


//SSockInfo struct
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

bool _tagSSockInfo::GetAttributeSection(std::string strAttr, std::string &strValue)
{
	bool bResult = false;
	do 
	{
		if (!s_bIshasRecvHttpHead)
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

unsigned long _tagSSockInfo::GetHttpState()
{
	unsigned long httpState = 0;
	do 
	{
		if (!s_bIshasRecvHttpHead)
			break;

		std::string strHttpHead(s_cHeadBuf);

		std::string::size_type pos1 = strHttpHead.find(" ");
		if (std::string::npos == pos1)
			break;

		httpState = strtoul(strHttpHead.c_str() + pos1 + 1, NULL, 10);
	} while (false);

	return httpState;
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
				s_bIshasRecvHttpHead = true;

				if(200 == GetHttpState())
				{
					size_t contentLen = 0;
					if(!GetContentLength(contentLen))
					{
						errorCode = GETCONTENTLENGTH_ERROR;
						break;
					}

					s_unContentLen = contentLen;
					if (NULL == s_cDataBuf)
					{
						s_cDataBuf = new char[s_unContentLen + 1];
						if (NULL == s_cDataBuf)
						{
							errorCode = NEWMEMORY_ERROR;
							break;
						}

						memset(s_cDataBuf, 0x0, s_unContentLen + 1);
					}

					memcpy(s_cDataBuf + s_unHasRecvContentData,  recvBuf + pos + 4, recvResult - (pos + 4));
					s_unHasRecvContentData += recvResult - (pos + 4);
				}
				else
				{
					break;
				}

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
		if (!s_bIshasRecvHttpHead)
		{
			errorCode = NOTRECVFINFSHHEAD;
			break;
		}

		if(s_hSocket == INVALID_SOCKET)
		{
			errorCode = NOTINISOCKET;
			break;
		}

		size_t contentLen = 0;
		if(!GetContentLength(contentLen))
		{
			errorCode = GETCONTENTLENGTH_ERROR;
			break;
		}

		s_unContentLen = contentLen;

		if (NULL == s_cDataBuf)
		{
			s_cDataBuf = new char[s_unContentLen + 1];
			if (NULL == s_cDataBuf)
			{
				errorCode = NEWMEMORY_ERROR;
				break;
			}

			memset(s_cDataBuf, 0x0, s_unContentLen + 1);
		}

		int recvLen = recv(s_hSocket, s_cDataBuf + s_unHasRecvContentData, s_unContentLen- s_unHasRecvContentData, 0);
		if (recvLen < 0)
		{
			errorCode = GetLastError();
			break;
		}
		else if(0 == recvLen)
		{
			errorCode = SOCKCLOSE;
			break;
		}

		s_unHasRecvContentData += recvLen;
		if (s_unHasRecvContentData == s_unContentLen)
		{
			s_bIsHasRecvContentData = true;
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
			WaitForSingleObject(m_hRecvThread, 500);
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
//example: strUrl = http://www.xxx.com:1234/123/456
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
			if (sndResult < 0)
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

bool  CHttpClient::Get(const unsigned unTaskID, const std::string &strUrl, int &errorCode, const std::string &strHeadExpand/* = ""*/)
{
	bool bResult = false;
	do 
	{
		SSockInfo_PTR taskSockInfo;
		{
			AutoLock lockGuide(m_mapLock);
			if (strUrl.empty() || m_taskidToSockInfo.end() == m_taskidToSockInfo.find(unTaskID))
			{
				errorCode = PARARM_ERROR;
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

		taskSockInfo->s_strOrigUrl = strUrl;

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
			errorCode = BUILDHTTPHEAD_ERROR;
			break;
		}

		errorCode = 0;
		bResult = Send(taskSockInfo, httpHead.str().c_str(), httpHead.str().size(), errorCode);

		if (bResult)
		{
			AutoLock lockGuide(m_readSetLock);
			FD_SET(taskSockInfo->s_hSocket, &m_readSockSet);
		}
	} while (false);

	return bResult;
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
	struct timeval tm = {0, 500000};
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
			//StopSerOnOwn();  ÔÝÊ±¹Ø±Õ
			if (10022 == GetLastError())
				continue;

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
					if (!(sockInfo_ptr->s_bIshasRecvHttpHead))
					{
						re = sockInfo_ptr->RecvHeadData(errorCode);
					}
					else
					{
						re = sockInfo_ptr->RecvContentData(errorCode);
					}
				}

				if (true == m_bShutDown)
					break;
			}
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
			sockInfo_ptr->s_DataLen;
		}

		bResult = true;
	} while (false);

	return bResult;
}


