#ifndef STUNPROTOCOL_H
#define STUNPROTOCOL_H

//************************************
// 作	 者:	Zhong.Hao
// 日	 期:	2014/08/27
// 说    明:    Protocol and the macro definition	
//************************************
#include <string>

#define      STUN_DEFAULT_CLINET_PORT			9854  //client default port

//STUN Protocol
#define      STUN_BINDING_REQUEST				0x0001
#define      STUN_BINDING_RESPONSE				0x0002
#define      STUN_CHANGE_IP_PORT				0x0003
#define      STUN_CHANGE_IP_PORT_RESPONSE		0x0004
#define      STUN_CHANGE_PORT					0x0005
#define      STUN_CHANGE_PORT_RESPONSE	    	0x0006



#define      STUN_ERROR_RESPONSE	    		0x000A
#define      STUN_ERROR_NO_SECOND_IP    		0x000B
#define      STUN_ECHO_MESSAGE					0x000C


typedef struct tagSStunMessage
{
	unsigned long			header_;
	std::string			    ip_;
	unsigned short	        port_;

	tagSStunMessage()
	: header_(0)
	, port_(0)
	{
	}

	tagSStunMessage& operator=(const tagSStunMessage& other)
	{
		if(&other == this)
		{
			return *this;
		}

		header_ = other.header_;
		ip_     = other.ip_;
		port_   = other.port_;
		return *this;
	}

}SStunMessage, *pSStunMessage;


#endif  //STUNPROTOCOL_H