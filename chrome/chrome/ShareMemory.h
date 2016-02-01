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

		HANDLE  m_hMutexSyncData; //同步共享内存的数据
		HANDLE  m_hFileMapObj;   //file map object. if server, createfilemapping(). if client, openfilemapping()
		char*   m_pMapView;      //映射的内存的地址
		PSWRPos m_pWRPos;        //指向开头的位置上
		char*   m_pUserBufBasePos;   //除去记录SWRPos结构体的数据部分的开始位置
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
