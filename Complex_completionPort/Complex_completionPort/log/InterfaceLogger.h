#pragma once
#include <string>
#include <assert.h>
#include "log_configure.h"
#include "PPSLoggerExport.h"
using namespace std;

/*!	xuxiaoyi	2012-5-31 14:28
*	˵����
*	1��ʹ����־����ʱ(ʹ��PrintA��PrintW����)Ҫע��ppslogger.dll ,regsvr32 dll��·��
*	2��Ҫ�������ļ���windowsĿ¼���½�ppslog.ini
*	[log]
*	open=1;  �ܵ���־����
*	��־��=1��Ҫд�����־�ļ��Ŀ���  
*   3����־����ʹ��ʱҪ�Ƚ������ã���������־���˹��� ��Configure��������
*/

//zln;���ⲿ���õĽӿ�

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
			param.emLevel = _Info;                                //��־����
			param.dwLogPrintType = PRINT_TO_FILE;		          //������������־���ļ�
			strcpy_s(param.szFileName,MAX_PATH,_T("test_c"));    //�����־���ļ���
			strcpy_s(param.szKeyName,64,_T("test_c"));			  //����־�ļ���һ��
			param.optName = RECORD;	                              //������ԣ���һ����������־�����
			int value = 20;
			memcpy_s(param.optVal, 64, &value, sizeof(value));    //ÿ��20��������־�����
			param.optLen = sizeof(value);			
		*/
		static bool Configure(const log_param& param);

		//ֻ���������Ϣ����־�ļ� ��������־�����������
		//������������������־����PSLOG_DEBUG��PPSLOG_INFO��PPSLOG_WARMING��PPSLOG_ERROR��PPSLOG_FATAL
		static bool PrintA(const char* szFileName, log_level emLevel, const char* lpszFormat, ...);
		//ֻ���������Ϣ����־�ļ�
		static bool PrintA(const char* szFileName, const char* lpszFormat, ...);

	private:
		static IPPSLogger* m_pPpsLogger;
	};
}