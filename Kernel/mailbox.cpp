// **************************************************************************
//                        mailbox.cpp  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  This file contains code for TMailBox class. It is a list of messages for automate.
//  It knows how to handle messages that are in buffers provided by TBuffers class.
// **************************************************************************

#ifdef WIN32
//included in h file
// #include <windows.h>
#elif defined(__LINUX__)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#else

#endif

#include <stdlib.h>


#include "../kernel/mailbox.h"
#include "../kernel/errorObject.h"

#ifdef WIN32
#elif defined(__LINUX__)
int TMailBox::SemsUsed=0;
int TMailBox::SemaphoreId=-1;
extern int errno;
#else
#endif

// Function: Constructor
// Parameters: 
//  (in) TBuffers *buffers - pointer on object for memory handling.
// Return value: none
// Description: 
//  Initialize object TMailBox, it contains messsages for single automate.
TMailBox::TMailBox(TBuffers *buffers){

  //validate data
  if( buffers == NULL)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010200));
  //initalize constants
  Buff = buffers;
  Head = Tail = NULL;
  Count = 0;
  InitilazieSystemDependent();
}

// Function: InitilazieSystemDependent
// Parameters:   none
// Return value: none
// Description: 
//  Initializes system dependent functionality, concurent approach data (critical section
//  and semaphores)
void TMailBox::InitilazieSystemDependent(){
  //initalise semaphores (critical section in case of WIN32)
#ifdef WIN32
  InitializeCriticalSection(&CsMailBox);
#elif defined(__LINUX__)
  key_t key;
  if(SemaphoreId == -1) {
    int cc=0;
  //each semaphore needs different key
    while( cc<256){
      key = ftok(".", (char) cc++);
      if( (SemaphoreId = semget(key, MAX_MAILBOXES_NUM, IPC_CREAT | IPC_EXCL | 0666)) != -1) break; //kreiramo jedan semafor
      int xx=errno;
      printf("errne: %d", xx);

    } //end while
    if(SemaphoreId == -1) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
    for(int semNum=0; semNum<MAX_MAILBOXES_NUM; semNum++){
      if( semctl( SemaphoreId, semNum, SETVAL, 1) ==-1)  throw TErrorObject( __LINE__, __FILE__, 0x01010200);
    }
  }
  SemUnlock.sem_num = SemsUsed;
  SemUnlock.sem_op  = 1;
  SemUnlock.sem_flg = IPC_NOWAIT;
  SemLock.sem_num   = SemsUsed;
  SemLock.sem_op    = -1;
  SemLock.sem_flg   = !IPC_NOWAIT;
  SemsUsed++;
#else
#endif  
}

// Function: DeinitilazieSystemDependent
// Parameters:   none
// Return value: none
// Description: 
//  Deinitializes system dependent functionality, concurent approach data (critical section
//  and semaphores)
void TMailBox::DeinitilazieSystemDependent(){
#ifdef WIN32
  DeleteCriticalSection( &CsMailBox);
#elif defined(__LINUX__)
  semctl(SemaphoreId, SemUnlock.sem_num, IPC_RMID, 0);
  if(SemUnlock.sem_num == 0){
    for(int i=SemsUsed; i<MAX_MAILBOXES_NUM;i++){
      semctl(SemaphoreId, i, IPC_RMID, 0);
    }
  }
#else
#endif
}

// Function: MbxLock
// Parameters:   none
// Return value: none
// Description: 
//  Locks the mailbox, so that concurrent processes can use it.
void TMailBox::MbxLock() {
#ifdef WIN32
  EnterCriticalSection(&CsMailBox);
#elif defined(__LINUX__)
  if(semop(SemaphoreId, &SemLock, 1) == -1){
    printf("MbxLock errno==%d", errno);
    throw TErrorObject( __LINE__, __FILE__, 0x01010201);
  }
#else
#endif
}

// Function: MbxUnlock
// Parameters:   none
// Return value: none
// Description: 
//  Unlocks the mailbox, so that concurrent processes can use it.
void  TMailBox::MbxUnlock(){
#ifdef WIN32
  LeaveCriticalSection(&CsMailBox);
#elif defined(__LINUX__)
 if(semop(SemaphoreId, &SemUnlock, 1) == -1)  throw TErrorObject( __LINE__, __FILE__, 0x01010201);
#else
#endif
}

// Function: GetFromHead
// Parameters: none
// Return value: 
//  uint8* - pointer on message.
// Description: 
//  Outques message from head of list and returns pointer to it. If there is no 
//  messages, returns NULLpointer.
uint8 *TMailBox::GetFromHead(){
  uint8 *info;
  MbxLock();
  if (Head == NULL)  {
    MbxUnlock();
    return(NULL);
  }

  info = Head;
  Head = GetMemPtr(Head + BUFF_NEXT);
  if (Head == NULL)   Tail = NULL;
  Count--;
  MbxUnlock();
  return(info);
}

/*
// Function: PutOnHead
// Parameters:
//  uint8* - pointer on message.
// Return value: none
// Description: 
//  Ques message in list at the begening of list, in fornt of all other messages. This
//  way, it will be the next message to be processed.
void TMailBox::PutOnHead(uint8 *info){
  MbxLock();
  SetMemPtr(info + BUFF_NEXT, Head);
  if ( Head == NULL) Tail = info;
  Head = info;
  Count++;
  MbxUnlock();
}

*/
// Function: PutOnTail
// Parameters:
//  uint8* - pointer on message.
// Return value: none
// Description: 
//  Ques message in list at the end of list.
void TMailBox::PutOnTail(uint8 *info){
  MbxLock();
  SetMemPtr(info + BUFF_NEXT, NULL);
  if (Tail == NULL) Head = info;
  else              SetMemPtr(Tail + BUFF_NEXT, info);
  Tail = info;
  Count++;
  MbxUnlock();
}

// Ime Funkcije:       AddFromInterrupt
// Ulazni parametri:   uint8 * - pokazivac na poruku
// Izlazni parametari: nema
// Opis: Ulancava poruku na kraj liste (iza ostalih poruka). Funkcija ne zabranjuje prekide
//       tj. predvidjena je za rad iz kriticnih sekcija. U slucaju NULL pokazivaca na poruku
//       generise izuzetak (exception).
//void TMailBox::AddFromInterrupt(uint8 *info)
//{
// if (info == NULL) panic(__FILE__, __LINE__);
//
// PutOnTail(info);
//}


// Function: Add
// Parameters:
//  uint8* - pointer on message.
// Return value: none
// Description: 
//  Ques message in list at the end of list. This is public function, to be used by
//  other objects. In case of NULL pointer it throws exception.
void TMailBox::Add(uint8 *info){
  if (info == NULL)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010202));

#ifdef __BUFFER_PROTECTION__
  if(info[BUFF_TRACKING_DATA] != USER_FREE) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
  info[BUFF_TRACKING_DATA] = USER_MBX;
#endif

  PutOnTail(info);
}

/*
// Function: Add
// Parameters:
//  uint8* - pointer on message.
// Return value: none
// Description: 
//  Ques message in list at the end of the list, after all other messages.
//  This is public function, to be used by other objects. In case of NULL
//  pointer it throwsexception.
void TMailBox::AddOnHead(uint8 *info){
  if (info == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010202));

#ifdef __BUFFER_PROTECTION__
  if(info[BUFF_TRACKING_DATA] != USER_FREE) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
  info[BUFF_TRACKING_DATA] = USER_MBX;
#endif

  PutOnHead(info);
}


*/
// Function: Get
// Parameters: none
// Return value: 
//  uint8* - pointer on message.
// Description: 
//  Outques message from head of list and returns pointer to it. If there is no 
//  messages, returns NULL pointer. If message is signed as DISCARDED, then it
//  returns it's buffer in list of free buffers.
uint8* TMailBox::Get(){
  uint8 *info;

  info = GetFromHead();
  if( info == NULL) return NULL;
  while ( GetUint16(info + MSG_CODE) == DISCARDED) {
#ifdef __BUFFER_PROTECTION__
  if(info[BUFF_TRACKING_DATA] != USER_MBX) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
  info[BUFF_TRACKING_DATA] = USER_FREE;
#endif
    Buff->RetBuffer(info);
    info = GetFromHead();
    if(info == NULL) return NULL;
  }  //end while
#ifdef __BUFFER_PROTECTION__
  if(info[BUFF_TRACKING_DATA] != USER_MBX) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21010503));
  info[BUFF_TRACKING_DATA] = USER_FREE;
#endif
  return info;
}


// Function: QueryHead
// Parameters: none
// Return value: 
//  uint8* - pointer on message.
// Description: 
//  Returns pointer on first message in the list, but does not outques it.
uint8 *TMailBox::QueryHead(){
  uint8 *tmp;
  MbxLock();
  tmp = Head;
  MbxUnlock();
  return tmp;
}


// Function: GetCount
// Parameters: none
// Return value: 
//  uint32 - number of messages in list.
// Description: 
//  Returns number of messages in mailbox.
uint32 TMailBox::GetCount(){
  uint32 ret;
  MbxLock();
  ret = Count;
  MbxUnlock();
  return(ret);
}


// Function: Purge
// Parameters:   none
// Return value: none
// Description: 
//  Deletes all messages from mailbox, and returns buffers to TBuffers object.
void TMailBox::Purge(){
  while( Head != NULL)  {
    uint8 *tmp = Get();
    if (tmp != NULL)
     Buff->RetBuffer(tmp);
  }//end while
}


// Function: destructor
// Parameters:   none
// Return value: none
// Description: 
//  Deletes all messages from mailbox, and returns buffers to TBuffers object.
TMailBox::~TMailBox(){
  Purge();
  DeinitilazieSystemDependent();
}
