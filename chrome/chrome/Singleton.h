#ifndef FOREST_SINGLETON_H_
#define FOREST_SINGLETON_H_
	
//*******************************************************************************
// ��	 ��:	�Ӻ�
// ��	 ��:	2013/06/28	
// �� ˵ ��:	����boost::singleton
//				���⽫����������Ϊprotected����ʹ��Instanceָ��ʱ��ֹ���delete
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
 		create_object.do_nothing();   //��������Ϊ�ӳٹ����ԭ��
		return obj;
	}

	static obj_type* PInstance()
	{
		return &Instance();
	}
};

template <class T>
typename Singleton<T>::object_create    //��������ģ�嶨���ڲ�ָ�����͵�ע��ط�
Singleton<T>::create_object;

#endif