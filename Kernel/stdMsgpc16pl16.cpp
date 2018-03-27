// **************************************************************************
//                        standardMsg.cpp  -  description
//                           -------------------
//  begin                : Tue Jun 20 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Description:
//  This file contains code for absract class MessageInterface, which is parrent of all
//  message handling classes. It defines interface which all claasses has to have,
//  but can extend it.
//  In this file is also code for stdMsg_pc16_pl16 which defines lokal format of 
//  massages and parameters. Message coded is only in header, while parameters are
//  coded as one byte type, one byte length and after length is parameter. In length
//  of parameter byte for code and byte for length are not included
// **************************************************************************
//
#include <string.h>
#include <assert.h>

#include "errorObject.h"
#include "../kernel/stdMsgpc16pl16.h"


// Function: Constructor
// Parameters:   none
// Return value: none
// Description: 
// Initalizes Standard Message interface
stdMsg_pc16_pl16::stdMsg_pc16_pl16() : MessageInterface() {
  CurrentOffset = 0;
}

stdMsg_pc16_pl16::~stdMsg_pc16_pl16()  {
}

// Function: Constructor
// Parameters:   
//  (in) uint8 paramCode -  code of serched parameter
// Return value: 
//  uint8 * - pointer on wanted parameter, if parameter is not found, returns NULL.
// Description: 
// Serach for given parameter in current message. If parameter is found pointer on
// it is returned, if not, NULL pointer is returned. CurrentOffset paramter is set
// to point on paramter if parameter is found. If paramter is not found CurrentOffset
// is zero.
uint8 *stdMsg_pc16_pl16::FindParam(uint32 paramCode, uint32 startOffset) {
  uint8 *tmp;

  assert(CurrentMessage != 0);
  CurrentOffset = startOffset;
  uint32 length = (uint32)(GetMsgInfoLength() + MSG_HEADER_LENGTH);
  if(length == MSG_HEADER_LENGTH) return 0;
  tmp = CurrentMessage + MSG_INFO+CurrentOffset;
  while( GetUint16(tmp) != 0) {
    CurrentOffset += GetUint16(tmp+PARAM_LENGTH_OFFSET)+ PARAM_HEADER_LENGTH;
    if( GetUint16(tmp) == paramCode) return tmp;
    tmp   += GetUint16(tmp+PARAM_LENGTH_OFFSET)+PARAM_HEADER_LENGTH;
    assert( CurrentOffset<length ); 
  }
  CurrentOffset = 0;
  return NULL;
}

// Function: FindParamPlace
// Parameters:   
//  (in) uint8 paramCode -  code of serched parameter
// Return value: 
//  uint8 * - pointer on wanted parameter, if parameter is not found, returns NULL.
// Description: 
// Serach for place of new parameter in Newmessage. This function is used before adding
// new parameter in message to find place where it should be set. CurrentOffset paramter
// is set to point on paramter if parameter is found. 
uint8 *stdMsg_pc16_pl16::FindParamPlace(uint32 paramCode, uint32 startOffset ) {
  uint8 *tmp;
  assert(NewMessage != 0);
  uint32 length = (uint32)GetNewMsgInfoLength();
  tmp = NewMessage + MSG_INFO;
  CurrentOffset = startOffset;
  if(length == 0) {
    SetENDOfParams();
  }
  while( GetUint16(tmp)) {
    if(GetUint16(tmp) > paramCode) return tmp;
    CurrentOffset += GetUint16(tmp+PARAM_LENGTH_OFFSET)+PARAM_HEADER_LENGTH;
    tmp   += GetUint16(tmp+PARAM_LENGTH_OFFSET)+PARAM_HEADER_LENGTH;
    assert( CurrentOffset<length );
  }
  return tmp;
}

// Function: GetParam
// Parameters:   
//  (in) uint8 paramCode -  code of serched parameter
// Return value: 
//  uint8 * - pointer on wanted parameter, if parameter is not found, returns NULL.
// Description: 
// Returns pointer on paramter if it exits in current message, if the parameter does not
// exits in current message it returns NULL pointer.
uint8 *stdMsg_pc16_pl16::GetParam(uint32 paramCode) {
  return FindParam(paramCode);
}

// Function: GetParam
// Parameters:   
//  (in) uint8 paramCode -  code of serched parameter
// Return value: 
//  uint8 * - pointer on wanted parameter, if parameter is not found, returns NULL.
// Description: 
// Returns pointer on paramter if it exits in current message, if the parameter does not
// exits in current message it returns NULL pointer.
uint8 *stdMsg_pc16_pl16::GetParam(uint32 paramCode, uint32 &paramLen) {
  uint8 *tmp;
  tmp = FindParam(paramCode);
  if(tmp == NULL) {
    paramLen = 0;
  }
  else {
    paramLen = GetUint16(tmp+PARAM_LENGTH_OFFSET);//Ilija
    tmp = tmp+PARAM_HEADER_LENGTH;
  }
  return tmp;
}

// Function: GetParamByte
// Parameters:   
//  (in)  uint8 paramCode -  code of serched parameter
//  (out) BYTE &param     -  refernce on byte through which value of paramter is returned
// Return value: 
//  bool - true if paramter is found, false if not.
// Description: 
// Returns first byte of parameter, usualy used when paramter is only one byte long. 
// Paramter is returned through refernce parameter. Function return is true if parameter
// is found othervise function returns false.
bool stdMsg_pc16_pl16::GetParamByte(uint32 paramCode, BYTE &param) {
  uint8 *tmp = FindParam(paramCode);
  if(tmp == NULL) return false;
  param = *(tmp+PARAM_HEADER_LENGTH); 
  return true;
}

// Function: GetParamWord
// Parameters:   
//  (in)  uint8 paramCode -  code of serched parameter
//  (out) WORD &param     -  refernce on word through which value of paramter is returned
// Return value: 
//  bool - true if paramter is found, false if not.
// Description: 
// Returns first word of paramter, usualy used when paramter is two bytes long
// Paramter is returned through refernce parameter. Function return is true if parameter
// is found othervise function returns false.
bool stdMsg_pc16_pl16::GetParamWord(uint32 paramCode, WORD &param) {
  uint8 *tmp = FindParam(paramCode);
  if(tmp == NULL) return false;
  param = GetUint16(tmp+PARAM_HEADER_LENGTH);
  return true;
}

// Function: GetParamDWord
// Parameters:   
//  (in)  uint8 paramCode -  code of serched parameter
//  (out) WORD &param     -  refernce on dword through which value of paramter is returned
// Return value: 
//  bool - true if paramter is found, false if not.
// Description: 
// Returns first dword of paramter, usualy used when paramter is two bytes long
// Paramter is returned through refernce parameter. Function return is true if parameter
// is found othervise function returns false.
bool stdMsg_pc16_pl16::GetParamDWord(uint32 paramCode, DWORD &param) {
  uint8 *tmp = FindParam(paramCode);
  if(tmp == NULL) return false;
  param = GetUint32(tmp+PARAM_HEADER_LENGTH);
  return true;
}

uint8 *stdMsg_pc16_pl16::GetNextParam(uint32 paramCode) {
  return FindParam(paramCode, CurrentOffset);
}

uint8 *stdMsg_pc16_pl16::GetNextParam(uint32 paramCode, uint32 &paramLen) {
  uint8 *tmp;
  tmp = FindParam(paramCode, CurrentOffset);
  if(tmp == NULL) {
    paramLen = 0;
  }
  else {
    paramLen = *(tmp+PARAM_LENGTH_OFFSET);
    tmp = tmp+PARAM_HEADER_LENGTH;
  }
  return tmp;
}


// Function: GetParamByte
// Parameters:   
//  (in)  uint8 paramCode -  code of serched parameter
//  (out) BYTE &param     -  refernce on byte through which value of paramter is returned
// Return value: 
//  bool - true if paramter is found, false if not.
// Description: 
// Returns first byte of parameter, usualy used when paramter is only one byte long. 
// Paramter is returned through refernce parameter. Function return is true if parameter
// is found othervise function returns false.
bool stdMsg_pc16_pl16::GetNextParamByte(uint32 paramCode, BYTE &param) {
  uint8 *tmp = FindParam(paramCode, CurrentOffset);
  if(tmp == NULL) return false;
  param = *(tmp+PARAM_HEADER_LENGTH); 
  return true;
}

// Function: GetParamWord
// Parameters:   
//  (in)  uint8 paramCode -  code of serched parameter
//  (out) WORD &param     -  refernce on word through which value of paramter is returned
// Return value: 
//  bool - true if paramter is found, false if not.
// Description: 
// Returns first word of paramter, usualy used when paramter is two bytes long
// Paramter is returned through refernce parameter. Function return is true if parameter
// is found othervise function returns false.
bool stdMsg_pc16_pl16::GetNextParamWord(uint32 paramCode, WORD &param) {
  uint8 *tmp = FindParam(paramCode, CurrentOffset);
  if(tmp == NULL) return false;
  param = GetUint16(tmp+PARAM_HEADER_LENGTH);
  return true;
}

// Function: GetParamDWord
// Parameters:   
//  (in)  uint8 paramCode -  code of serched parameter
//  (out) WORD &param     -  refernce on dword through which value of paramter is returned
// Return value: 
//  bool - true if paramter is found, false if not.
// Description: 
// Returns first dword of paramter, usualy used when paramter is two bytes long
// Paramter is returned through refernce parameter. Function return is true if parameter
// is found othervise function returns false.
bool stdMsg_pc16_pl16::GetNextParamDWord(uint32 paramCode, DWORD &param) {
  uint8 *tmp = FindParam(paramCode, CurrentOffset);
  if(tmp == NULL) return false;
  param = GetUint32(tmp+PARAM_HEADER_LENGTH);
  return true;
}

// Function: AddParam
// Parameters:   
//  (in)  uint8 *param - pointer on parameter coded as one byte code of paramtere, one byte
//                       length, then parameter  
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds paramtere in current new message, it adds paramtere in place where it should be, 
// all parameters are sorted in accesiding order. Function returns pointer on message.
uint8 *stdMsg_pc16_pl16::AddParam(uint8 *param) {
  return AddParam(GetUint16(param), GetUint16(param+PARAM_LENGTH_OFFSET), param+PARAM_HEADER_LENGTH);
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
uint8 *stdMsg_pc16_pl16::AddParam(uint32 paramCode, uint32 paramLength, uint8 *param) {
  //find where to put parameter, it sets the CurrentOffset where to put
  //parameter
  //find where to put parameter, it sets the CurrentOffset where to put
  //parameter
  uint8 *tmp = FindParamPlace(paramCode);

  //copy rest of the message where it is going to be
  //tmp += CurrentOffset + MSG_HEADER_LENGTH;
  memmove(tmp+paramLength+PARAM_HEADER_LENGTH, tmp, GetNewMsgInfoLength()-CurrentOffset);
  //copy new parameter in middle
  SetUint16(tmp, (uint16)paramCode);
  SetUint16(tmp+PARAM_LENGTH_OFFSET, (uint16)paramLength);
//  *tmp                       = (uint8) paramCode;
//  *(tmp+PARAM_LENGTH_OFFSET) = (uint8) paramLength;
  memmove(tmp+PARAM_HEADER_LENGTH, param, paramLength);
  SetMsgInfoLength(GetNewMsgInfoLength() + (uint16)paramLength+PARAM_HEADER_LENGTH);
  //if for some reason last paramter is not zero (end of parameters, set it)
//  if(NewMessage[GetNewMsgInfoLength() +MSG_HEADER_LENGTH] != 0) {
//    NewMessage[GetNewMsgInfoLength() + MSG_HEADER_LENGTH] = 0;
//    SetMsgInfoLength((uint16)(GetNewMsgInfoLength() +1));
//  }
  return NewMessage;
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
// on message.
uint8 *stdMsg_pc16_pl16::AddParamByte(uint32 paramCode, BYTE param) {
  return AddParam(paramCode, 1, &param);
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
// on message.
uint8 *stdMsg_pc16_pl16::AddParamWord(uint32 paramCode, WORD param) {
  return AddParam(paramCode, 2, (uint8 *)(&param));
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
// on message.
uint8 *stdMsg_pc16_pl16::AddParamDWord(uint32 paramCode, DWORD param) {
  return AddParam(paramCode, 4, (uint8 *)(&param));
}

// Function: RemoveParam
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
// Return value: 
//  bool - returns true if parameter is removed, returns false if there is no such parameter
//         in message
// Description: 
// Removes parameter from new message. If prameter is found and removed it returns true, if
// there is no such paramter in message it returns false. Paramter End Of Parameters (zero)
// should not be removed. In debug version it is prohibited in release version it is not.
bool stdMsg_pc16_pl16::RemoveParam(uint32 paramCode) {
  assert( NewMessage != NULL);
  assert( paramCode  != 0);
  uint8 *tmp = FindParamPlace(paramCode);
  if(GetUint16(tmp) != paramCode) return false;
  SetMsgInfoLength((uint16)(GetNewMsgInfoLength() -2 - GetUint16(tmp+PARAM_LENGTH_OFFSET)) );
  memmove(tmp, tmp+ GetUint16(tmp+PARAM_LENGTH_OFFSET)+PARAM_HEADER_LENGTH, GetNewMsgInfoLength()-CurrentOffset -PARAM_HEADER_LENGTH - GetUint16(tmp+PARAM_LENGTH_OFFSET));
  return true;
}

void stdMsg_pc16_pl16::SetENDOfParams(){
  SetUint16( NewMessage+GetNewMsgInfoLength() + MSG_HEADER_LENGTH,0);
  SetMsgInfoLength((uint16)(GetNewMsgInfoLength()+PARAM_LENGTH_OFFSET));
}
