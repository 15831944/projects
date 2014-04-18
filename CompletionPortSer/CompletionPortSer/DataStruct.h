#ifndef COMPLETIONPORTSER_DATASTRUCT_HEAD
#define COMPLETIONPORTSER_DATASTRUCT_HEAD
#include <WinSock2.h>
#define BUFFER_SIZE 1024

/*per_data*/
typedef struct _PER_HANDLE_DATA      
{
	SOCKET            sockConCli;       // client socket
	sockaddr_in       addr;             // client address

	_PER_HANDLE_DATA()
	{
		memset(this, 0x0, sizeof(*this));
	}
}PER_HANDLE_DATA, *PPER_HANDLE_DATA;


typedef struct _PER_IO_DATA
{
	OVERLAPPED      ol;                  
	char            buf[BUFFER_SIZE];     //data buf
	int             OperationType;        //operate type

	_PER_IO_DATA()
	{
		memset(this, 0x0, sizeof(*this));
	}

#define OP_READ   1
#define OP_WRITE  2
#define OP_ACCEPT 3

}PER_IO_DATA, *PPER_IO_DATA;

#endif