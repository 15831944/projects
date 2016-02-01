#include "stdafx.h"
#include "ThreadLock.h"

//-------------------CCriSecLock-----------------------------
//-------------------------------------------------------------

CCriSecLock::CCriSecLock()
{
#ifdef OS_WIN
	InitializeCriticalSection(&m_pCS);
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&m_mutex,&attr);
	pthread_mutexattr_destroy(&attr);
#endif
};

CCriSecLock::~CCriSecLock()
{
#ifdef OS_WIN
	::DeleteCriticalSection(&m_pCS);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
}

void CCriSecLock::Lock()
{
#ifdef OS_WIN
	EnterCriticalSection(&m_pCS);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}

void CCriSecLock::Unlock()
{
#ifdef OS_WIN
	LeaveCriticalSection(&m_pCS);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}




//-------------------CMutexLock------------------------------
//-------------------------------------------------------------
CMutexLock::CMutexLock()
{
#if defined(OS_WIN)
	m_Mutex = ::CreateMutex(NULL,FALSE,NULL);

#elif defined(OS_LINUX)
	pthread_mutex_init(&m_Mutex,NULL);
#endif
}


CMutexLock::~CMutexLock()
{
#if defined(OS_WIN)
	::CloseHandle(m_Mutex);

#elif defined(OS_LINUX)
	pthread_mutex_destroy(&m_Mutex);
#endif
}


void CMutexLock::Lock()
{
#if defined(OS_WIN)
	DWORD dw = WaitForSingleObject(m_Mutex,INFINITE);

#elif defined(OS_LINUX)
	pthread_mutex_lock(&m_Mutex);
#endif
}


void CMutexLock::Unlock()
{
#if defined(OS_WIN)
	::ReleaseMutex(m_Mutex);


#elif defined(OS_LINUX)
	pthread_mutex_unlock(&m_Mutex);
#endif
}


//-------------------CScopeLock---------------------------------
//-------------------------------------------------------------
CScopeLock::CScopeLock(IThreadLock* pThreadMutex)
: m_pThreadMutex(pThreadMutex)
{
	if(m_pThreadMutex)
	{
		m_pThreadMutex->Lock();
	}
}


CScopeLock::~CScopeLock()
{
	if(m_pThreadMutex)
	{
		m_pThreadMutex->Unlock();
	}
}
