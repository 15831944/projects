#include "ThreadBaseObj.h"
#include "AtomicInter.h"
#include <assert.h>
#ifndef OS_WIN
#include<sys/prctl.h>
#endif

namespace BaseClassLibrary
{

	CThreadObj::CThreadObj(void)
	{
		bCloseThread_ = false;
		lEnterWorkThread_ = 0;

#ifdef OS_WIN
		hWorkThread_=INVALID_HANDLE_VALUE;
#endif
	}

	CThreadObj::~CThreadObj(void)
	{
#ifdef OS_WIN
		if(hWorkThread_ != NULL && INVALID_HANDLE_VALUE != hWorkThread_)
		{
			CloseHandle(hWorkThread_);
			hWorkThread_ = NULL;
		}
#endif
	}


	bool CThreadObj::StartThread(const char *threadName)
	{
		if (lEnterWorkThread_)
		{
			return true;
		}
#ifdef OS_WIN
		long lRet=InterlockedExchange((LONG*)&lEnterWorkThread_,1);
		if (lRet)
		{
			return true;
		}
		if(hWorkThread_!=INVALID_HANDLE_VALUE)
		{
			return true;
		}

		ThreadParam *threadParam = new ThreadParam;
		if (!threadParam)
		{
			assert(false);
			return false;
		}
		if (threadName != NULL)
		{
			threadParam->threadName = threadName;
		}
		threadParam->pThis = this;

		unsigned int uiThreadCode = 0;
		hWorkThread_ = (HANDLE)_beginthreadex(NULL,0,
			(unsigned int (__stdcall *)(void *))CThreadObj::WorkThreadS,threadParam,0,&uiThreadCode);
		if (hWorkThread_ == INVALID_HANDLE_VALUE)
		{
			if (threadParam)
			{
				delete threadParam;
				threadParam=NULL;
			}
			lEnterWorkThread_=0;
			return false;
		}
		return true;
#else
		unsigned long lRet= InterlockedExchange(&lEnterWorkThread_,1);
		if (lRet)
		{
			return true;
		}

		ThreadParam *threadParam = new ThreadParam;
		if (!threadParam)
		{
			assert(false);
			return false;
		}
		if (threadName != NULL)
		{
			threadParam->threadName = threadName;
		}
		threadParam->pThis = this;

		if(pthread_create(&hWorkThread_,NULL,(void* (*)(void*))CThreadObj::WorkThreadS,threadParam) != 0)
		{
			if (threadParam)
			{
				delete threadParam;
				threadParam=NULL;
			}
			lEnterWorkThread_=0;
			return false;
		}
		return true;
#endif
	}

	void CThreadObj::WorkThreadS(void* lPVoid)
	{
		ThreadParam *param = (ThreadParam *)lPVoid;
		if(param)
		{
			SetThreadName(param->threadName.c_str());
			if (param->pThis)
			{
				param->pThis->ThreadFun();
			}

			delete param;
		}

		return;
	}

	void CThreadObj::StopThread()
	{
		lEnterWorkThread_ = 0;
		bCloseThread_=true;
#ifdef OS_WIN
		if (WaitForSingleObject(hWorkThread_,2000) == WAIT_TIMEOUT)
		{
			TerminateThread(hWorkThread_,0);	
		}
		hWorkThread_ = INVALID_HANDLE_VALUE;
#endif

	}

	bool CThreadObj::IsThreadStop()
	{
		return bCloseThread_;

	}

	void CThreadObj::SetThreadName( const char *szThreadName )
	{
#ifdef OS_WIN
		typedef struct tagTHREADNAME_INFO
		{
			DWORD dwType; // must be 0x1000
			LPCSTR szName; // pointer to name (in user addr space)
			DWORD dwThreadID; // thread ID (-1=caller thread)
			DWORD dwFlags; // reserved for future use, must be zero
		} THREADNAME_INFO;

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = szThreadName;
		info.dwThreadID = GetCurrentThreadId();
		info.dwFlags = 0;

		__try
		{
			RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info );
		}
		__except(EXCEPTION_CONTINUE_EXECUTION)
		{
		}
#elif defined(OS_IOS)
		if(szThreadName != "")
			pthread_setname_np(szThreadName);
#else
		prctl(PR_SET_NAME,szThreadName) ;
#endif
	}
}



