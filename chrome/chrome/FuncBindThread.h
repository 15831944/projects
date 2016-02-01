#pragma once

#include "ThreadTaskList.h"
#include "EventObj.h"
#include "ThreadBaseObj.h"
#include "FunBind.h"
#include "basictypes.h"
#include <map>

namespace BaseClassLibrary
{

	typedef  unsigned long long TimeSpec;		//ms

	struct FuncTask
	{
		Closure taskPtr;
		TimeSpec timeSpec;            //ms
		FuncTask(Closure tempTaskPtr,const TimeSpec &tempTimeKey)
		{
			taskPtr = tempTaskPtr;
			timeSpec = tempTimeKey;
		}  
	};

	class Thread:public CThreadObj
	{
	public:
		virtual bool StartWithOptions();
		virtual bool PostFuncTask(const Closure &taskPtr,uint64 u64Delay = 0);
		virtual void Stop();
		virtual int64 GetIncomingQueueSize();
		virtual int64 GetWorkQueueSize();
	protected:
		virtual void ThreadFun();

	public:

		Thread(const char *threadName = "");
		~Thread();


	private:

		void ReloadWorkTask();
		void Add2ImmediateTask(const Closure &taskPtr);
		void Add2DelayTask(const Closure &taskPtr,const TimeSpec &timeSpec);
		void RunImmediateTask();
		void RunDelayTask();

		TimeSpec GetCurTimeSpec();
		void ClearAllTask();

		ThreadTask<FuncTask,std::list>  incomingTask_;
		std::list<FuncTask>  workTask_;
		std::list<Closure  > immediateTask_;
		std::multimap<TimeSpec ,Closure > delayTask_;

		Event workEvent_;
		TimeSpec  delayMinTime_;
		Event exitEvent_;
	protected:
		bool   bStop_;
	private:
		std::string strThreadName_;  
	};
}



