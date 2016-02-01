#include "AtomicInter.h"

#ifndef OS_WIN
static CCriSecLock Atomic_lock;
static CCriSecLock g_interLock;

unsigned long InterlockedExchange(LONG volatile *Target,unsigned long Value)
{
	LONG temp = 0;
	{
		CScopeLock AutoL(&Atomic_lock); 
		temp = *Target;
		*Target = Value;
	}
	return temp;
}

unsigned long InterLockedIncrement(ULONG *lpAddend)
{
	return  __sync_fetch_and_add(lpAddend,1);
}

long InterlockedExchangeAdd(long *lpVar,long value)
{
	CScopeLock AutoL(&g_interLock);
	(*lpVar) += value;
	return (*lpVar);
}
#endif