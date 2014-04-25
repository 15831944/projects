#pragma once
#include "log_configure.h"

//ios,android,windows�����Լ�ʵ������ӿ�,��CLoggerʹ��

namespace logger{
	struct IPPSLogger 
	{
		IPPSLogger(void){};
		virtual ~IPPSLogger(void){};

		//��������ִ�������־����Ϣ�����߳�
		virtual bool start(void) = 0;
		//���ڽ���ִ�������־����Ϣ�����߳�
		virtual bool stop(void) = 0;
		//����������־���
		virtual bool configure(const logger::log_param& param) = 0;
		//���ansi����־����  ������szFileName �洢��־���ļ���,emLevel ��־����
		virtual bool PrintA(const char* szFileName, logger::log_level emLevel, const char* lpszContent) = 0;
		//����ܿ����ܷ��ӡ��־
		virtual bool CanPrint(void) = 0;
		//�����־�ļ���
		virtual logger::log_level GetLogLevel(void) = 0;
		//����·��
		virtual void SetPath(char* path) = 0;
	};

	IPPSLogger* CreateInterface(void);
	void DeleteInterface(void);
}








