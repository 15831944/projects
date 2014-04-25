#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif _LINUX
#include <stdarg.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#elif _LINUX
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

 
/************************************************************************/
/*                        创建目录                                       */
/************************************************************************/
bool CreatDir(string pszDir)
{
	int i = 0;

	int iRet = 0;
	int iLen = strlen(pszDir.c_str());


	if (pszDir[iLen - 1] != '\\')
	{
	//	pszDir += '\\';
	//	pszDir += '\0';
		pszDir += "\\";
	}

	for (i = 0;i < iLen + 2;i ++)
	{
		if (pszDir[i] == '\\')
		{
			pszDir[i] = '\0';

			//如果不存在,创建
			iRet = ACCESS(pszDir.c_str(),0);   //存在返回零
			if (iRet != 0)
			{
				iRet = MKDIR(pszDir.c_str());

				if (iRet != 0)
				{
					return false;
				} 
			}
			pszDir[i] = '\\';
		}
	}
	return true;
}


