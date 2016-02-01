#ifndef EVENT_OBJ_H
#define EVENT_OBJ_H

#define RESULT_OK 0
#define RESULT_TIME_OUT -1

#include "basictypes.h"

#ifdef OS_WIN
#include <Windows.h>
#else
#include <semaphore.h>
#include <pthread.h>
#endif

namespace BaseClassLibrary
{
	class Event
	{
	public:
		Event(void);
		Event(bool Manual_Reset,bool bInitialState);
		~Event(void);
		void SetEvent();
		void ResetEvent();
#ifndef OS_WIN
		bool GetSingal();
		void setSem(sem_t *sem)
		{
			m_sem = sem;
		}
#endif
		//µ•Œª  ∫¡√Î
		int Wait(int time = INFINITE);
#ifdef OS_WIN
		HANDLE getHandle()
		{
			return m_handle;
		}
#endif
	private:
#ifdef OS_WIN
		HANDLE m_handle;
#else
		bool m_bManual;
		pthread_cond_t  m_hEvent;
		pthread_mutex_t m_hMutex;
		bool m_bSignal;
		sem_t *m_sem;
#endif
	};
}
#endif //EVENT_OBJ_H
