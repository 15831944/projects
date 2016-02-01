/************************************************************************/
/* FileName:    basictypes.h
/* Date:        2014-12-12
/* Author:      zhonghao
/* Description: ����һЩ��������Ҫ�õ��Ļ�����������
/************************************************************************/

#ifndef _WHY_BASICTYPES_H_
#define _WHY_BASICTYPES_H_

#include <stddef.h>         // For size_t

// ����һЩ�꣬����ƽ̨���
#if defined(_WIN32) || defined(_WIN64)
#define OS_WIN 1
#elif defined(__linux__) || defined(__CYGWIN__)
#define OS_LINUX 1
#else
#error This code is not tested on this platform.Please make sure all the code can work well before doing any real work.
#endif

// ��������ͬ���ͱ������ĺ�
#if defined(_MSC_VER) 
#define COMPILER_MSVC 1
#elif defined(__GNUC__)
#define COMPILER_GCC 1
#else 
#error This code is not tested on this compiler. Please check it to make sure all the code can work well.
#endif 

// ������д�����ı�־
#if defined(COMPILER_MSVC)
#define OVERRIDE override
#else 
#define OVERRIDE
#endif

// �����з�������
typedef signed char         schar;
typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;

#if defined (__LP64__)
typedef long                int64;
#else 
typedef long long           int64;
#endif

// �����޷�������
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;

#if defined (__LP64__)
typedef unsigned long       uint64;
#else 
typedef unsigned long long  uint64;
#endif

// �������γ����ı��
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

// ��������͵ķ�Χ
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

// ����һ���꣬���ڲ�����һ������и�ֵ����͸�ֵ���������غ���
// ��Ҫ����һ����ġ�private:�������У�����������boost::noncopyable
#define DISALLOW_COPY_AND_ASSIGN(TypeName)       \
    TypeName(const TypeName&);                   \
    TypeName& operator=(const TypeName&);

// ����һ���꣬���ڽ�ֹһ�������Ĭ�ϵ���ʽ����
// ��Ҫ����һ����ġ�private:�������У��������ʵ��������
// �ú��һЩ����static��������Ƚ�����
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName();                                  \
    DISALLOW_COPY_AND_ASSIGN(TypeName);

// ��������Ĵ�С
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

// ������ʾ��ע���ú����ķ���ֵ�����õģ���Ϊ�������ķ���ֵû���ж�ʱ����һЩ��̬��⹤�ߣ���pclint�����������
template<typename T>
inline void ignore_result(const T&){}

// ����һ���꺯�� ��ʾʲô������
#define DO_NOTHING() 


//����ṹ���в�����ƫ��λ��
#define findparmoffset(structName, param) ((size_t)&(((structName *)0)->param))

#endif    // _WHY_BASICTYPES_H_
