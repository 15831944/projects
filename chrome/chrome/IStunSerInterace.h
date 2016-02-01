#pragma once

class IStunSerInter
{
public:
	virtual bool Start(const char* ip1, const char* ip2, unsigned short port1 = 3478, 
			      unsigned short port2 = 3479, unsigned short port3 = 3480, unsigned short port4 = 3480) = 0;

	virtual void Stop() = 0;
};