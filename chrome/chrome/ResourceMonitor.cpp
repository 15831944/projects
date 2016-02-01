#include "ResourceMonitor.h"
#include "PublicMethod.h"
#include <string>


//class define


/*************************************************
// <Summary>constructor</Summary>
// <DateTime>2014/12/02</DateTime>
*************************************************/
CResourceMonitor::CResourceMonitor(void)
: m_bIniFlag(false)
, m_bStopFlag(false)
, m_unProCount(1)
, m_unTotalPhyMem_MB(1)
, m_hQueryBySys(NULL)
, m_hQueryByPro(NULL)
, m_hProTimeCounterBySys(NULL)
, m_hProTimeCounterByPro(NULL)
, m_hDiskTimeCounterBySys(NULL)
, m_hIODataBytesSecCounterByPro(NULL)
, m_hAvaiMemCounterBySys(NULL)
, m_hWorkingSetCounterByPro(NULL)
, m_hNetTrafficCounterBySys(NULL)
, m_hDiskBytesBySys(NULL)
{

}

/*************************************************
// <Summary>destructor</Summary>
// <DateTime>2014/12/02</DateTime>
*************************************************/
CResourceMonitor::~CResourceMonitor(void)
{
	if (m_hQueryBySys)
	{
		PdhCloseQuery(m_hQueryBySys);
		m_hQueryBySys = NULL;
	}

	if(m_hQueryByPro)
	{
		PdhCloseQuery(m_hQueryByPro);
		m_hQueryByPro = NULL;
	}
}

/*************************************************
// <Summary>stop resource monitor</Summary>
// <DateTime>2014/12/10</DateTime>
// <Returns>void</Returns>
*************************************************/
void CResourceMonitor::Stop()
{
	m_bStopFlag = true;
	m_bIniFlag  = false;
	CloseProcessCounters();
	CloseSystemCounters();
	ClearEnv();
}


/*************************************************
// <Summary>initialize global data</Summary>
// <DateTime>2014/12/02</DateTime>
// <Parameter name="strProNam" type="IN">process name</Parameter>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::InitGlobalData(std::string strProNam)
{
	bool bResult = false;
	do 
	{
		m_strProName    = strProNam;
		m_unProCount    = GetProcessorNumber();
		if(false == GetPhyMem(m_unTotalPhyMem_MB))
			break;

		if(false == CreateAndAddCounters())
			break;

		//first collect data
		if(m_hQueryBySys)
			PdhCollectQueryData(m_hQueryBySys);
		if (m_hQueryByPro)
			PdhCollectQueryData(m_hQueryByPro);

		bResult     = true;
		m_bIniFlag  = true;
		m_bStopFlag = false;
	} while (false);

	return bResult;
}

/*************************************************
// <Summary>get physical memory </Summary>
// <DateTime>2014/12/02</DateTime>
// <Parameter name="TotalPhyMem" type="OUT">total physical memory(unit:MB)</Parameter>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::GetPhyMem(unsigned long &TotalPhyMem)
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	if(FALSE == GlobalMemoryStatusEx(&statex))
		return false;

	TotalPhyMem = static_cast<unsigned long>((statex.ullTotalPhys / (1024 * 1024)));
	return true;
}

/*************************************************
// <Summary>get processor count</Summary>
// <DateTime>2014/12/02</DateTime>
// <Returns>processor count</Returns>
*************************************************/
int CResourceMonitor::GetProcessorNumber()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}

/*************************************************
// <Summary>create the specified process counters</Summary>
// <DateTime>2014/12/03</DateTime>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::CreateSpecialProcessCounters(bool force/* = false*/)
{
	bool bResult = false;
	do 
	{
		if(!m_strProName.empty())
		{
			if (force || !m_hQueryByPro || !m_hProTimeCounterByPro || !m_hWorkingSetCounterByPro || !m_hIODataBytesSecCounterByPro)
			{
				if(m_hQueryByPro)
				{
					PdhCloseQuery(m_hQueryByPro);
					m_hQueryByPro = NULL;
				}

				if (ERROR_SUCCESS != PdhOpenQueryA(NULL, 0, &m_hQueryByPro))  // open query handle
					break;

				std::string strFormatCpuUsage;
				CStringMethod::StringCat(strFormatCpuUsage, "\\Process(%s)\\%% Processor Time", m_strProName.c_str());

				std::string strFormatMemUsage;
				CStringMethod::StringCat(strFormatMemUsage, "\\Process(%s)\\Working Set", m_strProName.c_str());

				std::string strFormatIOByteSec;
				CStringMethod::StringCat(strFormatIOByteSec, "\\Process(%s)\\IO Data Bytes/sec", m_strProName.c_str());


				// add counter to query handle
				if(ERROR_SUCCESS != PdhAddCounterA(m_hQueryByPro,strFormatCpuUsage.c_str(), 0, &m_hProTimeCounterByPro)
					|| ERROR_SUCCESS != PdhAddCounterA(m_hQueryByPro,strFormatMemUsage.c_str(), 0, &m_hWorkingSetCounterByPro)
					|| ERROR_SUCCESS != PdhAddCounterA(m_hQueryByPro, strFormatIOByteSec.c_str(), 0, &m_hIODataBytesSecCounterByPro))
				{
					break;
				}

				if (force)
					PdhCollectQueryData(m_hQueryByPro);
			}
		}

		bResult = true;
	} while (false);

	if (false == bResult)
	{
		if(m_hQueryByPro) 
			PdhCloseQuery(m_hQueryByPro);
		m_hQueryByPro = NULL;
	}

	return bResult;
}

/*************************************************
// <Summary>create system resource counters</Summary>
// <DateTime>2014/12/03</DateTime>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::CreateSystemCounters()
{
	bool bResult = false;
	do 
	{
		if (!m_hQueryBySys || !m_hProTimeCounterBySys || !m_hDiskTimeCounterBySys || !m_hAvaiMemCounterBySys || !m_hNetTrafficCounterBySys || !m_hDiskBytesBySys)
		{
			if(m_hQueryBySys)
			{
				PdhCloseQuery(m_hQueryBySys);
				m_hQueryBySys = NULL;
			}

			if (ERROR_SUCCESS != PdhOpenQueryA(NULL, 0, &m_hQueryBySys))  // open query handle
				break;

			// add counter to query handle
			if(ERROR_SUCCESS != PdhAddCounterA(m_hQueryBySys,"\\Processor Information(_Total)\\% Processor Time", 0, &m_hProTimeCounterBySys)
				|| ERROR_SUCCESS != PdhAddCounterA(m_hQueryBySys, "\\PhysicalDisk(_Total)\\% Disk Time", 0, &m_hDiskTimeCounterBySys)
				|| ERROR_SUCCESS != PdhAddCounterA(m_hQueryBySys, "\\Memory\\Available MBytes", 0, &m_hAvaiMemCounterBySys)
				|| ERROR_SUCCESS != PdhAddCounterA(m_hQueryBySys, "\\Network Interface(*)\\Bytes Total/sec", 0, &m_hNetTrafficCounterBySys)
				|| ERROR_SUCCESS != PdhAddCounterA(m_hQueryBySys, "\\PhysicalDisk(_Total)\\Disk Bytes/sec", 0, &m_hDiskBytesBySys))
			{
				break;
			}
		}

		bResult = true;
	} while (false);

	if (false == bResult)
	{
		if(m_hQueryBySys)
			PdhCloseQuery(m_hQueryBySys);
		m_hQueryBySys = NULL;
	}

	return bResult;
}

/*************************************************
// <Summary>close system counters</Summary>
// <DateTime>2014/12/10</DateTime>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::CloseSystemCounters()
{
	if (m_hQueryBySys)
	{
		PdhCloseQuery(m_hQueryBySys);
		m_hQueryBySys = NULL;
	}

	m_hProTimeCounterBySys    = NULL;
	m_hDiskTimeCounterBySys   = NULL;
	m_hAvaiMemCounterBySys    = NULL;
	m_hNetTrafficCounterBySys = NULL;
	m_hDiskBytesBySys         = NULL;

	return true;
}

/*************************************************
// <Summary>close process counters</Summary>
// <DateTime>2014/12/10</DateTime>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::CloseProcessCounters()
{
	if (m_hQueryByPro)
	{
		PdhCloseQuery(m_hQueryByPro);
		m_hQueryByPro = NULL;
	}

	m_hProTimeCounterByPro        = NULL;
	m_hIODataBytesSecCounterByPro = NULL;
	m_hWorkingSetCounterByPro     = NULL;
	return true;
}

/*************************************************
// <Summary>clear environment, in addition to overweight value</Summary>
// <DateTime>2014/12/10</DateTime>
// <Returns>void</Returns>
*************************************************/
void CResourceMonitor::ClearEnv()
{
	m_unProCount       = 1;
	m_unTotalPhyMem_MB = 1;
	m_strProName = "";
}

/*************************************************
// <Summary>create and add counters to query object</Summary>
// <DateTime>2014/12/03</DateTime>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::CreateAndAddCounters()
{
	bool bResult = false;
	do 
	{
		bResult = CreateSystemCounters();
		if (true != bResult)
			break;

		bResult = CreateSpecialProcessCounters();
	} while (false);

	if (false == bResult)
	{
		CloseSystemCounters();
		CloseProcessCounters();
		ClearEnv();
		m_bIniFlag = false;
	}
	return bResult;
}

/*************************************************
// <Summary>set monitor process name</Summary>
// <DateTime>2014/12/02</DateTime>
// <Parameter name="strProNam" type="IN">process name</Parameter>
// <Returns>true/false</Returns>
*************************************************/
bool CResourceMonitor::SetProcessMoni(std::string strProNam)
{
	bool bResult = false;
	do
	{
		if (m_bStopFlag || m_bIniFlag)
			break;

		if (strProNam.empty())
		{
			bResult = true;
			break;
		}

		m_strProName = strProNam;
		bResult =  CreateSpecialProcessCounters(true);

	}while(false);

	if (false == bResult)
	{
		CloseSystemCounters();
		CloseProcessCounters();
		ClearEnv();
		m_bIniFlag = false;
	}

	return bResult;
}

/*************************************************
// <Summary>get performance data of the system by the PDH method  </Summary>
// <DateTime>2014/12/02</DateTime>
// <Parameter name="mapMonValue" type="IN/OUT">return data (XML format)</Parameter>
// <Returns>0:run error, 1:success</Returns>
*************************************************/
unsigned CResourceMonitor::GetGlobalValueByPDH(std::map<std::string, double> &mapMonValue)
{
	unsigned bResult = 0;

	PDH_STATUS pdhStatus = ERROR_SUCCESS;  // PDH state
	PDH_FMT_COUNTERVALUE displayValue;     // counter value

	do 
	{
		if(m_hQueryBySys && m_hProTimeCounterBySys && m_hDiskTimeCounterBySys && m_hAvaiMemCounterBySys && m_hNetTrafficCounterBySys && m_hDiskBytesBySys)
		{
			//second collect data
			pdhStatus = PdhCollectQueryData(m_hQueryBySys);
			if(ERROR_SUCCESS != pdhStatus)
			{
				printf("second collect data is error! pdhStatus = %d\n", pdhStatus);
				break;
			}

			//get processor time value
			pdhStatus = PdhGetFormattedCounterValue(m_hProTimeCounterBySys, PDH_FMT_DOUBLE, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("get processor time value is error! pdhStatus = %d\n", pdhStatus);
				break;
			}


			mapMonValue["sys_cpu"] = displayValue.doubleValue;


			//get disk timer usage rate
			pdhStatus = PdhGetFormattedCounterValue(m_hDiskTimeCounterBySys, PDH_FMT_LONG, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("get disk timer usage rate is error! pdhStatus = %d\n", pdhStatus);
				break;
			}

			mapMonValue["sys_disktime"] = displayValue.longValue;


			//get disk I/O bytes
			pdhStatus = PdhGetFormattedCounterValue(m_hDiskBytesBySys, PDH_FMT_DOUBLE, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("get disk I/O bytes is error! pdhStatus = %d\n", pdhStatus);
				break;
			}

			mapMonValue["sys_diskio"] = displayValue.doubleValue / (1024 * 1024);


			//get memory usage
			pdhStatus = PdhGetFormattedCounterValue(m_hAvaiMemCounterBySys, PDH_FMT_LONG, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("get memory usage is error! pdhStatus = %d\n", pdhStatus);
				break;
			}

			mapMonValue["sys_phymem"] = ((m_unTotalPhyMem_MB - displayValue.longValue) * 1.0/ m_unTotalPhyMem_MB) * 100;


			//get network traffic
			pdhStatus = PdhGetFormattedCounterValue(m_hNetTrafficCounterBySys, PDH_FMT_LONG, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("get network traffic is error! pdhStatus = %d\n", pdhStatus);
				break;
			}

			mapMonValue["sys_nettra"] = displayValue.longValue * 1.0 / (1024 * 1024);

			bResult = 1;
		}
	} while (false);

	if (0 == bResult)
	{
		if(m_hQueryBySys)
			PdhCloseQuery(m_hQueryBySys);
		m_hQueryBySys = NULL;
	}

	return bResult;
}


/*************************************************
// <Summary>get performance data of specific process by the PDH method  </Summary>
// <DateTime>2014/12/02</DateTime>
// <Parameter name="mapMonValue" type="IN/OUT">return data (XML format)</Parameter>
// <Returns>0:run error, 1:success, 2:success and a resource overweight</Returns>
*************************************************/
unsigned CResourceMonitor::GetSpeciValueByPDH(std::map<std::string, double> &mapMonValue)
{
	unsigned bResult = 0;
	PDH_STATUS pdhStatus = ERROR_SUCCESS;  // PDH state
	PDH_FMT_COUNTERVALUE displayValue;     // counter value 

	do 
	{
		if (m_hQueryByPro && m_hProTimeCounterByPro && m_hWorkingSetCounterByPro)
		{
			//second collect data
			pdhStatus = PdhCollectQueryData(m_hQueryByPro);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("second collect data is error! pdhStatus = %d", pdhStatus);
				break;
			}


			//get processor time value
			pdhStatus = PdhGetFormattedCounterValue(m_hProTimeCounterByPro, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("sget processor time value is error! pdhStatus = %d", pdhStatus);
				break;
			}

			mapMonValue["pro_cpu"] = displayValue.doubleValue / m_unProCount;

			//get memory usage rate
			pdhStatus = PdhGetFormattedCounterValue(m_hWorkingSetCounterByPro, PDH_FMT_LONG, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("get memory usage rate is error! pdhStatus = %d", pdhStatus);
				break;
			}

			mapMonValue["pro_phymem"] = (displayValue.longValue / (1024 * 1024)) * 1.0/ m_unTotalPhyMem_MB * 100;

			//get I/O data bytes per second(unit: MB/s)
			pdhStatus = PdhGetFormattedCounterValue(m_hIODataBytesSecCounterByPro, PDH_FMT_LONG, NULL, &displayValue);
			if (ERROR_SUCCESS != pdhStatus)
			{
				printf("et I/O data bytes per second is error! pdhStatus = %d", pdhStatus);
				break;
			}

			mapMonValue["pro_iodata"] = (displayValue.longValue * 1.0) /(1024 * 1024);

			bResult = 1;
		}
	} while (false);

	if (0 == bResult)
	{
		if(m_hQueryByPro)
			PdhCloseQuery(m_hQueryByPro);
		m_hQueryByPro = NULL;
	}

	return bResult;
}


/*************************************************
// <Summary>get performance data of the system or specific process by the PDH method  </Summary>
// <DateTime>2014/12/02</DateTime>
// <Parameter name="mapMonValue" type="IN/OUT">return data (XML format)</Parameter>
// <Parameter name="error_code" type="IN/OUT">resource error code</Parameter>
// <Returns>0:run error, 1:success, 2:success and a resource overweight</Returns>
//注意：如果函数返回错误，这是函数内部是将整个计数器都关闭了的，如果上层还想再次使用GetGlobalAndSpeciValueByPDH
//必须重新初始化，即重新调用InitGlobalData函数
*************************************************/
unsigned CResourceMonitor::GetGlobalAndSpeciValueByPDH(std::map<std::string, double> &mapMonValue, int &error_code)
{
	unsigned bResult = 0;
	mapMonValue.empty();
	error_code = SUCCESS;
	do 
	{
		if (m_bStopFlag)
		{
			printf("error1\n");
			error_code = STOP;
			break;
		}

		if (m_bIniFlag)
		{
			bResult = GetGlobalValueByPDH(mapMonValue);

			if (bResult && m_hQueryByPro)
				bResult = GetSpeciValueByPDH(mapMonValue);

			if(0 == bResult)
				error_code = NOTCOLLECTDATA_ERROR;
		}
		else
			error_code = NOTINITENV_ERROR;

		if (m_bStopFlag)
		{
			printf("error2\n");
			error_code = STOP;
			bResult = 0;
			break;
		}
	} while (false);

	if (0 == bResult)
	{
		printf("bResult = 0 \n");
		CloseSystemCounters();
		CloseProcessCounters();
		ClearEnv();
		m_bIniFlag  = false;
	}

	return bResult;
}
