#ifndef MACROFUNCTION_H
#define MACROFUNCTION_H
#pragma once

/*************************************************
// <Summary>template:获取数组的元素个数 </Summary>
// <DateTime>2014/12/15</DateTime>
*************************************************/
template<typename T, size_t N>
char (&ArrayElemNumHelper(T (&array)[N]))[N];    //这是一个函数的声明

#define ArrayElemNum(array) (sizeof(ArrayElemNumHelper(array)))

/*************************************************
// <Summary>template:获取数组的字节数</Summary>
// <DateTime>2014/12/15</DateTime>
*************************************************/
template<typename T, size_t N>
T (&ArrayMemSizeHelper(T (&array)[N]))[N];

#define ArrayMemSize(array) (sizeof(ArrayMemSizeHelper(array)))


/*************************************************
// <Summary>禁止复制与赋值的宏</Summary>
// <DateTime>2014/12/23</DateTime>
*************************************************/
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	TypeName& operator=(const TypeName&);


#endif  //MACROFUNCTION_H



