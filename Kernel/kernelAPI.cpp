// **************************************************************************
//                        KernelAPI.cpp  -  description
//                           -------------------
//  begin                : Tue Jun 20 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
// **************************************************************************

#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <assert.h>

#include "errorObject.h"
#include "kernelAPI.h"

TPostOffice *KernelAPI::PostOffice = 0;
TBuffers    *KernelAPI::Buffers    = 0;
CTimer      *KernelAPI::Timer      = 0;


// Function: Constructor
// Parameters: 
//  (in) char *fileName - name of file
// Return value: none
// Description: 
//  Initialization of KernelAPI object, All data for its initaliztion are
//  in file 'fileName'. This function is yet to be implemented. It should create all
//  object from enviroment needed for FSM to function.
KernelAPI::KernelAPI(char *){ //char *fileName
//  FileOpen("../data/fsm.cfg)
//  FileGetParam("KERNEL", ")
  TimerResolution = Timer1s;
}

// Function: Constructor
// Parameters: 
//  (in) TPostOffice *postOffice - pointer on message exchange object
//  (in) TBuffers    *buffers    - pointer on memory handling object
//  (in) CTimer      *timer      - pointer on time controls object
// Return value: none
// Description: 
//  Initialization of KernelAPI object, It sets all objects from enviroment.
KernelAPI::KernelAPI(TPostOffice *postOffice, TBuffers *buffers, CTimer *timer){
#ifndef __NO_PARAMETER_CHECK__
  if( (postOffice==0) || (buffers==0) ||(timer==0) )
                           ThrowError(  TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  PostOffice = postOffice;
  Buffers    = buffers;
  Timer      = timer;
  TimerResolution = Timer1s;
}

// Function: SetKernelObjects
// Parameters: 
//  (in) TPostOffice *postOffice - pointer on message exchange object
//  (in) TBuffers    *buffers    - pointer on memory handling object
//  (in) CTimer      *timer      - pointer on time controls object
// Return value: none
// Description: 
//  It sets all objects from enviroment.
void KernelAPI::SetKernelObjects(TPostOffice *postOffice, TBuffers *buffers, CTimer *timer) {
#ifndef __NO_PARAMETER_CHECK__
  if( (postOffice==0) || (buffers==0) ||(timer==0) )
                           ThrowError(  TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  PostOffice = postOffice;
  Buffers    = buffers;
  Timer      = timer;
}

// Function: StartTimer
// Parameters: 
//  (in) uint16  code  - code of timer
//  (in) uint32  count - time of expiration
//  (in) uint8  *info  - pointer on optional data
// Return value: 
//  uint8 * - pointer on timer buffer
// Description: 
//  Starts timer control, this function is made to be layer between FSM and timer
//  object. It takes as parameters code of timer exparation message and length
//  of timer in ticks. It also takes optional parametar on data that is going
//  to be added to timer message. Timer message is coded as default param coding.
uint8 *KernelAPI::StartTimer(uint16 code, uint32 count/*, uint8 *info*/) {
  uint8  *timer;
  uint8 *info = 0;
  uint16 bufferLength = MSG_HEADER_LENGTH;
  if( info !=0 )  bufferLength = (uint16)(bufferLength + GetUint16(info+MSG_LENGTH));
  timer = Buffers->GetBuffer(bufferLength);

//  SetMsgFromObject(0, timer);

  SetMsgToAutomate(GetAutomate(), timer);
  SetMsgToGroup(GetGroup(), timer);

  SetMsgCode(code, timer);
  SetMsgCallId(GetCallId(), timer);
//  SetMsgObjectNumberFrom(0, timer);
  SetMsgObjectNumberTo(GetObjectId(), timer);
  SetMsgInfoCoding(GetDefaultParamCoding(), timer);
  if( info !=0 )  memcpy( timer+MSG_INFO, info+2, GetUint16(info+MSG_LENGTH) );
  SetMsgInfoLength( (uint16)(bufferLength-MSG_HEADER_LENGTH), timer);
  Timer->StartTimer(timer, GetMbxId(), count, TimerResolution);
  return timer;

//  SetTimer(id, Timer->StartTimer(timer, GetMyMailBoxId(), GetTimerCount(id)) );
}

// Function: SetMessageFromData
// Parameters:   none
// Return value: none
// Description: 
//  Sets data about sending object in message header.
void KernelAPI::SetMessageFromData(void) {
  SetMsgFromAutomate(GetAutomate());
  SetMsgFromGroup(GetGroup());
  SetMsgObjectNumberFrom(GetObjectId());
}


//inline void KernelAPI::SendMessageTest(uint8 mbxId/*, bool setMsgFrom*/){
  /*if(setMsgFrom == true){
    SetMessageFromData();
  }*/
//  PostOffice->Add( mbxId, GetNewMessage());
//  SetNewMessage(NULL);
//}

// Function: SendMessageLeft
// Parameters:   none
// Return value: none
// Description: 
//  Sets data about reciving object in message header. It is left object in communication.
void KernelAPI::SendMessageLeft() {
  SetMessageFromData();
  SetMsgToAutomate(GetLeftAutomate());
  SetMsgToGroup(GetLeftGroup());
  SetMsgObjectNumberTo(GetLeftObjectId());
  SetMsgCallId(GetCallId());
  PostOffice->Add( GetLeftMbx(), GetNewMessage());
  SetNewMessage(NULL);
}

// Function: SendMessageRight
// Parameters:   none
// Return value: none
// Description: 
//  Sets data about reciving object in message header. It is right object in communication.
void KernelAPI::SendMessageRight() {
  SetMessageFromData();
  SetMsgToAutomate(GetRightAutomate());
  SetMsgToGroup(GetRightGroup());
  SetMsgObjectNumberTo(GetRightObjectId());
  SetMsgCallId(GetCallId());
  PostOffice->Add( GetRightMbx(), GetNewMessage());
  SetNewMessage(NULL);
}

// Function: ReturnMsg
// Parameters:   none
// Return value: none
// Description: 
//  Sets data about sending and receiving object in message header. Receiving data are
//  taken from current received message. Basicly it sets recepint same as sender of
//  last message, 'new message is returned to sender'.
void KernelAPI::ReturnMsg(uint8 mbxId){
  SetMessageFromData();
  SetMsgToAutomate(GetMsgFromAutomate());
  SetMsgToGroup(GetMsgFromGroup());
  SetMsgObjectNumberTo(GetMsgObjectNumberFrom());
  SetMsgCallId(GetMsgCallId());
  PostOffice->Add( mbxId, GetNewMessage());
  SetNewMessage(NULL);
}

