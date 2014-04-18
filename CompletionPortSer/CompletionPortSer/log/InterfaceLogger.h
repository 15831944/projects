#pragma once
#include <string>
#include <assert.h>
#include "log_configure.h"
#include "PPSLoggerExport.h"
using namespace std;

/*!	xuxiaoyi	2012-5-31 14:28
*	说明：
*	1、使用日志功能时(使用PrintA、PrintW函数)要注册ppslogger.dll ,regsvr32 dll的路径
*	2、要有配置文件在windows目录下新建ppslog.ini
*	[log]
*	open=1;  总的日志开关
*	日志名=1；要写入的日志文件的开关  
*   3、日志级别使用时要先进行配置，才能有日志过滤功能 用Configure函数配置
*/

//zln;供外部调用的接口

namespace logger
{
	class CLogger
	{
	public:
		CLogger(void)
		{
			
		}

		~CLogger()
		{

		}
		static bool Start(void);

		static void Stop(void);
			
		static bool CanPrint(void);

		static void SetPath(char* path);

		/*! xuxiaoyi, 2012-6-26   15:12
		*	log_param param;
			param.emLevel = _Info;                                //日志级别
			param.dwLogPrintType = PRINT_TO_FILE;		          //输出方向：输出日志到文件
			strcpy_s(param.szFileName,MAX_PATH,_T("test_c"));    //存放日志的文件名
			strcpy_s(param.szKeyName,64,_T("test_c"));			  //与日志文件名一样
			param.optName = RECORD;	                              //输出策略：满一定条数的日志才输出
			int value = 20;
			memcpy_s(param.optVal, 64, &value, sizeof(value));    //每满20条数的日志就输出
			param.optLen = sizeof(value);			
		*/
		static bool Configure(const log_param& param);

		//只用于输出信息到日志文件 增加了日志级别输出控制
		//函数参数中增加了日志级别：PSLOG_DEBUG、PPSLOG_INFO、PPSLOG_WARMING、PPSLOG_ERROR、PPSLOG_FATAL
		static bool PrintA(const char* szFileName, log_level emLevel, const char* lpszFormat, ...);
		//只用于输出信息到日志文件
		static bool PrintA(const char* szFileName, const char* lpszFormat, ...);

	private:
		static IPPSLogger* m_pPpsLogger;
	};
}