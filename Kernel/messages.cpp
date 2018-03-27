// **************************************************************************
//                        messages.cpp  -  description
//                           -------------------
//  begin                : Tue Jun 20 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
// **************************************************************************

#include <assert.h>

#include "messages.h"
#include "errorObject.h"

//MessageInterface *MessageHandler::MessageHandlers[256];


// Function: MessageHandler (Constructor)
// Parameters:   none
// Return value: none
// Description: 
//  Initializes object of MessageHandler class.
MessageHandler::MessageHandler(){
  CurrentMessage = 0;
  NewMessage     = 0;
}

// Function: ParseMessage
// Parameters:   
//  (in) uint8 *mess - pointer on message buffer
// Return value: 
//  bool - true if message is OK, false if not.
// Description: 
//  Sets pointer on new recived message.
//  Checks message to determine weather messge is coded as specified.
//  For exapmple here should be checked if paramters aer good, messge code exists....
bool MessageHandler::ParseMessage(uint8 *msg) {
  assert(msg);
  CurrentMessage = msg;
  GetMessageInterface(GetMsgInfoCoding())->ParseMessage(msg);
  return true;
}


void MessageHandler::PrepareNewMessage(uint8 *msg){
  assert(!NewMessage);
  NewMessage = msg;
  MessageInterface *tmp = GetMessageInterface(GetNewMsgInfoCoding());
  tmp->PrepareNewMessage(msg);
}

// Function: PrepareNewMessage
// Parameters:   
//  (in) uint32 length    - expected length of message
//  (in) uint16 code      - message code]
//  (in) uint8 infoCoding - code of message format
// Return value: none
// Description: 
//  Gets buffer for new mwssage, and sets some data about it (message code, and code
//  of message format). It also calls defualt function for specified message format.
//  
void MessageHandler::PrepareNewMessage(uint32 length, uint16 code, uint8 infoCoding){
  assert(!NewMessage);
  NewMessage = GetBuffer(length+MSG_HEADER_LENGTH); 
//  SetDefaultHeader(infoCoding);
//others data are going to be set when sendfing the message
  SetMsgInfoCoding(infoCoding);
  SetMsgCode(code);
  SetMsgInfoLength(0x0000);
//  SetMsgInfoLength(0x0001);
//  NewMessage[MSG_INFO] = 0x00;
  MessageInterface *tmp = GetMessageInterface(GetNewMsgInfoCoding());
  tmp->PrepareNewMessage(NewMessage);
//  GetMessageInterface(GetNewMsgInfoCoding())->PrepareNewMessage(NewMessage);
}

// Function: CopyMessage
// Parameters:   none 
// Return value: none
// Description: 
//  Gets buffer for new message and copy current recived message in to it. If there was
//  already buffer for new message it is returned, and new one is oibtained.
//  
void MessageHandler::CopyMessage() {
  assert(CurrentMessage);
  if(NewMessage != 0) RetBuffer(NewMessage);
  if( (NewMessage = GetBuffer(GetMsgInfoLength() + MSG_HEADER_LENGTH) )==0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010600));
  GetMessageInterface(GetMsgInfoCoding())->CopyMessage(NewMessage); // no need for assert, it is already
                                                                // checked when Current Message is set
}

void MessageHandler::CopyMessage(uint8 *msg) {
  assert(msg);
  if(NewMessage != 0) RetBuffer(NewMessage);
  if( (NewMessage = GetBuffer(GetMsgInfoLength(msg) + MSG_HEADER_LENGTH) )==0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010600));
  GetMessageInterface(GetMsgInfoCoding())->CopyMessage(NewMessage, msg); // no need for assert, it is already
                                                                // checked when Current Message is set
}

void MessageHandler::CopyMessageInfo( uint8 infoCoding, uint16 lengthCorrection) {
  assert(CurrentMessage);
  if(NewMessage == 0) {
    if( (NewMessage = GetBuffer(GetMsgInfoLength() + MSG_HEADER_LENGTH+lengthCorrection) )==0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010600));
  }
  GetMessageInterface(GetMsgInfoCoding())->CopyMessageInfo( infoCoding, NewMessage); 
  SetMsgInfoCoding(infoCoding);
}

// Function: AddParamByte 
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
//  (in) BYTE  param     - parameter to be added in message
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds parameter one byte long in current new message, it adds paramtere in place where
// it should be, all parameters are sorted in accesiding order. Function returns pointer
// on message. This function must be overided and added check for buffer length. If buffer
// is to small for new parameter to be added, new buffer must be obtained and then called
// this function. This is neccesery because at this moment we are unable to know format
// of paramters and for that reason we can not know size of parameter.
uint8 *MessageHandler::AddParamByte(uint32 paramCode, BYTE param)  {
  CheckBufferSize( sizeof(BYTE));
  return GetMessageInterface(GetNewMsgInfoCoding())->AddParamByte(paramCode, param);
}

// Function: AddParamWord 
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
//  (in) WORD  param     - parameter to be added in message
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds parameter two bytes long in current new message, it adds paramtere in place where
// it should be, all parameters are sorted in accesiding order. Function returns pointer
// on message.  This function must be overided and added check for buffer length. If buffer
// is to small for new parameter to be added, new buffer must be obtained and then called
// this function. This is neccesery because at this moment we are unable to know format
// of paramters and for that reason we can not know size of parameter.
uint8 *MessageHandler::AddParamWord(uint32 paramCode, WORD param)  {
  CheckBufferSize( sizeof(WORD));
  return GetMessageInterface(GetNewMsgInfoCoding())->AddParamWord(paramCode, param);
}

// Function: AddParamDWord 
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
//  (in) DWORD  param    - parameter to be added in message
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds parameter four bytes long in current new message, it adds paramtere in place where
// it should be, all parameters are sorted in accesiding order. Function returns pointer
// on message. This function must be overided and added check for buffer length. If buffer
// is to small for new parameter to be added, new buffer must be obtained and then called
// this function. This is neccesery because at this moment we are unable to know format
// of paramters and for that reason we can not know size of parameter.
uint8 *MessageHandler::AddParamDWord(uint32 paramCode, DWORD param){
  CheckBufferSize( sizeof(DWORD));
  return GetMessageInterface(GetNewMsgInfoCoding())->AddParamDWord(paramCode, param);
}

// Function: AddParam
// Parameters:   
//  (in) uint8 paramCode   - code of paramere to be added
//  (in) uint8 paramLength - length of parameter to be added
//  (in) uint8 *param      - pointer on parameter 
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds paramtere in current new message, it adds paramtere in place where it should be, 
// all parameters are sorted in accesiding order. Function returns pointer on message.
// This function must be overided and added check for buffer length. If buffer
// is to small for new parameter to be added, new buffer must be obtained and then called
// this function. This is neccesery because at this moment we are unable to know format
// of paramters and for that reason we can not know size of parameter.
uint8 *MessageHandler::AddParam(uint32 paramCode, uint32 paramLength, uint8 *param) {
  CheckBufferSize( paramLength);
  return GetMessageInterface(GetNewMsgInfoCoding())->AddParam(paramCode, paramLength, param);
}

// Function: CheckBufferSize
// Parameters:   
//  (in) uint8 paramLength - length of parameter to be added
// Return value: none
// Description: 
// Checks if buffer is large enough for param to be added. Ig buffer is not large enough,
// gets newbuffer and copies message in it.
void MessageHandler::CheckBufferSize(uint32 paramLength) {
  if(IsBufferSmall( NewMessage, GetNewMsgInfoLength() + MSG_HEADER_LENGTH+ paramLength+8) == true){
  //10 in case some more parameters are added
    uint8 *tmp1 = GetBuffer(GetNewMsgInfoLength() + MSG_HEADER_LENGTH+ paramLength + 10);
    memcpy( tmp1, NewMessage, GetNewMsgInfoLength() + MSG_HEADER_LENGTH);
    RetBuffer(NewMessage);
    NewMessage = tmp1;
    GetMessageInterface(GetNewMsgInfoCoding())->SetNewMessage(NewMessage);
  }
}

// Function: MessageHandler (destructor)
// Parameters:   none 
// Return value: none
// Description: 
//  returns buffer for new message if there is one.
//  
MessageHandler::~MessageHandler() {
  assert(!CurrentMessage);
  assert(!NewMessage);
  if( NewMessage != 0 ) RetBuffer(NewMessage);
}

