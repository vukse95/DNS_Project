// **************************************************************************
//                        buffers.cpp  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  Descriprtsion:
//  This file contains code for Tbuffers class that handles memory management. 
// **************************************************************************

#include "../kernel/buffers.h"
#include "../kernel/errorObject.h"

#define __BUFFER_ALLOCATION_VER1__

// Function: Constructor
// Parameters: 
//  (in) uint8 classBufferNum  - number of different sizes of bufers, dimension
//                               of arrays buffersCount and buffersLength.
//  (in) uint16 *buffersCount  - pointer to array of numbers of buffers of each size,
//                               that is going to be initialized
//  (in) uint16 *buffersLength - pointer to arraye of length of each type of buffers.
// Return value: none
// Description: 
//  Initialization of object Tbuffers, which makes array of buffers queues with
//  buffers of defined size. In each list is defined number of buufers.
TBuffers::TBuffers(uint8 classBufferNum, uint32 *buffersCount, uint32 *buffersLength){
  uint32 i;
  uint32 j;
  uint32 tmp;

#ifndef __NO_PARAMETER_CHECK__
  //check input data if data is not valisd throw an exception
  if( classBufferNum == 0) ThrowError(TErrorObject( __LINE__, __FILE__, 0x01010600));
  if( classBufferNum == 0xff)ThrowError(TErrorObject( __LINE__, __FILE__, 0x01010600));
  if( classBufferNum > MAX_BUFF_CLASS_NO) ThrowError(TErrorObject( __LINE__, __FILE__, 0x01010600));
  
  if( buffersCount   == NULL) ThrowError(TErrorObject( __LINE__, __FILE__, 0x01010601));
  
  if( buffersLength  == NULL) ThrowError(TErrorObject( __LINE__, __FILE__, 0x01010602));
#endif
  
  ClassBufferNum = classBufferNum;
  //sort arrayes in future when buffer is needed arrayes it is search through sorted
  //arrayes for appropriate size
  for( i=0; i<(uint32)(classBufferNum-1); i++)  {
    for( j=(uint16)(i+1); j<(uint32)classBufferNum; j++) {
      if( buffersLength[i] > buffersLength[j] ) {
        tmp              = buffersLength[i];
        buffersLength[i] = buffersLength[j];
        buffersLength[j] = tmp;
        tmp              = buffersCount[i];
        buffersCount[i]  = buffersCount[j];
        buffersCount[j]  = tmp;
      } //end if
    }   //end for j
  }     //end for i
  

  //make list of buffers, for each size of buffers separete list of buffers
  for( i=0; i<(uint32)classBufferNum; i++) {
    Buffers[i] = new TBufferQue( buffersCount[i], buffersLength[i], (uint8) i);
    if (Buffers[i] == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x01030003));
    BuffersLength[i] = buffersLength[i];
  }  //end for i
  //rest of lists are not used so set them to NULL pointers
  for (i=classBufferNum; i<MAX_BUFF_CLASS_NO; i++) {
    Buffers[i]       = 0;
    BuffersLength[i] = 0;
  }  //end for i
}


// Function: GetBuffer
// Parameters: 
//  (in) uint32 bufferLength - length of needed buufer.
//  Return value: 
//   uint8* - pointer on wanted buffer.
// Description:
//  Gets buffer that is at least as large as asked for. Since there are discrete
//  buffers lengths it gets  first buffer that satisfies requirement. If there is
//  no buffers, it returns exception.
uint8 *TBuffers::GetBuffer(uint32 bufferLength){
  uint8 i;
  uint8 *buff;

  for( i=0; (bufferLength > BuffersLength[i]) && (i < ClassBufferNum); i++);
#ifndef __NO_PARAMETER_CHECK__
  if ( i == ClassBufferNum ) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000604));
#endif
#ifdef __BUFFER_ALLOCATION_VER1__
  while( (buff = (Buffers[i])->Get()) == NULL) {
    i++;
    if ( i == ClassBufferNum ) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000604));
  }
  return buff;
#else
  buff = (Buffers[i])->Get();
  if ( buff == 0) throw TErrorObject( __LINE__, __FILE__, 0x21030504);
  return buff;
#endif

}


// Function: RetBuffer
// Parameters:
//  (in) uint8 *info - pointer to buffer.
// Return value: none
// Description: 
//  Returns buffer into the list where it belongs.
void TBuffers::RetBuffer(uint8 *info){
#ifndef __NO_PARAMETER_CHECK__
 if ( info == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21000605));
 if (info[BUFF_CODE] >= ClassBufferNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21050609));
#endif 
  Buffers[info[BUFF_CODE]]->Add( info);
}


// Function: GetLength
// Parameters:
//  (in) uint8 data - index in array of buffers lists.
// Return value: 
//  uint32 - legnth of buffers in list
// Description: 
//  Returns length of buffers in designated list.
uint32 TBuffers::GetLength( uint8 data){
#ifndef __NO_PARAMETER_CHECK__
 if ( data >= ClassBufferNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21050607));
#endif
 return(BuffersLength[data]);
}


// Function: GetLength
// Parameters:
//  (in) uint8* info -pointer to buffer.
// Return value: 
//  uint32 - legnth of buffer.
// Description: 
//  Returns length of buffer.
uint32 TBuffers::GetLength( uint8 *info){
#ifndef __NO_PARAMETER_CHECK__
 if ( info == NULL) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21050608));
 if (info[BUFF_CODE] >= ClassBufferNum) ThrowError( TErrorObject( __LINE__, __FILE__, 0x21050609));
#endif
 return BuffersLength[info[BUFF_CODE]];	
}

// Function: decsructor
// Parameters:  none
// Return value: none
// Description: 
//  It destroys all buffers lists.
TBuffers::~TBuffers(){
 for( uint32 i=0; i<(uint32)ClassBufferNum; i++) delete Buffers[i];
}


