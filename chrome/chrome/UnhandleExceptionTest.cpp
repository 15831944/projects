#include "UnhandleException.h"
using namespace BaseClassLibrary;

int main()
{
	g_UnhandleException.SetSavePath("c:\\");   //����dump�ļ�������ȫĿ¼·��
	g_UnhandleException.OpenSaveDumpFun(false);  //�ر�dump�ļ��Ĳ���
}