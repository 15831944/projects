#pragma once

#include <pdh.h>
#include <string>
#include <vector>
#include <map>

#pragma comment(lib,"pdh.lib")


//error code macro
#define SUCCESS                    0
#define STOP                       -1
#define NOTINITENV_ERROR           -2
#define NOTCOLLECTDATA_ERROR       -3



class CResourceMonitor
{
public:
	CResourceMonitor();
	~CResourceMonitor();

	bool     InitGlobalData(std::string strProNam = ""); 
	unsigned CResourceMonitor::GetGlobalAndSpeciValueByPDH(std::map<std::string, double> &mapMonValue, int &error_code);
	bool     SetProcessMoni(std::string strProNam);
	void     Stop();

private:
	bool GetPhyMem(unsigned long &TotalPhyMem);
	int	 GetProcessorNumber();
	bool CreateAndAddCounters();
	bool CreateSpecialProcessCounters(bool force = false);
	bool CreateSystemCounters();
	bool CloseSystemCounters();
	bool CloseProcessCounters();
	void ClearEnv();
	unsigned CResourceMonitor::GetGlobalValueByPDH(std::map<std::string, double> &mapMonValue);
	unsigned  GetSpeciValueByPDH(std::map<std::string, double> &mapMonValue);

private:
	bool	            m_bIniFlag;
	bool                m_bStopFlag;
	unsigned		    m_unProCount;          //processor count
	unsigned long       m_unTotalPhyMem_MB;    //physical memory
	std::string         m_strProName;          //monitor process name

	HQUERY     m_hQueryBySys;                 // pdh query handle (use to system data)
	HCOUNTER   m_hProTimeCounterBySys;       // processor time counter handle
	HCOUNTER   m_hDiskTimeCounterBySys;      // disk time counter handle(蓝色的活动时间，是以毫秒为单位，表示在单位毫秒时间内，磁盘完成一个读写操作所耗费的时间比例。)
	HCOUNTER   m_hDiskBytesBySys;            // disk bytes counter handle (MB/s)
	HCOUNTER   m_hAvaiMemCounterBySys;       // available memory (MB) counter handle
	HCOUNTER   m_hNetTrafficCounterBySys;   //all adapter network traffic counter handle  (MB/s)  


	HQUERY     m_hQueryByPro;                      // pdh query handle (use to process data)
	HCOUNTER   m_hProTimeCounterByPro;             // process time counter handle
	HCOUNTER   m_hIODataBytesSecCounterByPro;      // process i/o bytes per second 
	HCOUNTER   m_hWorkingSetCounterByPro;          // process working set memory (B) counter handle   
};

