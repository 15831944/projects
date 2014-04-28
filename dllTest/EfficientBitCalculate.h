#pragma once

// check
#define check_return(x)				do { if (!(x)) { return ; } } while(0)
#define check_return_val(x, v)		do { if (!(x)) { return (v); } } while(0)
#define check_break(x)				{ if (!(x)) { break ; } }
#define check_continue(x)			{ if (!(x)) { continue ; } }

class EfficientBitCalculate
{
public:
	EfficientBitCalculate();
	~EfficientBitCalculate();

	static unsigned int CountZeroNumBeforeFirstOneFromLeft32(unsigned int x);  
	static unsigned long long CountZeroNumBeforeFirstOneFromLeft64(unsigned __int64 x);  

	static unsigned int FindFirstOneBitFromRight32(unsigned int x);  
	static unsigned long long FindFirstOneBitFromRight64(unsigned __int64 x);

	static unsigned int CountOneBitNum32(unsigned int x);
	static unsigned long long CountOneBitNum64(unsigned __int64 x);
}; 