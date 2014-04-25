#pragma once
#include "log_configure.h"

//ios,android,windows，需自己实现这个接口,由CLogger使用

namespace logger{
	struct IPPSLogger 
	{
		IPPSLogger(void){};
		virtual ~IPPSLogger(void){};

		//用于启动执行输出日志的消息任务线程
		virtual bool start(void) = 0;
		//用于结束执行输出日志的消息任务线程
		virtual bool stop(void) = 0;
		//用于配置日志输出
		virtual bool configure(const logger::log_param& param) = 0;
		//输出ansi的日志内容  参数：szFileName 存储日志的文件名,emLevel 日志级别
		virtual bool PrintA(const char* szFileName, logger::log_level emLevel, const char* lpszContent) = 0;
		//获得总开关能否打印日志
		virtual bool CanPrint(void) = 0;
		//获得日志的级别
		virtual logger::log_level GetLogLevel(void) = 0;
		//设置路径
		virtual void SetPath(char* path) = 0;
	};

	IPPSLogger* CreateInterface(void);
	void DeleteInterface(void);
}








