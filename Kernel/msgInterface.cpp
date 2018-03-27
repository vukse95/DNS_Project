#include <string.h>
#include <assert.h>

#include "errorObject.h"
#include "../kernel/msgInterface.h"

// Function: CopyMessage
// Parameters: 
//  (in) uint8 *newMessage  - Buffer for new message to be stored (copied).
// Return value: none
// Description: 
// Copies current message recived in to new message,
void MessageInterface::CopyMessage(uint8 *newMessage) {
  NewMessage = newMessage;
  assert(NewMessage);
  assert(CurrentMessage);
  memcpy( NewMessage, CurrentMessage, GetMsgInfoLength() + MSG_HEADER_LENGTH);
}

void MessageInterface::CopyMessage(uint8 *newMessage, uint8 *mess) {
  NewMessage = newMessage;
  assert(NewMessage);
  assert(mess);
  memcpy( NewMessage, mess, GetMsgInfoLength(mess) + MSG_HEADER_LENGTH);
}


void MessageInterface::CopyMessageInfo(uint8 infoCoding, uint8 *newMessage) {
  NewMessage = newMessage;
  assert(NewMessage);
  assert(CurrentMessage);
  memcpy( NewMessage+MSG_HEADER_LENGTH, CurrentMessage+MSG_HEADER_LENGTH, GetMsgInfoLength());
  SetMsgInfoLength(GetMsgInfoLength());
}

void MessageInterface::SetBitParamByteBasic(BYTE param, uint32 offset, uint32 mask){
  *(NewMessage+offset) &= ~mask;
  *(NewMessage+offset) |= param;
}

void MessageInterface::SetBitParamWordBasic(WORD param, uint32 offset, uint32 mask){
  uint16 newValue;
  newValue = GetUint16(NewMessage+offset);
  newValue &= ~mask;
  newValue |= param;
  SetUint16(NewMessage+offset, newValue);
}

void MessageInterface::SetBitParamDWordBasic(DWORD param, uint32 offset, uint32 mask){
  uint32 newValue;
  newValue = GetUint32(NewMessage+offset);
  newValue &= ~mask;
  newValue |= param;
  SetUint32(NewMessage+offset, newValue);
}

uint8 MessageInterface::GetBitParamByteBasic(uint32 offset, uint32 mask){
  return (uint8)(*(CurrentMessage+offset) & mask);
}

uint16 MessageInterface::GetBitParamWordBasic(uint32 offset, uint32 mask){
  uint16 newValue;
  newValue = GetUint16(CurrentMessage+offset);
  newValue &= mask;
  return newValue;
}

uint32 MessageInterface::GetBitParamDWordBasic(uint32 offset,  uint32 mask){
  uint32 newValue;
  newValue = GetUint32(CurrentMessage+offset);
  newValue &= mask;
  return newValue;
}


