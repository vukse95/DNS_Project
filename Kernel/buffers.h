// **************************************************************************
//                        buffers.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Descriprtsion:
//  This class is used for memory management. It has a array of queues
//  with buffers of different size. It handles getting, and returning buffer in
//  appropriate queue. The number of queues (different size of buffers is defined
//  in the moment of object making. Also number of buffers of determined  size is
//  defined at the same time.
//
//  class data:
//  TBufferQue *Buffers[MAX_BUFF_CLASS_NO] - array of buffers queues, each queue holds
//                                           buffers of differnet size.
//  uint32     BuffersLength[MAX_BUFF_CLASS_NO] - Length of buffers in each queue.
//                                           Index in Buffers queue is same ashere.
//  uint8      ClassBufferNum - Number of buffers queues.
// **************************************************************************


#ifndef __BUFFERS__
#define __BUFFERS__

#ifndef __BUFFERS_QUEUE__
#include "../kernel/buffersQueue.h"
#endif


class TBuffers {
  protected:
    TBufferQue *Buffers[MAX_BUFF_CLASS_NO];
    uint32     BuffersLength[MAX_BUFF_CLASS_NO];
    uint8      ClassBufferNum;

  public:
    TBuffers(uint8 classBufferNum, uint32 *buffersLength, uint32 *buffersCount);
    ~TBuffers();

    uint8 *GetBuffer(uint32 bufferLength);
    void   RetBuffer(uint8 *buffer);
    uint32 GetLength( uint8 data);
    uint32 GetLength( uint8 *buff);
};

#endif
