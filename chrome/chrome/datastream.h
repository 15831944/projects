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
//��������������
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
	//�˺�������̬�����ڴ�,һ�δ�ӡ�����ݳ��Ȳ�Ӧ�ó���������������֮һ,������ܵ���ʧ��   ??????
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
	//�˺��������ַ���
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
	bool move(int ilen)//��ǰָ������ƶ�ilen
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
		return 0;// PGP, 2010-7-13   15:54	����0������
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
		return 0;// PGP, 2010-7-13   15:54	����0������
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
		return 0;// PGP, 2010-7-13   15:54	����0������
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
	BYTE * readdata(DWORD dwLen)                                               //�ҿ��ܻ��õĺ���
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
	//���Ӱ�ȫ�ӿ�,�����ڴ�Խ��ʱ,���ش���
	//2008.5.5, Add by YP.
	bool readdata(DWORD dwLen,BYTE * pbyData)                             //�ҿ��ܻ��õĺ�����������һ���ã�
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
	
	//add by lh ԭ��ԭ����strcpy��������ʱû�н�0Copy��ȥ��������û�ж�Ӧ
	bool writestring(const char * szStr)
	{
		if(current&&szStr)
		{
			int ilen = (int)strlen(szStr);
			if(ilen>0)
			{
				if((m_isize-(current - buffer)) < (ilen +1))	//ԭ��Ϊ(ilen +2)
					return false;

				// 2011-4-18   16:59	�����ַ���ʱ�������ַ����Ƿ�Ϊ�գ���Ҫ����һ��0��ȥ
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

	//��ʾ�����Ƿ�ɹ�
	/*! Simon.M.Lu, 2007-11-22   11:10
	*	����good_bit
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
	int leavedata()//������ʣ���ֽ�
	{
		return m_isize-size();
	}
	const char * getbuffer(){return buffer;}
	int getbufferlength(void)const{return m_isize;}
protected :
	//�����Զ�д��������Ƿ�ɹ�,��ʧ��ʱ����Ϊfalse,��Ӧ�����ݷ���Ϊ0
	bool 	m_GoodBit;

	char* buffer;
	char* current;
	int m_isize;
	list<char*> m_stack;//��ջ
};


//������������
//use:
//	CDataStream cdrIn(pBuff, len);
//	cdrIn >> dwLen >> ucFlag;
// 	assert(cdrIn.good_bit());		//check

//	CDataStream cdOut(pBuff, len);
//	cdOut << dwLen << ucFlag;
// 	assert(cdOut.good_bit());		//check

//������
#ifdef WIN32
inline CDataStream & operator >> (CDataStream &is, DWORD & x)
{
	x = is.readdword();
	return  is;
}
#endif
/*!2012-5-25   14:08
*	���unsigned int
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
	if(pstr)//����ǿ�ָ��,��ֵ��string�����
		x = pstr;
	return  is;
}
inline CDataStream & operator >> (CDataStream &is, wstring & x)
{
	wchar_t * pstr = is.read_wstring();
	if(pstr)//����ǿ�ָ��,��ֵ��string�����
		x = pstr;
	return  is;
}


//�����
#ifdef WIN32
inline CDataStream & operator << (CDataStream &os, const DWORD & x)
{
	os.writedword(x);
	return  os;
}
#endif
/*! 2012-5-25   14:08
*	���unsigned int
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
//ʹ��
//CInputDataStream dsIn(pBuf, bufLen);
//dsIn >> dwLen1 >> ucFlag1;
// 	assert(dsIn.good_bit());		//check

//��������������,����
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

//�����,�����ڴ��Զ����书�ܼ�������������
//ʹ��
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
		//Ԥ����1024���ڴ�
		pOutBuf = new char[CHUNK];
		buffer =  pOutBuf;
		current = buffer;	
		m_isize = CHUNK;
	}
	//Ԥ����ָ���Ļ�������С
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
		//�����ڴ�����
		DWORD dwSize = m_isize * 2;
		size_t nSize = current - buffer;	//ԭ������Ϣ����
		m_isize = max(dwSize, static_cast<DWORD>(nLen+nSize));

		bool bDelete =true;
		if(NULL == pOutBuf)
			bDelete = false;
		//�ط����ڴ�
		//vctBuff.resize(m_isize);
		pOutBuf = new char[m_isize];

		//ԭ��������Copy�����»�����
		memcpy(pOutBuf, buffer, nSize);

		//�������û���λ�õ�
		if(bDelete)
			delete [] buffer;			//ɾ���ɵ�

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


//���ӷ�������


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
