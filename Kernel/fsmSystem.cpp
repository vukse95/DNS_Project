// **************************************************************************
//                        fsmSystem.cpp  -  description
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
#ifdef WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <stdio.h>

#ifdef __ARM_CORE__
#include <iostream>
#else
#include <iostream>
#endif

#include "../kernel/errorObject.h"
#include "fsmSystem.h"

TPostOffice *FSMSystem::PostOffice = 0;
TBuffers    *FSMSystem::Buffers    = 0;
CTimer      *FSMSystem::Timer      = 0;
bool    FSMSystem::UseFreeAutomateList[MAX_NUM_OF_AUTOMATES_TYPES];
FSMList FSMSystem::FreeAutomates[MAX_NUM_OF_AUTOMATES_TYPES];



FSMList::FSMList() {
	Head   = 0;
	Tail   = 0;
	
	FreeFSMCount = 0;
}


ptrFiniteStateMachine FSMList::GetFromHead() {
  ptrFiniteStateMachine info;
  if (Head == NULL)   return(NULL);

  info = Head;
  Head = info->Next;
  info->Next = 0;
  if (Head == NULL) Tail = NULL;

  return(info);
}

void FSMList::PutOnTail(ptrFiniteStateMachine info) {
  info->Next = 0;
  if (Tail == NULL) Head = info;
  else Tail->Next = info;
  Tail = info;
}

void FSMList::Add(ptrFiniteStateMachine info) {
#ifndef __NO_PARAMETER_CHECK__
  if (info == NULL)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
#endif
  PutOnTail( info);
  FreeFSMCount++;
}


ptrFiniteStateMachine FSMList::Get() {
  ptrFiniteStateMachine info;
#ifndef __NO_PARAMETER_CHECK__
  if ( ( (info = GetFromHead()) == NULL) || (FreeFSMCount == 0) )  ThrowError( TErrorObject( __LINE__, __FILE__, 0x21030504));
#endif
  FreeFSMCount--;
  return ( info);
}

bool FSMList::IsEmpty() {
  return (FreeFSMCount == 0 ? true : false);
}

FSMList::~FSMList() {
}

// Function: FSMSystem (Constructor)
// Parameters:   
//  (in) uint32 numOfAutomates - number of autmates type in system (size of array)
//  (in) uint32 numberOfMbx    - number of mail boxes in system
// Return value: none
// Description: 
//  Gets memory and initializes data structures..
FSMSystem::FSMSystem(uint8 numOfAutomates, uint8 numberOfMbx) {
  //set number of different autmates types there must be at least one
#ifndef __NO_PARAMETER_CHECK__
  if (numOfAutomates == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  if (numOfAutomates > MAX_NUM_OF_AUTOMATES_TYPES) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
#endif
  NumberOfAutomates = numOfAutomates;

//  //Create array of pointers on autmates of diferent types, it is 'array of arrays'
//  Automates = new ptrFiniteStateMachine[NumberOfAutomates];
//  if(Automates == NULL) throw TErrorObject( __LINE__, __FILE__, 0x01010200);

  //create array that defines size of arrays of automates types
//  NumberOfObjects = new uint32[NumberOfAutomates];
//  if(NumberOfObjects == NULL) throw TErrorObject( __LINE__, __FILE__, 0x01010200);

  //set number of mail boxes to be checked for messages
  if (numberOfMbx == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  NumberOfMbx = numberOfMbx;

  // initalize memory
  for(uint8 i=0; i<NumberOfAutomates; i++){
    Automates[i]       = 0;
    NumberOfObjects[i] = 0;
    UseFreeAutomateList[i] = false;
  }
  FreeKernelMemory = false;
  SystemWorking    = false;
  //create lsit of free objects
//  FreeAutomates = new FSMList[NumberOfAutomates];
}

// Function: InitKernel 
// Parameters:   
//  (in) uint8  buffClassNo   - number of different sizes of buffers, size of
//                              array buffersCount
//  (in) uint32 buffersCount  - array of numbers of buffers of each size
//  (in) uint32 buffersLength - array of size for each bufers class
//  (in) uint32 numOfMbxs     - Additional numer of mbx's that is not going to be used
//                              by the system.
// Return value: none
// Description: 
//  Creates kernel objects.
void FSMSystem::InitKernel( uint8 buffClassNo, uint32 *buffersCount,
         uint32 *buffersLength, uint8 numOfMbxs, TimerResolutionEnum timerRes ) {
  Buffers = new TBuffers(buffClassNo, buffersCount, buffersLength);
  if( Buffers ==  NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  PostOffice = new TPostOffice(NumberOfMbx+numOfMbxs, Buffers);
  if( PostOffice ==  NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  Timer = new CTimer( Buffers, PostOffice, timerRes);
  if( Timer ==  NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  FiniteStateMachine::SetKernelObjects(PostOffice, Buffers, Timer);

  FreeKernelMemory = true;
}

void FSMSystem::FreeFSM(ptrFiniteStateMachine object, uint8 automateType){
  if( UseFreeAutomateList[automateType] == true ) FreeAutomates[automateType].Add(object);
}

// Function: Add 
// Parameters:   
//  (in) ptrFiniteStateMachine object - first object in array of autmates of some type
//  (in) uint32 numOfObjects          - number of objects in array (size of array)
//  (in) uint32 automateType          - type of autoamte, index in array of Automates and
//                                      NumberOfObjects
// Return value: none
// Description: 
//  Gets memory and initializes object.
void FSMSystem::Add(ptrFiniteStateMachine object, uint8 automateType){

  assert(!(NumberOfAutomates <= automateType));
  assert( Automates[automateType] != 0);
  assert( NumberOfObjects[automateType] != 0);

  for(uint32 i=0; i <  NumberOfObjects[automateType]; i++){
    if( Automates[automateType][i] == 0) {
      Automates[automateType][i] = object;
      Automates[automateType][i]->SetObjectId(i);
			Automates[automateType][i]->SetGroup(serverId);
      Automates[automateType][i]->Initialize();
      if( UseFreeAutomateList[automateType] == true ) FreeAutomates[automateType].Add(object);
      return;
    } //end if
  }   //end for
  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
}

// Function: Add 
// Parameters:   
//  (in) ptrFiniteStateMachine object - first object in array of autmates of some type
//  (in) uint32 numOfObjects          - number of objects in array (size of array)
//  (in) uint32 automateType          - type of autoamte, index in array of Automates and
//                                      NumberOfObjects
// Return value: none
// Description: 
//  Gets memory and initializes object.
void FSMSystem::Add(ptrFiniteStateMachine object, uint8 automateType, uint32 numOfObjects,
                   bool useFreeList)
{
//  assert(objects);
  assert(numOfObjects);
  assert(!(NumberOfAutomates <= automateType));

  assert( Automates[automateType] == 0);
  assert( NumberOfObjects[automateType] == 0);


  NumberOfObjects[automateType] = numOfObjects;
  Automates[automateType]       = new ptrFiniteStateMachine[numOfObjects];
  Automates[automateType][0]    = object;
  Automates[automateType][0]->SetObjectId(0);
	Automates[automateType][0]->SetGroup(serverId);
  Automates[automateType][0]->Initialize();
  UseFreeAutomateList[automateType] = useFreeList;
  if( UseFreeAutomateList[automateType] == true ) FreeAutomates[automateType].Add(object);
  for(uint32 i=1; i<NumberOfObjects[automateType]; i++){
    Automates[automateType][i] = 0;
  }
}


// Function: Remove 
// Parameters:   
//  (in) uint32 automateType - type of autoamte, index in array of Automates and
//                                      NumberOfObjects
// Return value: none
// Description: 
//  Removes all automates of given type. After this command there is non of them in array.
//  memory for array is also released
void FSMSystem::Remove( uint8 automateType){

  assert( NumberOfAutomates >= automateType);
  if(Automates[automateType] == 0) return;
//  assert( Automates[automateType] != 0);
  assert( NumberOfObjects[automateType] != 0);

  for(uint32 i=0; i<NumberOfObjects[automateType]; i++){
    Automates[automateType][i] = 0;
  }
  NumberOfObjects[automateType]  = 0;
  delete []Automates[automateType];
}

// Function: Remove 
// Parameters:   
//  (in) uint32 automateType - type of autoamte, index in array of Automates and
//                                      NumberOfObjects
//  (in) uint32 object       - index of object in array
// Return value: none
// Description: 
//  Removes object from array.
ptrFiniteStateMachine FSMSystem::Remove( uint8 automateType, uint32 object){
  ptrFiniteStateMachine tmp;
  
  assert( NumberOfAutomates <= automateType);
  assert( Automates[automateType] != 0);
  assert( NumberOfObjects[automateType] != 0);

  tmp = Automates[automateType][object];
  Automates[automateType][object] = 0;
  return tmp;
}

// Function: Start 
// Parameters:   none
// Return value: none
// Description: 
//  Monitor loop of autmate system. It takes message from queue, find which autmate should 
//  process it and executes processing.
void FSMSystem::Start(){
  SystemWorking = true;
  uint8 mailBox;

  while(SystemWorking) {
    for(uint8 i=0; i<NumberOfMbx; i++) {
//      uint8 *msg = GetMsg(i);
      uint8 *msg = GetMsgSync(mailBox);
      if(msg == NULL){
#ifdef WIN32
				Sleep(10);
#endif
				continue;
			}
      uint8 automateType = GetMsgToAutomate(msg);
      if( (automateType > NumberOfAutomates) || (NumberOfObjects[automateType] == 0) ){
        //ReportError();
        DiscardMsg(msg);
        continue;
      }
      uint32 objNum = GetMsgObjectNumberTo(msg);
      if( objNum == 0xffffffff){
        ptrFiniteStateMachine object= FreeAutomates[automateType].Get();
        if( object != 0) object->Process(msg);
        else             (Automates[automateType][0])->NoFreeObjectProcedure(msg);
        continue;
      }
      else if(objNum > NumberOfObjects[automateType]) {
        //ReportError();
        DiscardMsg(msg);
        continue;
      } 
      else {
        (Automates[automateType][objNum])->Process(msg);
      }
    }
  }
}

FSMSystem::~FSMSystem() {
  for(uint8 i=0; i < NumberOfAutomates; i++){
    Remove(i);
  }
  if(FreeKernelMemory == true){
    delete Timer;
    Timer = 0;
    delete PostOffice;
    PostOffice = 0;

    delete Buffers;
    Buffers = 0;
  }
//  delete []FreeAutomates;
//  delete []NumberOfObjects;
//  delete []Automates;
}

//usewd only for pbx debuging
#include "kernelConsts.h"
/*#include "..\ccConsts.h"
void FSMSystem::TestMessage(){
  uint8 *buff;

  buff=Buffers->GetBuffer(30);

  buff[MSG_FROM_AUTOMATE]                   =0x00;
  //buff[MSG_FROM_OBJECT]                     =0x00;
  buff[MSG_OBJECT_ID_FROM]                     =0x00;
  buff[MSG_TO_AUTOMATE]                     =AG_FSM;
  //buff[MSG_TO_OBJECT]                       =0x00;
  buff[MSG_OBJECT_ID_TO]                       =0x00;  
  *((uint16 *)(buff+MSG_CODE))              =AGENT_SET_STATE;
// *((uint32 *)(buff+OBJECT_MSG_NUMBER_FROM))=INVALID_32;
// *((uint32 *)(buff+OBJECT_MSG_NUMBER_TO))  =0x01;
  *((uint32 *)(buff+CALL_ID))               =0x00;
  buff[MSG_INFO_CODING]                     =0x00;
  *((uint16 *)(buff+MSG_LENGTH))            =0x0000;
  PostOffice->Add(AG_MBX_ID, buff);

}

*/