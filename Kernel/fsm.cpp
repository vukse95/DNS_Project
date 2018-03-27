// **************************************************************************
//                        fsm.cpp  -  description
//                           -------------------
//  begin                : Tue Jun 27 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
// **************************************************************************

#ifdef WIN32
#include <windows.h>
#endif

#include <assert.h>

#include "../kernel/errorObject.h"
#include "fsm.h"
#include "fsmSystem.h"

uint32 FiniteStateMachine::CallCounter = 0;

// Function: SState (Constructor for structure)
// Parameters:   
//  (in) uint16 maxNumOfProceduresPerState - maximun number of proceures per state
// Return value: none
// Description: 
//  Allocates resources for structure state of automate. Structure holds all data that
//  are state specific, like branches ...
SState::SState (uint16 maxNumOfProceduresPerState){
  // State without a single procedure?!?
  if (maxNumOfProceduresPerState == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));

  PBranch = new SBranch[maxNumOfProceduresPerState];

  // If not enough memory
  if (!PBranch)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
//  NameString[0]          = 0;
  StateValid             = false;
  NumOfBranches          = 0;
  UnexpectedEventProcPtr = 0;
}

// Function: SState (Destructor for structure)
// Parameters:   none
// Return value: none
// Description: 
//  Releases resources for structure state of automate.
SState::~SState () {
  delete[] PBranch;
}

// Function: FiniteStateMachine (Constructor)
// Parameters:   
//  (in) uint16 numOfTimers - max. number of timers used (per FSM entity)
//  (in) uint16 numOfStates - max. number of FSM states
//  (in) uint16 maxNumOfProceduresPerState - max. number of procedures per one state
// Return value: none
// Description: 
//  Gets memory and initializes data structures..
FiniteStateMachine::FiniteStateMachine(
  uint8 automateId1, uint8 mbxId1,
  uint16 numOfTimers,
  uint16 numOfStates,
  uint16 maxNumOfProceduresPerState, bool getMemory) : LogAutomateNew( automateId1, mbxId1)
{

#ifndef __NO_PARAMETER_CHECK__
  if (numOfStates == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  NumOfStates = numOfStates;

  if (numOfTimers == 0)  NumOfTimers = 1;
  else                   NumOfTimers = numOfTimers;

  // At least one (default) procedure per state must exist
  if (maxNumOfProceduresPerState == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  MaxNumOfProcPerState = maxNumOfProceduresPerState;

//  if (NumOfStates > MAX_STATE_NO) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
  MaxNumOfProcPerState = maxNumOfProceduresPerState;

  TimerStore = 0;
  ReturnMemory = getMemory;
  States = 0;
  if(ReturnMemory == true){
    States = new ptrSState[NumOfStates];
    // Reserve memory for pointers to procedures
    for(uint16 j=0; j< NumOfStates; j++) {
      // Some memory optimization can be done here by moving this new to InitEventProc
      //and allocating for each state exactly numeber of brancehs
      States[j] = new SState(MaxNumOfProcPerState);

      // If not enough memory
      if (!States[j])  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
    } //end for
  } //end if

  // Reserve memory for m_NumOfTimers blocks and pointers to
  // buffers (of started timer) for each connection
  TimerStore = new TimerBlock[NumOfTimers];
  // If not enough memory
  if (!TimerStore)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  Next = 0;
  SetCallId(0xffffffff);
  SetInitialState();
}

// Function: FiniteStateMachine (Destructor)
// Parameters:   none
// Return value: none
// Description: 
//  Releases memory.
FiniteStateMachine::~FiniteStateMachine() {
  uint16 i;
  for( i=0; i<NumOfTimers; i++) {
 // If too big timer id (enlarge numOfTimers in constructor!)
//  if (tmrId > NumOfTimers) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
    assert(!(i > NumOfTimers));

    if(TimerStore[i].Valid == false) continue;
    if(TimerStore[i].TimerBuffer == 0) continue;
    KernelAPI::StopTimer(TimerStore[i].TimerBuffer);
	  TimerStore[i].TimerBuffer = 0;
    LogStopTimer(i);
  }
  delete[] TimerStore;
  if(ReturnMemory == true){
    for( i=0; i<NumOfStates; i++)  {
      delete States[i];
    } //end for
    delete[] States;
  }//end if
}

void FiniteStateMachine::SetFSMStructures( SState **states){
  uint16 i;
  if(States != 0){
    if(ReturnMemory == true){
      for( i=0; i<NumOfStates; i++)  {
        delete States[i];
      } //end for
      delete[] States;
    }//end if
  }
  States = states;
  ReturnMemory = false;
  States = states;
}

// Function:  FreeFSM
// Parameters:   none
// Return value: none
// Description: 
//  Adds automate in list of free objects to be used for procfessing of next call.
// 
void FiniteStateMachine::FreeFSM(){
  FSMSystem::FreeFSM(this, GetAutomate());
}


// Function: Reset
// Parameters:   none
// Return value: none
// Description: 
//  Sets automate in initila state, and stops all timers. In case of need for something 
//  else to be done this function should be overiden. This function is used to place
//  autmate in initial state.
void FiniteStateMachine::Reset(void) {
    // set initial state
    SetInitialState();

    // stop all timers
    for (uint16 i = 0; i<NumOfTimers; i++) {
        if ( IsTimerRunning(i) ) StopTimer(i);
    }
}

// Function: InitTimerBlock
// Parameters:   
//  (in) uint16 tmrId    - value used for later timer identifying [0, numOfTimers-1]
//  (in) uint32 count    - timer value in ticks
//  (in) uint16 signalId - on expiry message is sent that has this event code
// Return value: none
// Description: 
//  Initalizes timers for use by FSM. With this function each timer that is going to be
//  used is defined. Later for timer control is used tmrId value. 
//  It is important to have tmrId values defined inside interval
//  0 to number of timers - 1
void FiniteStateMachine::InitTimerBlock(uint16 tmrId, uint32 count, uint16 signalId) {
  // If too big timer id (enlarge numOfTimers in constructor!)
  if (tmrId > NumOfTimers) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));

	TimerStore[tmrId].Valid       = true;
	TimerStore[tmrId].Count       = count;
  TimerStore[tmrId].SignalId    = signalId;
  TimerStore[tmrId].TimerBuffer = 0;
}

// Function: StartTimer
// Parameters:   
//  (in) uint16 tmrId - timer identifier [0, numOfTimers-1]
// Return value: none
// Description: 
//  Starts timer control. All data needed for timer are set earlier with function
//  InitTimerBlock.
void FiniteStateMachine::StartTimer(uint16 tmrId /*, uint8 *info*/){

  // If too big timer id (enlarge numOfTimers in constructor!)
//  if (tmrId > NumOfTimers) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
  assert(!(tmrId > NumOfTimers));

  // If timer block for given timer Id not initialized
//  if (TimerStore[tmrId].Valid == false) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
  assert(TimerStore[tmrId].Valid != false);
  //if timer already started error or should we call Stop Timer???
  assert(TimerStore[tmrId].TimerBuffer == 0);

	TimerStore[tmrId].TimerBuffer = KernelAPI::StartTimer(TimerStore[tmrId].SignalId,
	     TimerStore[tmrId].Count/*, info*/);
  LogStartTimer(tmrId);
}

// Function: StopTimer
// Parameters:   
//  (in) uint16 tmrId - timer identifier [0, numOfTimers-1]
// Return value: none
// Description: 
//  Stops timer control. If timer is already expired it does nithibg. 
//  All data needed for timer are set earlier with function
//  InitTimerBlock.
void FiniteStateMachine::StopTimer(uint16 tmrId){
  // If too big timer id (enlarge numOfTimers in constructor!)
//  if (tmrId > NumOfTimers) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
  assert(!(tmrId > NumOfTimers));

  // If timer block for given timer Id not initialized
//  if (TimerStore[tmrId].Valid == false) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
  assert(TimerStore[tmrId].Valid != false);

  if(TimerStore[tmrId].TimerBuffer == 0) return;
  KernelAPI::StopTimer(TimerStore[tmrId].TimerBuffer);
	TimerStore[tmrId].TimerBuffer = 0;
  LogStopTimer(tmrId);
}

// Function: ResetTimer
// Parameters:   
//  (in) uint16 tmrId - timer identifier [0, numOfTimers-1]
// Return value: none
// Description: 
//  Clears pointer to timer buffer after timer expired.
void FiniteStateMachine::ResetTimer(uint16 tmrId) {
	TimerStore[tmrId].TimerBuffer = 0;
}

// Function: IsTimerRunning
// Parameters:   
//  (in) uint16 tmrId - timer identifier [0, numOfTimers-1]
// Return value: 
//  bool - true if timer is running, false ir not.
// Description: 
//  Checks id timer is running. If timer is running it returns OK, if timer is
//  not running it returns false
bool FiniteStateMachine::IsTimerRunning(uint16 tmrId) {
  // If too big timer id (enlarge numOfTimers in constructor!)
//  if (tmrId > NumOfTimers) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
  assert(!(tmrId > NumOfTimers));

  // If timer block for given timer Id not initialized
//  if (TimerStore[tmrId].Valid == false) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
  assert(TimerStore[tmrId].Valid != false);

  if(TimerStore[tmrId].TimerBuffer == 0) return false;
  return KernelAPI::IsTimerRunning(TimerStore[tmrId].TimerBuffer);
}

void FiniteStateMachine::setTimerCount(uint16 tmrId, uint32 count){
  assert(!(tmrId > NumOfTimers));
  assert(TimerStore[tmrId].Valid != false);
  TimerStore[tmrId].Count       = count;
}

void FiniteStateMachine::SetDefaultFSMData() {
}

// Function: Process
// Parameters:   
//  (in) uint8 *msg - pointer on message
// Return value: none
// Description: 
//  Process message, determines branch that has to process message and
//  process it.
void FiniteStateMachine::Process(uint8 *msg) {
  if( ParseMessage(msg) == false ) return;
  LogAutomateEventStart(GetMsgCode());
  LogMessage( GetMsgInfoLength() + MSG_HEADER_LENGTH, msg);
  (this->*GetProcedure(GetMsgCode()))();
  ClearMessage();
  LogAutomateEventEnd();
}

// Function: GetProcedure
// Parameters:   
//  (in) uint16 event - event (message) code
// Return value: 
//  PROC_FUN_PTR - pointer on procdure
// Description: 
//  Obtains pointer to procedure to be executed upon event received,
//  for FSM entity given according to its current state
PROC_FUN_PTR FiniteStateMachine::GetProcedure(uint16 event) {

  // If state not initialized (no procedures initialized for it)
  assert( States[State]->StateValid ==true);

  for (unsigned short i=0; i< States[State]->NumOfBranches; i++)
    if (States[State]->PBranch[i].EventCode == event)
      return States[State]->PBranch[i].ProcPtr;

  // Only if event (procedure) not found
  return States[State]->UnexpectedEventProcPtr;
}

// Function: InitEventProc
// Parameters:   
//  (in) uint8  state      - FSM state for which procedure is to be applied
//  (in) uint16 event      - event for which procedure is to be applied
//  (in) PROC_FUN_PTR proc - procedure to be executed
// Return value: none
// Description: 
//  Binds procedure passed to be executed in state passed on event passed
//  This function is called from Init method in descendants.
void FiniteStateMachine::InitEventProc(uint8 state, uint16 event, PROC_FUN_PTR proc) {

  // After first procedure init state becomes valid
  if (States[state]->StateValid == false) {
    States[state]->StateValid = true;
    States[state]->UnexpectedEventProcPtr = (PROC_FUN_PTR)&FiniteStateMachine::DoNothing;
  }


#ifndef __NO_PARAMETER_CHECK__
  for (unsigned short i=0; i<States[state]->NumOfBranches; i++)
    if (States[state]->PBranch[i].EventCode == event)  // Same event code used twice!?!
           ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  // It is clever to init most frequently used branch first
  States[state]->PBranch[States[state]->NumOfBranches].EventCode = event;
  States[state]->PBranch[States[state]->NumOfBranches].ProcPtr   = proc;
  States[state]->NumOfBranches++;
  // If max. number of procedures per state reached (see constructor)
#ifndef __NO_PARAMETER_CHECK__
  if (States[state]->NumOfBranches > MaxNumOfProcPerState)
      ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
}

// Function: InitUnexpectedEventProc
// Parameters:   
//  (in) uint8  state      - FSM state for which procedure is to be applied
//  (in) PROC_FUN_PTR proc - procedure to be executed
// Return value: none
// Description: 
//  Binds procedure passed to be executed in state passed on unexpected event
//  This function is called from Init method in descendants. Default function for
//  this prcedure is DoNothing.
void FiniteStateMachine::InitUnexpectedEventProc(uint8 state, PROC_FUN_PTR pFun) {

  // First, event procedures must be set
#ifndef __NO_PARAMETER_CHECK__
  if (States[state]->StateValid == false)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  States[state]->UnexpectedEventProcPtr = pFun;
}

// Function:  NoFreeObjectProcedure (abstract)
// Parameters:   none
// Return value: none
// Description: 
//  
void FiniteStateMachine::NoFreeObjectProcedure(uint8 *msg){
//  LogNoFreeObjectsProcedureStart();
  if( ParseMessage(msg) == false ) return;
  LogMessage(GetMsgInfoLength() + MSG_HEADER_LENGTH, msg);
  NoFreeInstances();
  ClearMessage();
//  LogNoFreeObjectsProcedureEnd();
}
