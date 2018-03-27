// **************************************************************************
//                        standardMsg.h  -  description
//                           -------------------
//  begin                : Tue Jun 20 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  Abstarct class that makes interface for message handling. Each new message
//  format is going to be handled by new class that is derived from this one
//  also here is defined class for handling internal message format.
//
//  MessageInterface Abstract class for messaqge handling. It provides standard
//  interface for FSM. This interface every message handling class has to support
//  Each new class can also extend interface with new functions.
//    uint8 *CurrentMessage - pointer on current message received. From this message
//                            are returned all requested parameters
//    uint8 *NewMessage     - new message created. To this message are aded all
//                            parameters
//  
// **************************************************************************
//
// DECRIPTION OF inline and abstract FUNCTIONS for class MessageInterface
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
// Function:  GetNewMsgInfoLength
// Parameters: none
// Return value:
//  uint16 - current received message length, only data in info part,
//           length without header
// Description: 
//  Function takes length from current message to be sent, created message, (NewMessage)
//  and returns it. This function is protected and inline.
//
// Function:   MessageInterface (Constructor)
// Parameters:   none
// Return value: none
// Description: 
//  Initalizes MessageInterface (message handling) data..
//
// Function:   ~MessageInterface (Destructor)
// Parameters:   none
// Return value: none
// Description: 
//  Currenttly does nothibg. Destructor is virtaul function,
//  and therefor will be executed when derived classes are destryed.
//
// Function:   ParseMessage
// Parameters:   
//  (in) uint8 *mess - pointer on new received message, message to take data from. 
// Return value: 
//  bool - true if ewverything is OK, othervice false. Curently alwayes returns true.
// Description: 
//  Sets pointer CurrentMessage on new received message. This fuction is virtual
//  and is planed to analize message when recived to determine if messahge is valid or not
//  It should also perform parameter validation check.
//
// Function:   PrepareNewMessage
// Parameters:   
//  (in) uint8 *buff - pointer on new received message, message to put data in.
// Return value: none
// Description: 
//  Sets pointer NewMessage on new message buffer. This fuction is virtual
//  and is planed to set default values in buffer, to create message with default parameters.
//  Curently it only set NewMessage pointer.
//
// Function:   SetNewMessage
// Parameters:   
//  (in) uint8 *buff - pointer on new received message, message to put data in.
// Return value: none
// Description: 
//  Sets pointer NewMessage on new message buffer.
//
// Function:   ClearMessage
// Parameters:   none
// Return value: none
// Description: 
//  Sets pointer NewMessage and CurrentMessage on NULL value.
//
// Function: GetParam (abstract)
// Parameters:   
//  (in) uint8 paramCode -  code of serched parameter
// Return value: 
//  uint8 * - pointer on wanted parameter, if parameter is not found, returns NULL.
// Description: 
// Returns pointer on paramter if it exits in current message, if the parameter does not
// exits in current message it returns NULL pointer.
//
// Function: GetParamByte (abstract)
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
// Function: GetParamWord (abstract)
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
// Function: GetParamDWord (abstract)
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
//
// Function: AddParam (abstract)
// Parameters:   
//  (in) uint8 paramCode   - code of paramere to be added
//  (in) uint8 paramLength - length of parameter to be added
//  (in) uint8 *param      - pointer on parameter 
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds paramtere in current new message, it adds paramtere in place where it should be, 
// all parameters are sorted in accesiding order. Function returns pointer on message.
//
// Function: AddParamByte (abstract)
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
//  (in) BYTE  param     - parameter to be added in message
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds parameter one byte long in current new message, it adds paramtere in place where
// it should be, all parameters are sorted in accesiding order. Function returns pointer
// on message.
//
// Function: AddParamWord (abstract)
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
//  (in) WORD  param     - parameter to be added in message
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds parameter two bytes long in current new message, it adds paramtere in place where
// it should be, all parameters are sorted in accesiding order. Function returns pointer
// on message.
//
// Function: AddParamDWord (abstract)
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
//  (in) DWORD  param    - parameter to be added in message
// Return value: 
//  uint8 * - pointer on new message
// Description: 
// Adds parameter four bytes long in current new message, it adds paramtere in place where
// it should be, all parameters are sorted in accesiding order. Function returns pointer
// on message.
//
// Function: RemoveParam (abstract)
// Parameters:   
//  (in) uint8 paramCode - code of paramere to be added
// Return value: 
//  bool - returns true if parameter is removed, returns false if there is no such parameter
//         in message
// Description: 
// Removes parameter from new message. If prameter is found and removed it returns true, if
// there is no such paramter in message it returns false.
//
// **************************************************************************


#ifndef message_interface__
#define message_interface__

#ifndef __KERNEL_TYPES__
#include "../kernel/kernelTypes.h"
#endif

#ifndef __KERNEL_CONSTS__
#include "../kernel/kernelConsts.h"
#endif

#define  MASK_1_BIT 0x00000001
#define  MASK_2_BIT 0x00000003
#define  MASK_3_BIT 0x00000007
#define  MASK_4_BIT 0x0000000f
#define  MASK_5_BIT 0x0000001f
#define  MASK_6_BIT 0x0000003f
#define  MASK_7_BIT 0x0000007f
#define  MASK_8_BIT 0x000000ff
#define  MASK_9_BIT 0x000001ff
#define MASK_10_BIT 0x000003ff
#define MASK_11_BIT 0x000007ff
#define MASK_12_BIT 0x00000fff
#define MASK_13_BIT 0x00001fff
#define MASK_14_BIT 0x00003fff
#define MASK_15_BIT 0x00007fff
#define MASK_16_BIT 0x0000ffff
#define MASK_17_BIT 0x0001ffff
#define MASK_18_BIT 0x0003ffff
#define MASK_19_BIT 0x0007ffff
#define MASK_20_BIT 0x000fffff
#define MASK_21_BIT 0x001fffff
#define MASK_22_BIT 0x003fffff
#define MASK_23_BIT 0x007fffff
#define MASK_24_BIT 0x00ffffff
#define MASK_25_BIT 0x01ffffff
#define MASK_26_BIT 0x03ffffff
#define MASK_27_BIT 0x07ffffff
#define MASK_28_BIT 0x0fff1fff
#define MASK_29_BIT 0x1fffffff
#define MASK_30_BIT 0x3fffffff
#define MASK_31_BIT 0x7fffffff
#define MASK_32_BIT 0xffffffff


class MessageInterface {
  private:

  protected:
    uint8 *CurrentMessage;
    uint8 *NewMessage;

    inline uint16 GetMsgInfoLength()              {return GetUint16(CurrentMessage+MSG_LENGTH);};
    inline uint16 GetMsgInfoLength(uint8 *msg)    {return GetUint16(msg+MSG_LENGTH);};
    inline uint16 GetNewMsgInfoLength()           {return GetUint16(NewMessage+MSG_LENGTH);};
    inline void   SetMsgInfoLength(uint16 length) {SetUint16(NewMessage+MSG_LENGTH, length);};

    uint8  GetBitParamByteBasic(uint32 offset,  uint32 mask=MASK_32_BIT);
    uint16 GetBitParamWordBasic(uint32 offset,  uint32 mask=MASK_32_BIT);
    uint32 GetBitParamDWordBasic(uint32 offset, uint32 mask=MASK_32_BIT);
    
    void SetBitParamByteBasic(BYTE param,   uint32 offset, uint32 mask=MASK_32_BIT);
    void SetBitParamWordBasic(WORD param,   uint32 offset, uint32 mask=MASK_32_BIT);
    void SetBitParamDWordBasic(DWORD param, uint32 offset, uint32 mask=MASK_32_BIT);

  public:
    MessageInterface()         {CurrentMessage = 0;NewMessage=0;};
    virtual ~MessageInterface(){};

    virtual bool ParseMessage(uint8 *mess)      { CurrentMessage = mess; return true;};
    virtual void PrepareNewMessage(uint8 *buff) {NewMessage = buff;};
            void SetNewMessage(uint8 *buff)     {NewMessage = buff;};

            void CopyMessage(uint8 *newMessage);
            void CopyMessage(uint8 *newMessage, uint8 *mess);
    virtual void CopyMessageInfo( uint8 infoCoding, uint8 *newMessage);
            void ClearMessage(){CurrentMessage=0; NewMessage=0; };

    //performed unther current message
    virtual uint8 *GetParam(uint32 paramCode)     = 0;
    virtual uint8 *GetParam(uint32 paramCode, uint32 &paramLen) = 0;
    virtual bool  GetParamByte(uint32 paramCode,  BYTE  &param) = 0;
    virtual bool  GetParamWord(uint32 paramCode,  WORD  &param) = 0;
    virtual bool  GetParamDWord(uint32 paramCode, DWORD &param) = 0;
    virtual bool  RemoveParam(uint32 paramCode)    = 0;

    virtual uint8 *GetNextParam(uint32 paramCode) {return 0;};
    virtual uint8 *GetNextParam(uint32 paramCode, uint32 &paramLen){return 0;};
    virtual bool  GetNextParamByte(uint32 paramCode,  BYTE  &param){return false;};
    virtual bool  GetNextParamWord(uint32 paramCode,  WORD  &param){return false;};
    virtual bool  GetNextParamDWord(uint32 paramCode, DWORD &param){return false;};

//performed unther current new message
    virtual uint8 *AddParam(uint8 *param) = 0;
    virtual uint8 *AddParam(uint32 paramCode, uint32 paramLength, uint8 *param) = 0;
    virtual uint8 *AddParamByte(uint32 paramCode,  BYTE param)  = 0;
    virtual uint8 *AddParamWord(uint32 paramCode,  WORD param)  = 0;
    virtual uint8 *AddParamDWord(uint32 paramCode, DWORD param) = 0;

    virtual bool  GetBitParamByte(uint32 paramCode,  BYTE  &param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0){return false;};
    virtual bool  GetBitParamWord(uint32 paramCode,  WORD  &param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0){return false;};
    virtual bool  GetBitParamDWord(uint32 paramCode, DWORD &param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0){return false;};

    virtual uint8 *SetBitParamByte(uint32 paramCode,  BYTE param,  uint32 mask=MASK_32_BIT, uint8 shiftBits=0){return 0;};
    virtual uint8 *SetBitParamWord(uint32 paramCode,  WORD param,  uint32 mask=MASK_32_BIT, uint8 shiftBits=0){return 0;};
    virtual uint8 *SetBitParamDWord(uint32 paramCode, DWORD param, uint32 mask=MASK_32_BIT, uint8 shiftBits=0){return 0;};

    virtual void SetENDOfParams()=0;
};

#endif