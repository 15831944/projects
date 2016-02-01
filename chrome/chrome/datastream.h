// DataStream.h: interface for the CDataStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASTREAM_H__D90A2534_EA73_4BEA_8B7E_87E59A3D1D26__INCLUDED_)
#define AFX_DATASTREAM_H__D90A2534_EA73_4BEA_8B7E_87E59A3D1D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h> 
#include <assert.h> 
#include <string>
#include <vector>
#include <list>
#include <stdarg.h>
using namespace std;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
//数据流操作函数
class CDataStream  
{
public :
	CDataStream(char * szBuf,int isize)
	{
		m_GoodBit=true;
		m_isize = isize;
		buffer = szBuf;
		current = buffer;
	}
	~CDataStream()
	{
	}
    void clear()
	{
		current = buffer;
		current[0]=0;
	}
	//此函数不动态增加内存,一次打印的数据长度不应该超过缓冲区的三分之一,否则可能导致失败   ??????
	bool printf(const char * format,...)
	{
		if(current)
		{
			if(current - buffer > (m_isize*2)/3)
				return false;
			va_list argPtr ;
			va_start( argPtr, format ) ;
			int count = vsprintf_s( current, leavedata(), format, argPtr ) ;
			va_end( argPtr );
			current += count ;
			return true;
		}
		return false;
	}
	//此函数拷贝字符串
	bool strcpy(const char * szStr)
	{
		if(current&&szStr)
		{
			int ilen = (int)strlen(szStr);
			if((m_isize-(current - buffer)) < (ilen +2))
				return false;
			memcpy(current,szStr,ilen+1);
			current += ilen;
			return true;
		}
		return false;
	}
	char * getcurrentpos()
	{
		return current;
	}
	bool move(int ilen)//当前指针向后移动ilen
	{

		assert((current + ilen) <= (buffer + m_isize));
		if(m_GoodBit && (current + ilen) <= (buffer + m_isize))
		{
			current += ilen;
		}else{
			m_GoodBit	= false;
		}

		return m_GoodBit;

	}
	void reset()
	{
		current = buffer;
	}
	void push()
	{
		m_stack.push_back(current);
	}
	void pop()
	{
		current = *m_stack.rbegin();
		m_stack.pop_back();
	}
	BYTE readbyte()
	{
		assert((current + 1) <= (buffer + m_isize));
		if(m_GoodBit && (current + 1) <= (buffer + m_isize))
		{
			current ++;                
			return *(current-1);
		}
		m_GoodBit = false;
		//return (BYTE)-1;
		return 0;// PGP, 2010-7-13   15:54	返回0更合适
	}
	void writebyte(BYTE btValue)
	{
		assert((current + 1) <= (buffer + m_isize));
		if(m_GoodBit && (current + 1) <= (buffer + m_isize))
		{
			*current = btValue;
			current ++; 	
		}
		else
			m_GoodBit = false;

	}
	WORD readword()
	{
		assert((current + 2) <= (buffer + m_isize));
		if(m_GoodBit && (current + 2) <= (buffer + m_isize))
		{
			current +=2;
			return *((WORD*)(current-2));
		}

		m_GoodBit = false;
		//return (WORD)-1;
		return 0;// PGP, 2010-7-13   15:54	返回0更合适
	}
	void writeword(WORD wValue)
	{		
		assert((current + 2) <= (buffer + m_isize));
		if(m_GoodBit && (current + 2) <= (buffer + m_isize))
		{
			*((WORD*)current) = wValue;
			current +=2;
		}
		else
			m_GoodBit = false;
	}
	float readfloat()
	{
		assert((current + sizeof(float)) <= (buffer + m_isize));
		if(m_GoodBit && (current + sizeof(float)) <= (buffer + m_isize))
		{
			current +=sizeof(float);
			return *((float*)(current-sizeof(float)));
		}
		m_GoodBit = false;
		return 0;
		
	}
	void writefloat(float fValue)
	{
		assert((current + sizeof(float)) <= (buffer + m_isize));
		if((current + sizeof(float)) <= (buffer + m_isize))
		{
			*((float*)current) = fValue;
			current +=sizeof(float);
		}
		else
			m_GoodBit = false;
	}
	int readint()
	{
		assert((current + sizeof(int)) <= (buffer + m_isize));
		if((current + sizeof(int)) <= (buffer + m_isize))
		{
			current +=sizeof(int);
			return *((int*)(current-sizeof(int)));
		}
		m_GoodBit = false;
		return 0;

	}
	void writeint(int iValue)
	{
		assert((current + sizeof(int)) <= (buffer + m_isize));
		if((current + sizeof(int)) <= (buffer + m_isize))
		{
			*((int*)current) = iValue;
			current +=sizeof(int);
		}
		else
			m_GoodBit = false;
	}
	DWORD readdword()
	{
		assert((current + 4) <= (buffer + m_isize));
		if(m_GoodBit && (current + 4) <= (buffer + m_isize))
		{
			current +=4;
			return *((DWORD*)(current-4));
		}
		m_GoodBit = false;
		return 0;
	}
	void writedword(DWORD dwValue)
	{
		assert((current + 4) <= (buffer + m_isize));
		if((current + 4) <= (buffer + m_isize))
		{
			*((DWORD*)current) = dwValue;
			current +=4;
		}
		else
			m_GoodBit = false;
	}
	__int64 readint64()
	{
		assert((current + 8) <= (buffer + m_isize));
		if(m_GoodBit && (current + 8) <= (buffer + m_isize))
		{
			current +=8;
			return *((__int64*)(current-8));
		}
		
		m_GoodBit = false;
		//return (__int64)-1;
		return 0;// PGP, 2010-7-13   15:54	返回0更合适
	}
	void writeint64(__int64 iValue)
	{
		assert((current + 8) <= (buffer + m_isize));
		if((current + 8) <= (buffer + m_isize))
		{
			*((__int64*)current) = iValue;
			current +=8;
		}
		else
			m_GoodBit = false;
	}
	BYTE * readdata(DWORD dwLen)                                               //我可能会用的函数
	{
		assert((current + dwLen) <= (buffer + m_isize));
		if(m_GoodBit && (current + dwLen) <= (buffer + m_isize))
		{
			current +=dwLen;
			return (BYTE*)(current-dwLen);
		}
		
		m_GoodBit = false;
		return NULL;
	}
	//增加安全接口,当读内存越界时,返回错误
	//2008.5.5, Add by YP.
	bool readdata(DWORD dwLen,BYTE * pbyData)                             //我可能会用的函数（比上面一个好）
	{ 
		if(m_GoodBit && (current + dwLen) <= (buffer + m_isize))
		{
			memcpy(pbyData,current,dwLen);
			current +=dwLen;
			return true;
		}
		m_GoodBit = false; //add by pgp
		return false;
	}
	void writedata(BYTE * pData,DWORD dwLen)
	{
		assert((current + dwLen) <= (buffer + m_isize));
		if((current + dwLen) <= (buffer + m_isize))
		{
			memcpy(current,pData,dwLen);		
			current +=dwLen;
		}
		else
			m_GoodBit = false;
	}

	wchar_t* read_wstring()
	{
		int ilen = 0;
		int buf_left = leavedata()/sizeof(wchar_t);
		bool good = false;
		for(ilen=0; m_GoodBit && ilen<buf_left; ++ilen)
		{
			if(0==((wchar_t*)current)[ilen])
			{
				good	= true;
				break;
			}
		}
		if(!good)
		{
			m_GoodBit	= false;
			return NULL;
		}
		wchar_t * szRes = (wchar_t*)current;
		if(m_GoodBit && (current + ilen*sizeof(wchar_t)+2) <= (buffer + m_isize))
		{
			current +=(ilen*sizeof(wchar_t)+2);
			return szRes;
		}
		m_GoodBit = false;
		return NULL;
	}

	char * readstring()
	{
		int ilen = 0;
		int buf_left = leavedata();
		bool good = false;
		for(ilen=0; m_GoodBit && ilen<buf_left; ++ilen)
		{
			if(0==current[ilen])
			{
				good	= true;
				break;
			}
		}
		if(!good)
		{
			m_GoodBit	= false;
			return "";
		}
		char * szRes = current;
		if(m_GoodBit && (current + ilen+1) <= (buffer + m_isize))
		{
			current +=(ilen+1);
			return szRes;
		}
		m_GoodBit = false;
		return "";
	}
	
	//add by lh 原因：原来的strcpy函数操作时没有将0Copy进去，函数名没有对应
	bool writestring(const char * szStr)
	{
		if(current&&szStr)
		{
			int ilen = (int)strlen(szStr);
			if(ilen>0)
			{
				if((m_isize-(current - buffer)) < (ilen +1))	//原来为(ilen +2)
					return false;

				// 2011-4-18   16:59	拷贝字符串时，不管字符串是否为空，都要拷贝一个0进去
				memcpy(current,szStr,ilen);
				current += (ilen);	
			}
		}
		writebyte(0);
		return true;
	}

	bool write_wstring(const wchar_t * szStr)
	{
		if(current&&szStr)
		{
			int ilen = (int)wcslen(szStr);
			if(ilen>0)
			{
				if((m_isize-(current - buffer)) < (ilen*2+2))
					return false;

				memcpy(current,szStr,ilen*2);
				current += (ilen*2);
			}
		}
		writeword(0);
		return true;
	}

	//标示操作是否成功
	/*! Simon.M.Lu, 2007-11-22   11:10
	*	设置good_bit
	*/
	void good_bit(bool flag){m_GoodBit=flag;}
	bool good_bit()
	{
		return m_GoodBit;
	}

	//add end 
	int size()
	{
		return (int)(current-buffer);
	}
	int leavedata()//缓冲区剩余字节
	{
		return m_isize-size();
	}
	const char * getbuffer(){return buffer;}
	int getbufferlength(void)const{return m_isize;}
protected :
	//用来对读写操作检查是否成功,当失败时设置为false,对应的数据返回为0
	bool 	m_GoodBit;

	char* buffer;
	char* current;
	int m_isize;
	list<char*> m_stack;//堆栈
};


//增加流操作符
//use:
//	CDataStream cdrIn(pBuff, len);
//	cdrIn >> dwLen >> ucFlag;
// 	assert(cdrIn.good_bit());		//check

//	CDataStream cdOut(pBuff, len);
//	cdOut << dwLen << ucFlag;
// 	assert(cdOut.good_bit());		//check

//输入流
#ifdef WIN32
inline CDataStream & operator >> (CDataStream &is, DWORD & x)
{
	x = is.readdword();
	return  is;
}
#endif
/*!2012-5-25   14:08
*	添加unsigned int
*/
 inline CDataStream & operator >> (CDataStream &is, unsigned int & x)
 {
	 x = is.readdword();
	 return  is;
 }
inline CDataStream & operator >> (CDataStream &is, WORD & x)
{
	x = is.readword();
	return  is;
}
inline CDataStream & operator >> (CDataStream &is, BYTE & x)
{
	x = is.readbyte();
	return  is;
}

inline CDataStream & operator >> (CDataStream &is, float & x)
{
	x = is.readfloat();
	return  is;
}
inline CDataStream & operator >> (CDataStream &is, __int64 & x)
{
	x = is.readint64();
	return  is;
}

inline CDataStream & operator >> (CDataStream &is, int & x)
{
	x = is.readint();
	return  is;
}

inline CDataStream & operator >> (CDataStream &is, unsigned __int64 & x)
{
	x = is.readint64();
	return  is;
}
inline CDataStream & operator >> (CDataStream &is, string & x)
{
	char * pstr = is.readstring();
	if(pstr)//如果是空指针,赋值给string会崩溃
		x = pstr;
	return  is;
}
inline CDataStream & operator >> (CDataStream &is, wstring & x)
{
	wchar_t * pstr = is.read_wstring();
	if(pstr)//如果是空指针,赋值给string会崩溃
		x = pstr;
	return  is;
}


//输出流
#ifdef WIN32
inline CDataStream & operator << (CDataStream &os, const DWORD & x)
{
	os.writedword(x);
	return  os;
}
#endif
/*! 2012-5-25   14:08
*	添加unsigned int
*/
 inline CDataStream & operator << (CDataStream &os, const unsigned int & x)
 {
	 os.writedword(x);
	 return  os;
 }
inline CDataStream & operator << (CDataStream &os, const WORD & x)
{
	os.writeword(x);
	return  os;
}
inline CDataStream & operator << (CDataStream &os, const BYTE & x)
{
	os.writebyte(x );
	return  os;
}

inline CDataStream & operator << (CDataStream &os, const float & x)
{
	os.writefloat(x);
	return  os;
}
inline CDataStream & operator << (CDataStream &os, const __int64 & x)
{
	os.writeint64(x);
	return  os;
}
inline CDataStream & operator << (CDataStream &os, const int & x)
{
	os.writeint(x);
	return  os;
}

inline CDataStream & operator << (CDataStream &os, unsigned __int64 & x)
{
	os.writeint64(x);
	return  os;
}

inline CDataStream & operator << (CDataStream &os, const string & x)
{
	os.writestring(x.c_str());
	return  os;
}
inline CDataStream & operator << (CDataStream &os, const wstring & x)
{
	os.write_wstring(x.c_str());
	return  os;
}
//使用
//CInputDataStream dsIn(pBuf, bufLen);
//dsIn >> dwLen1 >> ucFlag1;
// 	assert(dsIn.good_bit());		//check

//数据流操作函数,输入
class CInputDataStream : public CDataStream
{
public:
	CInputDataStream(char * szBuf, int isize)
		:CDataStream(szBuf, isize)
	{
	}
	CInputDataStream & operator >> (DWORD & dwValue)
	{
		dwValue = readdword();
		return *this;
	}
	
	CInputDataStream & operator >> (WORD & wValue)
	{
		wValue = readword();
		return *this;
	}
	CInputDataStream & operator >> (BYTE & ucValue)
	{
		ucValue = readbyte();
		return *this;
	}

	CInputDataStream & operator >> (float & fValue)
	{
		fValue = readfloat();
		return *this;
	}
	CInputDataStream & operator >> (__int64 & i64Value)
	{
		i64Value = readint64();
		return *this;
	}
	CInputDataStream & operator >> (string & strValue)
	{
		strValue = readstring();
		return *this;
	}
};

//输出流,增加内存自动分配功能及流操作符重载
//使用
//COutputDataStream dsOut;
//dsOut << (DWORD)0 << BYTE('Z') << (WORD)wTest;
//assert(dsOut.good_bit());		//check

class COutputDataStream : public CDataStream
{
public:
	#define CHUNK 1024
	COutputDataStream()
		:CDataStream(NULL, 0),
		pOutBuf(NULL)
	{
		//预分配1024的内存
		pOutBuf = new char[CHUNK];
		buffer =  pOutBuf;
		current = buffer;	
		m_isize = CHUNK;
	}
	//预分配指定的缓冲区大小
	COutputDataStream(int isize)
		:CDataStream(NULL, isize)
		,pOutBuf(NULL)
	{
		pOutBuf = new char[isize];
		buffer =  pOutBuf;
		current = buffer;	
		m_isize = isize;
	}

	COutputDataStream(char * szBuf, int isize);
	//	:CDataStream(szBuf, isize),
	//	pOutBuf(NULL)
	//{
	//}

	~COutputDataStream()
	{
		if(NULL != pOutBuf)
		{
			delete [] pOutBuf;
		}
		pOutBuf = NULL;
		buffer = NULL;
		m_isize = 0;
	}

	inline void ReAllocMem(size_t nLen = 0)
	{
		//分配内存增倍
		DWORD dwSize = m_isize * 2;
		size_t nSize = current - buffer;	//原来的消息长度
		m_isize = max(dwSize, static_cast<DWORD>(nLen+nSize));

		bool bDelete =true;
		if(NULL == pOutBuf)
			bDelete = false;
		//重分配内存
		//vctBuff.resize(m_isize);
		pOutBuf = new char[m_isize];

		//原来的数据Copy到的新缓冲区
		memcpy(pOutBuf, buffer, nSize);

		//重新设置缓冲位置等
		if(bDelete)
			delete [] buffer;			//删除旧的

		buffer =  pOutBuf;
		current = buffer + nSize;
	}

	COutputDataStream& operator << ( DWORD & dwValue )
	{
		if((current + sizeof(dwValue)) > (buffer + m_isize))
		{
			ReAllocMem();
		}
		writedword(dwValue);
		return *this;
	}
	
	void writedata(BYTE * pData,DWORD dwLen)
	{
		if((current + dwLen) > (buffer + m_isize))
		{
			ReAllocMem(dwLen);
		}
		memcpy(current,pData,dwLen);		
		current += dwLen;
	}

	COutputDataStream& operator << ( WORD & wValue )
	{
		if((current + sizeof(WORD)) > (buffer + m_isize))
		{
			ReAllocMem();
		}
		writeword(wValue);
		return *this;
	}

	COutputDataStream& operator << ( BYTE & ucValue )
	{
		if((current + sizeof(BYTE)) > (buffer + m_isize))
		{
			ReAllocMem();
		}
		writebyte(ucValue);
		return *this;
	}

	COutputDataStream& operator << ( float & fValue )
	{
		if((current + sizeof(float)) > (buffer + m_isize))
		{
			ReAllocMem();
		}
		writefloat(fValue);
		return *this;
	}

	COutputDataStream& operator << ( __int64 & i64Value )
	{
		if((current + sizeof(__int64)) > (buffer + m_isize))
		{
			ReAllocMem();
		}
		writeint64(i64Value);
		return *this;
	}

	COutputDataStream& operator << ( string & strValue )
	{
		if(current)
		{
			int ilen = static_cast<int>(strValue.size());
			if((m_isize-(current - buffer)) < (ilen +1))
			{
				ReAllocMem((current - buffer));
			}
		}

		writestring(strValue.c_str());
		return *this;
	}

private:
	//vector<char>	vctBuff;
	char * 	pOutBuf;

};


//增加符号重载


class CNetworkByteOrder
{
public:
	static unsigned short int convert(unsigned short int iValue)
	{
		unsigned short int iData;
		((BYTE*)&iData)[0] = ((BYTE*)&iValue)[1];
		((BYTE*)&iData)[1] = ((BYTE*)&iValue)[0];
		return iData;
	}
	static int convert(int iValue)
	{
		int iData;
		((BYTE*)&iData)[0] = ((BYTE*)&iValue)[3];
		((BYTE*)&iData)[1] = ((BYTE*)&iValue)[2];
		((BYTE*)&iData)[2] = ((BYTE*)&iValue)[1];
		((BYTE*)&iData)[3] = ((BYTE*)&iValue)[0];
		return iData;
	}
	static __int64 convert(__int64 iValue)
	{
		__int64 iData;
		((BYTE*)&iData)[0] = ((BYTE*)&iValue)[7];
		((BYTE*)&iData)[1] = ((BYTE*)&iValue)[6];
		((BYTE*)&iData)[2] = ((BYTE*)&iValue)[5];
		((BYTE*)&iData)[3] = ((BYTE*)&iValue)[4];
		((BYTE*)&iData)[4] = ((BYTE*)&iValue)[3];
		((BYTE*)&iData)[5] = ((BYTE*)&iValue)[2];
		((BYTE*)&iData)[6] = ((BYTE*)&iValue)[1];
		((BYTE*)&iData)[7] = ((BYTE*)&iValue)[0];
		return iData;
	}

};
#endif // !defined(AFX_DATASTREAM_H__D90A2534_EA73_4BEA_8B7E_87E59A3D1D26__INCLUDED_)
