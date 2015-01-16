#ifndef STUNPROTOCOL_H
#define STUNPROTOCOL_H

//************************************
// 作	 者:	Zhong.Hao
// 日	 期:	2014/08/27
// 说    明:    Protocol and the macro definition	
//************************************
#include <string>

//STUN Protocol
#define      STUN_BINDING_REQUEST				0x101
#define      STUN_BINDING_RESPONSE				0x201
#define      STUN_CHANGE_IP_PORT				0x102
#define      STUN_CHANGE_IP_PORT_RESPONSE		0x202
#define      STUN_CHANGE_PORT					0x103
#define      STUN_CHANGE_PORT_RESPONSE	    	0x203



#define      STUN_ERROR_RESPONSE	    		0x000A
#define      STUN_ERROR_NO_SECOND_IP    		0x000B
#define      STUN_ECHO_MESSAGE					0x000C


typedef struct tagSStunMessage
{
	unsigned long           task_id_;
	unsigned long			header_;
	std::string			    ip_;        //Network byte order
	unsigned short	        port_;      //Network byte order

	tagSStunMessage()
	: task_id_(0)
	, header_(0)
	, port_(0)
	{
	}
}SStunMessage, *pSStunMessage;


#endif  //STUNPROTOCOL_H