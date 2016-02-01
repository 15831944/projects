#pragma once
#include <Windows.h>
#include <string>

namespace BaseClassLibrary
{

	class IDataReceiver
	{
	public:
		virtual void OnRecv(const char*name,char* pData,int len) = 0; 
	};


	class CShareMemBase
	{
	public:
		CShareMemBase();
		~CShareMemBase();

	protected:
		void close();

		typedef struct _tagSWRPos
		{
			int mem_len;        //The Shared memory size
			int read_pos;       //the read position
			int write_pos;      //the write position

			_tagSWRPos()
			{
				mem_len   = 0;
				read_pos  = 0;
				write_pos = 0;
			}
		}SWRPos, *PSWRPos;

		HANDLE  m_hMutexSyncData; //ͬ�������ڴ������
		HANDLE  m_hFileMapObj;   //file map object. if server, createfilemapping(). if client, openfilemapping()
		char*   m_pMapView;      //ӳ����ڴ�ĵ�ַ
		PSWRPos m_pWRPos;        //ָ��ͷ��λ����
		char*   m_pUserBufBasePos;   //��ȥ��¼SWRPos�ṹ������ݲ��ֵĿ�ʼλ��
		HANDLE m_hWait[2];
	};   


	class CShareMem: public CShareMemBase
	{
	public:
		CShareMem();
		~CShareMem();
		bool Create(std::string strSMName, unsigned unSMMSize);
		bool Open(std::string strSMName);
		bool Send(char* dataBuf, unsigned int bufLen);
		bool Read(IDataReceiver *pDataRecver);
		
	private:
		bool Recv(IDataReceiver *pDataRecver);

	private:
		bool m_bSMSuccess;
		std::string m_strSMName;
	};
}
