#pragma once
#include <stdlib.h>
#include <string.h>


namespace logger
{
	//定义日志的级别，数字越小级别越低
	enum log_level
	{
		_Debug	=	0x01,            
		_Warn	=	0x02,
		_Error	=	0x03,
		_Fatal	=	0x04,
	};

	enum log_print__type
	{
		PRINT_TO_FILE			= 0x01,		//输出到文件
		PRINT_TO_DATABASE		= 0x02,     //输出到数据库
		PRINT_TO_DEBUG_TRACE	= 0x04,		//输出到DEBUG VIEW  xuaioyi 2012-5-28
	};

	enum strategy_type
	{
		STRATEGY_RECORD = 0x01,				//多少条记录才写
		STRATEGY_SIZE,						//多大才写
	};

	struct log_param
	{
		unsigned long      dwSize;                 //这个结构体的大小(扩展用)
		char      szFileName[260];   //日志文件名，与CLogger::PrintwA或CLogger::PrintwB中第一个参数(文件名)相关联
		log_level  emLevel;				   //日志的级别，输出时如果小于配置的级别就无法输出
		unsigned long      dwLogPrintType;         //日志输出类型，见log_print_type可以类型相组合,用 | 或操作
		char      szKeyName[64];          //日志开关 设为与文件名相同
		int        optName;                //策略类型,见strategy_type
		char	   optVal[64];             //策略值   
		int        optLen;                 

		log_param(void) 
		{
			memset(this, 0, sizeof(*this));
			dwSize = sizeof(*this);
		}
	};
}	
