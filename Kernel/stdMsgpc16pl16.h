// **************************************************************************
//
// class StandardMessage:
// Description:
//  Derived fropm class MessageInterface, handles standard message format.
//  Standard message format (local format) is defined as folow:
//  Message code is in header of message. In data part of message (information part)
//  are only parameters coded as folow: one byte parameter code, one byte parameter length
//  and after that information part of parameter. Message ends with eof parameter, coded as
//  zero.
//  uint32 CurrentOffset - Current offset in message used by some function to 
//                         mark where is current paramter located in message.
//  
// **************************************************************************
//
// DECRIPTION OF inline and abstract FUNCTIONS for class StandardMessage
//
// Function:  SetMsgInfoLength
// Parameters:
//  (in) uint16 length - length of message
// Return value: none
// Description: 
//  Function sets length of message in NewMessage buffer (new created message),
//  used by functions which adds paramters in message.
//
// **************************************************************************
#ifndef standard_message_pc16_pl16__
#define standard_message_pc16_pl16__

//standard message param code 8 bits, prama length 8 bits
#define STD_MSG_PC16_PL16 0x01

#include "msgInterface.h"

class stdMsg_pc16_pl16 : public MessageInterface {
  private:
    uint32 CurrentOffset;
//    uint32 oldMsgCurrentOffset;
    enum{PARAM_LENGTH_OFFSET  = 2, PARAM_HEADER_LENGTH = 4};

    inline uint8 *FindParam(uint32 paramCode, uint32 startOffset = 0);       //performed unther current message
    inline uint8 *FindParamPlace(uint32 paramCode, uint32 startOffset = 0);  //performed unther current new message

  public:
    stdMsg_pc16_pl16();
    ~stdMsg_pc16_pl16();
//performed unther current message
    virtual uint8 *GetParam(uint32 paramCode);
    virtual uint8 *GetParam(uint32 paramCode, uint32 &paramLen);
    virtual bool  GetParamByte(uint32 paramCode,  BYTE  &param);
    virtual bool  GetParamWord(uint32 paramCode,  WORD  &param);
    virtual bool  GetParamDWord(uint32 paramCode, DWORD &param);

    virtual uint8 *GetNextParam(uint32 paramCode);
    virtual uint8 *GetNextParam(uint32 paramCode, uint32 &paramLen);
    virtual bool  GetNextParamByte(uint32 paramCode,  BYTE  &param);
    virtual bool  GetNextParamWord(uint32 paramCode,  WORD  &param);
    virtual bool  GetNextParamDWord(uint32 paramCode, DWORD &param);

//performed unther current new message
    virtual uint8 *AddParam(uint8 *param);
    virtual uint8 *AddParam(uint32 paramCode, uint32 paramLength, uint8 *param);
    virtual uint8 *AddParamByte(uint32 paramCode,  BYTE param);
    virtual uint8 *AddParamWord(uint32 paramCode,  WORD param);
    virtual uint8 *AddParamDWord(uint32 paramCode, DWORD param);
    virtual bool  RemoveParam(uint32 paramCode);

    virtual void SetENDOfParams();
};
#endif

