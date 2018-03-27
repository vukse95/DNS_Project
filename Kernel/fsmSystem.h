// **************************************************************************
//                        fsmSystem.h  -  description
//                           -------------------
//  begin                : Tue Jun 27 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//   FSMSystem class controls system of FSMs. All FSMs are reported to it.
//   It has control over execution of each message (autmate).
//   It takes message from queue and gives it to aprorpitae automate for
//   processing.
//   In this class can be defined strategy for execution of automates.
//
// **************************************************************************

#ifndef __FSM_SYSTEM__
#define __FSM_SYSTEM__

#ifndef __BUFFERS__
#include "../kernel/buffers.h"
#endif

#ifndef __POST_OFFICE
#include "../kernel/postOffice.h"
#endif

#ifndef __TIMER__
#include "../kernel/timer.h"
#endif

#ifndef __FSM__
#include "fsm.h"
#endif

// **************************************************************************
//
//   class data
// 	ptrFiniteStateMachine Head - pointer on first free FiniteStateMachine
//                               object (autmate)
//  ptrFiniteStateMachine Tail - pointer on alst free FiniteStateMachine
//                               object (autmate)
//  uint32 FreeFSMCount        - number of elements in list
//
// **************************************************************************
//
// DECRIPTION OF inline and abstract FUNCTIONS for class FSMList
//
// Function:  Available (inline)
// Parameters: none
// Return value:
//  uint32 - number of objects in list
// Description: 
//  Returns number of objects in list. This number represents number
//  of available objects.
//
// **************************************************************************
typedef FiniteStateMachine* ptrFiniteStateMachine;

class FSMList {

  protected:
  	ptrFiniteStateMachine Head;
  	ptrFiniteStateMachine Tail;
	
	  uint32 FreeFSMCount;
	
	  inline ptrFiniteStateMachine GetFromHead();
	  inline void PutOnTail(ptrFiniteStateMachine elem);

  public:
  	FSMList();
  	virtual ~FSMList();
	
  	void Add(ptrFiniteStateMachine info);
  	ptrFiniteStateMachine Get();        //uzima sa pocetka reda (head)
	
  	uint32 Available() { return FreeFSMCount; };
  	bool   IsEmpty();
};


// **************************************************************************
//
//  class data
//   static TPostOffice *PostOffice - private member function, provides access to 
//                                    message excange object.
//   static TBuffers    *Buffers    - private member, provides access to memory
//                                    handling system
//   static TTimer      *Timer      - private member, provides access to timers
//
//   uint32   NumberOfAutomates       - number of automate types, size of array
//                                      NumberOfObjects and Automates.
//   uint8    NumberOfMbx             - number of mail boxes in Post Office
//   uint32  *NumberOfObjects         - number of 
//   FSMList *FreeAutomates           - Liat of free objects od some type of automate
//   ptrFiniteStateMachine *Automates - pointer on array of autmates
//   bool UseFreeAutomateList         - If true all free automate objects are in list.
//                                      In case of message for unknown automate 
//                                      (objecId == INVALID_32) new automate is
//                                      assigned for processing a message.
//
// **************************************************************************
//
// DECRIPTION OF inline and abstract FUNCTIONS for class FSMList
//
// Function:  Available (inline)
// Parameters: none
// Return value:
//  uint32 - number of objects in list
// Description: 
//  Returns number of objects in list. This number represents number
//  of available objects.
//
// **************************************************************************

//numbuer of diferent automates types
#define MAX_NUM_OF_AUTOMATES_TYPES 40

class FSMSystem {
//  private:
    static TPostOffice *PostOffice;
    static TBuffers    *Buffers;
    static CTimer      *Timer;

protected:
    friend class FiniteStateMachine;
    friend class TCPSystemSupport; //Add by David


    ptrFiniteStateMachine *Automates[MAX_NUM_OF_AUTOMATES_TYPES];
    uint8   NumberOfMbx;
    uint8   NumberOfAutomates;
    uint32  NumberOfObjects[MAX_NUM_OF_AUTOMATES_TYPES];
    bool    FreeKernelMemory;
    bool    SystemWorking;
		uint8	 serverId;

    static bool    UseFreeAutomateList[MAX_NUM_OF_AUTOMATES_TYPES];
    static FSMList FreeAutomates[MAX_NUM_OF_AUTOMATES_TYPES];

  protected:

    inline uint8  GetMsgToAutomate(uint8 *msg)       {return msg[MSG_TO_AUTOMATE];};
		inline uint8  GetMsgToGroup(uint8 *msg)          {return msg[MSG_TO_GROUP];};
		inline uint8  GetMsgInfoLength(uint8 *msg)       {return msg[MSG_LENGTH];};
    inline uint32 GetMsgObjectNumberTo(uint8 *msg)   {return GetUint32(msg+MSG_OBJECT_ID_TO);};//{return *((uint32 *)(msg+MSG_OBJECT_ID_TO));};
    inline void   DiscardMsg(uint8 *msg)             {Buffers->RetBuffer(msg);};
    inline uint8 *GetMsg(uint8 mbxId)                {return PostOffice->Get(mbxId);};
    inline uint8 *GetMsgSync(uint8 &mbxId)           {return PostOffice->GetSync(mbxId);};
    inline uint8 *GetBuffer(uint32 bufferLength)     {return Buffers->GetBuffer(bufferLength);};
		inline void   SendToMbx(uint8* msg, uint8 mbx=3) {PostOffice->Add(mbx, msg);};

    static void   FreeFSM(ptrFiniteStateMachine object, uint8 automateType);

  public:
    FSMSystem(uint8 numOfAutomates, uint8 numberOfMbx);
    virtual ~FSMSystem();

    void Add(ptrFiniteStateMachine object, uint8 automateType);
    void Add(ptrFiniteStateMachine object, uint8 automateType, uint32 numOfObjects, bool useFreeList = false);
    void Remove( uint8 automateType);
    ptrFiniteStateMachine Remove( uint8 automateType, uint32 object);
    void Delete( uint8 automateType);

    virtual void Start();
    void StopSystem(){SystemWorking = false;};

    void InitKernel( uint8 buffClassNo, uint32 *buffersCount, uint32 *buffersLength, uint8 numOfMbxs=0, TimerResolutionEnum timerRes = Timer1s );

    void TestMessage();
};

#ifndef __NO_TCP_SUPPORT__
#include "systemTCP.h"

class FSMSystemWithTCP : public FSMSystem, public TCPSystemSupport{
  public:
    FSMSystemWithTCP(uint8 numOfAutomates, uint8 numberOfMbx):FSMSystem(numOfAutomates, numberOfMbx){;};
    virtual ~FSMSystemWithTCP(){;};
};
#endif

#endif