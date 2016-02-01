#pragma once
#include "basictypes.h"

#ifdef OS_WIN
#include <Windows.h>
#else
#include <pthread.h>
#endif
//--------------LockInterface----------------
//-------------------------------------------
class IThreadLock
{
public:
	virtual ~IThreadLock(){};

	virtual void Lock() = 0;
	virtual void Unlock() = 0;
};


//------------Win32 CriticalSection----------
//-------------------------------------------
class CCriSecLock : public IThreadLock
{
public:
	CCriSecLock();
	virtual ~CCriSecLock();

public:
	virtual void Lock();
	virtual void Unlock();

private:
#ifdef OS_WIN
	CRITICAL_SECTION m_pCS;
#else
	pthread_mutex_t  m_mutex;
#endif
	
};


//------------------ThreadMutex--------------
//-------------------------------------------
class CMutexLock : public IThreadLock
{
public:
	CMutexLock();
	virtual ~CMutexLock();

public:
	virtual void Lock();
	virtual void Unlock();

private:
#ifdef OS_WIN
	HANDLE m_Mutex;
#else
	pthread_mutex_t m_Mutex;
#endif
};



//------------------ScopeLock(lock guide)----------------
//-------------------------------------------
class CScopeLock
{
public:
	CScopeLock(IThreadLock*);
	~CScopeLock();

private:
	IThreadLock* m_pThreadMutex;
};


//(lock guide)
template <class T = IThreadLock>
class TScopeLock
{
public:
	TScopeLock(const T* pThreadLock)
		: m_pThreadLock(pThreadLock)
	{
		if(m_pThreadLock)
		{
			m_pThreadLock->Lock();
		}
	}

	~TScopeLock()
	{
		if(m_pThreadLock)
		{
			m_pThreadLock->Unlock();
		}
	}

private:
	const T* m_pThreadLock;
};

