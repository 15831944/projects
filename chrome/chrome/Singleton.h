#ifndef FOREST_SINGLETON_H_
#define FOREST_SINGLETON_H_
	
//*******************************************************************************
// 作	 者:	钟浩
// 日	 期:	2013/06/28	
// 类 说 明:	参照boost::singleton
//				另外将析构函数置为protected，在使用Instance指针时防止外界delete
//*******************************************************************************
template <class T>
class Singleton
{
	struct object_create
	{
		object_create(){Singleton<T>::Instance();}
		inline void do_nothing() const {}
	};

	static object_create create_object;

protected:
	Singleton(){}
	virtual ~Singleton(){}
	Singleton& operator = (const Singleton&);
	Singleton(const Singleton&);

public:
	typedef T obj_type;
	static obj_type& Instance()
	{
		static obj_type obj;
 		create_object.do_nothing();   //可能是因为延迟构造的原因
		return obj;
	}

	static obj_type* PInstance()
	{
		return &Instance();
	}
};

template <class T>
typename Singleton<T>::object_create    //这里是在模板定义内部指定类型的注意地方
Singleton<T>::create_object;

#endif