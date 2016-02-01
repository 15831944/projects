#ifndef MACROFUNCTION_H
#define MACROFUNCTION_H
#pragma once

/*************************************************
// <Summary>template:��ȡ�����Ԫ�ظ��� </Summary>
// <DateTime>2014/12/15</DateTime>
*************************************************/
template<typename T, size_t N>
char (&ArrayElemNumHelper(T (&array)[N]))[N];    //����һ������������

#define ArrayElemNum(array) (sizeof(ArrayElemNumHelper(array)))

/*************************************************
// <Summary>template:��ȡ������ֽ���</Summary>
// <DateTime>2014/12/15</DateTime>
*************************************************/
template<typename T, size_t N>
T (&ArrayMemSizeHelper(T (&array)[N]))[N];

#define ArrayMemSize(array) (sizeof(ArrayMemSizeHelper(array)))


/*************************************************
// <Summary>��ֹ�����븳ֵ�ĺ�</Summary>
// <DateTime>2014/12/23</DateTime>
*************************************************/
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	TypeName& operator=(const TypeName&);


#endif  //MACROFUNCTION_H



