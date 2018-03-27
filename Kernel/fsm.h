// **************************************************************************
//                        fsm.h  -  description
//                           -------------------
//  begin                : Tue Jun 27 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Description:
//  This class represents one automate object. There is a lot of autmates of some type
//  this class is parent of such autmate type. It gives functionality of autmoate
//  and through its parent classes it has encapsulation of kernel and message handling.
//  In this file is also defined class FSMList which is used by FSMSytem
//  to hold free automates. Each autmate that is not in use is stored here
//  when message for new call arrives autmate from this list is apointed
//  to process it.
//
// **************************************************************************


#ifndef __FSM__
#define __FSM__

#ifdef WIN32
#include <winsock.h>
#endif

#ifndef __NO_TCP_SUPPORT__
#include "uniSock.h"
#endif

#ifndef __LOG_AUTOMATE__
#include "logAutomate.h"
#endif

//some default values
//const uint16 MAX_STATE_NO  = 10; //default number of states
const uint16 MAX_NAME_LEN  = 16; //default name of length

// Default number of timers that can be handled by FSM.
// Can be changed in constructor, if needed.
const unsigned DEFAULT_TIMER_NO = 0;

// Default number of events that can be handled in one FSM state.
// Can be changed in constructor, if needed.
const unsigned DEFAULT_PROCEDURE_NO_PER_STATE = 10;

// Default number of FSM states.
const unsigned DEFAULT_STATE_NO = 1;

// Pointer to kernel buffer typedef
typedef uint8* ptrBuff;

// A typedef for a pointer to an event handler function
typedef void (KernelAPI::*PROC_FUN_PTR )();

// Struct TimerBlock encapsulates data needed for timer handling
// in kernel. Pointers to currently active timers
// are also stored here.
struct TimerBlock {
  TimerBlock(uint16 v, uint16 s) :
    Count(v), SignalId(s), Valid(false), TimerBuffer(0){}
  TimerBlock() :
    Count(INVALID_32), SignalId(INVALID_16), Valid(false), TimerBuffer(0) {}

  uint32  Count;       // in time slices
  uint16  SignalId;    // message code of timer expired
  bool    Valid;       // if true timer data (Count, SignalId) are set if not timer
                       // date are invalid
  ptrBuff TimerBuffer; // After timer is started ptr to it's buffer is set here.
};

// Struct SBranch is used for storing event codes and pointers
// to their associated procedures. Each state has several SBranch objects.
struct SBranch {
  uint16       EventCode; // code of eveent, message code
  PROC_FUN_PTR ProcPtr;   // pointer on function that proccess message
};

// Struct SState encapsulates data important for one FSM state.
// One FSM has a number of SState objects. However state value
// is stored separately in FSM object.
struct SState {
  SState  (uint16 maxNumOfProceduresPerState);
  ~SState ();

  bool           StateValid;             //if true, data are valid
  unsigned short NumOfBranches;          //number of branches in state
  PROC_FUN_PTR   UnexpectedEventProcPtr; //procedure for proccessing unexpected message
  SBranch*       PBranch;                //pointer on data for each branch
//  Name of State, used for debuging and tracking
//  char           NameString[MAX_NAME_LEN];
};

// Pointer to kernel buffer typedef
typedef SState* ptrSState;

// **************************************************************************
//  calss FiniteStateMachine data description:
//
//   static uint32 CallCounter - used to generate unique call id
//   FiniteStateMachine *Next  - pointer on next automate in free automate list
//   TimerBlock *TimerStore    - used to store data about all timers in automate
//   uint16 NumOfStates        - number of different FSM states
//   uint16 NumOfTimers        - number of different timers (values & id's)
//   uint16 MaxNumOfProcPerState  - maximum number of procceures per state used to 
//                                  determine size of array of branches 
//   SState *States            - state data with procedure ptrs
//   uint32 ConnectionId       - id of currently processed connection, index of cuurent object
//                               in array of objects of automate group
//   uint32 CallId             - id of current call, used for call tracking 
//   uint8  LeftMbx            - left mbx id
//   uint8  LeftAutomate       - left automate group
//   uint8  LeftObject         - left object type
//   uint32 LeftObjectId       - number of left object
//   uint8  RightMbx           - right mbx id
//   uint8  RightAutomate      - right automate group
//   uint8  RightObject        - right object type
//   uint32 RightObjectId      - number of right object
//   uint8 State               - current state of automate
//
// **************************************************************************
//
// DECRIPTION OF inline and abstract FUNCTIONS for class MessageInterface
//
// Function:  GetPointId (virtual)
// Parameters: none
// Return value:
//  uint32 - point id
// Description: 
//  Returns point id, used for tracking call process, by default it is zero.
//  In case of distributed automates each signaling point (FSMSystem object on 
//  different computers) should be assigned different value. This value has to ocupie
//  highest nible in DWORD. All other nibles should be zero.
//
// Function:  GetState (inline)
// Parameters: none
// Return value:
//  uint8 - current state of automate
// Description: 
//  Returns current state of automate.
//
// Function:  SetState (inline)
// Parameters: 
//  (in) uint8 state - new state
// Return value: none
// Description: 
//  Function sets new state of automate.
//
// Function:  SetCallId (inline)
// Parameters:   none
// Return value: none
// Description: 
//  Function sets new value for CallId, from CallCounter and increments CallCounter,
//
// Function:  SetCallId (inline)
// Parameters:   
//  (in) uint32 id - new id
// Return value: none
// Description: 
//  Function sets new value for CallId,
//
// Function:  SetCallIdFromMsg (inline)
// Parameters:   none
// Return value: none
// Description: 
//  Function sets new value for CallId, New value is taken from recived message.
//
// Function:  GetCallId (inline)
// Parameters:   none
// Return value: 
//  uint32 - call id
// Description: 
//  Returns value of CallId.
//
// Function:  GetCallId (inline)
// Parameters:   none
// Return value: 
//  uint32 - call id
// Description: 
//  Returns value of CallId.
//
// Function:  SetLeftMbx (inline)
// Parameters:   
//  (in) uint8 mbx - mail box id
// Return value: none
// Description: 
//  Sets LeftMbx value on mbx. It is used to determine mbx of left automate in 
//  communication.
//
// Function:  SetLeftAutomate (inline)
// Parameters:   
//  (in) uint8 automate - automate group
// Return value: none
// Description: 
//  Sets left autmate group code. It is used to determine autmate group for left 
//  automate in communication.
//
// Function:  SetLeftObject (inline)
// Parameters:   
//  (in) uint8 automate - object type
// Return value: none
// Description: 
//  Sets left object type code. It is used to determine object type for left 
//  automate in communication.
//
// Function:  SetLeftObjectId (inline)
// Parameters:   
//  (in) uint32 id - object number
// Return value: none
// Description: 
//  Sets left object number. It is used to determine object number for left 
//  automate in communication. Object number uniqly defines autmate in group 
//  of automate. Usualy it is index of automate in array of atomates of some type
//
// Function:  SetRightMbx (inline)
// Parameters:   
//  (in) uint8 mbx - mail box id
// Return value: none
// Description: 
//  Sets RightMbx value on mbx. It is used to determine mbx of right automate in 
//  communication.
//
// Function:  SetRightAutomate (inline)
// Parameters:   
//  (in) uint8 automate - automate group
// Return value: none
// Description: 
//  Sets right autmate group code. It is used to determine autmate group for right 
//  automate in communication.
//
// Function:  SetRightObject (inline)
// Parameters:   
//  (in) uint8 automate - object type
// Return value: none
// Description: 
//  Sets right object type code. It is used to determine object type for right 
//  automate in communication.
//
// Function:  SetRightObjectId (inline)
// Parameters:   
//  (in) uint32 id - object number
// Return value: none
// Description: 
//  Sets right object number. It is used to determine object number for right 
//  automate in communication. Object number uniquly defines autmate in group 
//  of automate. Usualy it is index of automate in array of atomates of some type
//
// Function:  GetLeftMbx (inline)
// Parameters:   none
// Return value: 
//  uint8 - mbx id
// Description: 
//  Returns mbx id for left automate in communication.
//
// Function:  GetLeftAutomate (inline)
// Parameters:   none
// Return value: 
//  uint8 - automate group (ISUP, CC ..)
// Description: 
//  Returns autmate group for left automate in communication.
//
// Function:  GetLeftGroup (inline)
// Parameters:   none
// Return value: 
//  uint8 - automate type (CPCI, CPCO, FE7 ...)
// Description: 
//  Returns autmate type for left automate in communication.
//
// Function:  GetLeftObjectId (inline)
// Parameters:   none
// Return value: 
//  uint32 - object id
// Description: 
//  Returns unique object id (inside autmate grouop) for left automate in communication.
//
// Function:  GetRightMbx (inline)
// Parameters:   none
// Return value: 
//  uint8 - mbx id
// Description: 
//  Returns mbx id for right automate in communication.
//
// Function:  GetRightAutomate (inline)
// Parameters:   none
// Return value: 
//  uint8 - automate group (ISUP, CC ..)
// Description: 
//  Returns autmate group for right automate in communication.
//
// Function:  GetRightGroup (inline)
// Parameters:   none
// Return value: 
//  uint8 - automate type (CPCI, CPCO, FE7 ...)
// Description: 
//  Returns autmate type for right automate in communication.
//
// Function:  GetRightObjectId (inline)
// Parameters:   none
// Return value: 
//  uint32 - object id
// Description: 
//  Returns unique object id (inside autmate grouop) for right automate in communication.
//
// Function:  SetInitialState (virtual)
// Parameters:   none
// Return value: none
// Description: 
//  Sets current state of automate on initial state.
//
// Function:  GetInitialState (virtual)
// Parameters:   none
// Return value: 
//  uint8 - inital state
// Description: 
//  Returns initila state of automate.
//
// Function:  GetRightAutomate (abstract)
// Parameters:   none
// Return value: 
//  uint8 - automate group (ISUP, CC ..)
// Description: 
//  Returns autmate group for current automate.
//
// Function:  GetGroup (abstract)
// Parameters:   none
// Return value: 
//  uint8 - automate type (CPCI, CPCO, FE7 ...)
// Description: 
//  Returns autmate type for current automate.
//
// Function:  GetObjectId (inline)
// Parameters:   none
// Return value: 
//  uint32 - object id
// Description: 
//  Returns unique object id (inside autmate grouop) for current automate
//  in communication.
//
// Function:  SetObjectId (inline)
// Parameters:   
//  (in) uint32 id - object id
// Return value: none
// Description: 
//  Sets unique object id (inside autmate grouop) for current automate
//  in communication.
//
// Function:  SetDefaultFSMData (abstract)
// Parameters:   none
// Return value: none
// Description: 
//  Sets default data for autmate, usualy this data includes data about left and 
//  right object in communication, and all data that can be changed while 
//  proccesing one call.
//
// Function:  Initialize (abstract)
// Parameters:   none
// Return value: none
// Description: 
//  Initialzation is made for each ancestor in order to initialize ptr's to
//  procedures for each event in each state, ptr's to unexpected
//  event procedures, state name strings (for debug) and optionally
//  timer blocks. Protected init methods are used for that.
//  Init should be called from constructor.
//
// Function:  NoFreeObjectProcedure
// Parameters:   none
// Return value: none
// Description: 
//  This procedure is called when there is no more free resources to process new 
//  calls. It takes care about message checjing and message clearing. It
//  calls fuction NoFreeInstances.
//  
// Function:  NoFreeInstances (abstract)
// Parameters:   none
// Return value: none
// Description: 
//  This procedure is called when there is no more free resources to process new 
//  calls. Generaly this function should refuse call. Userhould specifie reaction
//  in derived class.
//  
//
//    
//    void DoNothing(){};
//    bool GetFreeObject(){return false;};
//    virtual void FreeResources();
//    virtual void FreeObject();
//
// Function:  acceptConnection (virtual)
// Parameters:   SOCKET newSocket - socket for establishing the new connection.
// Return value: int - if less then zero, connection is refused.
// Description: 
//  When new TCP connection is requested, by this function connection is accepted
//  
//
//    
//    void DoNothing(){};
//    bool GetFreeObject(){return false;};
//    virtual void FreeResources();
//    virtual void FreeObject();
//
// **************************************************************************

//class FSMSytem;

class FiniteStateMachine : public LogAutomateNew {
  private:
    friend class FSMSystem;  //this should go out in next release
    friend class FSMList;   //this should go out as soon as possible
    static uint32 CallCounter;  //used to generate unique call id

    FiniteStateMachine *Next; //pointer on next automate in free automate list
    bool ReturnMemory;
    
    // Kernel interface data
    TimerBlock *TimerStore;   //Timer data are set here

    //
    // Finite State Machine encapsulation
    //
    uint16 NumOfStates; // Number of different FSM states
    uint16 NumOfTimers; // Number of different timers (values & id's)
    uint16 MaxNumOfProcPerState;  // 
//    SState *States[MAX_STATE_NO]; // State data with procedure ptrs
    SState **States; // State data with procedure ptrs
    uint32 ConnectionId;  // Id of currently processed connection
		uint8 GroupId;				//computer Id
    uint32 CallId;        // id of current call, used for call tracking 


    //Name of FSM, used for debuging and tracking
//    char NameString[MAX_NAME_LEN];

    //Left automate DATA, used for current call
    uint8  LeftMbx;       //left mbx id
    uint8  LeftAutomate;  //left automate group
    uint8  LeftGroup;    //left object type
    uint32 LeftObjectId;  //number of left object
    //Right automate DATA, used for current call
    uint8  RightMbx;      //right mbx id
    uint8  RightAutomate; //right automate group
    uint8  RightGroup;   //right object type
    uint32 RightObjectId; //number of right object

    //automate state dependent data
    uint8 State; //state of automate

  protected:
    virtual inline uint32 GetPointId()          {return 0;};
    //virtual inline uint8  GetState()            {return State;};
            inline  void  SetState(uint8 state) {State = state;};
            inline  void  SetCallId()           {CallId = (CallCounter++) | GetPointId(); 
                                                 CallCounter %= 0x0fffffff;};
            inline  void   SetCallId(uint32 id) {CallId = id;};
            inline  void   SetCallIdFromMsg()   {CallId = GetMsgCallId();};
    virtual inline  uint32 GetCallId()          {return CallId;};


    inline void SetLeftMbx(uint8 mbx)           {LeftMbx = mbx;};
    inline void SetLeftAutomate(uint8 automate) {LeftAutomate = automate;};
    inline void SetLeftObject(uint8 group)      {LeftGroup = group;};
    inline void SetLeftObjectId(uint32 id)      {LeftObjectId = id;};

    inline void SetRightMbx(uint8 mbx)           {RightMbx = mbx;};
    inline void SetRightAutomate(uint8 automate) {RightAutomate = automate;};
    inline void SetRightObject(uint8 group)      {RightGroup = group;};
    inline void SetRightObjectId(uint32 id)      {RightObjectId = id;};

    virtual inline uint8  GetLeftMbx()       {return LeftMbx;};
    virtual inline uint8  GetLeftAutomate()  {return LeftAutomate;};
    virtual inline uint8  GetLeftGroup()     {return LeftGroup;};
    virtual inline uint32 GetLeftObjectId()  {return LeftObjectId;};

    virtual inline uint8  GetRightMbx()      {return RightMbx;};
    virtual inline uint8  GetRightAutomate() {return RightAutomate;};
    virtual inline uint8  GetRightGroup()    {return RightGroup;};
    virtual inline uint32 GetRightObjectId() {return RightObjectId;};
    
    virtual void  SetInitialState()  {SetState(GetInitialState());};
    virtual uint8 GetInitialState()  {return 0;};
    
    // If no timers used in ancestor, no need to call InitTimerBlock
    void InitTimerBlock(uint16 tmrId, uint32 count, uint16 signalId);
    void StartTimer(uint16 tmrId /*, uint8 *info=0*/);
    void StopTimer(uint16 tmrId);
    void RestartTimer(uint16 tmrId, uint8 *info=0){StopTimer(tmrId); StartTimer(tmrId/*, info*/);};
    // Reset timer is used after timer expiration to set buffer ptr to NULL
    // if timer not started again
    void ResetTimer(uint16 id);
    bool IsTimerRunning(uint16 id);
    void setTimerCount(uint16 tmrId, uint32 count);

    //automate data
    virtual uint8  GetGroup()             {return GroupId;};
    virtual uint32 GetObjectId()          {return ConnectionId;};
    inline  void   SetObjectId(uint32 id) {ConnectionId = id;};
		inline  void   SetGroup(uint8 id) {GroupId = id;};

    virtual void SetDefaultFSMData()=0;

    //automate functioning
    // Init is made for each ancestor in order to initialize ptr's to
    // procedures for each event in each state, ptr's to unexpected
    // event procedures, state name strings (for debug) and optionally
    // timer blocks. Protected init methods are used for that.
    // Init should be called from constructor.
    virtual void Initialize(void) = 0;
    // FSM Initialization part, these methods should be called
    // from Init pure virtual method.
    void InitEventProc(uint8 state, uint16 event, PROC_FUN_PTR fun);
    void InitUnexpectedEventProc(uint8 state, PROC_FUN_PTR fun);
    //sets default data values for FSM
    virtual void Reset();
    // Returns ptr to procedure that handles that event
    // (it depends on FSM state also)
    PROC_FUN_PTR GetProcedure(uint16 event);
    //emtpy procedure, default for unexpected event
    void DoNothing(){};
    //this procedure is performed when there is no free object to handle new connection
    //it prepares object and then calls function NoFreeInstances
    void NoFreeObjectProcedure(uint8 *msg);
    //in this function user should define reaction on no free
    virtual void NoFreeInstances(){};
    //retuns object in list of free objects in system object
    void FreeFSM();

    //this procedure returns true if there is free object to handle new connection. If there is
    //it sets CurrentConnectionId on value of new connection. This is good place to set data about one
    //side of communuication.
//    bool GetFreeObject(){return false;};

//this is not in use any more
    //Free all resources assigned to automate
//    virtual void FreeResources();
    //Free all resources assigned and clear the data.
//    virtual void FreeObject();
//this is not in use any more
    void SetFSMStructures( SState **states);

  public:
//  FiniteStateMachine constructor takes four arguments (only two mandatory):
//   - mbx = mailbox from which messages to process are taken
//   - initialState = Initial FSM state
//   - numOfConn = optionally present, number of connections needed.
//     Default value is 1 (one finite state machine entity). For example
//     ISUP protocol needs as many connections as many circuits it controls.
//   - numOfTimers = optionaly present, number of different timers needed.
//     If not present default value is used.
//   - numOfState = optionaly present, number of different states needed.
//     If not present default value is used.
//   - maxNumOfProceduresPerState = Maximum number of procedures that can
//     be handled by FSM per one state. It determines memory allocation
//     for procedure pointers. If not present default value is used.
//
     FiniteStateMachine(
			 uint8 automateId1, uint8 mbxId1,
      uint16 numOfTimers = DEFAULT_TIMER_NO,
      uint16 numOfState  = DEFAULT_STATE_NO,
      uint16 maxNumOfProceduresPerState = DEFAULT_PROCEDURE_NO_PER_STATE, bool getMemory = true);
    virtual ~FiniteStateMachine();
//    void Main(void);
//    virtual bool IsAutomateStoped() = 0;

    // Method that processes event for given connection
    virtual void Process(uint8 *msg);
	virtual inline uint8  GetState()            {return State;};
#ifndef __NO_TCP_SUPPORT__
    virtual int  acceptConnection(SOCKET newSocket, void *exParam = 0) {return 0;};
#endif    
};



#endif 

