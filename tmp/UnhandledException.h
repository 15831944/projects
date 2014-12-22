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
        public:  // Set Get 函数
            static std::string GetDumpFileDir() {return m_strDumpFileDir;}                     // 获得dump文件的保存目录
            static void SetDumpFileDir(const std::string &dir) {m_strDumpFileDir = dir;}       // 设置dump文件的保存目录

        public:  // 公有函数
            static void EnableProduceDumpFile(bool bEnable);    // 是否在程序崩溃时 生成dump文件
            static std::string GetDumpFileName();               // 得到dump文件名

        private: // 私有函数
            DISALLOW_IMPLICIT_CONSTRUCTORS(UnhandledException); // 禁止该类进行默认构造和拷贝构造

        private: // 私有变量
            static std::string m_strDumpFileDir;      // dump文件的保存目录
            static std::string m_strDumpFileName;     // dump文件的文件名
        };
    }
}

#endif  // _WHY_WIN_UNHANDLEDEXCEPTION_H_