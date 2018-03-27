// **************************************************************************
//                        buffersQueue.cpp  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Description:
//  This file contains code for TbufferQue class used for handling buffers of 
//  determined size.
// **************************************************************************

#include <assert.h>
//#include <stdio.h>
//#include <iostream.h>

#include "../kernel/buffersQueue.h"
#include "../kernel/errorObject.h"

#ifdef __LINUX__
int TBufferQue::SemsUsed=0;
int TBufferQue::SemaphoreId=-1;
#endif

// Function: Constructor
// Parameters: 
//  (in) uint32 buffersNo    - number of buffers that is going to be created.
//  (in) uint32 bufferLength - length of buffers that are going to be created.
//  (in) uint8  bufferCode   - index in array of buffers list in TBuffers object.
// Return value: none
// Description: 
//  Initialization of object TBufferQue, which is list of buffers all of size that is
//  defined. For each buffer data about in which list it belongs, pointers for next and
//  previous in lists are stored in front of pointer on buffer that is returned to
//  application. FOr more details see kernelConsts.h
TBufferQue::TBufferQue(uint32 buffersNo, uint32 bufferLength, uint8 bufferCode){
  uint32 memSize;
  uint8 *tmp;

  //validate data if error throw an exception
#ifndef __NO_PARAMETER_CHECK__
  if( buffersNo    == 0 ) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010500));
  if( bufferLength == 0)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010501));
#endif

  //calcute size of memory for all buffers and gert memory for them
  memSize = bufferLength + BUFF_HEADER_LENGTH;
  memSize *= buffersNo;
  BufferPtr = new uint8[memSize];
  if( BufferPtr == NULL)  ThrowError( TErrorObject( __LINE__, __FILE__, 0x01010501));

#ifdef WIN32
  InitializeCriticalSection(&CsBuffer);
#elif defined(__LINUX__)
  key_t key;
  if(SemaphoreId == -1) {
    int cc=0;
  //each semaphore needs different key
    while( cc<256){
      key = ftok(".", (char) cc++);
      if( (SemaphoreId = semget(key, MAX_BUFF_CLASS_NO, IPC_CREAT | IPC_EXCL | 0666)) != -1) break; //kreiramo jedan semafor
    } //end while
    if(SemaphoreId == -1) throw TErrorObject( __LINE__, __FILE__, 0x01010200);
    for(int semNum=0; semNum<MAX_BUFF_CLASS_NO; semNum++){
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
#endif  

  //trunck memory in to approriate buffers
  FreeBufferCount  = 0;
  BuffersInitiated = buffersNo;
  Head = Tail = NULL;
  tmp  = BufferPtr;
  for( uint32 i=0; i<buffersNo; i++) {
    tmp += BUFF_HEADER_LENGTH;
    tmp[BUFF_CODE]          = bufferCode;

#ifdef __BUFFER_PROTECTION__
    tmp[BUFF_TRACKING_DATA] = USER_FREE;
#endif

#ifndef __NO_PREV_PTR__
    SetMemPtr(tmp + BUFF_PREV, NULL);
#endif
    SetMemPtr(tmp + BUFF_NEXT, NULL);
    Add(tmp);
    tmp += bufferLength;
  }//end for i
  //initalise semaphores (critical section in case of WIN32)
}

// Function: LockBuff
// Parameters:   none
// Return value: none
// Description: 
//  Locks the buufer queue, so that concurrent processes can use it.
void TBufferQue::LockBuff() {
#ifdef WIN32
  EnterCriticalSection(&CsBuffer);
#elif defined(__LINUX__)
 if(semop(SemaphoreId, &SemLock, 1) == -1)  throw TErrorObject( __LINE__, __FILE__, 0x01010201);
#endif
}

// Function: UnlockBuff
// Parameters:   none
// Return value: none
// Description: 
//  Unlocks the buufer queue, so that concurrent processes can use it.
void  TBufferQue::UnlockBuff(){
#ifdef WIN32
  LeaveCriticalSection(&CsBuffer);
#elif defined(__LINUX__)
 if(semop(SemaphoreId, &SemUnlock, 1) == -1)  throw TErrorObject( __LINE__, __FILE__, 0x01010201);
#endif
}



// Function: GetFromHead
// Parameters: none
// Return value: 
//  uint8* - pointer to buffer.
// Description: 
//  Returns pointer to free buffer. In case there is no more free buffers, it returns
//  NULL pointer. 
uint8 *TBufferQue::GetFromHead(){
  uint8 *info;
  LockBuff();
  if (Head == NULL)   return(NULL);
  info = Head;
  Head = GetMemPtr(Head + BUFF_NEXT);
  if (Head == NULL)   Tail = NULL;
  FreeBufferCount--;
  UnlockBuff();

  return(info);
}


// Function: PutOnTail
// Parameters: 
//  uint8* info - pointer on buffer.
// Return value: none
// Description: 
//  Returns buffer in list of free buffers. It does not check weather pointer
//  to buffer is NULL pointer. This is private function and check on NULL pointer
//  is done somewhere else.
void TBufferQue::PutOnTail(uint8 *info){
  SetMemPtr(info + BUFF_NEXT, NULL);
  LockBuff();
  if   (Tail == NULL) Head = info;
  else SetMemPtr(Tail + BUFF_NEXT, info);
  Tail = info;
  FreeBufferCount++;
  UnlockBuff();
}


// Function: Add
// Parameters:
//  uint8* info - pointer to buffer.
// Return value: none
// Description: 
//  Returns buffer in list of free buffers. It does not check for NULL pointer
//  because it is done in class TBuffers in file buffers.cpp, If ever someone use it
//  separatly it has to uncoment line below.
void TBufferQue::Add(uint8 *info){
  assert(info != NULL);
#ifdef __BUFFER_PROTECTION__
  if(info[BUFF_TRACKING_DATA] != USER_FREE) ThrowError(  TErrorObject( __LINE__, __FILE__, 0x21010503));
  info[BUFF_TRACKING_DATA] = USER_BUFFERS;
#endif
  PutOnTail( info);
}


// Function: Get
// Parameters: none
// Return value:
//  uint8 * - pointer on buffer
// Description: 
//  Gets buffer from list. If there is no more free buffers, it returns NULL pointer
uint8 *TBufferQue::Get(){
  uint8 *info;
  info = GetFromHead();
  assert(info != NULL);
#ifdef __BUFFER_PROTECTION__
  if(info[BUFF_TRACKING_DATA] != USER_BUFFERS) ThrowError(  TErrorObject( __LINE__, __FILE__, 0x21010503));
  info[BUFF_TRACKING_DATA] = USER_FREE;
#endif
  return ( info);
}

// Function: IsEmpty
// Parameters: none
// Return value:
//  bool - true if there are buffers in list, othervice false.
// Description: 
//  Function checks weather there are buffers in list or no. 
bool TBufferQue::IsEmpty(){
  bool retValue;
  LockBuff();
  retValue = (FreeBufferCount == 0 ? true : false);
  UnlockBuff();
  return retValue;
}


// Function: Descructor
// Parameters: none
// Return value: none
// Description: 
//  Descructor checks weather all buffers are returned or not. If not, it generates
//  exception.
TBufferQue::~TBufferQue() {
  delete []BufferPtr;
  assert(FreeBufferCount == BuffersInitiated);

//  if( FreeBufferCount != BuffersInitiated) {
//  printf("\r\nNowFree:%lu, InitFree:%lu\r\n", FreeBufferCount, BuffersInitiated);
//  throw TErrorObject( __LINE__, __FILE__, 0x11fe0105);
// }

#ifdef WIN32
  DeleteCriticalSection( &CsBuffer);
#elif defined(__LINUX__)
  semctl(SemaphoreId, SemUnlock.sem_num, IPC_RMID, 0);
  if(SemUnlock.sem_num == 0){
    for(int i=SemsUsed; i<MAX_BUFF_CLASS_NO;i++){
      semctl(SemaphoreId, i, IPC_RMID, 0);
    }
  }
#endif
}

