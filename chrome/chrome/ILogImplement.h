#ifndef ILOGIMPLEMENT_H
#define ILOGIMPLEMENT_H


enum eLogLevel
{
	_Info    = 0x01,
	_Debug   = 0x02,
	_Warn    = 0x03,
	_Error   = 0x04,
	_Fatal   = 0x05
};


class ILogImplement
{
public:
	ILogImplement(){};
	virtual ~ILogImplement(){}


	virtual bool StartLog() = 0;
	virtual void StopLog() = 0;
	virtual void SetLogDirPath(const char *cszDirPath) = 0;
	virtual bool PrintLog(const char* cszFileName, eLogLevel emLevel, const char *cszDataBuf) = 0;
};

#endif //ILOGIMPLEMENT_H