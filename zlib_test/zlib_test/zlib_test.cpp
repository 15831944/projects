// zlib_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "GZipHelper.h"
#include "ziplib/zlib.h"
#include <iostream>

//gzip
int test_GzipByMem()
{
	char plainText[]="Plain text here";
	CA2GZIP gzip(plainText,strlen(plainText));
	// do compressing here;
	LPGZIP pgzip=gzip.pgzip; // pgzip is zipped data pointer,
	// you can use it directly
	int len=gzip.Length; // Length is length of zipped
	// data;

	CGZIP2A plain(pgzip,len); // do uncompressing here

	char *pplain=plain.psz; // psz is plain data pointer
	int aLen=plain.Length; // Length is length of unzipped
	// data.
	return 0;
}

//zlib
int test_compress()
{
	Byte unCompData[1024] = "zhong hao";
	Byte comData[1024] = {0};
	unsigned long cmpLen;
	int error = compress(comData, &cmpLen, unCompData, strlen((char *)unCompData)+1);

	if (0 != error)
	{
		//error log
	}

	//do compressing here;
	Byte buf[1024] = {0};
	unsigned long len;
	int err = uncompress(buf, &len, comData, cmpLen);
	if (0 != err)
	{
		//error log	
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	test_GzipByMem();
	test_compress();
	return 0;
}

