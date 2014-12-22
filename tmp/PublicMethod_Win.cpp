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
// wchar_t�ַ���ת��char�ַ���
// ת�����ַ���ʼ����null�ַ���β ʹ�ú�ע�⶯̬�ڴ�ռ���ͷ�
// </Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pWcsStr" type="IN">wchar_t�ַ���</Parameter>
// <Returns>ת���õ���char�ַ���</Returns>
char* CPublicMethod::W2A(const wchar_t *pWcsStr)
{
    size_t wcsStrLen = wcsnlen_s(pWcsStr, _TRUNCATE);       // ���ַ���pWcsStr �ַ�����
    size_t mallocLen = (wcsStrLen + 1) * sizeof(wchar_t);
    char *pDestString = (char *)malloc(mallocLen);          // ����ת������ַ����ڴ�ռ�
    if (NULL == pDestString) 
    {
        return NULL;
    }
    memset(pDestString, 0, mallocLen);

    size_t numberOfCharConverted = 0;                      // ʵ��ת�����ַ����� ����null�ַ�
    wcstombs(pDestString, pWcsStr, wcsStrLen);
    //wcstombs_s(&numberOfCharConverted, pDestString, mallocLen, pWcsStr, _TRUNCATE);
    return pDestString;
}

// <Summary>
// char�ַ���ת��wchar_t�ַ���
// ת�����ַ���ʼ����null�ַ���β ʹ�ú�ע�⶯̬�ڴ�ռ���ͷ�
// </Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pMbsStr" type="IN">char�ַ���</Parameter>
// <Returns>ת���õ���wchar_t�ַ���</Returns>
wchar_t* CPublicMethod::A2W(const char *pMbsStr)
{
    size_t mbsStrLen = strnlen_s(pMbsStr, _TRUNCATE);         // �ַ���pMbsStr �ַ�����
    size_t mallocLen = (mbsStrLen + 1) * sizeof(wchar_t);
    wchar_t *pDestString = (wchar_t *)malloc(mallocLen);      // ����ת������ַ����ڴ�ռ�
    if (NULL == pDestString)
    {
        return NULL;
    }
    memset(pDestString, 0, mallocLen);

    size_t numberOfCharConverted = 0;                         // ʵ��ת�����ַ����� ����null�ַ�
    mbstowcs(pDestString, pMbsStr, mbsStrLen);
    //mbstowcs_s(&numberOfCharConverted, pDestString, mallocLen, pMbsStr, _TRUNCATE);
    return pDestString;
}

// <Summary>wchar_t�ַ���ת��Ϊstring�ַ���</Summary>
// <DateTime>2014/2/21</DateTime>
// <Parameter name="pWcsStr" type="IN">wchar_t�ַ���</Parameter>
// <Returns>ת�����string�ַ���</Returns>
string CPublicMethod::W2Str(const wchar_t *pWcsStr)
{
    char *pDest = W2A(pWcsStr);
    string strDest = pDest;
    ReleaseMemory(pDest);
    return strDest;
}

// <Summary>char�ַ���ת��Ϊwstring�ַ���</Summary>
// <DateTime>2014/2/21</DateTime>
// <Parameter name="pMbsStr" type="IN">char�ַ���</Parameter>
// <Returns>ת���õ���wstring�ַ���</Returns>
wstring CPublicMethod::A2WStr(const char *pMbsStr)
{
    wchar_t *pDest = A2W(pMbsStr);
    wstring wstrDest = pDest;
    ReleaseMemory(pDest);
    return wstrDest;
}

// <Summary>�ļ��Ƿ����</Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pFilePath" type="IN">�ļ�����·��</Parameter>
// <Returns>���ڷ���true ���򷵻�false</Returns>
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

// <Summary>�ļ��Ƿ����</Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="pFilePath" type="IN">�ļ�����·��</Parameter>
// <Returns>���ڷ���true ���򷵻�false</Returns>
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

// <Summary>��ÿ�ִ���ļ����ڵ�Ŀ¼</Summary>
// <DateTime>2013/9/10</DateTime>
// <Returns>��ִ���ļ����ڵ�Ŀ¼</Returns>
string CPublicMethod::GetExePath()
{
	char szModulePath[256] = {0};
	GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath)); // ��ȡ��ִ���ļ�����·��
	string modulePath(szModulePath);
	return modulePath.substr(0, modulePath.find_last_of('\\'));
}

// <Summary>�ж��ļ����Ƿ����</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pDirPath" type="IN">�ļ���·��</Parameter>
// <Returns>����ļ��д��ڷ���true ���򷵻�false</Returns>
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

// <Summary>�����ļ���</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pDirPath" type="IN">�ļ���·��</Parameter>
// <Returns>�����ɹ�����true ���򷵻�false</Returns>
bool CPublicMethod::CreateDirectory(const char *pDirPath)
{
	if (NULL == pDirPath)
	{
		return false;
	}
    if (DirectoryExist(pDirPath)) // ����ļ����Ѿ������򷵻�true
    {
        return true;
    }

	if (0 == _mkdir(pDirPath))
	{
		return true;
	}
	return false;
}

// <Summary>�����ļ���</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pDirPath" type="IN">�ļ���·��</Parameter>
// <Parameter name="pFilter" type="IN">���е��ļ����� �硰*.*����ʾ�����ļ�����*.jpg����ʾjpg�����ļ� ��ʱ��֧�ֶ���ͬʱƥ����������ļ�</Parameter>
// <Parameter name="filesPathList" type="OUT">��ȡ���������ļ�������·��</Parameter>
void CPublicMethod::GetFilesInDirectory(const char *pDirPath, const char *pFilter, vector<string> &filesPathList)
{
	if (pDirPath == NULL || pFilter == NULL)
	{
		return;
	}
	bool isRelativePath = true;  // pDirPath�Ƿ������·��
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

// <Summary>�����ļ�����·������ļ���</Summary>
// <DateTime>2013/9/10</DateTime>
// <Parameter name="pFilePath" type="IN">�ļ�����·��</Parameter>
// <Parameter name="getExt" type="IN">�Ƿ����ļ���չ�� Ĭ��Ϊtrue</Parameter>
// <Returns>�ɹ������ļ��� �Ƿ񷵻ؿ��ַ���</Returns>
string CPublicMethod::GetFileName(const char *pFilePath, bool getExt)
{
	if (NULL == pFilePath)
	{
		return string();
	}

	// �Ȼ���ļ��� ������չ��
	string filePath(pFilePath);
	string::size_type pos = filePath.find_last_of("\\");
	if (string::npos == pos)
	{
		pos = filePath.find_last_of("/");
	}
	if (pos != string::npos)
	{
        filePath = filePath.substr(pos + 1); // �ļ��� �����ļ���չ��
	}
	if (getExt)
	{
		return filePath;
	}

	return filePath.substr(0, filePath.find_last_of("."));
}

// <Summary>�����ļ�</Summary>
// <DateTime>2013/9/11</DateTime>
// <Parameter name="pSrcPath" type="IN">Դ�ļ�·��</Parameter>
// <Parameter name="pDstPath" type="IN">Ŀ��·��</Parameter>
void CPublicMethod::CopyFile(const char *pSrcPath, const char *pDstPath)
{
	if (NULL == pSrcPath || !FileExist(pSrcPath) || NULL == pDstPath)
	{
		return;
	}

	fstream fin(pSrcPath, ios::binary | ios::in);    // ��ȡԭ�ļ�����
	fstream fout(pDstPath, ios::binary | ios::out);  // ��Ŀ���ļ�������ļ������� �򴴽��ļ���
	fout << fin.rdbuf();
}

// <Summary>ɾ��ָ���ļ�</Summary>
// <DateTime>2013/9/11</DateTime>
// <Parameter name="pFilePath" type="IN">�ļ�·��</Parameter>
void CPublicMethod::DeleteFile(const char *pFilePath)
{
	char cmd[256] = {0};
	sprintf_s(cmd, "del %s", pFilePath);
	system(cmd);
}

// <Summary>�ַ������Ӳ���</Summary>
// <DateTime>2013/9/12</DateTime>
// <Parameter name="srcString" type="INOUT">ԭ�ַ���</Parameter>
// <Parameter name="formatString" type="IN">��ʽ���ַ���</Parameter>
// <Returns>���Ӻ���ַ��� ���ӷ���ֵ����ֱ��ʹ�øú������б��ʽ����</Returns>
const string& CPublicMethod::StringCat(std::string &srcString, const char *formatString, ...)
{
    va_list args;
    char tempString[2048] = {0};  // ��ʽ���ַ������֧��2048���ֽ�
    va_start(args, formatString);
    vsnprintf_s(tempString, sizeof(tempString), formatString, args);
    srcString += tempString;
    return srcString;
}

// <Summary>�ַ�����ʽ��</Summary>
// <DateTime>2013/9/12</DateTime>
// <Parameter name="srcString" type="INOUT">ԭ�ַ���</Parameter>
// <Parameter name="formatString" type="IN">��ʽ���ַ���</Parameter>
// <Returns>��ʽ������ַ���</Returns>
const string& CPublicMethod::StringFormat(std::string &srcString, const char *pFormatString, ...)
{
    va_list args;
    char tempString[2048] = {0};  // ��ʽ���ַ������֧��2048���ֽ�
    va_start(args, pFormatString);
    vsnprintf_s(tempString, sizeof(tempString), pFormatString, args);
    srcString = tempString;
    return srcString;
}

// <Summary>����ַ������ָ���ַ�</Summary>
// <DateTime>2014/3/17</DateTime>
// <Parameter name="srcString" type="INOUT">ԭ�ַ���</Parameter>
// <Parameter name="ch" type="IN">�ַ�</Parameter>
// <Returns>�������ַ���</Returns>
const string& CPublicMethod::StringLeftTrim(std::string &srcString, char ch)
{
    string::size_type count = 0;   // �������ch�ַ��ĸ���

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

// <Summary>����ַ����Ҷ�ָ���ַ�</Summary>
// <DateTime>2014/3/17</DateTime>
// <Parameter name="srcString" type="INOUT">ԭ�ַ���</Parameter>
// <Parameter name="ch" type="IN">�ַ�</Parameter>
// <Returns>�������ַ���</Returns>
const string& CPublicMethod::StringRightTrim(std::string &srcString, char ch)
{
    string::size_type count = 0;   // �������ch�ַ��ĸ���

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

// <Summary>����ַ�������ָ���ַ�</Summary>
// <DateTime>2014/3/17</DateTime>
// <Parameter name="srcString" type="INOUT">ԭ�ַ���</Parameter>
// <Parameter name="ch" type="IN">�ַ�</Parameter>
// <Returns>�������ַ���</Returns>
const string& CPublicMethod::StringTrim(std::string &srcString, char ch)
{
    StringLeftTrim(srcString, ch);
    StringRightTrim(srcString, ch);

    return srcString;
}

// <Summary>�ָ��ַ���</Summary>
// <DateTime>2014/5/9</DateTime>
// <Parameter name="strSrc" type="IN">Դ�ַ���</Parameter>
// <Parameter name="strToken" type="IN">�ָ���־</Parameter>
// <Returns>�ָ�������ַ�������</Returns>
vector<string> CPublicMethod::SplitString(const std::string &strSrc, const std::string &strToken)
{
    vector<string> splitStrList;
    string::size_type offset = 0;                   // ��Դ�ַ����л�ȡ�Ӵ���ƫ��λ��
    string::size_type pos = strSrc.find(strToken);  // �ָ���־��λ��
    while(pos != string::npos)
    {
        string str = strSrc.substr(offset, pos - offset); // ��ȡ�Ӵ�
        if (!str.empty())  // ���Ӵ��ǿյĻ� ����vector��
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

// <Summary>�õ�ϵͳ����ʱ�䣨��1970-1-1��������������</Summary>
// <DateTime>2013/9/13</DateTime>
// <Returns>ϵͳ������1970-1-1������������</Returns>
time_t CPublicMethod::GetSysBootTime()
{
    time_t runTime = GetTickCount() / 1000;
    // time_t runTime = GetTickCount64() / 1000;  // ϵͳ��������
    time_t nowTime = time(NULL) + Second_Bias_1900_1970;  // ��ǰʱ�䣨��1970-1-1��������������
    return nowTime - runTime;
}

// <Summary>�õ�ϵͳ����ʱ��</Summary>
// <DateTime>2013/9/13</DateTime>
// <Returns>ϵͳ����ʱ�䣨��%d��%dʱ%d��%d����ʽ�ַ��������</Returns>
string CPublicMethod::GetSystemRunTime()
{
    time_t runTime = GetTickCount() / 1000;
	// time_t runTime = GetTickCount64() / 1000;  // ϵͳ�Ѿ����е�����
	// �ֱ�����Ѿ������졢ʱ������
	int day = static_cast<int>(runTime / (3600 * 24));
	int hour = static_cast<int>((runTime % (3600 * 24)) / 3600); 
	int minute = static_cast<int>((runTime / 60) % 60);
	int second = static_cast<int>(runTime % 60);
	char runTimeString[256] = {0};
	if (day > 0)
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%d��%dʱ%d��%d��", day, hour, minute, second);
	}
	else if (hour > 0)
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%dʱ%d��%d��", hour, minute, second);
	}
	else if (minute > 0)
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%d��%d��", minute, second);
	}
	else 
	{
		sprintf_s(runTimeString, sizeof(runTimeString), "%d��", second);
	}
	return string(runTimeString);
}

// <Summary>��ò���ϵͳ������</Summary>
// <DateTime>2013/9/13</DateTime>
// <Returns>����ϵͳ����</Returns>
string CPublicMethod::GetOSName()
{
    string osName;

    OSVERSIONINFOEX osVersion = {0};
    osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);  
    if (GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osVersion)))
    {
        switch(osVersion.dwMajorVersion)  // �ж����汾��
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
                osName = "δ֪ϵͳ���ƻ���ϵͳ�汾����";
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

// <Summary>���CPU����</Summary>
// <DateTime>2013/9/16</DateTime>
// <Parameter name="" type="OUT">CPUռ����</Parameter>
// <Returns>CPU����</Returns>
unsigned long CPublicMethod::GetCPUNumber(unsigned short *pCpuRate)
{
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    if (pCpuRate != NULL)
    {
        PDH_STATUS pdhStatus = ERROR_SUCCESS;  // PDH״̬
        HQUERY hQuery = NULL;                  // pdh��ѯ���
        HCOUNTER hCounter = NULL;              // ���������
        PDH_FMT_COUNTERVALUE displayValue;     // ��������ֵ

        *pCpuRate = 0;
        if (PdhOpenQueryA(NULL, 0, &hQuery) == ERROR_SUCCESS)  // �򿪲�ѯ���
        {
            // ���������
            if (ERROR_SUCCESS == PdhAddCounterA(hQuery, "\\Processor Information(_Total)\\% Processor Time", 0, &hCounter))
            {
                // ��ȡcpuռ�������ý��������ռ�CPUʱ������ �м���50ms
                PdhCollectQueryData(hQuery);  // ��һ���ռ�����
                Sleep(50);
                PdhCollectQueryData(hQuery);  // �ڶ����ռ�����
                pdhStatus = PdhGetFormattedCounterValue(hCounter, PDH_FMT_LONG, NULL, &displayValue); // �õ���ʽ������
                if (pdhStatus == ERROR_SUCCESS)
                {
                    *pCpuRate = static_cast<unsigned short>(displayValue.longValue);
                }
            }
        }
        
    }
    return systemInfo.dwNumberOfProcessors;
}

// <Summary>��ȡ�ڴ��С  ��λMB</Summary>
// <DateTime>2013/9/16</DateTime>
// <Parameter name="pFreeSize" type="OUT">��ǰ�����ڴ��С</Parameter>
// <Parameter name="pMemoryRate" type="OUT">�ڴ�ʹ����</Parameter>
// <Returns>�ڴ��С</Returns>
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

// <Summary>��ȡӲ�̴�С ��λGB</Summary>
// <DateTime>2013/9/16</DateTime>
// <Parameter name="pFreeSize" type="OUT">Ӳ�̿��д�С</Parameter>
// <Parameter name="pHardDiskRate" type="OUT">Ӳ��ռ����</Parameter>
// <Returns>Ӳ�̴�С</Returns>
unsigned long CPublicMethod::GetHardDiskSize(unsigned long *pFreeSize, unsigned short *pHardDiskRate)
{
    ULARGE_INTEGER totalNumberOfBytes;          // ��Ӧ�̷����ڴ�����
    ULARGE_INTEGER totalNumberOfFreeBytes;      // ��Ӧ�̷��Ŀ����ڴ��С
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