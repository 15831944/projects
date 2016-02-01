#ifndef THREAD_TASK_LIST_H
#define THREAD_TASK_LIST_H
#pragma once

#include <list>
#include "ThreadLock.h"

namespace BaseClassLibrary
{

	template <typename TASK ,template<typename _Tp, typename _Alloc = std::allocator<_Tp> > class COND = std::list>
	class ThreadTask
	{
	public:
		ThreadTask()
		{

		}
		~ThreadTask()
		{

		}
		bool PushTask(const TASK &task)
		{

			CScopeLock autoLock(&lock_);
			listTask_.push_back(task);
			return true;

		}
		bool PopTask(TASK &task)
		{
			CScopeLock autoLock(&lock_);
			if (listTask_.empty())
			{
				return false;
			}
			task = listTask_.front();
			listTask_.pop_front();
			return true;
		}

		bool GetBeginTask(TASK &task)
		{
			CScopeLock autoLock(&lock_);
			if (listTask_.empty())
			{
				return false;
			}
			task = listTask_.front();
			return true;
		}
		bool EraseBeginTask()
		{
			CScopeLock autoLock(&lock_);
			if (listTask_.empty())
			{
				return false;
			}
			listTask_.pop_front();
			return true;
		}

		bool SwapTask(COND<TASK> &tasks)
		{
			CScopeLock autoLock(&lock_);
			if (listTask_.empty())
			{
				return false;
			}
			listTask_.swap(tasks);
			return true;
		}
		int GetSize()
		{
			CScopeLock autoLock(&lock_);
			return listTask_.size();
		}
		void clear()
		{
			CScopeLock autoLock(&lock_);
			return listTask_.clear();
		}
	private:
		CCriSecLock lock_;
		COND<TASK> listTask_;
	};
}

#endif  //THREAD_TASK_LIST_H
