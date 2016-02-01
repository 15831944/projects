#include "DownLoadSpeedLimit.h"
CDLSpeedLim::CDLSpeedLim()
: m_iDLSpeedUp(400.0)
, m_iLastRecvTotalByte(0)
{

}

CDLSpeedLim::~CDLSpeedLim()
{

}

void CDLSpeedLim::Read(int iByteRecv)
{
	CScopeLock autoLock(&m_queueLock);
	DWORD dwCurrentTick  = GetTickCount();
	struct ElemType recvData;
	recvData.tick           =  dwCurrentTick;
	recvData.ByteCount      =  iByteRecv;
	m_iLastRecvTotalByte    += iByteRecv;
	if(m_qRecvData.empty())
		m_qRecvData.push(recvData);
	else
	{
		if(m_qRecvData.back().tick == dwCurrentTick)
			m_qRecvData.back().ByteCount += iByteRecv;
		else
			m_qRecvData.push(recvData);
	}
}

bool CDLSpeedLim::IsLimitSpeed()
{
	bool bResult = false;
	DWORD dwCurrentTick = GetTickCount();
	do 
	{
		CScopeLock autoLock(&m_queueLock);
		if(m_qRecvData.empty())
			break;
		while(dwCurrentTick - m_qRecvData.front().tick > 1000)
		{
			m_iLastRecvTotalByte -= m_qRecvData.front().ByteCount;
			m_qRecvData.pop();
			if(m_qRecvData.empty())
				break;
		}

		if(m_qRecvData.empty())
			break;

	    DWORD dwTickInterval = dwCurrentTick - m_qRecvData.front().tick;

		if(dwTickInterval)
		{
			if(((m_iLastRecvTotalByte * 1.0) / (dwTickInterval * 1000)) / 1024 >= m_iDLSpeedUp)
				bResult = true;
		}
		else
		{
			if(m_iLastRecvTotalByte / 1024 >= m_iDLSpeedUp)
				bResult = true;
		}

	} while (false);

	return bResult;
}