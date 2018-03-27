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
#ifdef WIN32
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "logAutomate.h"
#include "errorObject.h"

LogInterface *LogAutomateNew::LogingObject = 0;
uint16 LogAutomateNew::SysLogFlag = BIT_ALL_STARTED;

//void   LogAutomateNew::SysSetLogFlag(uint16 newFlag);
//void   LogAutomateNew::SysClearLogFlag();
//void   LogAutomateNew::SysStartAll();
//void   LogAutomateNew::SysStartLogFunc(uint16 flag);
//void   LogAutomateNew::SysStopLogFunc(uint16 flag);
//uint16 LogAutomateNew::SysGetLogFlag();


LogAutomateNew::LogAutomateNew( uint8 automateId1, uint8 mbxId1) : KernelAPI( automateId1, mbxId1){
  LogingObject = 0;
  LogFlag      = BIT_ALL_STARTED;
}

void LogAutomateNew::LogStarted(){
  if(LogFlag & BIT_LOG_STARTED & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_START, GetAutomate(), GetObjectId());
    LogingObject->Write();
  }
}

void LogAutomateNew::LogEnded(){
  if(LogFlag & BIT_LOG_ENDED & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_END, GetAutomate(), GetObjectId());
    LogingObject->Write();
  }
}

void LogAutomateNew::LogAutomateEventStart(uint16 msgCode){
  if(LogFlag & BIT_EVENT_START & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_EVENT_START, GetAutomate(), GetObjectId());
    LogingObject->AddParamByte(LOG_PARAM_STATE, GetState());
    LogingObject->AddParamDWord(LOG_PARAM_CALL_ID,  GetCallId());
//  LogingObject->AddParamWord(LOG_PARAM_MSG_CODE, msgCode);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogAutomateEventEnd(){
  if(LogFlag & BIT_EVENT_END & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_EVENT_END, GetAutomate(), GetObjectId());
    LogingObject->AddParamByte(LOG_PARAM_STATE, GetState());
    LogingObject->AddParamDWord(LOG_PARAM_CALL_ID,  GetCallId());
    LogingObject->Write();
  }
}

void LogAutomateNew::LogMessage(uint16 length, uint8 *msg){
  if(LogFlag & BIT_LOG_MESSAGE & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_EVENT_MESSAGE, GetAutomate(), GetObjectId());
    LogingObject->AddParam(LOG_PARAM_MESSAGE, (uint8 )length, msg);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogStartTimer(uint16 id){
  if(LogFlag & BIT_START_TIMER & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_START_TIMER, GetAutomate(), GetObjectId());
    LogingObject->AddParamWord(LOG_PARAM_TIMER_ID, id);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogStopTimer(uint16 id){
  if(LogFlag & BIT_STOP_TIMER & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_STOP_TIMER, GetAutomate(), GetObjectId());
    LogingObject->AddParamWord(LOG_PARAM_TIMER_ID, id);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogSendMessage(uint16 length, uint8 *msg){
  if(LogFlag & BIT_SEND_MESSAGE & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_MESSAGE_SENT, GetAutomate(), GetObjectId());
    LogingObject->AddParam(LOG_PARAM_MESSAGE, (uint8 )length, msg);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogRecvMessage(uint16 length, uint8 *msg){
  if(LogFlag & BIT_RECV_MESSAGE & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_MESSAGE_RECV, GetAutomate(), GetObjectId());
    LogingObject->AddParam(LOG_PARAM_MESSAGE, (uint8 )length, msg);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogBuffer(uint16 length, uint8 *msg){
  if(LogFlag & BIT_SEND_MESSAGE & SysLogFlag) {
    LogingObject->PrepareNewRecord(LOG_BUFFER, 0xff, 0xffff);
    LogingObject->AddParam(LOG_PARAM_MESSAGE, (uint8 )length, msg);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogString(char* str, ...){
  char stringBuff[128];
  int length;

  if(LogFlag & BIT_STRING) {
    va_list argptr;
 	  va_start(argptr, str);
#ifdef WIN32
	  length = _vsnprintf( stringBuff, 127, str, argptr);
#else
  	length = vsnprintf( stringBuff, 127, str, argptr);
#endif
	  va_end(argptr);
    if(length < 0) return;

    LogingObject->PrepareNewRecord(LOG_STRING, GetAutomate(), GetObjectId());
    LogingObject->AddParam(LOG_PARAM_STRING, (uint8)length, (uint8 *)stringBuff);
    LogingObject->Write();
  }
}

void LogAutomateNew::LogUserParam(uint8 length, uint8 *param){
  if(LogFlag & BIT_USER_PARAM) {
    LogingObject->PrepareNewRecord(LOG_USER_DATA, GetAutomate(), GetObjectId());
    LogingObject->AddParam(LOG_PARAM_USER_DATA, length, param);
    LogingObject->Write();
  }
}

LogAutomateNew::~LogAutomateNew(){
}

LogInterface::LogInterface(){
  ComputerId  = INVALID_08;
  ParamOffset = 0;
}

LogInterface::LogInterface(char *initString){
  ComputerId  = INVALID_08;
  ParamOffset = 0;
}


void LogInterface::Write(){
}

void LogInterface::AddParam(uint8 code, uint8 length, uint8 *data){
  assert(length!=0);
  LogBuffer[ParamOffset++] = code;
  LogBuffer[ParamOffset++] = length;
  memcpy(LogBuffer+ParamOffset, data, length);
  ParamOffset += length;
  SetMsgInfoLength(GetNewMsgInfoLength() + length +2);
}

void LogInterface::AddParam(uint16 length, uint8 *data){
  assert(length > 3);
  memcpy(LogBuffer+ParamOffset, data, length);
  SetMsgInfoLength(GetNewMsgInfoLength() + length+2);
  ParamOffset += length;
}

void LogInterface::AddParamByte(uint8 code, uint8 param){
  LogBuffer[ParamOffset++] = code;
  LogBuffer[ParamOffset++] = 1;
  LogBuffer[ParamOffset++] = param;
  SetMsgInfoLength(GetNewMsgInfoLength() + 3);
}

void LogInterface::AddParamWord(uint8 code, uint16 param){
  LogBuffer[ParamOffset++] = code;
  LogBuffer[ParamOffset++] = 2;
  SetUint16(LogBuffer+ParamOffset, param);
  SetMsgInfoLength(GetNewMsgInfoLength() + 4);
  ParamOffset += 2;
}

void LogInterface::AddParamDWord(uint8 code, uint32 param){
  LogBuffer[ParamOffset++] = code;
  LogBuffer[ParamOffset++] = 4;
  SetUint32(LogBuffer+ParamOffset, param);
  SetMsgInfoLength(GetNewMsgInfoLength() + 6);
  ParamOffset += 4;
}

uint16 LogInterface::GetNewMsgInfoLength() {
	return GetUint16(LogBuffer+2);
}

void LogInterface::SetMsgInfoLength(uint16 len){
	SetUint16(LogBuffer+2, len);
}

void LogInterface::SetMsgLogCode(uint16 code){
	SetUint16(LogBuffer, code);
}
void LogInterface::PrepareNewRecord(uint16 code, uint8 automate, uint32 objectId){
  time_t ltime;
  SetMsgLogCode(code);
  //LogBuffer[0] = code;
  ParamOffset  = 4;
  //LogBuffer[2] = ParamOffset;  
  SetMsgInfoLength(4);
  time(&ltime); 
  AddParamDWord(LOG_PARAM_TIME, ltime);
  AddParamByte(LOG_PARAM_COMPUTER_ID, ComputerId);
  AddParamByte(LOG_PARAM_AUTOMATE, automate);
  AddParamDWord(LOG_PARAM_OBJECT_ID, objectId);
}

LogInterface::~LogInterface(){
  ComputerId = INVALID_08;
}



