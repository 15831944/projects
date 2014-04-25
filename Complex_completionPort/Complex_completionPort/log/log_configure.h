#pragma once
#include <stdlib.h>
#include <string.h>


namespace logger
{
	//������־�ļ�������ԽС����Խ��
	enum log_level
	{
		_Debug	=	0x01,            
		_Warn	=	0x02,
		_Error	=	0x03,
		_Fatal	=	0x04,
	};

	enum log_print__type
	{
		PRINT_TO_FILE			= 0x01,		//������ļ�
		PRINT_TO_DATABASE		= 0x02,     //��������ݿ�
		PRINT_TO_DEBUG_TRACE	= 0x04,		//�����DEBUG VIEW  xuaioyi 2012-5-28
	};

	enum strategy_type
	{
		STRATEGY_RECORD = 0x01,				//��������¼��д
		STRATEGY_SIZE,						//����д
	};

	struct log_param
	{
		unsigned long      dwSize;                 //����ṹ��Ĵ�С(��չ��)
		char      szFileName[260];   //��־�ļ�������CLogger::PrintwA��CLogger::PrintwB�е�һ������(�ļ���)�����
		log_level  emLevel;				   //��־�ļ������ʱ���С�����õļ�����޷����
		unsigned long      dwLogPrintType;         //��־������ͣ���log_print_type�������������,�� | �����
		char      szKeyName[64];          //��־���� ��Ϊ���ļ�����ͬ
		int        optName;                //��������,��strategy_type
		char	   optVal[64];             //����ֵ   
		int        optLen;                 

		log_param(void) 
		{
			memset(this, 0, sizeof(*this));
			dwSize = sizeof(*this);
		}
	};
}	
