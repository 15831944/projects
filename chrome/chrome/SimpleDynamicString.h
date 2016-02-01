#ifndef SIMPLE_DYNAMIC_STRING_H
#define SIMPLE_DYNAMIC_STRING_H
#include <stdio.h>


//支持二进制数据的保存

typedef char* sds;
struct sdshdr
{
	//记录buf数组中已经使用字节的数量
	int len;

	//记录buf中未使用的字节数量
	int free;

	//字符数组，用于保存字符串
	char buf[0];  //尾部会多加1字节的'\0'，不会记入len中
};


//function

static inline int sdslen(const sdshdr *sour)
{
	if(NULL == sour) return 0;
	return sour->len;
}

static inline int sdsavail(const sdshdr *sour)
{
	if(NULL == sour) return 0;
	return sour->free;
}

sdshdr*    sdsnew(const char *init);
sdshdr*    sdsnewlen(const void *init, int ininLen);
sdshdr*    sdsempty(void);
sdshdr*    sdsdup(const sdshdr*);

void       sdsfree(sdshdr *s);
void       sdsclear(sdshdr *sh);
sdshdr*    sdsremovefreespace(sdshdr *&sh);

sdshdr*    sdsMakeRoomFor(sdshdr *&sh, int addlen);
sdshdr*    sdscatlen(sdshdr *&dest, const void *sour, int sourLen);
sdshdr*    sdscat(sdshdr *&dest, const char *sour);
sdshdr*    sdscatsds(sdshdr *&dest, const sdshdr *sour);
sdshdr*    sdsgrowzero(sdshdr *&dest, int len);

sdshdr*    sdstrim(sdshdr *&sh, char *cset);

sdshdr*    sdscpylen(sdshdr *&dest, const void *szSour, int sourLen);
sdshdr*    sdscpy(sdshdr *&dest, const char *szSour);


#endif  //SIMPLE_DYNAMIC_STRING_H