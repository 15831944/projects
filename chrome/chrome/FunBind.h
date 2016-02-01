#ifndef __FUNC_BIND_H
#define __FUNC_BIND_H

#include <boost/shared_ptr.hpp>

namespace BaseClassLibrary
{
	class TaskBase
	{
	public:
		~TaskBase(){};
		virtual void Run()=0;
	};
	template <typename FUNC>
	class Task;
	//__stdcall的偏特化 只完成了7个参数
	template <typename R>
	class Task<R (__stdcall*)()>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)() )
		{
			_func = func;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func();
		}
		R (__stdcall*_func)();
	};


	template <typename R,typename P1>
	class Task<R (__stdcall*)(P1)>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)(P1),P1 p1)
		{
			_func = func;
			_p1 = p1;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func(_p1);
		}
		R (__stdcall*_func)(P1);
		P1 _p1;
	};

	template <typename R,typename P1,typename P2>
	class Task<R (__stdcall*)(P1,P2)>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)(P1,P2),P1 p1,P2 p2)
		{
			_func = func;
			_p1 = p1;
			_p2 = p2;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func(_p1,_p2);
		}
		R (__stdcall*_func)(P1,P2);
		P1 _p1;
		P2 _p2;
	};

	template <typename R,typename P1,typename P2,typename P3>
	class Task<R (__stdcall*)(P1,P2,P3)>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)(P1,P2,P3),P1 p1,P2 p2,P3 p3)
		{
			_func = func;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func(_p1,_p2,_p3);
		}
		R (__stdcall*_func)(P1,P2,P3);
		P1 _p1;
		P2 _p2;
		P3 _p3;
	};


	template <typename R,typename P1,typename P2,typename P3,typename P4>
	class Task<R (__stdcall*)(P1,P2,P3,P4)>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)(P1,P2,P3,P4),P1 p1,P2 p2,P3 p3,P4 p4)
		{
			_func = func;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func(_p1,_p2,_p3,_p4);
		}
		R (__stdcall*_func)(P1,P2,P3,P4);
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
	};

	template <typename R,typename P1,typename P2,typename P3,typename P4,typename P5>
	class Task<R (__stdcall*)(P1,P2,P3,P4,P5)>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)(P1,P2,P3,P4,P5),P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
		{
			_func = func;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
			_p5 = p5;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func(_p1,_p2,_p3,_p4,_p5);
		}
		R (__stdcall*_func)(P1,P2,P3,P4,P5);
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
		P5 _p5;
	};

	template <typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
	class Task<R (__stdcall*)(P1,P2,P3,P4,P5,P6)>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)(P1,P2,P3,P4,P5,P6),P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6)
		{
			_func = func;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
			_p5 = p5;
			_p6 = p6;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func(_p1,_p2,_p3,_p4,_p5,_p6);
		}
		R (__stdcall*_func)(P1,P2,P3,P4,P5,P6);
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
		P5 _p5;
		P6 _p6;
	};

	template <typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
	class Task<R (__stdcall*)(P1,P2,P3,P4,P5,P6,P7)>:public TaskBase
	{
	public:
		explicit Task(R (__stdcall*func)(P1,P2,P3,P4,P5,P6,P7),P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7)
		{
			_func = func;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
			_p5 = p5;
			_p6 = p6;
			_p7 = p7;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return _func(_p1,_p2,_p3,_p4,_p5,_p6);
		}
		R (__stdcall*_func)(P1,P2,P3,P4,P5,P6,P7);
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
		P5 _p5;
		P6 _p6;
		P7 _p7;
	};
	//
	//__cdecl的偏特化 只完成了7个参数
	//	template <typename R>
	//	class Task<R (__cdecl*)()>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)() )
	//		{
	//			_func = func;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func();
	//		}
	//		R (__cdecl*_func)();
	//	};
	//
	//
	//	template <typename R,typename P1>
	//	class Task<R (__cdecl*)(P1)>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)(P1),P1 p1)
	//		{
	//			_func = func;
	//			_p1 = p1;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func(_p1);
	//		}
	//		R (__cdecl*_func)(P1);
	//		P1 _p1;
	//	};
	//
	//	template <typename R,typename P1,typename P2>
	//	class Task<R (__cdecl*)(P1,P2)>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)(P1,P2),P1 p1,P2 p2)
	//		{
	//			_func = func;
	//			_p1 = p1;
	//			_p2 = p2;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func(_p1,_p2);
	//		}
	//		R (__cdecl*_func)(P1,P2);
	//		P1 _p1;
	//		P2 _p2;
	//	};
	//
	//	template <typename R,typename P1,typename P2,typename P3>
	//	class Task<R (__cdecl*)(P1,P2,P3)>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)(P1,P2,P3),P1 p1,P2 p2,P3 p3)
	//		{
	//			_func = func;
	//			_p1 = p1;
	//			_p2 = p2;
	//			_p3 = p3;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func(_p1,_p2,_p3);
	//		}
	//		R (__cdecl*_func)(P1,P2,P3);
	//		P1 _p1;
	//		P2 _p2;
	//		P3 _p3;
	//	};
	//
	//
	//	template <typename R,typename P1,typename P2,typename P3,typename P4>
	//	class Task<R (__cdecl*)(P1,P2,P3,P4)>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)(P1,P2,P3,P4),P1 p1,P2 p2,P3 p3,P4 p4)
	//		{
	//			_func = func;
	//			_p1 = p1;
	//			_p2 = p2;
	//			_p3 = p3;
	//			_p4 = p4;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func(_p1,_p2,_p3,_p4);
	//		}
	//		R (__cdecl*_func)(P1,P2,P3,P4);
	//		P1 _p1;
	//		P2 _p2;
	//		P3 _p3;
	//		P4 _p4;
	//	};
	//
	//	template <typename R,typename P1,typename P2,typename P3,typename P4,typename P5>
	//	class Task<R (__cdecl*)(P1,P2,P3,P4,P5)>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)(P1,P2,P3,P4,P5),P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
	//		{
	//			_func = func;
	//			_p1 = p1;
	//			_p2 = p2;
	//			_p3 = p3;
	//			_p4 = p4;
	//			_p5 = p5;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func(_p1,_p2,_p3,_p4,_p5);
	//		}
	//		R (__cdecl*_func)(P1,P2,P3,P4,P5);
	//		P1 _p1;
	//		P2 _p2;
	//		P3 _p3;
	//		P4 _p4;
	//		P5 _p5;
	//	};
	//
	//	template <typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
	//	class Task<R (__cdecl*)(P1,P2,P3,P4,P5,P6)>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)(P1,P2,P3,P4,P5,P6),P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6)
	//		{
	//			_func = func;
	//			_p1 = p1;
	//			_p2 = p2;
	//			_p3 = p3;
	//			_p4 = p4;
	//			_p5 = p5;
	//			_p6 = p6;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func(_p1,_p2,_p3,_p4,_p5,_p6);
	//		}
	//		R (__cdecl*_func)(P1,P2,P3,P4,P5,P6);
	//		P1 _p1;
	//		P2 _p2;
	//		P3 _p3;
	//		P4 _p4;
	//		P5 _p5;
	//		P6 _p6;
	//	};
	//
	//	template <typename R,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
	//	class Task<R (__cdecl*)(P1,P2,P3,P4,P5,P6,P7)>:public TaskBase
	//	{
	//	public:
	//		explicit Task(R (__cdecl*func)(P1,P2,P3,P4,P5,P6,P7),P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7)
	//		{
	//			_func = func;
	//			_p1 = p1;
	//			_p2 = p2;
	//			_p3 = p3;
	//			_p4 = p4;
	//			_p5 = p5;
	//			_p6 = p6;
	//			_p7 = p7;
	//		}
	//		virtual void Run()
	//		{
	//			RunFunc();
	//		}
	//	private:
	//		R RunFunc()
	//		{
	//			return _func(_p1,_p2,_p3,_p4,_p5,_p6);
	//		}
	//		R (__cdecl*_func)(P1,P2,P3,P4,P5,P6,P7);
	//		P1 _p1;
	//		P2 _p2;
	//		P3 _p3;
	//		P4 _p4;
	//		P5 _p5;
	//		P6 _p6;
	//		P7 _p7;
	//	};

	//类的偏特化 只完成了7个参数
	template <typename R,typename C>
	class Task<R (C::*)()>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(),C *objP)
		{
			_func = func;
			_objP = objP;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)();
		}
		R (C::*_func)();
		C  *_objP;
	};


	template <typename R,typename C,typename P1>
	class Task<R (C::*)(P1)>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(P1),C *objP,P1 p1)
		{
			_func = func;
			_objP = objP;
			_p1 = p1;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)(_p1);
		}
		R (C::*_func)(P1);
		C  *_objP;
		P1 _p1;
	};


	template <typename R,typename C,typename P1,typename P2>
	class Task<R (C::*)(P1,P2)>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(P1,P2),C *objP,P1 p1,P2 p2)
		{
			_func = func;
			_objP = objP;
			_p1 = p1;
			_p2 = p2;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)(_p1,_p2);
		}
		R (C::*_func)(P1,P2);
		C  *_objP;
		P1 _p1;
		P2 _p2;
	};

	template <typename R,typename C,typename P1,typename P2,typename P3>
	class Task<R (C::*)(P1,P2,P3)>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(P1,P2,P3),C *objP,P1 p1,P2 p2,P3 p3)
		{
			_func = func;
			_objP = objP;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)(_p1,_p2,_p3);
		}
		R (C::*_func)(P1,P2,P3);
		C  *_objP;
		P1 _p1;
		P2 _p2;
		P3 _p3;
	};

	template <typename R,typename C,typename P1,typename P2,typename P3,typename P4>
	class Task<R (C::*)(P1,P2,P3,P4)>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(P1,P2,P3,P4 ),C *objP,P1 p1,P2 p2,P3 p3,P4 p4)
		{
			_func = func;
			_objP = objP;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)(_p1,_p2,_p3,_p4);
		}
		R (C::*_func)(P1,P2,P3,P4);
		C  *_objP;
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
	};


	template <typename R,typename C,typename P1,typename P2,typename P3,typename P4,typename P5>
	class Task<R (C::*)(P1,P2,P3,P4,P5)>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(P1,P2,P3,P4,P5),C *objP,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
		{
			_func = func;
			_objP = objP;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
			_p5 = p5;
		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)(_p1,_p2,_p3,_p4,_p5);
		}
		R (C::*_func)(P1,P2,P3,P4,P5);
		C  *_objP;
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
		P5 _p5;
	};


	template <typename R,typename C,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
	class Task<R (C::*)(P1,P2,P3,P4,P5,P6)>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(P1,P2,P3,P4,P5,P6),C *objP,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6)
		{
			_func = func;
			_objP = objP;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
			_p5 = p5;
			_p6 = p6;

		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)(_p1,_p2,_p3,_p4,_p5,_p6);
		}
		R (C::*_func)(P1,P2,P3,P4,P5,P6);
		C  *_objP;
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
		P5 _p5;
		P6 _p6;
	};


	template <typename R,typename C,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
	class Task<R (C::*)(P1,P2,P3,P4,P5,P6,P7)>:public TaskBase
	{
	public:
		explicit Task(R (C::*func)(P1,P2,P3,P4,P5,P6,P7),C *objP,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7)
		{
			_func = func;
			_objP = objP;
			_p1 = p1;
			_p2 = p2;
			_p3 = p3;
			_p4 = p4;
			_p5 = p5;
			_p6 = p6;
			_p7 = p7;

		}
		virtual void Run()
		{
			RunFunc();
		}
	private:
		R RunFunc()
		{
			return  (*_objP.*_func)(_p1,_p2,_p3,_p4,_p5,_p6,_p7);
		}
		R (C::*_func)(P1,P2,P3,P4,P5,P6,P7);
		C  *_objP;
		P1 _p1;
		P2 _p2;
		P3 _p3;
		P4 _p4;
		P5 _p5;
		P6 _p6;
		P7 _p7;
	};


	typedef boost::shared_ptr<TaskBase> Closure;
	template <typename FUNC>
	Closure BindF(FUNC func)
	{
		return Closure(new Task<FUNC>(func));
	}

	template <typename FUNC,typename P1>
	Closure BindF(FUNC func,P1 p1)
	{
		return Closure(new Task<FUNC>(func,p1));
	}

	template <typename FUNC,typename P1,typename P2>
	Closure BindF(FUNC func,P1 p1,P2 p2)
	{
		return Closure(new Task<FUNC>(func,p1,p2));
	}

	template <typename FUNC,typename P1,typename P2,typename P3>
	Closure BindF(FUNC func,P1 p1,P2 p2)
	{
		return Closure(new Task<FUNC>(func,p1,p2));
	}

	template <typename FUNC,typename P1,typename P2,typename P3,typename P4>
	Closure BindF(FUNC func,P1 p1,P2 p2,P3 p3,P4 p4)
	{
		return Closure(new Task<FUNC>(func,p1,p2,p3,p4));
	}

	template <typename FUNC,typename P1,typename P2,typename P3,typename P4,typename P5>
	Closure BindF(FUNC func,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
	{
		return Closure(new Task<FUNC>(func,p1,p2,p3,p4,p5));
	}

	template <typename FUNC,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
	Closure BindF(FUNC func,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6)
	{
		return Closure(new Task<FUNC>(func,p1,p2,p3,p4,p5,p6));
	}

	template <typename FUNC,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
	Closure BindF(FUNC func,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7)
	{
		return Closure(new Task<FUNC>(func,p1,p2,p3,p4,p5,p6,p7));
	}


	template <typename FUNC,typename CLASSP>
	Closure Bind(FUNC func,CLASSP cp)
	{
		return Closure(new Task<FUNC>(func,cp));
	}


	template <typename FUNC,typename CLASSP,typename P1>
	Closure Bind(FUNC func,CLASSP cp,P1 p1)
	{
		return Closure(new Task<FUNC>(func,cp,p1));
	}

	template <typename FUNC,typename CLASSP,typename P1,typename P2>
	Closure Bind(FUNC func,CLASSP cp,P1 p1,P2 p2)
	{
		return Closure(new Task<FUNC>(func,cp,p1,p2));
	}

	template <typename FUNC,typename CLASSP,typename P1,typename P2,typename P3>
	Closure Bind(FUNC func,CLASSP cp,P1 p1,P2 p2,P3 p3)
	{
		return Closure(new Task<FUNC>(func,cp,p1,p2,p3));
	}

	template <typename FUNC,typename CLASSP,typename P1,typename P2,typename P3,typename P4>
	Closure Bind(FUNC func,CLASSP cp,P1 p1,P2 p2,P3 p3,P4 p4)
	{
		return Closure(new Task<FUNC>(func,cp,p1,p2,p3,p4));
	}

	template <typename FUNC,typename CLASSP,typename P1,typename P2,typename P3,typename P4,typename P5>
	Closure Bind(FUNC func,CLASSP cp,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5)
	{
		return Closure(new Task<FUNC>(func,cp,p1,p2,p3,p4,p5));
	}

	template <typename FUNC,typename CLASSP,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6>
	Closure Bind(FUNC func,CLASSP cp,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6)
	{
		return Closure(new Task<FUNC>(func,cp,p1,p2,p3,p4,p5,p6));
	}

	template <typename FUNC,typename CLASSP,typename P1,typename P2,typename P3,typename P4,typename P5,typename P6,typename P7>
	Closure Bind(FUNC func,CLASSP cp,P1 p1,P2 p2,P3 p3,P4 p4,P5 p5,P6 p6,P7 p7)
	{
		return Closure(new Task<FUNC>(func,cp,p1,p2,p3,p4,p5,p6,p7));
	}
}

#endif

