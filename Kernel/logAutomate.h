// **************************************************************************
//                        logAutomate.h  -  description
//                           -------------------
//  begin                : Tue Jun 27 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Description:
//  This class is interface calss for loging events in autmate. It has functions
//  that provide all necessery interface for loging any event in system. What is 
//  going to be loged is defined with flags.
//  Class provides liging on diferent medimus, curently this medimus are file,
//  and over UDP socket. In future it sholud be expandwed with RS232 and other
//  transport mediums.
//
// **************************************************************************

#ifndef __LOG_AUTOMATE__
#define __LOG_AUTOMATE__

#ifndef __PROGRAMMING_INTERFACE__
#include "kernelAPI.h"
#endif

#ifndef __KERNEL_CONSTS__
#include "../kernel/kernelConsts.h"
#endif

//log messages codes
const uint8 LOG_START         = 0x01;
const uint8 LOG_END           = 0x02;
const uint8 LOG_EVENT_START   = 0x03;
const uint8 LOG_EVENT_END     = 0x04;
const uint8 LOG_EVENT_MESSAGE = 0x05;  //for proccessing
const uint8 LOG_MESSAGE_SENT  = 0x06;  
const uint8 LOG_MESSAGE_RECV  = 0x07;  
const uint8 LOG_START_TIMER   = 0x08;  
const uint8 LOG_STOP_TIMER    = 0x09;  
const uint8 LOG_USER_DATA     = 0x0a;
const uint8 LOG_STRING        = 0x0b;
const uint8 LOG_BUFFER        = 0x0c;

//log parameters codes
const uint8 LOG_PARAM_STATE       = 0x01;
const uint8 LOG_PARAM_CALL_ID     = 0x02;
const uint8 LOG_PARAM_MSG_CODE    = 0x03;
const uint8 LOG_PARAM_MESSAGE     = 0x04;
const uint8 LOG_PARAM_TIMER_ID    = 0x05;
const uint8 LOG_PARAM_STRING      = 0x06;
const uint8 LOG_PARAM_USER_DATA   = 0x07;
const uint8 LOG_PARAM_TIME        = 0x08;
const uint8 LOG_PARAM_COMPUTER_ID = 0x09;
const uint8 LOG_PARAM_AUTOMATE    = 0x0a;
const uint8 LOG_PARAM_OBJECT_ID   = 0x0b;


class LogInterface  {
  private:
    uint8  ComputerId;

  protected:
    uint8  LogBuffer[8192];
    uint16 ParamOffset;
	  uint16 *offset;

  public:
    LogInterface();
    LogInterface(char *initString);
    ~LogInterface();

    virtual void Write();
    void AddParam(uint8 code, uint8 lentgh, uint8 *data);
    void AddParam(uint16 lentgh, uint8 *data);
    void AddParamByte(uint8 code, uint8 param);
    void AddParamWord(uint8 code, uint16 param);
    void AddParamDWord(uint8 code, uint32 param);
	//void PrepareNewRecord(uint8 code, uint8 automate, uint32 objectId);
    void PrepareNewRecord(uint16 code, uint8 automate, uint32 objectId);
    void SetComputerId(uint8 id){ ComputerId = id;};
	void SetMsgLogCode(uint16 code);
	void SetMsgInfoLength(uint16 len);
	uint16 GetNewMsgInfoLength();
};

class LogAutomateNew : public KernelAPI{
  private:
    static LogInterface *LogingObject;
    static uint16 SysLogFlag;

    uint16 LogFlag;

    void LogPrepareNewRecord(uint8 code, uint8 automate, uint32 objectId);

  protected:
    virtual uint8 GetState() = 0;

    void LogStarted();
    void LogEnded();
    void LogAutomateEventStart(uint16 msgCode);
    void LogAutomateEventEnd();
    void LogMessage(uint16 length, uint8 *msg);
    void LogStartTimer(uint16 id);
    void LogStopTimer(uint16 id);
    void LogSendMessage(uint16 length, uint8 *msg);
    void LogRecvMessage(uint16 length, uint8 *msg);
    void LogString(char* str, ...);
    void LogUserParam(uint8 length, uint8 *param);

  public:
    enum LogFlagBits { 
      BIT_LOG_STARTED = 0x0001, BIT_LOG_ENDED    = 0x0002, BIT_EVENT_START  = 0x0004,
      BIT_EVENT_END   = 0x0008, BIT_LOG_MESSAGE  = 0x0010, BIT_START_TIMER  = 0x0020,
      BIT_STOP_TIMER  = 0x0040, BIT_SEND_MESSAGE = 0x0080, BIT_RECV_MESSAGE = 0x0100,
      BIT_STRING      = 0x0200, BIT_USER_PARAM   = 0x0400, BIT_ALL_STARTED  = 0xffff,
      BIT_ALL_STOPPED = 0x0000
    };

    LogAutomateNew( uint8 automateId1, uint8 mbxId1);
    virtual ~LogAutomateNew();

    static void SetLogInterface(LogInterface *logingObject){LogingObject = logingObject;};
    void   SetLogFlag(uint16 newFlag) {LogFlag = newFlag;};
    void   ClearLogFlag()             {LogFlag = BIT_ALL_STOPPED;};
    void   StartAll()                 {LogFlag = BIT_ALL_STARTED;};
    void   StartLogFunc(uint16 flag)  {LogFlag |= flag;};
    void   StopLogFunc(uint16 flag)   {LogFlag &= ~flag;};
    uint16 GetLogFlag()               {return LogFlag;};

    static void   SysSetLogFlag(uint16 newFlag) {SysLogFlag = newFlag;};
    static void   SysClearLogFlag()             {SysLogFlag = BIT_ALL_STOPPED;};
    static void   SysStartAll()                 {SysLogFlag = BIT_ALL_STARTED;};
    static void   SysStartLogFunc(uint16 flag)  {SysLogFlag |= flag;};
    static void   SysStopLogFunc(uint16 flag)   {SysLogFlag &= ~flag;};
    static uint16 SysGetLogFlag()               {return SysLogFlag;};

    void LogBuffer(uint16 length, uint8 *msg);
};

#endif