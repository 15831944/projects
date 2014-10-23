#ifndef COMPLEXCOMPLETIONPORT_IRECVINTERFACE_HEAD
#define COMPLEXCOMPLETIONPORT_IRECVINTERFACE_HEAD

#include <string>

class IRecvSink
{
public:
	virtual void OnRecv(char *buf, int bufLen, std::string remoteIp, unsigned short remotePort) = 0;
};
#endif