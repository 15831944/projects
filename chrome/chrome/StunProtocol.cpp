#include "StunProtocol.h"

CDataStream& operator<<(CDataStream& ds,SStunMessage& vpb)
{
	ds << vpb.messageID_;
	ds << vpb.ip_;
	ds << vpb.port_;
	return ds;
}

CDataStream& operator>>(CDataStream& ds,SStunMessage& vpb)
{
	ds >> vpb.messageID_;
	ds >> vpb.ip_;
	ds >> vpb.port_;
	return ds;
}
