#include "UnhandleException.h"
using namespace BaseClassLibrary;

int main()
{
	g_UnhandleException.SetSavePath("c:\\");   //设置dump文件产生的全目录路径
	g_UnhandleException.OpenSaveDumpFun(false);  //关闭dump文件的产生
}