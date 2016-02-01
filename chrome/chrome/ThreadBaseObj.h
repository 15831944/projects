#ifndef THREAD_BASE_OBJ_H
#define THREAD_BASE_OBJ_H

#pragma once
#include "basictypes.h"

#ifdef OS_WIN
#include <windows.h>
#include <process.h>
#else 
#include <pthread.h>
#endif

#include <string>

namespace BaseClassLibrary
{


	class CThreadObj;
	struct ThreadParam
	{
		CThreadObj *pThis;
		std::string threadName;
	};

	//�����ʱ��һ�������࣬�����ⲿȥʵ��ThreadFun�߳�ִ�к���
	class CThreadObj
	{
	public:
		CThreadObj(void);
		~CThreadObj(void);
	public:
		//�����߳�
		virtual bool StartThread(const char *threadName=NULL);

		//ֹͣ�߳�
		virtual void StopThread();
	protected:
		//�߳�ִ�к���
		virtual void ThreadFun()=0;
		bool IsThreadStop();
	private:
		static  void WorkThreadS(void *lPVoid);
		static void SetThreadName( const char *szThreadName);

		bool bCloseThread_;
		unsigned long lEnterWorkThread_;


#ifdef OS_WIN
		HANDLE hWorkThread_;
#else
		pthread_t hWorkThread_;
#endif

	};

}


#endif   //THREAD_BASE_OBJ_H