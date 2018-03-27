// **************************************************************************
//                        timer.cpp  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  Contains code for CTimer class, which provides handling of timers. 
//  It uses delta list for storing them until timer expires. Each timer 
//  wetaher stopped or not will expire and will be add in mailbox. 
//  If timer is stooped it will only be marked as "to be discarded" and
//  handled later. 
//  file data:
//   Windows only:
//    uint16 TimerIdent - handle to timer, returned by the system.
//    CRITICAL_SECTION CsTimer - Critical sections to esure all operations on list.
//  Linux only:
//   int Proj - used for unique semaphore key generation.
//   SemaphoreId - handle for semaphore.
//   sembuf SemUnlock  = { 0, 1, IPC_NOWAIT} -  data used to unlock semaphore.
//   sembuf SemLock    = { 0,-1, !IPC_NOWAIT} - data used to lock semaphore.
//
//  Common data:
//  uint8 *Head - pointer on timer list head
//  uint8 DummyMessage[BUFF_HEADER_LENGTH] - buffer for dummy message.
// **************************************************************************

#ifdef WIN32
#include <windows.h>
#include <time.h>
#include <stdio.h>
#elif defined(__LINUX__)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <string.h>
#else
#endif

//#include <stdlib.h>
//#include <string.h>

#include "../kernel/errorObject.h"
#include "../kernel/timer.h"


//podaci semafora

#ifdef WIN32
uint32 TimerIdent;
static CRITICAL_SECTION CsTimer;
void CALLBACK TimerIntHandler(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

#elif defined(__LINUX__)
static int Proj                 = 0; //for semaphore key generation
static int SemaphoreId          = -1;
static struct sembuf SemUnlock  = { 0, 1, IPC_NOWAIT};
static struct sembuf SemLock    = { 0,-1, !IPC_NOWAIT};
#else
#endif

TBuffers    *CTimer::Buffers    = NULL;
TPostOffice *CTimer::PostOffice = NULL;

static uint8 *Head              = NULL;  // timer list head
static uint8 DummyMessage[BUFF_HEADER_LENGTH+MSG_HEADER_LENGTH+5];
//static uint8 TimersCount = 0;

// Function: Constructor
// Parameters: 
//  (in) TBuffers *buffers - pointer on object for memory handling.
//  (in) TPostOffice *postOffice - pointer on object for message exchange.
// Return value: none
// Description: 
//  Initialization of CTimer object, it is object used time control (timers)

// Function: Constructor
// Parameters: 
//  (in) TBuffers    *buffers    - pointer on buffers object (memory handling object)
//  (in) TPostOffice *postOffice - pointer on post office object (messages handling object)
// Return value: none
// Description: 
//  It initalizes object CTimer which is object for handling time controils.
CTimer::CTimer( TBuffers *buffers, TPostOffice *postOffice, TimerResolutionEnum timerRes){

  //validate parameters
#ifndef __NO_PARAMETER_CHECK__
  if (buffers    == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010400));
  if (postOffice == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010401));
#endif
  //initalize data
  Buffers    = buffers;
  PostOffice = postOffice;
  
//  T1sConst    = 1000/timerRes;
//  T100msConst =  100/timerRes;
//  T10msConst  =   10/timerRes;
//  T1msConst   =    1/timerRes;
  TimerResolution = timerRes;
/*  switch(timerRes){
    case Timer1ms:
      T1sConst    = 1000;
      T100msConst = 100;
      T10msConst  = 10;
      T1msConst   = 1;
      break;
    case Timer10ms:
      T1sConst    = 100;
      T100msConst = 10;
      T10msConst  = 1;
      T1msConst   = 0;
      break;
    case Timer100ms:
      T1sConst    = 10;
      T100msConst = 1;
      T10msConst  = 0;
      T1msConst   = 0;
      break;
    case Timer1s:
    default:
      T1sConst    = 1;
      T100msConst = 0;
      T10msConst  = 0;
      T1msConst   = 0;
      break;
  }*/



  //initilize first pointer in timer list, it is alwayes in list, it is never outqued,
  //by this, operations on queue are alwayes the same, at it adds to speed anf simplification
  //of code
//  Dummy = DummyMessage;
  Head  = DummyMessage + BUFF_HEADER_LENGTH;
#ifndef __NO_PREV_PTR__
  SetMemPtr(Head + BUFF_PREV, NULL);
#endif
  SetMemPtr(Head + BUFF_NEXT, NULL);
  SetUint32(Head + TIMER_COUNT, 0);
  InitTimerSystem(timerRes);
}

// Function: InitTymerSystem
// Parameters: none
// Return value: none
// Description: 
//  Initalizes timer platform dependent system, 
void CTimer::InitTimerSystem(uint16 timerRes){
// initalize semaphores and timer
#ifdef WIN32
  InitializeCriticalSection(&CsTimer);
  TimerIdent = timeSetEvent( timerRes, 20, TimerIntHandler, NULL, TIME_PERIODIC);
  if( TimerIdent == 0) throw TErrorObject( __LINE__, __FILE__, 0x01010802);
#elif defined(__LINUX__)
  key_t key;
  struct itimerval value, ovalue;
  struct sigaction newAction, oldAction;

  if(Proj > 255)  throw TErrorObject( __LINE__, __FILE__, 0x01010400);
  while( Proj < 256){
    key = ftok(".",(char)Proj);
    Proj++;
    if( (SemaphoreId = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666)) != -1) break; //kreiramo jedan semafor
  }//end while

  if( semctl( SemaphoreId, 0, SETVAL, 1) ==-1)throw TErrorObject( __LINE__, __FILE__, 0x0104);
// inicijalizacija tajmera
  newAction.sa_handler  = TimerIntHandler;
//  newAction.sa_mask     = 0;
  newAction.sa_flags    = SA_NOMASK | SA_RESTART;
  newAction.sa_restorer = NULL;

  if( sigaction(SIGALRM, &newAction, &oldAction) == -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
  value.it_interval.tv_sec  = 0;
  value.it_interval.tv_usec = 1000*timerRes;
  value.it_value.tv_sec     = 0;
  value.it_value.tv_usec    = 1000*timerRes;
  if( setitimer( ITIMER_REAL, &value, &ovalue)== -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
#else
#endif
}


// Function: Add
// Parameters: 
//  (in) uint8 *timer - pointer on the timer's buffer.
// Return value: none
// Description: 
//  It queues new timer in delta list of timers.
void CTimer::Add(uint8 *timer){
  uint8 *current, *prev;
  current = Head;
  prev    = NULL;

#ifdef __BUFFER_PROTECTION__
  if(timer[BUFF_TRACKING_DATA] != USER_FREE) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
  timer[BUFF_TRACKING_DATA] = USER_TIMER;
#endif

  DisableTimer();
  //find where to queue new timer. Timer list is delta list.
  while ( (GetUint32(timer + TIMER_COUNT) >= GetUint32(current + TIMER_COUNT))
                && ( GetMemPtr(current + BUFF_NEXT) != NULL) )
  {
    SetUint32(timer + TIMER_COUNT, GetUint32(timer + TIMER_COUNT) - GetUint32(current + TIMER_COUNT) );
    prev    = current;
    current = GetMemPtr(current + BUFF_NEXT);
  } //end while
  // queue before current
  if (GetUint32(timer+TIMER_COUNT) < GetUint32(current + TIMER_COUNT) ) { 
    SetUint32(current + TIMER_COUNT, GetUint32(current + TIMER_COUNT) - GetUint32(timer + TIMER_COUNT) );
    SetMemPtr(timer + BUFF_NEXT, current);
    SetMemPtr(prev + BUFF_NEXT, timer);
  } //end if
  else {   // queue after current - at end of list
    SetUint32(timer + TIMER_COUNT, GetUint32(timer + TIMER_COUNT) - GetUint32(current + TIMER_COUNT) );
    SetMemPtr(timer + BUFF_NEXT, NULL);
    SetMemPtr(current + BUFF_NEXT, timer);
  } //end else

  EnableTimer();
  return;
}

// Function: DisableTimer
// Parameters: none
// Return value: none
// Description: 
//  Entering the critical section. It protects from accessing thes same data
//  from two different fuctions in same time.
void CTimer::DisableTimer(){
#ifdef WIN32
  EnterCriticalSection(&CsTimer); 
#elif defined(__LINUX__)
  if(semop(SemaphoreId, &SemLock, 1) == -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
#else
#endif
}

// Function: EnableTimer
// Parameters: none
// Return value: none
// Description: 
//  Leaving the critical section. It protects from accessing thes same data
//  from two different fuctions in same time.
void CTimer::EnableTimer(){
#ifdef WIN32
  LeaveCriticalSection(&CsTimer); 
#elif defined(__LINUX__)
  if(semop(SemaphoreId, &SemUnlock, 1) == -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
#else
#endif
}

// Function: Purge
// Parameters: none
// Return value: none
// Description: 
//  Deletes all timers from queue.
void CTimer::Purge(){
  uint8 *tmp;
  DisableTimer();
  while ( GetMemPtr(Head + BUFF_NEXT) != NULL) {
    tmp  = GetMemPtr(Head + BUFF_NEXT);
    SetMemPtr(Head + BUFF_NEXT,  GetMemPtr( GetMemPtr(Head + BUFF_NEXT) + BUFF_NEXT) );
#ifdef __BUFFER_PROTECTION__
    if(tmp[BUFF_TRACKING_DATA] != USER_TIMER) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
    tmp[BUFF_TRACKING_DATA] = USER_FREE;
#endif
    Buffers->RetBuffer( tmp);
  } //end while
  EnableTimer();
}

/*
// Function: StartTimer
// Parameters: 
//  (in) uint32 count - duration of time control in time slices (currently in sceonds).
//  (in) uint8 mailBoxId - mailbox ID, where timer message is going to be queued after
//                         expiration.
//  (in) uint8 msgTo - To wich automate message is going to be send
//  (in) uint16 msgSignalID - message code.
//  (in) uint8 *info - pointer on inforamtion part of message, NULL if none.
// Return value: 
//  uint8 * - pointer on time control buffer.
// Description: 
//  Starts new timer with given data. It gets buffer for it. In case some parameter is
//  not valid, it throws exception.
uint8 *CTimer::StartTimer(uint32 count, uint8 mailBoxId, uint8 msgTo,
                    uint16 msgSignalID, uint8 *info, uint16 timerRes)
{
  uint8 *timerMessage;
  uint8 len;

  //vlidate parameters
  if (count == 0)               ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000403));
  if (msgSignalID == DISCARDED) ThrowError( TErrorObject( __LINE__, __FILE__, 0x0104));
  switch(timerRes){
    case Timer1ms:
      count *= T1msConst;
      break;
    case Timer10ms:
      count *= T10msConst;
      break;
    case Timer100ms:
      count *= T100msConst;
      break;
    case Timer1s:
    default:
      count *= T1sConst;
      break;
  }
  
  //get buffer for timer
  if (info == NULL) {
    timerMessage = Buffers->GetBuffer(MSG_HEADER_LENGTH);
    len          = 0;
  } //end if
  else {
    timerMessage = Buffers->GetBuffer(MSG_HEADER_LENGTH + info[0]);
    len          = info[0];
    memcpy( timerMessage + MSG_HEADER_END, info, *info);
  } //end else

  //set timer data
  memset( timerMessage, 0, MSG_HEADER_LENGTH);
  SetUint16(timerMessage + MSG_LENGTH, len);
  SetUint16(timerMessage + MSG_CODE, msgSignalID);
  *((uint32 *)(timerMessage + TIMER_COUNT)) = count;
  timerMessage[MSG_FROM_AUTOMATE]           = TIMER;
  timerMessage[MSG_TO_AUTOMATE]             = msgTo;
  timerMessage[TIMER_MBX_ID]                = mailBoxId;

  Add(timerMessage);
  return(timerMessage );
}
*/
// Function: StartTimer
// Parameters: 
//  (in) uint8 *timerMessage - pointer on timer buffer.
//  (in) uint8 mailBoxId - mailbox ID, where timer message is going to be queued after
//                         expiration.
//  (in) uint32 count - duration of time control in time slices (currently in sceonds).
// Return value: none
// Description: 
//  Starts new timer with given data. Timer buffer is given to this function and
//  header data are already set. In case some parameter is not valid, it throws 
//  exception.
void CTimer::StartTimer(uint8 *timerMessage, uint8 mailBoxId, uint32 count, TimerResolutionEnum timerRes){
#ifndef __NO_PARAMETER_CHECK__
  if (count == 0) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000403));
#endif
/*  switch(timerRes){
    case Timer1ms:
      count *= T1msConst;
      break;
    case Timer10ms:
      count *= T10msConst;
      break;
    case Timer100ms:
      count *= T100msConst;
      break;
    case Timer1s:
    default:
      count *= T1sConst;
      break;
  }*/
  count *= timerRes/TimerResolution;
  SetUint32(timerMessage + TIMER_COUNT, count);
  timerMessage[MSG_FROM_AUTOMATE] = TIMER;
  timerMessage[TIMER_MBX_ID]      = mailBoxId;
  Add(timerMessage);
}

// Function: RestartTimer
// Parameters: 
//  (in) uint8 *timerMessage - pointer on timer buffer.
//  (in) uint32 count - duration of time control in time slices (currently in sceonds).
// Return value: 
//   uint8 * - pointer on new timer.
// Description: 
//  Stops timer given to to function (timerMessage) and starts new one with the data
//  taken from stopped one. In case some parameter is not valid, it throws 
//  exception.
uint8 *CTimer::RestartTimer( uint8 *timerMessage, uint32 count, TimerResolutionEnum timerRes){
#ifndef __NO_PARAMETER_CHECK__
  if (count == 0)           ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000406));
  if (timerMessage == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000407));
  if ( GetUint16(timerMessage + MSG_CODE) == DISCARDED)
                            ThrowError( TErrorObject( __LINE__, __FILE__, 0x21090408));
#endif
/*  switch(timerRes){
    case Timer1ms:
      count *= T1msConst;
      break;
    case Timer10ms:
      count *= T10msConst;
      break;
    case Timer100ms:
      count *= T100msConst;
      break;
    case Timer1s:
    default:
      count *= T1sConst;
      break;
  }*/
  count *= timerRes/TimerResolution;
  uint8 *tmp = Buffers->GetBuffer(MSG_HEADER_LENGTH + GetUint16(timerMessage + MSG_LENGTH));
  memset( tmp, 0, MSG_HEADER_LENGTH);
  memcpy( tmp, timerMessage , MSG_HEADER_LENGTH + GetUint16(timerMessage + MSG_LENGTH) );

  SetUint32(tmp + TIMER_COUNT, count);
  Add( tmp);
  StopTimer(timerMessage);
  return( tmp);
}

// Function: StopTimer
// Parameters: 
//  (in) uint8 *timerMessage - pointer on timer buffer.
// Return value: none
// Description: 
//  Stops timer. Timer is stopped by seting message code data on DISCARDED. When this
//  control expires, it will be queued in mailbox, and there it will be discarded.
void CTimer::StopTimer(uint8 *timerMessage){
  if (timerMessage == NULL) return;
  SetUint16(timerMessage + MSG_CODE, DISCARDED);
}


// Function: TimerRunning
// Parameters: 
//  (in) uint8 *timerMessage - pointer on timer buffer.
// Return value: 
//  bool - true if timer is not stopped (DISCARDED) else false.
// Description: 
//  Returns true if timer is not stopped (DISCARDED) else false. Timer is 
//  considered running even if it is in mailbox, as long as it's not marked discarded.
bool CTimer::TimerRunning(uint8 *timerMessage){
#ifndef __NO_PARAMETER_CHECK__
  if (timerMessage == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000409));
#endif
  if( GetUint16(timerMessage + MSG_CODE ) == DISCARDED) return(false);
  else  return(true);
}

// Ime Funkcije:       IsEmpty
// Ulazni parametri:   nema
// Izlazni parametari: nema
// Opis: Vraca true ako niz vremenskih kontrola nije prazan.

// Function: IsEmpty
// Parameters: none
// Return value: 
//  bool - true if there is timers in list.
// Description: 
//  Checks weather timer queue is empty. If it is, returns true, othervise false.
bool CTimer::IsEmpty(){
  bool tmp;
  DisableTimer();
  if( GetMemPtr(Head + BUFF_NEXT) == NULL) tmp = true;
  else                                     tmp = false;
  EnableTimer();
  return tmp;
}


void CTimer::DeinitTimerSystem(){
#ifdef WIN32
  DeleteCriticalSection(&CsTimer);
  if(timeKillEvent(TimerIdent) != TIMERR_NOERROR ){
    throw TErrorObject( __LINE__, __FILE__, 0x0101080f);
  }
#elif defined(__LINUX__)
  struct itimerval value, ovalue;
  value.it_interval.tv_sec  = 0;
  value.it_interval.tv_usec = 0;
  value.it_value.tv_sec     = 0;
  value.it_value.tv_usec    = 0;
  if( setitimer( ITIMER_REAL, &value, &ovalue) == -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
  semctl(SemaphoreId, 0, IPC_RMID, 0);
#else
#endif
}

// Function: destructor
// Parameters:   none
// Return value: none
// Description: 
//  Stops all timers and returns buffers to TBuffers object.
CTimer::~CTimer(){
  Purge();
//Deinitalize tajmer
  DeinitTimerSystem();
}


// Function: TimerIntHandler
// Parameters: not used
// Return value: none
// Description: 
//  Count ticks and checks weather some timer is expired. When timer expires it's
//  outqueued from timer delta list and queued in apropirate mailbox.
#ifdef WIN32
void CALLBACK TimerIntHandler(UINT , UINT , DWORD , DWORD , DWORD )  
#elif defined(__LINUX__)
void TimerIntHandler(int)
#else
void TimerIntHandler(int)
#endif
{
  uint8 *tmp;

#ifdef WIN32
//  char timeStr[100];
//  _strtime(timeStr);
//  printf("time: %s \n",timeStr);

  EnterCriticalSection(&CsTimer);
#elif defined(__LINUX__)
//  struct itimerval value, ovalue;
//  value.it_interval.tv_sec  = 100;
//  value.it_interval.tv_usec = 0;
//  value.it_value.tv_sec     = 1;
//  value.it_value.tv_usec    = 0;
//  if( setitimer( ITIMER_REAL, &value, &ovalue) == -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
  if( semop(SemaphoreId, &SemLock, 1)          == -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
#else
#endif
  tmp = GetMemPtr(Head + BUFF_NEXT);
  if (tmp != NULL)  {
    SetUint32(  tmp + TIMER_COUNT, GetUint32(tmp + TIMER_COUNT)-1 );
    while ( GetUint32( GetMemPtr(Head + BUFF_NEXT) + TIMER_COUNT) == 0) {  // while time out
      tmp = GetMemPtr(Head + BUFF_NEXT);
      SetMemPtr(Head + BUFF_NEXT, GetMemPtr( GetMemPtr(Head + BUFF_NEXT) + BUFF_NEXT) );

#ifdef __BUFFER_PROTECTION__
  if(tmp[BUFF_TRACKING_DATA] != USER_TIMER) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
  tmp[BUFF_TRACKING_DATA] = USER_FREE;
#endif

      CTimer::PostOffice->Add( tmp[TIMER_MBX_ID] , tmp);
      if ( GetMemPtr(Head + BUFF_NEXT) == NULL)   break;
    }
  }
#ifdef WIN32
  LeaveCriticalSection(&CsTimer); 
#elif defined(__LINUX__)
  if(semop(SemaphoreId, &SemUnlock, 1) == -1) throw TErrorObject( __LINE__, __FILE__, 0x0104);
#else
#endif
}
