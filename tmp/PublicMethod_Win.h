/************************************************************************/
/* 类名称：  CPublicMethod
/* 命名空间：无
/* 文件名：  PublicMethod_Win.h  PublicMethod_Win.cpp
/* 创建时间: 2013-9-5
/* 作者：    wanghongyun
/* 类说明：  该类为静态类，提供一些公共方法 这些方法均以静态成员方式提供 和具体对象无关  
/************************************************************************/

#ifndef PUBLICMETHOD_H
#define PUBLICMETHOD_H

#include <string>
#include <tchar.h>
#include <vector>

// 定义释放内存类型枚举 
// 使用这种方式 模拟C#枚举类型
struct ReleaseMemoryType 
{
	enum Enum  // 枚举
	{
		RMT_FREE,          // 通过free函数释放    
		RMT_DELETE,        // 通过delete操作符释放
		RMT_DELETE_ARRAY   // 通过delete[]释放
	};
private:
	Enum _value;  // 枚举值
	
public:
	// 默认构造函数 模拟通过（）进行变量赋值的方式
	ReleaseMemoryType(Enum value = RMT_FREE):_value(value){} 
	// 重载=运算符函数
	ReleaseMemoryType& operator=(Enum value)
	{
		_value = value;
		return *this;
	}
	// 重载转换运算符 默认将ReleaseMemoryType隐式转化成Enum
	operator Enum() const 
	{
		return this->_value;
	}
};

class CPublicMethod
{
public:
	// 文件及文件夹操作
	// 文件是否存在
    static bool FileExist(const char *pFilePath);
	static bool FileExist(const wchar_t *pFilePath);
	// 目录是否存在
	static bool DirectoryExist(const char *pDirPath);
	// 创建目录
	static bool CreateDirectory(const char *pDirPath);
	// 遍历目录
	static void GetFilesInDirectory(const char *pDirPath, const char *pFilter, std::vector<std::string> &filesPathList);
	// 获取可执行文件目录
	static std::string GetExePath();
	// 从文件完整路径中获得文件名
	static std::string GetFileName(const char *pFilePath, bool getExt = true);
	// 复制文件
	static void CopyFile(const char *pSrcPath, const char *pDstPath);
	// 删除文件
	static void DeleteFile(const char *pFilePath);

	// ANSI和UNICODE字符集转换
	static char* W2A(const wchar_t *pWcsStr);
	static wchar_t* A2W(const char *pMbsStr);
    static std::string W2Str(const wchar_t *pWcsStr);
    static std::wstring A2WStr(const char *pMbsStr);

    // 字符串相关函数
    // 将格式化字符串加到string字符串的后面
    static const std::string &StringCat(std::string &srcString, const char *formatString, ...);
    static const std::string &StringFormat(std::string &srcString, const char *pFormatString, ...);
    static const std::string &StringLeftTrim(std::string &srcString, char ch = ' ');
    static const std::string &StringRightTrim(std::string &srcString, char ch = ' ');
    static const std::string &StringTrim(std::string &srcString, char ch = ' ');
    static std::vector<std::string> SplitString(const std::string &strSrc, const std::string &strToken);

	// 内存释放
    template<typename T>
	static void ReleaseMemory(T *&point, ReleaseMemoryType type = ReleaseMemoryType::RMT_FREE);

	// 得到系统启动时间（距1970-1-1所经过的秒数）
	static time_t GetSysBootTime();
	// 得到系统运行时间（以%d天%d时%d分%d秒形式字符串输出）
	static std::string GetSystemRunTime();
    // 获得操作系统的名称
    static std::string GetOSName();
    //获得操作系统的版本信息
    static std::string GetOSVersion();
    // 获得CPU个数
    static unsigned long GetCPUNumber(unsigned short *pCpuRate = NULL);
    // 获取内存大小  单位MB
    static unsigned long GetMemorySize(unsigned long *pFreeSize = NULL, unsigned short *pMemoryRate = NULL);
    // 获取硬盘大小 单位GB
    static unsigned long GetHardDiskSize(unsigned long *pFreeSize = NULL, unsigned short *pHardDiskRate = NULL);

protected:
	static const time_t Second_Bias_1900_1970 = 2208988800i64; // 1900-1-1到1970-1-1经过的秒数
};

// <Summary>动态内存空间的释放</Summary>
// <DateTime>2013/9/5</DateTime>
// <Parameter name="point" type="IN">动态内存的指针</Parameter>
// <Parameter name="type" type="IN">进行释放的内存类型 决定采用free函数还是delete操作符 默认为RMT_FREE</Parameter>
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

