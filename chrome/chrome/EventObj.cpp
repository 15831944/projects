#include "EventObj.h"
namespace BaseClassLibrary
{
	Event::Event(void)
	{
#ifdef OS_WIN
		m_handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
#else
		pthread_cond_init(&m_hEvent, NULL);
		pthread_mutex_init(&m_hMutex, NULL);
		m_bSignal = false;
		m_bManual = false;
		m_sem = NULL;
#endif

	}

	Event::Event(bool Manual_Reset,bool bInitialState)
	{
#ifdef OS_WIN
		m_handle = ::CreateEvent(NULL, Manual_Reset, bInitialState, NULL);
#else
		m_bManual = Manual_Reset;
		m_bSignal = bInitialState;
		pthread_cond_init(&m_hEvent, NULL);
		pthread_mutex_init(&m_hMutex, NULL);
		m_sem = NULL;
#endif
	}

	Event::~Event(void)
	{
#ifdef OS_WIN
		if (m_handle)
			CloseHandle(m_handle);
#else
		pthread_cond_destroy(&m_hEvent);
		pthread_mutex_destroy(&m_hMutex);
#endif
	}

	void Event::ResetEvent()
	{
#ifndef OS_WIN
		pthread_mutex_lock(&m_hMutex);
		m_bSignal = false;
		pthread_mutex_unlock(&m_hMutex);
#else
		::ResetEvent(m_handle);
#endif
	}

#ifndef OS_WIN
	bool Event::GetSingal()
	{
		return m_bSignal;
	}
#endif

	void Event::SetEvent()
	{
#ifdef OS_WIN
		::SetEvent(m_handle);
#else
		if (m_bManual)
		{
			pthread_mutex_lock(&m_hMutex);
			m_bSignal = true;
			pthread_cond_broadcast(&m_hEvent);
			pthread_mutex_unlock(&m_hMutex);
		}
		else
		{
			pthread_mutex_lock(&m_hMutex);
			if (!m_bSignal)
			{
				m_bSignal = true;
				pthread_cond_signal(&m_hEvent);
			}
			pthread_mutex_unlock(&m_hMutex);
		}
		if (m_sem)
		{
			sem_post(m_sem);
		}
#endif
	}


	int Event::Wait(int time /* = 0 */)
	{
		int result = RESULT_OK;
#ifdef OS_WIN
		DWORD ret = WaitForSingleObject(m_handle,time);
		if (ret == WAIT_TIMEOUT)
		{
			result = RESULT_TIME_OUT;
		}
#else
		pthread_mutex_lock(&m_hMutex);
		if(!m_bSignal)
		{
			if (time == INFINITE)
			{
				pthread_cond_wait(&m_hEvent, &m_hMutex);
			}
			else
			{
				struct timeval now;
				struct timespec outtime;
				gettimeofday(&now, NULL);
				outtime.tv_sec = now.tv_sec + time/1000;
				outtime.tv_nsec = (now.tv_usec + time%1000*1000)*1000;

				if (outtime.tv_nsec>=(long)1000000000 )
				{
					outtime.tv_sec ++;
					outtime.tv_nsec -= 1000000000;
				}
#ifdef OS_IOS
				while(1)
				{

					int retcode = pthread_cond_timedwait(&m_hEvent, &m_hMutex, &outtime);
					if (retcode == ETIMEDOUT)
					{
						break;
					}
					else if(m_bSignal)
					{   
						break;
					}


				}

#else
				int retcode = pthread_cond_timedwait(&m_hEvent, &m_hMutex, &outtime);
				if (retcode == ETIMEDOUT)
				{
					result = RESULT_TIME_OUT;
				}

#endif                
			}
		}
		if(!m_bManual)
		{
			m_bSignal = false;
		}
		pthread_mutex_unlock(&m_hMutex);
#endif
		return result;
	}
}