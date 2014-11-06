#pragma once

#ifdef WIN32
#include<Windows.h>
#else
#include <pthread.h>
#endif

/*windows:采用了临界区来实现的
linux:采用了互斥变量实现*/

class CLock
{
public:
	CLock(void);
	~CLock(void);
	void Lock();
	void UnLock();
private:
#ifdef WIN32
	CRITICAL_SECTION  m_cs;
#else
	pthread_mutex_t  m_mutex;
#endif
};

//local lock guard
class AutoLock
{
public:
	AutoLock(CLock &lock);
	~AutoLock();
private:
	CLock &m_lock;
};

