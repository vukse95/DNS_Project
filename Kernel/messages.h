// **************************************************************************
//                        messages.h  -  description
//                           -------------------
//  begin                : Tue Jun 20 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  MessageHandler is 
//  class for handling of messages. It provides functionality of parameter adding,
//  removing, finding, message creation. It knows how to handle different kind of
//  parameter coding.
//    uint8 *CurrentMessage - pointer on current received message, all function
//                            that takes informations
//                            from message are default with under this message.
//    uint8 *NewMessage     - pointer on current message to send, all function
//                            that puts (adds) parametars in 
//                            message are default done with this pointer.
//
// **************************************************************************
//
// DECRIPTION OF inline and abstract FUNCTIONS for class MessageHandler
//
// Function:  GetMsgInfoLength
// Parameters: none
// Return value:
//  uint16 - current received message length, only data in info part,
//           length without header
// Description: 
//  Function takes length from current received message (CurrentMessage) and returns it.
//  This function is protected and inline.
//
// Function:  GetMessageInterface (abstract)
// Parameters: 
//  (in) uint32 id - code of message interface (local, ISUP, DSS1 ...)
// Return value:
//  MessageInterface * - pointer on class MessageInterface which can handle 
//                       messages coded in specified format.
// Description: 
//  Function defines interface for providening objects that can handle messages coded
//  in specified formats. SInce in each application there is only a couple of
//  formats that can be handled, and per automat usualy no more than two, it is
//  left to programer to specifie which formats it is going to handle.
//  In case of format that automate can not handle, function should throw an 
//  exception.
//
// Function:   ClearMessage
// Parameters:   none
// Return value: none
// Description: 
//  Returns buffer of current message and sets ponter to it (CurrentMessage)
//  on NULL value. In debug version it also checks weather NewMessage is NULL.
//  if it is not NULL it means that message is not sent.
//
// Function:   SetDefaultHeader (abstract)
// Parameters:   
//  (in) uint8 infoCoding - code of default message format.
// Return value: none
// Description: 
//  Function sets defualt data for message header, This function is called 
//  each time new message is created.
//
// Function:   GetNewMessage
// Parameters:   
// Return value:
//  uint8 * - returns pointer on NewMessage buffer.
// Description: 
//  Function sets defualt data for message header, This function is called 
//  each time new message is created.
//
// Function:   SetNewMessage
// Parameters:
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function returns NewMessage pointer
//
// Function:   GetBuffer (abstract)
// Parameters:
//  (in) uint32 length - length of buffer requested
// Return value:
//  uint8 * - pointer on requested buffer
// Description: 
//  Function returns pointer on buffer. Size of buffer is at least of size length
//  in case there is no free buffers, memory handling object should throw an
//  exception.
//
// Function:   RetBuffer (must be overloaded)
// Parameters:
//  (in) uint8 * - pointer on buffer to be freed
// Return value: none
// Description: 
//  Function returns buffer in pool of free buffers. In case NULL pointer is
//  given as paramter, memory handling object should throw an exception.
//
// Function:   IsBufferSmall (abstract)
// Parameters:
//  (in) uint8  *buff  - pointer on buffer to be checked
//  (in) uint32 length - size of buffer for comparation
// Return value: 
//  bool - returns true if buffer is larger than lengrth othervise false
// Description: 
//  Compare size of buffer with length. If buffer is larger theb length returns
//  false, othervice returns true. Usualy used to check weather new parameter can be
//  added in message.
//
// Function: GetParam 
// Parameters:   
//  (in) uint8 paramCode -  code of serched parameter
// Return value: 
//  uint8 * - pointer on wanted parameter, if parameter is not found, returns NULL.
// Description: 
// Returns pointer on paramter if it exits in current message, if the parameter does not
// exits in current message it returns NULL pointer.
//
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
//
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
//    
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
//
// Function: AddParam
// Parameters:   
//  (in) uint8 *param - pointer on parameter 
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds paramtere in current new message, it adds paramtere in place where it should be, 
// all parameters are sorted in accesiding order. Function returns pointer on message.
// This function must be overided and added check for buffer length. If buffer
// is to small for new parameter to be added, new buffer must be obtained and then called
// this function. This is neccesery because at this moment we are unable to know format
// of paramters and for that reason we can not know size of parameter.
//
// Function: RemoveParam 
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
// Return value: 
//  bool - returns true if parameter is removed, returns false if there is no such parameter
//         in message
// Description: 
// Removes parameter from new message. If prameter is found and removed it returns true, if
// there is no such paramter in message it returns false.
//
// Function: GetMsgFromAutomate 
// Parameters:  none
// Return value: 
//  uint8 - code of automate that sended message
// Description: 
//  Function returns code of automate that sended message from message header.
//  Parametar is extracted from current received message.
//
// Function: GetMsgFromGroup 
// Parameters:  none
// Return value: 
//  uint8 - code of Group that sended message
// Description: 
//  Function returns code of Group that sended message from message header.
//  Parametar is extracted from current received message.
//
// Function: GetMsgToAutomate 
// Parameters:  none
// Return value: 
//  uint8 - code of automate that should receive message
// Description: 
//  Function returns code of automate that should receive message from message header.
//  Parametar is extracted from current received message.
//
// Function: GetMsgToGroup 
// Parameters:  none
// Return value: 
//  uint8 - code of Group that should receive message
// Description: 
//  Function returns code of Group that should receive message from messge header.
//  Parametar is extracted from current received message.
//
// Function: GetMsgCode 
// Parameters:  none
// Return value: 
//  uint16 - code of message
// Description: 
//  Function returns code message from message header.
//  Parametar is extracted from current received message.
//
// Function: GetMsgObjectNumberFrom 
// Parameters:  none
// Return value: 
//  uint32 - ID of object that sended message.
// Description: 
//  Function returns unique ID of object that sended message from message header.
//  This is objects INDEX in array of objects of its type.
//  Parametar is extracted from current received message.
//
// Function: GetMsgObjectNumberTo 
// Parameters:  none
// Return value: 
//  uint32 - ID of object that should receive message.
// Description: 
//  Function returns unique ID of object that should receive message from
//  message header.This is objects INDEX in array of objects of its type.
//  Parametar is extracted from current received message.
//
// Function: GetMsgCallId 
// Parameters:  none
// Return value: 
//  uint32 - call id
// Description: 
//  Function returns unique call ID. This parameter is same for all messages
//  regarding procesing of one call.
//  Parametar is extracted from current received message.
//
// Function: GetMsgInfoCoding 
// Parameters:  none
// Return value: 
//  uint8 - code of message foramt
// Description: 
//  Function returns code of message format. THis parameter is used to
//  determine how message is coded. 
//  Parametar is extracted from current received message.
//
// Function: GetMsgInfoLength 
// Parameters:  none
// Return value: 
//  uint16 - message length
// Description: 
//  Function returns length of message data. In this length is not included
//  header.
//  Parametar is extracted from current received message.
//
// Function: GetNewMsgInfoLength 
// Parameters:  none
// Return value: 
//  uint16 - message length
// Description: 
//  Function returns length of message data. In this length is not included
//  header.
//  Parametar is extracted from new message created, message that is going to be sent.
//
// Function: SetMsgFromAutomate 
// Parameters: 
//  (in) uint8 from - code of automate that sends message
// Return value: none
// Description: 
//  Function sets information about which autmate sended message.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgFromGroup 
// Parameters: 
//  (in) uint8 from - code of Group that sends message
// Return value: none
// Description: 
//  Function sets information about which Group type (ISUP, MTP, CC ...)
//  sended message.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgToAutomate 
// Parameters: 
//  (in) uint8 to - code of automate that recives message
// Return value: none
// Description: 
//  Function sets information about which autmate recives message.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgToGroup 
// Parameters: 
//  (in) uint8 to - code of Group that recives message
// Return value: none
// Description: 
//  Function sets information about which Group type (ISUP, MTP, CC ...)
//  recives message.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgCode 
// Parameters: 
//  (in) uint16 code - code of message
// Return value: none
// Description: 
//  Function sets code of message to be sent.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgObjectNumberFrom 
// Parameters: 
//  (in) uint32 from - id of object that sends message
// Return value: none
// Description: 
//  Function sets information about which object sended message.
//  This is inforamtion that uniquely identifies object that sended messgae.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgObjectNumberTo 
// Parameters: 
//  (in) uint32 to - id of object that recives message
// Return value: none
// Description: 
//  Function sets information about which object recives message.
//  This is inforamtion that uniquely identifies object that recives messgae.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgCallId 
// Parameters: 
//  (in) uint32 id - call id
// Return value: none
// Description: 
//  Function sets unique call ID. This parameter is same for all messages
//  regarding procesing of one call.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgInfoCoding 
// Parameters: 
//  (in) uint8 codingType - code of message format
// Return value: none
// Description: 
//  Function sets code of message format. Parameter needed to properly 
//  decode message.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgInfoLength 
// Parameters: 
//  (in) uint16 length - message length
// Return value: none
// Description: 
//  Function sets message length. In message length, size of header is not
//  included.
//  Parametar is put in new message, message that is going to be sent.
//
// Function: SetMsgFromAutomate 
// Parameters: 
//  (in) uint8 from - code of automate that sends message
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets information about which autmate sended message.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgFromGroup 
// Parameters: 
//  (in) uint8 from - code of Group that sends message
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets information about which Group type (ISUP, MTP, CC ...)
//  sended message.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgToAutomate 
// Parameters: 
//  (in) uint8 to - code of automate that recives message
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets information about which autmate recives message.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgToGroup 
// Parameters: 
//  (in) uint8 to - code of Group that recives message
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets information about which object type (ISUP, MTP, CC ...)
//  recives message.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgCode 
// Parameters: 
//  (in) uint16 code - code of message
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets code of message to be sent.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgObjectNumberFrom 
// Parameters: 
//  (in) uint32 from - id of object that sends message
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets information about which object sended message.
//  This is inforamtion that uniquely identifies object that sended messgae.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgObjectNumberTo 
// Parameters: 
//  (in) uint32 to - id of object that recives message
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets information about which object recives message.
//  This is inforamtion that uniquely identifies object that recives messgae.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgCallId 
// Parameters: 
//  (in) uint32 id - call id
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets unique call ID. This parameter is same for all messages
//  regarding procesing of one call.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgInfoCoding 
// Parameters: 
//  (in) uint8 codingType - code of message format
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets code of message format. Parameter needed to properly 
//  decode message.
//  Parametar is put in message buffer provided as parameter.
//
// Function: SetMsgInfoLength 
// Parameters: 
//  (in) uint16 length - message length
//  (in) uint8 *msg - pointer on message buffer
// Return value: none
// Description: 
//  Function sets message length. In message length, size of header is not
//  included.
//  Parametar is put in new message, message that is going to be sent.
//  Parametar is put in message buffer provided as parameter.
//
// **************************************************************************

#ifndef __MESSAGES__
#define __MESSAGES__

#include <string.h>
#include <assert.h>

#ifndef __KERNEL_TYPES__
#include "../kernel/kernelTypes.h"
#endif

#ifndef __KERNEL_CONSTS__
#include "../kernel/kernelConsts.h"
#endif

#ifndef __STANDARD_MESSAGE__
#include "../kernel/standardMsg.h"
#endif

#ifndef __ERROR_OBJECT__
#include "../kernel/errorObject.h"
#endif

//Types of parameters Coding
const uint8 LOCAL_PARAM_CODING =0x00; //DSS1 like, every message ends with zero

class MessageHandler {
	friend class TALE;
	friend class TAL_Disp;
  private:
    uint8 *CurrentMessage;
    uint8 *NewMessage;
//    static MessageInterface *MessageHandlers[256];

  protected:
    virtual MessageInterface *GetMessageInterface(uint32 id)=0;

    virtual bool ParseMessage(uint8 *msg);
    virtual void PrepareNewMessage(uint8 *msg);
    virtual void PrepareNewMessage(uint32 length, uint16 code, uint8 infoCode=LOCAL_PARAM_CODING);
    virtual void CopyMessage();
    virtual void CopyMessage(uint8 *msg);
    virtual void CopyMessageInfo( uint8 infoCoding, uint16 lengthCorrection = 0);
    virtual void ClearMessage() {RetBuffer(CurrentMessage);CurrentMessage=0;/*assert(!NewMessage);*/};
    virtual void SetDefaultHeader(uint8 infoCoding) = 0;

    inline uint8 *GetNewMessage()          {return NewMessage;};
    inline uint8 *GetCurrentMessage()      {return CurrentMessage;};
    inline void  SetNewMessage(uint8 *msg) {NewMessage=msg;};

    virtual uint8 *GetBuffer(uint32 length) =0;
    virtual void  RetBuffer(uint8 *)        {ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010500));};
    virtual bool  IsBufferSmall(uint8 *buff, uint32 length)=0;

    
//performed unther current message
    uint8 *GetParam(uint32 paramCode)                  {return GetMessageInterface(GetMsgInfoCoding())->GetParam(paramCode);};
    uint8 *GetParam(uint32 paramCode, uint32 &paramLen){return GetMessageInterface(GetMsgInfoCoding())->GetParam(paramCode, paramLen);};
    bool  GetParamByte(uint32 paramCode, BYTE   &param){return GetMessageInterface(GetMsgInfoCoding())->GetParamByte(paramCode,  param);};
    bool  GetParamWord(uint32 paramCode, WORD   &param){return GetMessageInterface(GetMsgInfoCoding())->GetParamWord(paramCode,  param);};
    bool  GetParamDWord(uint32 paramCode, DWORD &param){return GetMessageInterface(GetMsgInfoCoding())->GetParamDWord(paramCode, param);};

    uint8 *GetNextParam(uint32 paramCode)                  {return GetMessageInterface(GetMsgInfoCoding())->GetNextParam(paramCode);};
    uint8 *GetNextParam(uint32 paramCode, uint32 &paramLen){return GetMessageInterface(GetMsgInfoCoding())->GetNextParam(paramCode, paramLen);};
    bool  GetNextParamByte(uint32 paramCode, BYTE   &param){return GetMessageInterface(GetMsgInfoCoding())->GetNextParamByte(paramCode,  param);};
    bool  GetNextParamWord(uint32 paramCode, WORD   &param){return GetMessageInterface(GetMsgInfoCoding())->GetNextParamWord(paramCode,  param);};
    bool  GetNextParamDWord(uint32 paramCode, DWORD &param){return GetMessageInterface(GetMsgInfoCoding())->GetNextParamDWord(paramCode, param);};

    bool  GetBitParamByte(uint32 paramCode,  BYTE  &param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0)
      {return GetMessageInterface(GetMsgInfoCoding())->GetBitParamByte(paramCode, param, mask, shiftBits);};
    bool  GetBitParamWord(uint32 paramCode,  WORD  &param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0)
      {return GetMessageInterface(GetMsgInfoCoding())->GetBitParamWord(paramCode, param, mask, shiftBits);};
    bool  GetBitParamDWord(uint32 paramCode, DWORD &param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0)
      {return GetMessageInterface(GetMsgInfoCoding())->GetBitParamDWord(paramCode, param, mask, shiftBits);};

    uint8 *SetBitParamByte(uint32 paramCode,  BYTE param,  uint32 mask=MASK_32_BIT, uint8 shiftBits=0)
      {return GetMessageInterface(GetMsgInfoCoding())->SetBitParamByte(paramCode, param, mask, shiftBits);};
    uint8 *SetBitParamWord(uint32 paramCode,  WORD param,  uint32 mask=MASK_32_BIT, uint8 shiftBits=0)
      {return GetMessageInterface(GetMsgInfoCoding())->SetBitParamWord(paramCode, param, mask, shiftBits);};
    uint8 *SetBitParamDWord(uint32 paramCode, DWORD param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0)
      {return GetMessageInterface(GetMsgInfoCoding())->SetBitParamDWord(paramCode, param, mask, shiftBits);};


//performed unther current new message
//    uint8 *AddParam(uint8 *param)                {return GetMessageInterface(GetNewMsgInfoCoding())->AddParam(param);};
    uint8 *AddParam(uint32 paramCode, uint32 paramLength, uint8 *param);
    uint8 *AddParamByte(uint32 paramCode, BYTE param);  //{return GetMessageInterface(GetNewMsgInfoCoding())->AddParamByte(paramCode, param);};
    uint8 *AddParamWord(uint32 paramCode, WORD param);  //{return GetMessageInterface(GetNewMsgInfoCoding())->AddParamWord(paramCode, param);};
    uint8 *AddParamDWord(uint32 paramCode, DWORD param);//{return GetMessageInterface(GetNewMsgInfoCoding())->AddParamDWord(paramCode, param);};
    bool   RemoveParam(uint32 paramCode)          {return GetMessageInterface(GetNewMsgInfoCoding())->RemoveParam(paramCode);};

    virtual void CheckBufferSize(uint32 paramLength);

//performed unther current message
    inline uint8  GetMsgFromAutomate()    {return CurrentMessage[MSG_FROM_AUTOMATE];};
    inline uint8  GetMsgFromGroup()       {return CurrentMessage[MSG_FROM_GROUP];};
    inline uint8  GetMsgToAutomate()      {return CurrentMessage[MSG_TO_AUTOMATE];};
    inline uint8  GetMsgToGroup()         {return CurrentMessage[MSG_TO_GROUP];};
    inline uint16 GetMsgCode()            {return GetUint16(CurrentMessage+MSG_CODE);};
    inline uint32 GetMsgObjectNumberFrom(){return GetUint32(CurrentMessage+MSG_OBJECT_ID_FROM);};
    inline uint32 GetMsgObjectNumberTo()  {return GetUint32(CurrentMessage+MSG_OBJECT_ID_TO);};
    inline uint32 GetMsgCallId()          {return GetUint32(CurrentMessage+CALL_ID);};
    inline uint8  GetMsgInfoCoding()      {return CurrentMessage[MSG_INFO_CODING];};
    inline uint16 GetMsgInfoLength()      {return GetUint16(CurrentMessage+MSG_LENGTH);};

    //performed unther current new message
    inline uint16 GetNewMsgInfoLength(){return GetUint16(NewMessage+MSG_LENGTH);};
    inline uint8  GetNewMsgInfoCoding(){return NewMessage[MSG_INFO_CODING];};

    inline void SetMsgFromAutomate(uint8 from) {NewMessage[MSG_FROM_AUTOMATE] =from;};
    inline void SetMsgFromGroup(uint8 from )   {NewMessage[MSG_FROM_GROUP]    =from;};
    inline void SetMsgToAutomate(uint8 to)     {NewMessage[MSG_TO_AUTOMATE]   =to;};
    inline void SetMsgToGroup(uint8 to)        {NewMessage[MSG_TO_GROUP]      =to;};
    inline void SetMsgCode(uint16 code)            {SetUint16(NewMessage+MSG_CODE,code);};
    inline void SetMsgObjectNumberFrom(uint32 from){SetUint32(NewMessage+MSG_OBJECT_ID_FROM,from);};
    inline void SetMsgObjectNumberTo(uint32 to)    {SetUint32(NewMessage+MSG_OBJECT_ID_TO,to);};
    inline void SetMsgCallId(uint32 id)            {SetUint32(NewMessage+CALL_ID, id);};
    inline void SetMsgInfoCoding(uint8 codingType) {NewMessage[MSG_INFO_CODING] = codingType;};
    inline void SetMsgInfoLength(uint16 length)    {SetUint16(NewMessage+MSG_LENGTH, length);};

//performed unther specified buffer
    inline uint16 GetMsgInfoLength(uint8 *msg) {return GetUint16(msg+MSG_LENGTH);};

    inline void SetMsgFromAutomate(uint8 from, uint8 *msg){msg[MSG_FROM_AUTOMATE] =from;};
    inline void SetMsgFromGroup(uint8 from, uint8 *msg)   {msg[MSG_FROM_GROUP]    =from;};
    inline void SetMsgToAutomate(uint8 to, uint8 *msg)    {msg[MSG_TO_AUTOMATE]   =to;};
    inline void SetMsgToGroup(uint8 to, uint8 *msg)       {msg[MSG_TO_GROUP]      =to;};
    inline void SetMsgCode(uint16 code, uint8 *msg)            {SetUint16(msg+MSG_CODE,code);};
    inline void SetMsgObjectNumberFrom(uint32 from, uint8 *msg){SetUint32(msg+MSG_OBJECT_ID_FROM,from);};
    inline void SetMsgObjectNumberTo(uint32 to, uint8 *msg)    {SetUint32(msg+MSG_OBJECT_ID_TO, to);};
    inline void SetMsgCallId(uint32 id, uint8 *msg)            {SetUint32(msg+CALL_ID, id);};
    inline void SetMsgInfoCoding(uint8 codingType, uint8 *msg) {msg[MSG_INFO_CODING] = codingType;};
    inline void SetMsgInfoLength(uint16 length, uint8 *msg)    {SetUint16(msg+MSG_LENGTH, length);};
  public:
    MessageHandler();
    virtual ~MessageHandler();
};


#endif 

