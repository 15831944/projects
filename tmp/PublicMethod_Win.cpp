#include <Windows.h>
#include <fstream>
#include <direct.h>
#include <sys/stat.h>
#include <io.h>
#include <Pdh.h>
#include <time.h>

#include "PublicMethod_Win.h"

#pragma comment(lib, "Pdh.lib")

using std::fstream;
using std::ios;
using std::string;
using std::vector;
using std::wstring;

// <Summary>
// wchar_t字符串转成char字符串
// 转换的字符串始终以null字符结尾 使用后注意动态内存空间的释放
// </Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pWcsStr" type="IN">wchar_t字符串</Parameter>
// <Returns>转换得到的char字符串</Returns>
char* CPublicMethod::W2A(const wchar_t *pWcsStr)
{
    size_t wcsStrLen = wcsnlen_s(pWcsStr, _TRUNCATE);       // 宽字符串pWcsStr 字符个数
    size_t mallocLen = (wcsStrLen + 1) * sizeof(wchar_t);
    char *pDestString = (char *)malloc(mallocLen);          // 申请转换后的字符串内存空间
    if (NULL == pDestString) 
    {
        return NULL;
    }
    memset(pDestString, 0, mallocLen);

    size_t numberOfCharConverted = 0;                      // 实际转化的字符个数 包括null字符
    wcstombs(pDestString, pWcsStr, wcsStrLen);
    //wcstombs_s(&numberOfCharConverted, pDestString, mallocLen, pWcsStr, _TRUNCATE);
    return pDestString;
}

// <Summary>
// char字符串转成wchar_t字符串
// 转换的字符串始终以null字符结尾 使用后注意动态内存空间的释放
// </Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pMbsStr" type="IN">char字符串</Parameter>
// <Returns>转换得到的wchar_t字符串</Returns>
wchar_t* CPublicMethod::A2W(const char *pMbsStr)
{
    size_t mbsStrLen = strnlen_s(pMbsStr, _TRUNCATE);         // 字符串pMbsStr 字符个数
    size_t mallocLen = (mbsStrLen + 1) * sizeof(wchar_t);
    wchar_t *pDestString = (wchar_t *)malloc(mallocLen);      // 申请转换后的字符串内存空间
    if (NULL == pDestString)
    {
        return NULL;
    }
    memset(pDestString, 0, mallocLen);

    size_t numberOfCharConverted = 0;                         // 实际转化的字符个数 包括null字符
    mbstowcs(pDestString, pMbsStr, mbsStrLen);
    //mbstowcs_s(&numberOfCharConverted, pDestString, mallocLen, pMbsStr, _TRUNCATE);
    return pDestString;
}

// <Summary>wchar_t字符串转换为string字符串</Summary>
// <DateTime>2014/2/21</DateTime>
// <Parameter name="pWcsStr" type="IN">wchar_t字符串</Parameter>
// <Returns>转换后的string字符串</Returns>
string CPublicMethod::W2Str(const wchar_t *pWcsStr)
{
    char *pDest = W2A(pWcsStr);
    string strDest = pDest;
    ReleaseMemory(pDest);
    return strDest;
}

// <Summary>char字符串转换为wstring字符串</Summary>
// <DateTime>2014/2/21</DateTime>
// <Parameter name="pMbsStr" type="IN">char字符串</Parameter>
// <Returns>转换得到的wstring字符串</Returns>
wstring CPublicMethod::A2WStr(const char *pMbsStr)
{
    wchar_t *pDest = A2W(pMbsStr);
    wstring wstrDest = pDest;
    ReleaseMemory(pDest);
    return wstrDest;
}

// <Summary>文件是否存在</Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pFilePath" type="IN">文件完整路径</Parameter>
// <Returns>存在返回true 否则返回false</Returns>
bool CPublicMethod::FileExist(const char *pFilePath)
{
	if (NULL == pFilePath)
	{
		return false;
	}
	bool result = true;
	fstream file;
	file.open(pFilePath, ios::in);
	if (!file)
	{
		result = false;
	}
	file.clear();
	file.close();
	return result;
}

// <Summary>文件是否存在</Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pFilePath" type="IN">文件完整路径</Parameter>
// <Returns>存在返回true 否则返回false</Returns>
bool CPublicMethod::FileExist(const wchar_t *pFilePath)
{
	if (NULL == pFilePath)
	{
		return false;
	}
	char* pTempFilePath = W2A(pFilePath);
	if (NULL == pTempFilePath)
	{
		return false;
	}
	bool result = FileExist(pTempFilePath);
	ReleaseMemory(pTempFilePath);
	return result;
}

// <Summary>获得可执行文件所在的目录</Summary>
// <DateTime>2013/9/10</DateTime>
// <Returns>可执行文件所在的目录</Returns>
string CPublicMethod::GetExePath()
{
	char szModulePath[256] = {0};
	GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath)); // 获取可执行文件完整路径
	string modulePath(szModulePath);
	return modulePath.substr(0, modulePath.find_last_of('\\'));
}

// <Summary>判断文件夹是否存在</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pDirPath" type="IN">文件夹路径</Parameter>
// <Returns>如果文件夹存在返回true 否则返回false</Returns>
bool CPublicMethod::DirectoryExist(const char *pDirPath)
{
	if (NULL == pDirPath)
	{
		return false;
	}
	struct _stat dirStat;
	if (0 == _stat(pDirPath, &dirStat) && dirStat.st_mode & _S_IFDIR)
	{
		return true;
	}
	return false;
}

// <Summary>创建文件夹</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pDirPath" type="IN">文件夹路径</Parameter>
// <Returns>创建成功返回true 否则返回false</Returns>
bool CPublicMethod::CreateDirectory(const char *pDirPath)
{
	if (NULL == pDirPath)
	{
		return false;
	}
    if (DirectoryExist(pDirPath)) // 如果文件夹已经存在则返回true
    {
        return true;
    }

	if (0 == _mkdir(pDirPath))
	{
		return true;
	}
	return false;
}

// <Summary>遍历文件夹</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pDirPath" type="IN">文件夹路径</Parameter>
// <Parameter name="pFilter" type="IN">所有的文件类型 如“*.*”表示所有文件，“*.jpg”表示jpg类型文件 暂时不支持多种同时匹配多种类型文件</Parameter>
// <Parameter name="filesPathList" type="OUT">获取到的所有文件的完整路径</Parameter>
void CPublicMethod::GetFilesInDirectory(const char *pDirPath, const char *pFilter, vector<string> &filesPathList)
{
	if (pDirPath == NULL || pFilter == NULL)
	{
		return;
	}
	bool isRelativePath = true;  // pDirPath是否是相对路径
	filesPathList.clear();

	_finddata_t fileInfo;
	char dirPath[256] = {0};
	if (string(pDirPath).find("\\") != string::npos)
	{
		isRelativePath = false;
		sprintf_s(dirPath, sizeof(dirPath), "%s\\%s", pDirPath, pFilter);
	}
	else
	{
		sprintf_s(dirPath, _TRUNCATE, "%s/%s", pDirPath, pFilter);
	}
	long findHandle = _findfirst(dirPath, &fileInfo);
	if (findHandle == -1L)
	{
		return;
	}
	do 
	{
		if (0 == strncmp(fileInfo.name, ".", sizeof(".")) || 0 == strncmp(fileInfo.name, "..", sizeof("..")))
		{
			continue;
		}
		char filePath[256] = {0};
		if (isRelativePath)
		{
			sprintf_s(filePath, _TRUNCATE, "%s/%s", pDirPath, fileInfo.name);
		}
		else 
		{
			sprintf_s(filePath, _TRUNCATE, "%s\\%s", pDirPath, fileInfo.name);
		}
		filesPathList.push_back(string(filePath));
	} 
	while (0 == _findnext(findHandle, &fileInfo));
}

// <Summary>根据文件完整路径获得文件名</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pFilePath" type="IN">文件完整路径</Parameter>
// <Parameter name="getExt" type="IN">是否获得文件扩展名 默认为true</Parameter>
// <Returns>成功返回文件名 是否返回空字符串</Returns>
string CPublicMethod::GetFileName(const char *pFilePath, bool getExt)
{
	if (NULL == pFilePath)
	{
		return string();
	}

	// 先获得文件名 包括扩展名
	string filePath(pFilePath);
	string::size_type pos = filePath.find_last_of("\\");
	if (string::npos == pos)
	{
		pos = filePath.find_last_of("/");
	}
	if (pos != string::npos)
	{
        filePath = filePath.substr(pos + 1); // 文件名 包括文件扩展名
	}
	if (getExt)
	{
		return filePath;
	}

	return filePath.substr(0, filePath.find_last_of("."));
}

// <Summary>复制文件</Summary>
// <DateTime>2013/9/11</DateTime>
// <Parameter name="pSrcPath" type="IN">源文件路径</Parameter>
// <Parameter name="pDstPath" type="IN">目的路径</Parameter>
void CPublicMethod::CopyFile(const char *pSrcPath, const char *pDstPath)
{
	if (NULL == pSrcPath || !FileExist(pSrcPath) || NULL == pDstPath)
	{
		return;
	}

	fstream fin(pSrcPath, ios::binary | ios::in);    // 读取原文件数据
	fstream fout(pDstPath, ios::binary | ios::out);  // 打开目的文件（如果文件不存在 则创建文件）
	fout << fin.rdbuf();
}

// <Summary>删除指定文件</Summary>
// <DateTime>2013/9/11</DateTime>
// <Parameter name="pFilePath" type="IN">文件路径</Parameter>
void CPublicMethod::DeleteFile(const char *pFilePath)
{
	char cmd[256] = {0};
	sprintf_s(cmd, "del %s", pFilePath);
	system(cmd);
}

// <Summary>字符串连接操作</Summary>
// <DateTime>2013/9/12</DateTime>
// <Parameter name="srcString" type="INOUT">原字符串</Parameter>
// <Parameter name="formatString" type="IN">格式化字符串</Parameter>
// <Returns>连接后的字符串 增加返回值便于直接使用该函数进行表达式运算</Returns>
const string& CPublicMethod::StringCat(std::string &srcString, const char *formatString, ...)
{
    va_list args;
    char tempString[2048] = {0};  // 格式化字符串最大支持2048个字节
    va_start(args, formatString);
    vsnprintf_s(tempString, sizeof(tempString), formatString, args);
    srcString += tempString;
    return srcString;
}

// <Summary>字符串格式化</Summary>
// <DateTime>2013/9/12</DateTime>
// <Parameter name="srcString" type="INOUT">原字符串</Parameter>
// <Parameter name="formatString" type="IN">格式化字符串</Parameter>
// <Returns>格式化后的字符串</Returns>
const string& CPublicMethod::StringFormat(std::string &srcString, const char *pFormatString, ...)
{
    va_list args;
    char tempString[2048] = {0};  // 格式化字符串最大支持2048个字节
    va_start(args, pFormatString);
    vsnprintf_s(tempString, sizeof(tempString), pFormatString, args);
    srcString = tempString;
    return srcString;
}

// <Summary>清除字符串左端指定字符</Summary>
// <DateTime>2014/3/17</DateTime>
// <Parameter name="srcString" type="INOUT">原字符串</Parameter>
// <Parameter name="ch" type="IN">字符</Parameter>
// <Returns>处理后的字符串</Returns>
const string& CPublicMethod::StringLeftTrim(std::string &srcString, char ch)
{
    string::size_type count = 0;   // 保存左端ch字符的个数

    for (string::size_type i = 0; i < srcString.size(); ++i)
    {
        if (srcString[i] == ch)
        {
            ++count;
        }
        else 
        {
            break;
        }
    }

    if (count > 0)
    {
    	srcString.erase(0, count);
    }
    return srcString;
}

// <Summary>清除字符串右端指定字符</Summary>
// <DateTime>2014/3/17</DateTime>
// <Parameter name="srcString" type="INOUT">原字符串</Parameter>
// <Parameter name="ch" type="IN">字符</Parameter>
// <Returns>处理后的字符串</Returns>
const string& CPublicMethod::StringRightTrim(std::string &srcString, char ch)
{
    string::size_type count = 0;   // 保存左端ch字符的个数

    string::size_type i = srcString.size() - 1;
    for (; i >= 0; --i)
    {
        if (srcString[i] == ch)
        {
            ++count;
        }
        else 
        {
            break;
        }
    }

    if (count > 0)
    {
        srcString.erase(i + 1, count);
    }
    return srcString;
}

// <Summary>清除字符串两端指定字符</Summary>
// <DateTime>2014/3/17</DateTime>
// <Parameter name="srcString" type="INOUT">原字符串</Parameter>
// <Parameter name="ch" type="IN">字符</Parameter>
// <Returns>处理后的字符串</Returns>
const string& CPublicMethod::StringTrim(std::string &srcString, char ch)
{
    StringLeftTrim(srcString, ch);
    StringRightTrim(srcString, ch);

    return srcString;
}

// <Summary>分割字符串</Summary>
// <DateTime>2014/5/9</DateTime>
// <Parameter name="strSrc" type="IN">源字符串</Parameter>
// <Parameter name="strToken" type="IN">分隔标志</Parameter>
// <Returns>分隔后的子字符串集合</Returns>
vector<string> CPublicMethod::SplitString(const std::string &strSrc, const std::string &strToken)
{
    vector<string> splitStrList;
    string::size_type offset = 0;                   // 从源字符串中获取子串的偏移位置
    string::size_type pos = strSrc.find(strToken);  // 分隔标志的位置
    while(pos != string::npos)
    {
        string str = strSrc.substr(offset, pos - offset); // 获取子串
        if (!str.empty())  // 该子串非空的话 插入vector中
        {
            splitStrList.push_back(str);
        }
        offset = pos + strToken.size();
        pos = strSrc.find(strToken, offset);
    }
    string str = strSrc.substr(offset);
    if (!str.empty())
    {
        splitStrList.push_back(str);
    }

    return splitStrList;
}

// <Summary>得到系统启动时间（距1970-1-1所经过的秒数）</Summary>
// <DateTime>2013/9/13</DateTime>
// <Returns>系统启动距1970-1-1所经过的秒数</Returns>
time_t CPublicMethod::GetSysBootTime()
{
    time_t runTime = GetTickCount() / 1000;
    // time_t runTime = GetTickCount64() / 1000;  // 系统运行秒数
    time_t nowTime = time(NULL) + Second_Bias_1900_1970;  // 当前时间（距1970-1-1所经过的秒数）
    return nowTime - runTime;
}

// <Summary>得到系统运行时间</Summary>
// <DateTime>2013/9/13</DateTime>
// <Returns>系统运行时间（以%d天%d时%d分%d秒形式字符串输出）</Returns>
string CPublicMethod::GetSystemRunTime()
{
    time_t runTime = GetTickCount() / 1000;
	// time_t runTime = GetTickCount64() / 1000;  // 系统已经运行的秒数
	// 分别计算已经运行天、时、分秒
	int day = static_cast<int>(runTime / (3600 * 24));
	int hour = static_cast<int>((runTime % (3600 * 24)) / 3600); 
	int minute = static_cast<int>((runTime / 60) % 60);
	int second = static_cast<int>(runTime % 60);
	char runTimeString[256] = {0};
	if (day > 0)
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%d天%d时%d分%d秒", day, hour, minute, second);
	}
	else if (hour > 0)
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%d时%d分%d秒", hour, minute, second);
	}
	else if (minute > 0)
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%d分%d秒", minute, second);
	}
	else 
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%d秒", second);
	}
	return string(runTimeString);
}

// <Summary>获得操作系统的名称</Summary>
// <DateTime>2013/9/13</DateTime>
// <Returns>操作系统名称</Returns>
string CPublicMethod::GetOSName()
{
    string osName;

    OSVERSIONINFOEX osVersion = {0};
    osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);  
    if (GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osVersion)))
    {
        switch(osVersion.dwMajorVersion)  // 判断主版本号
        {
        case 5:
            {
                switch(osVersion.dwMinorVersion)
                {
                case 0:
                    {
                        osName = "Microsoft Windows 2000";
                    }
                    break;
                case 1:
                    {
                        osName = "Microsoft Windows XP";
                    }
                    break;
                case 2:
                    {
                        osName = "Microsoft Windows Server 2003";
                    }
                    break;
                }
            }
            break;
        case 6:
            {
                switch(osVersion.dwMinorVersion)
                {
                case 0:
                    {
                        if (osVersion.wProductType == VER_NT_WORKSTATION)
                        {
                            osName = "Microsoft Windows Vista";
                        }
                        else 
                        {
                            osName = "Microsoft Windows Server 2008";
                        }
                    }
                    break;
                case 1:
                    {
                        if (osVersion.wProductType == VER_NT_WORKSTATION)
                        {
                            osName = "Microsoft Windows 7";
                        }
                        else
                        {
                            osName = "Microsoft Windows Server 2008 R2";
                        }
                    }
                    break;
                case 2:
                    {
                        osName = "Microsoft Windows 8";
                    }
                    break;
                }
            }
            break;
        default:
            {
                osName = "未知系统名称或者系统版本过老";
            }
        }
    }
    return osName;
}

// <Summary></Summary>
// <DateTime>2013/9/13</DateTime>
// <Parameter name="" type="IN"></Parameter>
// <Returns></Returns>
// <Exception cref=""></Exception>
string CPublicMethod::GetOSVersion()
{
    return "";
}

// <Summary>获得CPU个数</Summary>
// <DateTime>2013/9/16</DateTime>
// <Parameter name="" type="OUT">CPU占用率</Parameter>
// <Returns>CPU个数</Returns>
unsigned long CPublicMethod::GetCPUNumber(unsigned short *pCpuRate)
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    if (pCpuRate != NULL)
    {
        PDH_STATUS pdhStatus = ERROR_SUCCESS;  // PDH状态
        HQUERY hQuery = NULL;                  // pdh查询句柄
        HCOUNTER hCounter = NULL;              // 计数器句柄
        PDH_FMT_COUNTERVALUE displayValue;     // 计数器的值

        *pCpuRate = 0;
        if (PdhOpenQueryA(NULL, 0, &hQuery) == ERROR_SUCCESS)  // 打开查询句柄
        {
            // 加入计数器
            if (ERROR_SUCCESS == PdhAddCounterA(hQuery, "\\Processor Information(_Total)\\% Processor Time", 0, &hCounter))
            {
                // 获取cpu占用率需用进行两次收集CPU时间数据 中间间隔50ms
                PdhCollectQueryData(hQuery);  // 第一次收集数据
                Sleep(50);
                PdhCollectQueryData(hQuery);  // 第二次收集数据
                pdhStatus = PdhGetFormattedCounterValue(hCounter, PDH_FMT_LONG, NULL, &displayValue); // 得到格式化数据
                if (pdhStatus == ERROR_SUCCESS)
                {
                    *pCpuRate = static_cast<unsigned short>(displayValue.longValue);
                }
            }
        }
        
    }
    return systemInfo.dwNumberOfProcessors;
}

// <Summary>获取内存大小  单位MB</Summary>
// <DateTime>2013/9/16</DateTime>
// <Parameter name="pFreeSize" type="OUT">当前空闲内存大小</Parameter>
// <Parameter name="pMemoryRate" type="OUT">内存使用率</Parameter>
// <Returns>内存大小</Returns>
unsigned long CPublicMethod::GetMemorySize(unsigned long *pFreeSize, unsigned short *pMemoryRate)
{
    MEMORYSTATUSEX memoryStatus = {0};
    memoryStatus.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memoryStatus))
    {
        return 0;
    }
    if (pFreeSize != NULL)
    {
        
        *pFreeSize =  static_cast<unsigned long>(memoryStatus.ullAvailPhys / 1024 / 1024);
    }
    if (pMemoryRate != NULL)
    {

        *pMemoryRate = static_cast<unsigned short>(memoryStatus.dwMemoryLoad);
    }
    return static_cast<unsigned long>(memoryStatus.ullTotalPhys / 1024 / 1024);
}

// <Summary>获取硬盘大小 单位GB</Summary>
// <DateTime>2013/9/16</DateTime>
// <Parameter name="pFreeSize" type="OUT">硬盘空闲大小</Parameter>
// <Parameter name="pHardDiskRate" type="OUT">硬盘占用率</Parameter>
// <Returns>硬盘大小</Returns>
unsigned long CPublicMethod::GetHardDiskSize(unsigned long *pFreeSize, unsigned short *pHardDiskRate)
{
    ULARGE_INTEGER totalNumberOfBytes;          // 相应盘符的内存总量
    ULARGE_INTEGER totalNumberOfFreeBytes;      // 相应盘符的空闲内存大小
    LONGLONG freeSize = 0;
    LONGLONG totalSize = 0;

    char szDriveCode[256] = {0};
    if (0 == GetLogicalDriveStringsA(256, szDriveCode))
    {
        return 0;
    }

    for (char *pDriveCode = szDriveCode; *pDriveCode; pDriveCode += strlen(pDriveCode) + 1)
    {
        
        if (GetDriveTypeA(pDriveCode) == DRIVE_FIXED)
        {
            if (GetDiskFreeSpaceExA(pDriveCode, NULL, &totalNumberOfBytes, &totalNumberOfFreeBytes))
            {
                totalSize += totalNumberOfBytes.QuadPart;
                freeSize += totalNumberOfFreeBytes.QuadPart;
            }
        }
    }

    if (pFreeSize != NULL)
    {
        *pFreeSize = static_cast<unsigned long>(freeSize / 1024 / 1024 / 1024);
    }
    if (pHardDiskRate != NULL)
    {
        *pHardDiskRate = static_cast<unsigned short>(100 * (totalSize - freeSize) / totalSize);
    }
    return static_cast<unsigned long>(totalSize / 1024 / 1024 / 1024);
}