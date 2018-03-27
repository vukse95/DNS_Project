// **************************************************************************
//                        buffersQueue.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Description:
//  TbufferQue is used for handling buffers of determined size. It has functions
//  for getting and returning buffers. At the moment of creation, memory for
//  buffers is reserved.
//
//  WIN 32 Data:
//  CRITICAL_SECTION CsBuffer - Critical section data for windows, used to lock and 
//                              unlock buffer queue.
//  Linux Data:
//  int Proj - for unique semaphore key generation
//  int SemaphoreId - Id of semaphore.
//  static struct sembuf SemUnlock - data needed to unlock semaphore.
//  static struct sembuf SemLock   - data needed to lock semaphore.
//  
//  class data:
//	uint8 *Head - pointer to head of buffers queue.
//	uint8 *Tail - pointer to tail of buffers queue.
//	uint8 *BufferPtr - pointer to memory that is devided in to buffers.
//	uint32 FreeBufferCount  - Number of buffers in queue.
//	uint32 BuffersInitiated - Number of buffers created.
// **************************************************************************



#ifndef __BUFFERS_QUEUE__
#define __BUFFERS_QUEUE__

#ifndef __KERNEL_TYPES__
#include "../kernel/kernelTypes.h"
#endif

#ifndef __KERNEL_CONSTS__
#include "../kernel/kernelConsts.h"
#endif
#ifdef WIN32
#include <windows.h>
#elif defined(__LINUX__)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

class TBufferQue {
  protected:
#ifdef WIN32
    CRITICAL_SECTION CsBuffer;
#elif defined(__LINUX__)
    static int    SemsUsed;
    static int    SemaphoreId;
    struct sembuf SemUnlock;
    struct sembuf SemLock;
#else  //Linux Version
#endif
	  uint8 *Head;
	  uint8 *Tail;
	  uint8 *BufferPtr;
	
	  uint32 FreeBufferCount;
	  uint32 BuffersInitiated;

  	  inline void LockBuff();
	  inline void UnlockBuff();
	
	  inline uint8 *GetFromHead();
	  inline void  PutOnTail(uint8 *elem);
	
  public:
	  TBufferQue(uint32 buffersNo, uint32 bufferLength, uint8 bufferCode = 0);
	  ~TBufferQue();
	
	  void  Add(uint8 *info);
	  uint8 *Get();        //uzima sa pocetka reda (head)
	
	  uint32 GetAvailable() { return FreeBufferCount; };
	  bool   IsEmpty();
};


#endif


