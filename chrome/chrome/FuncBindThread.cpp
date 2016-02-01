#include "FuncBindThread.h"
//#include "base/include/AtomicInter.h"
#include "AtomicInter.h"
#ifndef OS_WIN
#include <sys/time.h>
#endif
#include "assert.h"

namespace BaseClassLibrary
{

	Thread::Thread(const char *threadName):workEvent_(false,false),exitEvent_(false,false)
	{
		delayMinTime_ = 0;
		bStop_=false;
		strThreadName_ = threadName;
	}

	Thread::~Thread()
	{
		Stop();
	}
	bool Thread::StartWithOptions()
	{
		CThreadObj::StartThread(strThreadName_.c_str());
		return true;
	}


	void Thread::Stop()
	{
		if(bStop_)
			return;
		workEvent_.SetEvent();
		bStop_=true;
		CThreadObj::StopThread();

		if (RESULT_TIME_OUT==exitEvent_.Wait(1000))
		{
			//assert(false);
		}
	}




	void Thread::ThreadFun()
	{
		while (!bStop_)
		{
			if(immediateTask_.empty()&&incomingTask_.GetSize()==0)
			{
				if (delayMinTime_ == 0)
				{
					workEvent_.Wait(INFINITE);
				}
				else
				{
					TimeSpec curTimeSpec = GetCurTimeSpec();
					int nSleepMS = int(delayMinTime_ - curTimeSpec);
					if (nSleepMS>0)
					{
						workEvent_.Wait(nSleepMS);
					}
				}

				if (bStop_)
				{
					break;
				}
			}
			ReloadWorkTask();

			TimeSpec curTimeSpec = GetCurTimeSpec();
			int nWorkSize = (int)workTask_.size();
			for (int i = 0;i<(int)nWorkSize;i++)
			{
				if (bStop_)
				{
					break;
				}
				FuncTask &funTask = workTask_.front();
				if (funTask.timeSpec <= curTimeSpec)
				{
					Add2ImmediateTask(funTask.taskPtr);
				}
				else
				{
					Add2DelayTask(funTask.taskPtr,funTask.timeSpec);
				}
				workTask_.pop_front();
			}

			RunImmediateTask();
			RunDelayTask();
		}

		ClearAllTask();


	}

	bool Thread::PostFuncTask(const Closure &taskPtr,uint64 u64Delay )
	{
		if (bStop_)
		{
			return false;
		}

		TimeSpec curTimeSpec = GetCurTimeSpec();
		curTimeSpec =curTimeSpec+u64Delay;
		incomingTask_.PushTask(FuncTask(taskPtr,curTimeSpec));
		workEvent_.SetEvent();
		return true;
	}

	void Thread::ReloadWorkTask()
	{
		incomingTask_.SwapTask(workTask_);
	}

	void Thread::Add2ImmediateTask(const Closure &taskPtr)
	{
		immediateTask_.push_back(taskPtr);
	}
	void Thread::Add2DelayTask(const Closure &taskPtr,const TimeSpec  &timeSpec)
	{

		delayTask_.insert(std::make_pair(timeSpec, taskPtr));
		{
			if (delayMinTime_==0)
			{
				delayMinTime_ = timeSpec;
			}
			else
			{
				if (timeSpec < delayMinTime_ )
				{
					delayMinTime_ = timeSpec;
				}
			}
		}
	}

	void Thread::RunImmediateTask()
	{
		for (int i= 0;i<(int)immediateTask_.size();i++ )
		{

			Closure &task = immediateTask_.front();
			if (task)
			{
				task->Run();
			}
			immediateTask_.pop_front();
			if (bStop_)
			{
				break;
			}
		}
	}
	void Thread::RunDelayTask()
	{
		TimeSpec curTimeSpec = GetCurTimeSpec();
		if(delayMinTime_ <= curTimeSpec)
		{
			for (std::multimap<TimeSpec ,Closure >::iterator iterMap = delayTask_.begin(); iterMap !=delayTask_.end();)
			{
				if (bStop_)
				{
					break;
				}
				if (iterMap->first <= curTimeSpec)
				{
					Add2ImmediateTask(iterMap->second);
					delayTask_.erase(iterMap++);
					if(delayTask_.end() == iterMap)
						break;
				}
				else
				{
					delayMinTime_ = iterMap->first;
					break;
				}
			}

		}
		if (delayTask_.empty())
		{
			delayMinTime_ = 0;
		}
	}

	TimeSpec Thread::GetCurTimeSpec()
	{
#ifndef OS_WIN
		timeval curTime;
		gettimeofday(&curTime, NULL);
		return  TimeSpec(curTime.tv_sec * 1000 + curTime.tv_usec/1000);
#else
		TimeSpec ts = GetTickCount();
		//	ts=ts*1000;
		return ts;
#endif
	}

	void Thread::ClearAllTask()
	{
		workTask_.clear();
		immediateTask_.clear();
		delayTask_.clear();
		incomingTask_.clear();
		exitEvent_.SetEvent();
	}



	int64 Thread::GetIncomingQueueSize()
	{
		return incomingTask_.GetSize();
	}

	int64 Thread::GetWorkQueueSize()
	{
		return workTask_.size();
	}

}
