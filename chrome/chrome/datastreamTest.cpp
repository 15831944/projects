#include <string>
#include "datastream.h"
typedef struct tagSStunMessage
{
	unsigned long			messageID_;    
	std::string			    ip_;           
	unsigned short	        port_;        
	tagSStunMessage()
		: messageID_(0)
		, port_(0)
	{
	}
}SStunMessage, *pSStunMessage;

CDataStream& operator<<(CDataStream& ds,SStunMessage& vpb);
CDataStream& operator>>(CDataStream& ds,SStunMessage& vpb);

//上面的往往被发到一个.h文件中
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

int main()
{
	SStunMessage sm;
	sm.ip_ = "111";
	sm.messageID_ = 1;
	sm.port_ = 0;

	char buff[100] = {0};
	CDataStream sendStream(buff, 100);
	sendStream << sm;

	//socket.send(sendStream.getbuffer(), sendStream.size())
	return 0;
}