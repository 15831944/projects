#include "ThreadLock.h"
#include "basictypes.h"

#ifndef OS_WIN

unsigned long InterlockedExchange(LONG volatile* Target,unsigned long Value);

unsigned long InterLockedIncrement(ULONG *lpAddend);
#endif