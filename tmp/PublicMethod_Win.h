/************************************************************************/
/* �����ƣ�  CPublicMethod
/* �����ռ䣺��
/* �ļ�����  PublicMethod_Win.h  PublicMethod_Win.cpp
/* ����ʱ��: 2013-9-5
/* ���ߣ�    wanghongyun
/* ��˵����  ����Ϊ��̬�࣬�ṩһЩ�������� ��Щ�������Ծ�̬��Ա��ʽ�ṩ �;�������޹�  
/************************************************************************/

#ifndef PUBLICMETHOD_H
#define PUBLICMETHOD_H

#include <string>
#include <tchar.h>
#include <vector>

// �����ͷ��ڴ�����ö�� 
// ʹ�����ַ�ʽ ģ��C#ö������
struct ReleaseMemoryType 
{
	enum Enum  // ö��
	{
		RMT_FREE,          // ͨ��free�����ͷ�    
		RMT_DELETE,        // ͨ��delete�������ͷ�
		RMT_DELETE_ARRAY   // ͨ��delete[]�ͷ�
	};
private:
	Enum _value;  // ö��ֵ
	
public:
	// Ĭ�Ϲ��캯�� ģ��ͨ���������б�����ֵ�ķ�ʽ
	ReleaseMemoryType(Enum value = RMT_FREE):_value(value){} 
	// ����=���������
	ReleaseMemoryType& operator=(Enum value)
	{
		_value = value;
		return *this;
	}
	// ����ת������� Ĭ�Ͻ�ReleaseMemoryType��ʽת����Enum
	operator Enum() const 
	{
		return this->_value;
	}
};

class CPublicMethod
{
public:
	// �ļ����ļ��в���
	// �ļ��Ƿ����
    static bool FileExist(const char *pFilePath);
	static bool FileExist(const wchar_t *pFilePath);
	// Ŀ¼�Ƿ����
	static bool DirectoryExist(const char *pDirPath);
	// ����Ŀ¼
	static bool CreateDirectory(const char *pDirPath);
	// ����Ŀ¼
	static void GetFilesInDirectory(const char *pDirPath, const char *pFilter, std::vector<std::string> &filesPathList);
	// ��ȡ��ִ���ļ�Ŀ¼
	static std::string GetExePath();
	// ���ļ�����·���л���ļ���
	static std::string GetFileName(const char *pFilePath, bool getExt = true);
	// �����ļ�
	static void CopyFile(const char *pSrcPath, const char *pDstPath);
	// ɾ���ļ�
	static void DeleteFile(const char *pFilePath);

	// ANSI��UNICODE�ַ���ת��
	static char* W2A(const wchar_t *pWcsStr);
	static wchar_t* A2W(const char *pMbsStr);
    static std::string W2Str(const wchar_t *pWcsStr);
    static std::wstring A2WStr(const char *pMbsStr);

    // �ַ�����غ���
    // ����ʽ���ַ����ӵ�string�ַ����ĺ���
    static const std::string &StringCat(std::string &srcString, const char *formatString, ...);
    static const std::string &StringFormat(std::string &srcString, const char *pFormatString, ...);
    static const std::string &StringLeftTrim(std::string &srcString, char ch = ' ');
    static const std::string &StringRightTrim(std::string &srcString, char ch = ' ');
    static const std::string &StringTrim(std::string &srcString, char ch = ' ');
    static std::vector<std::string> SplitString(const std::string &strSrc, const std::string &strToken);

	// �ڴ��ͷ�
    template<typename T>
	static void ReleaseMemory(T *&point, ReleaseMemoryType type = ReleaseMemoryType::RMT_FREE);

	// �õ�ϵͳ����ʱ�䣨��1970-1-1��������������
	static time_t GetSysBootTime();
	// �õ�ϵͳ����ʱ�䣨��%d��%dʱ%d��%d����ʽ�ַ��������
	static std::string GetSystemRunTime();
    // ��ò���ϵͳ������
    static std::string GetOSName();
    //��ò���ϵͳ�İ汾��Ϣ
    static std::string GetOSVersion();
    // ���CPU����
    static unsigned long GetCPUNumber(unsigned short *pCpuRate = NULL);
    // ��ȡ�ڴ��С  ��λMB
    static unsigned long GetMemorySize(unsigned long *pFreeSize = NULL, unsigned short *pMemoryRate = NULL);
    // ��ȡӲ�̴�С ��λGB
    static unsigned long GetHardDiskSize(unsigned long *pFreeSize = NULL, unsigned short *pHardDiskRate = NULL);

protected:
	static const time_t Second_Bias_1900_1970 = 2208988800i64; // 1900-1-1��1970-1-1����������
};

// <Summary>��̬�ڴ�ռ���ͷ�</Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="point" type="IN">��̬�ڴ��ָ��</Parameter>
// <Parameter name="type" type="IN">�����ͷŵ��ڴ����� ��������free��������delete������ Ĭ��ΪRMT_FREE</Parameter>
template<typename T>
void CPublicMethod::ReleaseMemory(T *&point, ReleaseMemoryType type)
{
    if (point != NULL)
    {
        if (ReleaseMemoryType::RMT_FREE == type)
        {
            free(point);
        }
        else if (ReleaseMemoryType::RMT_DELETE == type)
        {
            delete point;
        }
        else if (ReleaseMemoryType::RMT_DELETE_ARRAY == type)
        {
            delete[] point;
        }
        point = NULL;
    }
}

#endif

