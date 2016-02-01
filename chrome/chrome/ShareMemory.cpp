#include "ShareMemory.h"

#define HDR_LEN 4

namespace BaseClassLibrary
{
	CShareMemBase::CShareMemBase()
	: m_hMutexSyncData(NULL)
	, m_hFileMapObj(NULL)
	, m_pMapView(NULL)
	, m_pWRPos(NULL)
	, m_pUserBufBasePos(NULL)
	{
		m_hWait[0] = NULL;
		m_hWait[1] = NULL;
	}

	CShareMemBase::~CShareMemBase()
	{
		close();
	}

	void CShareMemBase::close()
	{

		ReleaseSemaphore(m_hWait[1], 1, NULL);

		Sleep(0);

		if(m_hWait[1])
			CloseHandle(m_hWait[1]);


		if(m_hWait[0])
			CloseHandle(m_hWait[0]);

		if(m_pMapView)
		{
			UnmapViewOfFile(m_pMapView);
			m_pMapView = NULL;
		}

		if(m_hFileMapObj)
		{
			CloseHandle(m_hFileMapObj);
			m_hFileMapObj = NULL;
		}
		
		if(m_hMutexSyncData)
		{
			CloseHandle(m_hMutexSyncData);
			m_hMutexSyncData = NULL;
		}
	}

	CShareMem::CShareMem()
	: m_bSMSuccess(false)
	{
	}

	CShareMem::~CShareMem()
	{

	}

	bool CShareMem::Create(std::string strSMName, unsigned unSMMSize)
	{
		bool bResult = false;
		do 
		{
			if(strSMName.empty() || 0 == unSMMSize || unSMMSize < sizeof(SWRPos))
				break;

			m_strSMName = strSMName;


			SECURITY_ATTRIBUTES SecAttr, *pSec = 0;
			SECURITY_DESCRIPTOR SecDesc;  
			if (strSMName.find("Global\\") != std::string::npos) 
			{     
				InitializeSecurityDescriptor(&SecDesc, SECURITY_DESCRIPTOR_REVISION);
				SetSecurityDescriptorDacl(&SecDesc, TRUE, (PACL)0, FALSE);  //Add the ACL to the security descriptor.这里设置的是一个空的DACL
				SecAttr.nLength = sizeof(SecAttr); 
				SecAttr.lpSecurityDescriptor = &SecDesc;  
				SecAttr.bInheritHandle = TRUE;  
				pSec = &SecAttr; 
			}


			std::string strMutexName = strSMName + "Mutex";
			m_hMutexSyncData = CreateMutexA(pSec, FALSE, strMutexName.c_str());
			if(NULL == m_hMutexSyncData)
				break;

			std::string strSemName = strSMName + "Recv";
			m_hWait[0] = CreateSemaphoreA(pSec, 0, 5000, strSemName.c_str());
			if(NULL == m_hWait[0])
				break;

			std::string strSemExit = strSMName + "Exit";
			m_hWait[1] = CreateSemaphoreA(pSec, 0, 1, strSemExit.c_str());
			if(NULL == m_hWait[0])
				break;


			//当你创建了一个命名管道，它的安全参数指定了NULL，这就表明只有创建者才可以作为Client访问这个命名管道。
			m_hFileMapObj = CreateFileMappingA(INVALID_HANDLE_VALUE, pSec, PAGE_READWRITE, 0, unSMMSize, strSMName.c_str());
			DWORD gg = GetLastError();
			if(NULL == m_hFileMapObj)
				break;

			//将其映射到本进程的地址空间中
			m_pMapView	= (char *)MapViewOfFile(m_hFileMapObj, FILE_MAP_ALL_ACCESS, 0, 0, 0); 
			if(NULL == m_pMapView)
				break;

			m_pWRPos = reinterpret_cast<PSWRPos>(m_pMapView);
			m_pWRPos->mem_len   = unSMMSize - sizeof(SWRPos);
			m_pWRPos->read_pos  = 0;
			m_pWRPos->write_pos = 0;
			m_pUserBufBasePos = m_pMapView + sizeof(SWRPos);

			m_bSMSuccess = bResult = true;
		} while (false);

		if(!bResult)
		{
			close();
		}

		return bResult;
	}

	bool CShareMem::Open(std::string strSMName)
	{
		bool bResult = false;
		do 
		{
			if(strSMName.empty())
				break;

			m_strSMName = strSMName;

			if(!m_hMutexSyncData)
			{

				m_hFileMapObj = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, strSMName.c_str());
				if(NULL == m_hFileMapObj)
					break;

				m_pMapView = (char *)MapViewOfFile(m_hFileMapObj, FILE_MAP_ALL_ACCESS,0,0,0);
				if(NULL == m_pMapView)
					break;

				std::string strMutexName = strSMName + "Mutex";
				m_hMutexSyncData = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, strMutexName.c_str());
				if(NULL == m_hMutexSyncData)
					break;

				std::string strSemName = strSMName + "Recv";
					m_hWait[0] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS , FALSE, strSemName.c_str());
				if(NULL == m_hWait[0])
					break;

				std::string strSemExit = strSMName + "Exit";
				m_hWait[1] = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, strSemExit.c_str());
				if(NULL == m_hWait[0])
					break;

				m_pWRPos = reinterpret_cast<PSWRPos>(m_pMapView);
				m_pUserBufBasePos = m_pMapView + sizeof(SWRPos);

			}

			m_bSMSuccess = bResult = true;
		} while (false);

		if(!bResult)
			close();

		return bResult;
	}

	bool CShareMem::Send(char* dataBuf, unsigned int bufLen)
	{
		bool bResult = false;
		int  iActLen = 0;
		do 
		{
			if(!m_bSMSuccess)
				break;

			DWORD dwRes = WaitForSingleObject(m_hMutexSyncData, 5000);
			if(WAIT_OBJECT_0 == dwRes)
			{
				int writeTotalLen = bufLen + HDR_LEN;
				if(m_pWRPos->write_pos >= m_pWRPos->read_pos)
				{
					int rightIdleLen = m_pWRPos->mem_len - m_pWRPos->write_pos;
					if(rightIdleLen >= writeTotalLen)
					{
						char *pWritePos = &m_pUserBufBasePos[m_pWRPos->write_pos];
						memcpy(pWritePos, &bufLen, HDR_LEN);
						pWritePos += HDR_LEN;
						memcpy(pWritePos, dataBuf, bufLen);
						m_pWRPos->write_pos += writeTotalLen;
						iActLen = bufLen;
					}
					else
					{
						int rlIdleLen = rightIdleLen + m_pWRPos->read_pos;
						if(rlIdleLen > writeTotalLen)
						{
							//1.线将数据包长度+数据合在一起
							char *tmpPkt = new char[writeTotalLen];
							memcpy(tmpPkt, &bufLen, HDR_LEN);
							memcpy(tmpPkt+HDR_LEN, dataBuf, bufLen);
							//分两段来写
							char *pWritePos = &m_pUserBufBasePos[m_pWRPos->write_pos];
							
							memcpy(pWritePos, tmpPkt, rightIdleLen);

							m_pWRPos->write_pos = 0;
							pWritePos = &m_pUserBufBasePos[m_pWRPos->write_pos];
							memcpy(pWritePos, tmpPkt+rightIdleLen, writeTotalLen - rightIdleLen);
							m_pWRPos->write_pos = writeTotalLen - rightIdleLen;
							iActLen = bufLen;
							delete []tmpPkt;
						}
						else
							break;
					}

				}
				else
				{
					if(m_pWRPos->read_pos - m_pWRPos->write_pos > writeTotalLen)
					{
						char *pWritePos = &m_pUserBufBasePos[m_pWRPos->write_pos];
						memcpy(pWritePos, &bufLen, HDR_LEN);
						pWritePos += HDR_LEN;
						memcpy(pWritePos, dataBuf, bufLen);
						m_pWRPos->write_pos += writeTotalLen;
						iActLen = bufLen;
					}
					else
						break;
				}
			}
			else
				break;


			bResult = true;
		} while (false);

		if(iActLen > 0)
		{
			ReleaseSemaphore(m_hWait[0], 1, NULL);
		}
		ReleaseMutex(m_hMutexSyncData); 
		return bResult;
	}


	//由外部一个单独的线程调用，来监控数据的接收
	bool CShareMem::Read(IDataReceiver *pDataRecver)
	{
		bool bResult = true;

		do 
		{
			DWORD dwResult;
			while(true)
			{
				dwResult = WaitForMultipleObjects(2, m_hWait, FALSE, INFINITE);
				if(WAIT_OBJECT_0 != dwResult)
				{
					//说明收到线程退出的消息
					break;
				}
				else
				{
					Recv(pDataRecver);
				}
			}

			bResult = true;
		} while (false);

		return bResult;
	}

	bool CShareMem::Recv(IDataReceiver *pDataRecver)
	{
		bool bResult = false;
		do 
		{
			if(!m_bSMSuccess)
				break;

			DWORD dwRes = WaitForSingleObject(m_hMutexSyncData, 5000);
			if(WAIT_OBJECT_0 == dwRes)
			{
				if(m_pWRPos->read_pos != m_pWRPos->write_pos)
				{
					char *pReadPos = &m_pUserBufBasePos[m_pWRPos->read_pos];
					if(m_pWRPos->write_pos > m_pWRPos->read_pos)
					{
						unsigned int readLen = *((int *)pReadPos);
						pReadPos += HDR_LEN;
						if(pDataRecver)
							pDataRecver->OnRecv(m_strSMName.c_str(), pReadPos, readLen);  

						m_pWRPos->read_pos += (HDR_LEN + readLen);
					}
					else
					{
						unsigned int readLen = 0;
						unsigned int rightIdleLen = m_pWRPos->mem_len - m_pWRPos->read_pos;
						bool bWrap = false;
						if(rightIdleLen < HDR_LEN)
						{
							//长度被分段了
							memcpy(&readLen, pReadPos, rightIdleLen);
							memcpy((char *)&readLen + rightIdleLen, m_pUserBufBasePos, HDR_LEN - rightIdleLen);
							bWrap = true;
						}
						else
						{
							readLen = *((int *)pReadPos);
							if(readLen + HDR_LEN > rightIdleLen)
								bWrap = true;
							else
							{
								pReadPos += HDR_LEN;
								if(pDataRecver)
									pDataRecver->OnRecv(m_strSMName.c_str(), pReadPos, readLen);  

								m_pWRPos->read_pos += (HDR_LEN + readLen);
							}
						}

						if(bWrap && readLen)
						{
							char *wrapPkt = new char[HDR_LEN + readLen];
							memcpy(wrapPkt, pReadPos, rightIdleLen);
							m_pWRPos->read_pos = 0;
							pReadPos = &m_pUserBufBasePos[m_pWRPos->read_pos];
							memcpy(wrapPkt+rightIdleLen, pReadPos, HDR_LEN + readLen - rightIdleLen);
							char* pktData = wrapPkt + HDR_LEN;
							if(pDataRecver)
								pDataRecver->OnRecv(m_strSMName.c_str(), pktData, readLen);  
							m_pWRPos->read_pos = HDR_LEN + readLen - rightIdleLen;

							delete []wrapPkt;

						}
					}
				}
				else
					; //没有数据可读

				ReleaseMutex(m_hMutexSyncData);
			}
			else
				break;

			bResult = true;
		} while (false);

		return bResult;
	}
}