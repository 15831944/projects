#include "SimpleDynamicString.h"
#include <string.h>
#include <malloc.h>

//֧�ֶ������ļ�
sdshdr* sdsnewlen(const void *init, int ininLen)
{
	struct sdshdr *sh;
	sh = (sdshdr *)malloc(sizeof(sdshdr) + ininLen + 1);
	if(NULL == sh) return NULL;
	memset(sh, 0x0, sizeof(sdshdr) + ininLen + 1);

	sh->len  = ininLen;
	sh->free = 0;
	if(ininLen && init)
		memcpy(sh->buf, init, ininLen);

	sh->buf[ininLen] = '\0';
	return sh;
}

//��֧�ֶ������ļ�
sdshdr* sdsnew(const char * init)
{
	int strLen = (NULL == init) ? 0 : strlen(init);
	return sdsnewlen((void *)init, strLen);
}

sdshdr* sdsempty(void)
{
	return sdsnewlen("", 0);
}

//֧�ֶ������ļ�
sdshdr* sdsdup(const sdshdr* sour)
{
	return sdsnewlen(sour->buf, sdslen(sour));
}

void sdsfree(sdshdr *s)
{
	if(NULL == s) return;
	free(s);
}

void sdsclear(sdshdr *sh)
{
	if(NULL == sh) return;
	sh->buf[0] = '\0';
	sh->free += sh->len;
	sh->len = 0;
}

sdshdr* sdsremovefreespace(sdshdr *&sh)
{
	if(NULL == sh) return NULL;
	sh = (sdshdr *)realloc(sh, sizeof(struct sdshdr) + sh->len + 1);
	if(NULL == sh) return NULL;
	sh->free = 0;
	return sh;
}

/*************************************************
// <Summary>������ָ�����ַ�����չbuf�Ĵ洢�ռ�</Summary>
// <DateTime> 2015/07/10 </DateTime>
// <Parameter name="sh" type="IN/OUT">Ŀ���ַ���</Parameter>
// <Parameter name="addlen" type="IN">��Ҫ���ӵĶ���ĳ���</Parameter>
// <Returns>���ؿռ䱻��չ�˵��ַ���</Returns>
// @author  zhonghao
*************************************************/
sdshdr* sdsMakeRoomFor(sdshdr *&sh, int addlen)
{
	int free = sdsavail(sh);
	int len, newLen;
	if(NULL == sh) return NULL;
	

	if(free >= addlen) return sh;
	len = sdslen(sh);
	newLen = addlen + len;
	if(newLen < 1048576)
		newLen *= 2;
	else
		newLen += 1048576;

	sh = (sdshdr *)realloc(sh, sizeof(struct sdshdr) + newLen + 1);
	if(NULL == sh) return NULL;
	memset(sh->buf + len, 0x0, newLen - len + 1);
	sh->free = newLen - len;

	return sh; 
}

//֧�ֶ������ļ�
sdshdr* sdscatlen(sdshdr *&dest, const void *sour, int sourLen)
{
	if(NULL == dest) return NULL;
	dest = sdsMakeRoomFor(dest, sourLen);
	if(NULL == dest) return NULL;

	memcpy(dest->buf + dest->len, sour, sourLen);
	dest->free -= sourLen;
	dest->len  += sourLen;
	dest->buf[dest->len] = '\0';
	return dest;
}

//��֧�ֶ������ļ�
sdshdr* sdscat(sdshdr *&dest, const char *sour)
{
	int len = (NULL == sour) ? 0 : strlen(sour);
	return sdscatlen(dest, sour, len);
}

//֧�ֶ������ļ�
sdshdr* sdscatsds(sdshdr *&dest, const sdshdr *sour)
{
	return sdscatlen(dest, sour->buf, sdslen(sour));
}


/*************************************************
// <Summary>�ÿ��ַ���SDS��buf�ֶ���չ�������ĳ���</Summary>
// <DateTime> 2015/07/10 </DateTime>
// <Parameter name="dest" type="IN/OUT">Ŀ���ַ���</Parameter>
// <Parameter name="len" type="IN">��չbuf���ĳ���</Parameter>
// <Returns>��չ����ַ���</Returns>
// @author  zhonghao
*************************************************/
sdshdr* sdsgrowzero(sdshdr *&dest, int len)
{
	if(NULL == dest) return NULL;
	if(len <= dest->len) return dest;
	dest = sdsMakeRoomFor(dest, len - dest->len);
	if(NULL == dest) return NULL;

	memset(dest->buf + dest->len, 0x0, len - dest->len);
	dest->free -= (len - dest->len);
	dest->len  += (len - dest->len);
	dest->buf[dest->len] = '\0';
	return dest;
}


sdshdr*  sdscpylen(sdshdr *&dest, const void *szSour, int sourLen)
{
	if(NULL == dest) return NULL;
	if(sourLen > dest->free + dest->len)
		dest = sdsMakeRoomFor(dest, sourLen - dest->len);

    if(NULL == dest)
		return NULL;

	int total = dest->len + dest->free;
	memcpy(dest->buf, szSour, sourLen);
	dest->buf[sourLen] = '\0';
	dest->len = sourLen;
	dest->free = total- sourLen;

	return dest;
}
sdshdr* sdscpy(sdshdr *&dest, const char *szSour)
{
	int len =  (NULL == szSour)? 0 : strlen(szSour); 
	return sdscpylen(dest, szSour, len);
}

sdshdr* sdstrim(sdshdr *&sh, const char* cset)
{
	char *start, *end, *sp, *ep;

	sp = start = sh->buf;
	ep = end   = sh->buf + sh->len - 1;
	while(sp <= end && strchr(cset, *sp)) sp++;
	while(ep >start && strchr(cset, *ep)) ep--;

	int len = (sp > ep) ? 0 : ((ep-sp)+1);
	if(sh->buf != sp) memmove(sh->buf, sp, len);
	sh->buf[len] = '\0';
	sh->free = sh->free + (sh->len-len);
	sh->len = len;
	return sh;
}


