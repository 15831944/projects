#pragma once

//************************************
// ��	 ��:	Zhong.Hao
// ��	 ��:	2014/08/27
// ˵    ��:    Protocol and the macro definition	
//************************************
#include <string>
#include "datastream.h"

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
	unsigned long			messageID_;    //��ϢID
	std::string			    ip_;           //���ڿͷ����Ǳ��ص�˽��IP�����ڷ������ǿ����Ŀͷ��˵Ĺ���IP (�����ֽ���)
	unsigned short	        port_;         //���ڿͷ����Ǳ��ص�˽��PORT�����ڷ������ǿ����Ŀͷ��˵Ĺ���PORT  (�����ֽ���)

	tagSStunMessage()
	: messageID_(0)
	, port_(0)
	{
	}

	tagSStunMessage(const tagSStunMessage& other)
	{
		messageID_ = other.messageID_;
		ip_        = other.ip_;
		port_      = other.port_;
	}

	tagSStunMessage& operator=(const tagSStunMessage& other)
	{
		if(this == &other)
			return *this;

		messageID_ = other.messageID_;
		ip_        = other.ip_;
		port_      = other.port_;
		return *this;
	}
}SStunMessage, *pSStunMessage;

CDataStream& operator<<(CDataStream& ds,SStunMessage& vpb);
CDataStream& operator>>(CDataStream& ds,SStunMessage& vpb);


