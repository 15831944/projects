
#include <windows.h>
#include <stdio.h>
#include <string>

#include "EfficientBitCalculate.h"
;


void main()
{
	EfficientBitCalculate bit;
	UINT32 num32 = 30768; 
	UINT64 num64 = 199584;

	int a = bit.CountZeroNumBeforeFirstOneFromLeft32(num32);
	int b = bit.CountZeroNumBeforeFirstOneFromLeft64(num64);

	printf("%d,%d\n", a, b);
    system("pause");
	return;
}