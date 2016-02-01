#include "ThreadLock.h"
#include <Windows.h>
#include <queue>

class CDLSpeedLim
{
public:

	CDLSpeedLim();
	~CDLSpeedLim();
	void SetDownLoadSpeed(double iDLSpeed){ m_iDLSpeedUp = iDLSpeed;}
	void Read(int iByteRecv);
	bool IsLimitSpeed();
private:
	struct ElemType
	{
		unsigned long tick;
		int           ByteCount; 
		ElemType()
		: tick(0)
		, ByteCount(0)
		{

		}
	};

	CCriSecLock m_queueLock;
	double      m_iDLSpeedUp;   //KB/s  default 400K/s
	int         m_iLastRecvTotalByte;
	std::queue<ElemType> m_qRecvData;

};