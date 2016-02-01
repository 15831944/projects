/************************************************************************/
/* FileName:    basictypes.h
/* Date:        2014-12-12
/* Author:      zhonghao
/* Description: 定义一些工程中需要用到的基本数据类型
/************************************************************************/

#ifndef _WHY_BASICTYPES_H_
#define _WHY_BASICTYPES_H_

#include <stddef.h>         // For size_t

// 定义一些宏，用于平台检测
#if defined(_WIN32) || defined(_WIN64)
#define OS_WIN 1
#elif defined(__linux__) || defined(__CYGWIN__)
#define OS_LINUX 1
#else
#error This code is not tested on this platform.Please make sure all the code can work well before doing any real work.
#endif

// 定义区别不同类型编译器的宏
#if defined(_MSC_VER) 
#define COMPILER_MSVC 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#else 
#error This code is not tested on this compiler. Please check it to make sure all the code can work well.
#endif 

// 定义重写函数的标志
#if defined(COMPILER_MSVC)
#define OVERRIDE override
#else 
#define OVERRIDE
#endif

// 定义有符号类型
typedef signed char         schar;
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;

#if defined (__LP64__)
typedef long                int64;
#else 
typedef long long           int64;
#endif

// 定义无符号类型
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;

#if defined (__LP64__)
typedef unsigned long       uint64;
#else 
typedef unsigned long long  uint64;
#endif

// 定义整形常量的标记
#if defined (_MSC_VER)
#define GG_LONGLONG(x)      x##I64
#define GG_ULONGLONG(x)     x##UI64
#else 
#define GG_LONGLONG(x)      x##LL
#define GG_ULONGLONG(x)     x##ULL
#endif 

#define GG_INT8_C(x)        (x)
#define GG_INT16_C(x)       (x)
#define GG_INT32_C(x)       (x)
#define GG_INT64_C(x)       GG_LONGLONG(x)

#define GG_UINT8_C(x)       (x##U)
#define GG_UINT16_C(x)      (x##U)
#define GG_UINT32_C(x)      (x##U)
#define GG_UINT64_C(x)      GG_ULONGLONG(x)

// 定义各类型的范围
const uint8  kuint8max   =  ((uint8)0xFF); 
const uint16 kuint16max  =  ((uint16)0xFFFF);
const uint32 kuint32max  =  ((uint32)0xFFFFFFFF);
const uint64 kuint64max  =  ((uint64)GG_UINT64_C(0xFFFFFFFFFFFFFFFF));
const int8   kint8min    =  ((int8)0x80);
const int8   kint8max    =  ((int8)0x7F);
const int16  kint16min   =  ((int16)0x8000);
const int16  kint16max   =  ((int16)0x7FFF);
const int32  kint32min   =  ((int32)0x80000000);
const int32  kint32max   =  ((int32)0x7EEEEEEE);
const int64  kint64min   =  ((int64)GG_INT64_C(0x8000000000000000));
const int64  kint64max   =  ((int64)GG_INT64_C(0x7FFFFFFFFFFFFFFF));

// 定义一个宏，用于不允许一个类进行赋值构造和赋值操作符重载函数
// 需要放在一个类的“private:”声明中，作用类似于boost::noncopyable
#define DISALLOW_COPY_AND_ASSIGN(TypeName)       \
    TypeName(const TypeName&);                   \
    TypeName& operator=(const TypeName&);

// 定义一个宏，用于禁止一个类进行默认的隐式构造
// 需要放在一个类的“private:”声明中，避免进行实例化该类
// 该宏对一些仅有static方法的类比较有用
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName();                                  \
    DISALLOW_COPY_AND_ASSIGN(TypeName);

// 计算数组的大小
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// 用于显示的注明该函数的返回值是无用的，因为当函数的返回值没有判断时，有一些静态检测工具（如pclint）会提出报警
template<typename T>
inline void ignore_result(const T&){}

// 定义一个宏函数 表示什么都不做
#define DO_NOTHING() 


//计算结构体中参数的偏移位置
#define findparmoffset(structName, param) ((size_t)&(((structName *)0)->param))

#endif    // _WHY_BASICTYPES_H_
