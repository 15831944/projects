#ifndef SIMPLE_DYNAMIC_STRING_H
#define SIMPLE_DYNAMIC_STRING_H
#include <stdio.h>


//֧�ֶ��������ݵı���

typedef char* sds;
struct sdshdr
{
	//��¼buf�������Ѿ�ʹ���ֽڵ�����
	int len;

	//��¼buf��δʹ�õ��ֽ�����
	int free;

	//�ַ����飬���ڱ����ַ���
	char buf[0];  //β������1�ֽڵ�'\0'���������len��
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