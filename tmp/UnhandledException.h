/************************************************************************/
/* ClassName:   UnHandledException     
/* Namespace:   why::win 
/* FileName:    UnHandledEx
/* Date:        2014-12-10
/* Author:      wanghongyun
/* Description:    
/************************************************************************/

#ifndef _WHY_WIN_UNHANDLEDEXCEPTION_H_
#define _WHY_WIN_UNHANDLEDEXCEPTION_H_

#include <string>
#include "../basictypes.h"

namespace why
{
    namespace win
    {
        class UnhandledException
        {
        public:  // Set Get ����
            static std::string GetDumpFileDir() {return m_strDumpFileDir;}                     // ���dump�ļ��ı���Ŀ¼
            static void SetDumpFileDir(const std::string &dir) {m_strDumpFileDir = dir;}       // ����dump�ļ��ı���Ŀ¼

        public:  // ���к���
            static void EnableProduceDumpFile(bool bEnable);    // �Ƿ��ڳ������ʱ ����dump�ļ�
            static std::string GetDumpFileName();               // �õ�dump�ļ���

        private: // ˽�к���
            DISALLOW_IMPLICIT_CONSTRUCTORS(UnhandledException); // ��ֹ�������Ĭ�Ϲ���Ϳ�������

        private: // ˽�б���
            static std::string m_strDumpFileDir;      // dump�ļ��ı���Ŀ¼
            static std::string m_strDumpFileName;     // dump�ļ����ļ���
        };
    }
}

#endif  // _WHY_WIN_UNHANDLEDEXCEPTION_H_