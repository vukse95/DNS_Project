// **************************************************************************
//                        KernelAPI.h  -  description
//                           -------------------
//  begin                : Tue Jun 20 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  KernelAPI derived from MessageHandler class gives interface for
//  kernel functions so that it is easy to change kernel and to keep all the code
//  that is made. It gives functions for memory handling, time control and message
//  exchange.
//  
// **************************************************************************
//
//  static TPostOffice *PostOffice - private member function, provides access to 
//                                   message excange object.
//  static TBuffers    *Buffers    - private member, provides access to memory
//                                   handling system
//  static TTimer      *Timer      - private member, provides access to timers
//
// **************************************************************************
//
// DECRIPTION OF inline and abstract FUNCTIONS for class MessageInterface
//
// Function:  RetBufferSystem (static)
// Parameters:
//  (in) uint8 * buff - pointer on buffer
// Return value: none
// Description: 
//  Function returns buffer in pool of free buffers, it static and its only use is
//  by FSMSystem class, in next release this function will be removed. Do no use it
//
// Function:  GetBuffer 
// Parameters:
//  (in) uint32 length - size of requested buffer
// Return value: 
//  (in) uint8 * - pointer on buffer
// Description: 
//  Function returns buffer from pool of free buffers, It provides interface in case
//  there is change in objects that handles memory.
//
// Function:   RetBuffer
// Parameters:
//  (in) uint8 * - pointer on buffer to be freed
// Return value: none
// Description: 
//  Function returns buffer in pool of free buffers. In case NULL pointer is
//  given as paramter, memory handling object should throw an exception.
//
// Function:   IsBufferSmall
// Parameters:
//  (in) uint8  *buff  - pointer on buffer to be checked
//  (in) uint32 length - size of buffer for comparation
// Return value: 
//  bool - returns true if buffer is larger than lengrth othervise false
// Description: 
//  Compare size of buffer with length. If buffer is larger then length returns
//  false, othervice returns true. Usualy used to check weather new parameter can be
//  added in message.
//
// Function:   GetBufferLength
// Parameters:
//  (in) uint8  *buff  - pointer on buffer to be checked
// Return value: 
//  uint32 - size of buffer
// Description: 
//  Function determines size of buffer, and returns it.
//
// Function:   GetCount
// Parameters:
//  (in) uint8  mbx  - mail box id (index)
// Return value: 
//  uint32 - number of messages in mbx
// Description: 
//  Function returns number of messages currently queued in mail box.
//
// Function:   GetMsg
// Parameters: none
// Return value: 
//  uint8 * - pointer on message
// Description: 
//  Function returns first message from automate mail box. Each type of automate has its
//  own mail box, one and only one mail box, from which it recives messages. This
//  function takes first message from that mail box.
//
// Function:   GetMsg
// Parameters: 
//  (in) uint8 mbx - mail boc id (index)
// Return value: 
//  uint8 * - pointer on message
// Description: 
//  Function returns first message from mail box mbx.
//
// Function:   PurgeMailBox
// Parameters:   none
// Return value: none
// Description: 
//  Function deletes all messages from automate mail box,
//
// Function:   Discard (virtual)
// Parameters:   
//  (in) uint8 *buff - pointer on message
// Return value: none
// Description: 
//  Function discrads messge. It returns messgae buffer to pool of free buffers.
//
// Function:   SendMessage 
// Parameters:   
//  (in) uint8 mbx - mail boc id (index)
// Return value: none
// Description: 
//  Function sends new message in mail box mbx.
//
// Function:   SendMessage 
// Parameters:   
//  (in) uint8 mbx - mail boc id (index)
//  (in) uint8 *buff - pointer on message
// Return value: none
// Description: 
//  Function sends message in buffer buff, in mail box mbx.
//
// Function:   StopTimer (virtual)
// Parameters:   
//  (in) uint8 *timer - pointer on timer
// Return value: none
// Description: 
//  Function stops timer defined by buffer timer.
//
// Function:   IsTimerRunning (virtual)
// Parameters:   
//  (in) uint8 *timer - pointer on timer
// Return value:
//  bool - true if timer is runnig, false if not.
// Description: 
//  Function checks weather timer defined by buffer timer is running or not. 
//  If timer is running returns true, false if not.
//
// Function:   IsTimerRunning (virtual)
// Parameters:   
//  (in) uint8 *timer - pointer on timer
// Return value:
//  bool - true if timer is runnig, false if not.
// Description: 
//  Function checks weather timer defined by buffer timer is running or not. 
//  If timer is running returns true, false if not.
//
// Function:   GetCallId (virtual)
// Parameters:   none
// Return value:
//  uint32 - call id
// Description: 
//  Function returns call id.
//
// Function:   GetMbxId (abstract)
// Parameters:   none
// Return value:
//  uint8 - mbx id
// Description: 
//  Function returns mbx id of automate group.
//
// Function:   GetAutomate (abstract)
// Parameters:   none
// Return value:
//  uint8 - autmate
// Description: 
//  Function returns code of automate group.
//
// Function:   GetGroup (abstract)
// Parameters:   none
// Return value:
//  uint8 - object type
// Description: 
//  Function returns code of object group.
//
// Function:   GetObjectId (virtual)
// Parameters:   none
// Return value:
//  uint32 - object id
// Description: 
//  Function returns unique code object.
//
// Function:   GetDefaultParamCoding (virtual)
// Parameters:   none
// Return value:
//  uint8 - code
// Description: 
//  Function returns code of default foramt of messages.
//
// Function:   GetRightAutomate (virtual)
// Parameters:   none
// Return value:
//  uint8 - code
// Description: 
//  Function returns code for right automate in communication.
//  Usualy used by function SendRight
//
// Function:   GetRightGroup (virtual)
// Parameters:   none
// Return value:
//  uint8 - object type
// Description: 
//  Function returns object type for right automate in communication.
//  Usualy used by function SendRight
//
// Function:   GetRightObjectId (virtual)
// Parameters:   none
// Return value:
//  uint32 - object type
// Description: 
//  Function returns unique object id  for right automate in communication.
//  Usualy used by function SendRight
//
// Function:   GetRightMbx (virtual)
// Parameters:   none
// Return value:
//  uint8 - mbx id (index)
// Description: 
//  Function returns mbx id  for right automate in communication.
//  Usualy used by function SendRight
//
// Function:   GetLeftAutomate (virtual)
// Parameters:   none
// Return value:
//  uint8 - code
// Description: 
//  Function returns code for right automate in communication.
//  Usualy used by function SendLeft
//
// Function:   GetLeftGroup (virtual)
// Parameters:   none
// Return value:
//  uint8 - object type
// Description: 
//  Function returns object type for right automate in communication.
//  Usualy used by function SendLeft
//
// Function:   GetLeftObjectId (virtual)
// Parameters:   none
// Return value:
//  uint32 - object type
// Description: 
//  Function returns unique object id  for right automate in communication.
//  Usualy used by function SendLeft
//
// Function:   GetLeftMbx (virtual)
// Parameters:   none
// Return value:
//  uint8 - mbx id (index)
// Description: 
//  Function returns mbx id  for right automate in communication.
//  Usualy used by function SendLeft
//  
// Function:   KernelAPI (constructor)
// Parameters:   none
// Return value: none
// Description: 
//  Initalizes KernelAPI, this is default contructor which sets
//  all pointers (TPostOffice *PostOffice, TBuffers *Buffers,
//  TTimer *Timer) to NULL value.
//  
// **************************************************************************

#ifndef __PROGRAMMING_INTERFACE__
#define __PROGRAMMING_INTERFACE__

#ifndef __BUFFERS__
#include "../kernel/buffers.h"
#endif

#ifndef __POST_OFFICE
#include "../kernel/postOffice.h"
#endif

#ifndef __TIMER__
#include "../kernel/timer.h"
#endif

#ifndef __MESSAGES__
#include "../kernel/messages.h"
#endif

//class FSMSystem;

class KernelAPI : public MessageHandler {
  private:
    static TPostOffice *PostOffice;
    static TBuffers    *Buffers;
    static CTimer      *Timer;

		uint8 mbxId;
		uint8 automateId;

    TimerResolutionEnum TimerResolution;

  protected:

    static void SetKernelObjects(TPostOffice *postOffice, TBuffers *buffers, CTimer *timer);

    //buffers handling
    virtual uint8 *GetBuffer(uint32 length){return Buffers->GetBuffer(length);};
    virtual void  RetBuffer(uint8 *buff)   {Buffers->RetBuffer(buff);};
    virtual bool  IsBufferSmall(uint8 *buff, uint32 length)
                                        {return (Buffers->GetLength(buff)<=length ? true:false);};
    uint32 GetBufferLength(uint8 *buff) {return Buffers->GetLength(buff);};

    //message handling
    uint32 GetCount(uint8 mbx)       { return PostOffice->GetCount(mbx); };
    uint8* GetMsg(void)              { return PostOffice->Get(GetMbxId()); };
    //static uint8* GetMsg(uint8 mbx)  { return PostOffice->Get(mbx); };
    void PurgeMailBox()              {PostOffice->PurgeMailBox(GetMbxId());};
    //diascard message, pointer to message
    virtual void Discard(uint8* buff) {RetBuffer(buff);};

    void SetMessageFromData(void);
    
	  inline void SendMessage(uint8 mbxId/*, bool setMsgFrom = true*/){
		  SetMessageFromData();
		  PostOffice->Add( mbxId, GetNewMessage());
		  SetNewMessage(NULL);};
    inline void SendMessage(uint8 mbxId, uint8 *msg) {PostOffice->Add( mbxId, msg);};
           void SendMessageLeft();
           void SendMessageRight();
           void ReturnMsg(uint8 mbxId);

    //timers handling
    uint8 *StartTimer(uint16 code, uint32 count/*, uint8 *info=0*/);
    void  StopTimer(uint8 *timer)      { Timer->StopTimer(timer);};
    bool  IsTimerRunning(uint8 *timer) {return Timer->TimerRunning(timer);};

    inline void SetTimerResolution(TimerResolutionEnum timerRes){TimerResolution = timerRes;};
    inline TimerResolutionEnum GetTimerResolution()               {return TimerResolution;};

    //sprega sa FSM klasom
//    virtual bool   IsTimerValid();
//    virtual uint16 GetTimerCode(uint16 id);
//    virtual uint32 GetTimerCount(uint16 id);

    virtual uint32 GetCallId()   =0;
    uint8  GetMbxId() { return mbxId;};
    uint8  GetAutomate() { return automateId;};
    virtual uint8  GetGroup()   =0;
    virtual uint32 GetObjectId() =0;
    virtual uint8  GetDefaultParamCoding(){return LOCAL_PARAM_CODING;};
    virtual uint8  GetRightAutomate() =0;
    virtual uint8  GetRightGroup()    =0;
    virtual uint32 GetRightObjectId() =0;
    virtual uint8  GetRightMbx()      =0;
    virtual uint8  GetLeftAutomate()  =0;
    virtual uint8  GetLeftGroup()    =0;
    virtual uint32 GetLeftObjectId()  =0;
    virtual uint8  GetLeftMbx()       =0;

  public:
    KernelAPI( uint8 automateId1, uint8 mbxId1){ automateId = automateId1; mbxId = mbxId1; PostOffice=0; Buffers=0; Timer=0; TimerResolution = Timer1s;};
    KernelAPI(char *fileNmae);
    KernelAPI(TPostOffice *postOffice, TBuffers *buffers, CTimer *timer);
	static uint8* GetMsg(uint8 mbx)  { return PostOffice->Get(mbx); };
};

#endif 

