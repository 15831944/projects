/*************************************************
��ͷ�ļ�����Ҫ������һЩ�������÷���ĺ꺯��
*************************************************/


#ifndef COMPLIERSPECIFIC_H
#define COMPLIERSPECIFIC_H

/*************************************************
// <Summary>ȥ�����뾯�淽��ĺ�</Summary>
// <DateTime>2014/12/24</DateTime>
***********************/
#define MSVC_PUSH_DISABLE_WARNING(n) __pragma( warning(push) )   \
	__pragma( warning(disable:n) )

#define MSVC_POP_WARNING  __pragma(warning(pop))


//ȥ���ڹ��캯����ʹ��thisָ���4355����
#define ALLOW_THIS_IN_INITIALIZER_LIST(code) MSVC_PUSH_DISABLE_WARNING(4355) \
	code  \
	MSVC_POP_WARNING


#endif //COMPLIERSPECIFIC_H