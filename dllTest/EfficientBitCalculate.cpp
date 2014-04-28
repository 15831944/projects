#include "EfficientBitCalculate.h"

EfficientBitCalculate::EfficientBitCalculate()
{

}

EfficientBitCalculate::~EfficientBitCalculate()
{

}

//返回右起第一个1的位置(32位); 若x=0,则返回32
unsigned int EfficientBitCalculate::FindFirstOneBitFromRight32(unsigned int x)
{
	check_return_val(x, 32);

	unsigned int n = 31;
	if (x & 0x0000ffff) { n -= 16; 	} else x >>= 16;
	if (x & 0x00ff) 	{ n -= 8; 	} else x >>= 8;
	if (x & 0x0f) 		{ n -= 4; 	} else x >>= 4;
	if (x & 0x3) 		{ n -= 2; 	} else x >>= 2;
	if (x & 0x1) 		{ n--; 		}

	return n;
}

//返回右起第一个1的位置(64位); 若x=0,则返回64
unsigned long long EfficientBitCalculate::FindFirstOneBitFromRight64(unsigned __int64 x)
{
	check_return_val(x, 64);

	unsigned long long n = FindFirstOneBitFromRight32((unsigned int)x);
	if (n == 32) n += FindFirstOneBitFromRight32((unsigned int)(x >> 32));

	return n;
}

//返回左起第一个1之前0的个数; 若x=0,则返回32
unsigned int EfficientBitCalculate::CountZeroNumBeforeFirstOneFromLeft32(unsigned int x)
{
	check_return_val(x, 32);

	unsigned int n = 31;
	if (x & 0xffff0000) { n -= 16; 	x >>= 16; 	}
	if (x & 0xff00) 	{ n -= 8; 	x >>= 8; 	}
	if (x & 0xf0) 		{ n -= 4; 	x >>= 4; 	}
	if (x & 0xc) 		{ n -= 2; 	x >>= 2; 	}
	if (x & 0x2) 		{ n--; 					}

	return n;
}

//返回左起第一个1之前0的个数; 若x=0,则返回64
unsigned long long EfficientBitCalculate::CountZeroNumBeforeFirstOneFromLeft64(unsigned __int64 x)
{
	check_return_val(x, 64);

	unsigned long long n = CountZeroNumBeforeFirstOneFromLeft32((unsigned int)(x >> 32));
	if (n == 32) n += CountZeroNumBeforeFirstOneFromLeft32((unsigned int)x);

	return n;
}

//计算1的个数（32位）
unsigned int EfficientBitCalculate::CountOneBitNum32(unsigned int x)
{
    check_return_val(x, 0);
    
	x = x - ((x >> 1) & 0x77777777) - ((x >> 2) & 0x33333333) - ((x >> 3) & 0x11111111);
	x = (x + (x >> 4) & 0x0f0f0f0f);
	x = (x * 0x01010101) >> 24;
 
    return x;
}

//计算1的个数（64位）
unsigned long long EfficientBitCalculate::CountOneBitNum64(unsigned __int64 x)
{
    check_return_val(x, 0);
    
	x = x - ((x >> 1) & 0x7777777777777777ULL) - ((x >> 2) & 0x3333333333333333ULL) - ((x >> 3) & 0x1111111111111111ULL);
	x = (x + (x >> 4) & 0x0f0f0f0f0f0f0f0fULL);
	x = (x * 0x0101010101010101ULL) >> 56;
   
    return (unsigned long long)x;
}