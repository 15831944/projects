/*************************************************
此头文件中主要定义了一些编译设置方面的宏函数
*************************************************/


#ifndef COMPLIERSPECIFIC_H
#define COMPLIERSPECIFIC_H

/*************************************************
// <Summary>去除编译警告方面的宏</Summary>
// <DateTime>2014/12/24</DateTime>
***********************/
#define MSVC_PUSH_DISABLE_WARNING(n) __pragma( warning(push) )   \
	__pragma( warning(disable:n) )

#define MSVC_POP_WARNING  __pragma(warning(pop))


//去除在构造函数中使用this指针的4355警告
#define ALLOW_THIS_IN_INITIALIZER_LIST(code) MSVC_PUSH_DISABLE_WARNING(4355) \
	code  \
	MSVC_POP_WARNING


#endif //COMPLIERSPECIFIC_H