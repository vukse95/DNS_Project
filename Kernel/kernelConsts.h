// **************************************************************************
//                        kernelConsts.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description
//  Defines necessary for kernel functioning. Most of this data is not needed
//  by the application, but by the kernel and FSM base class. If there is need
//  for application to add some defines there is defined some link.  
// **************************************************************************
#ifndef __KERNEL_CONSTS__
#define __KERNEL_CONSTS__

#ifndef __KERNEL_TYPES__
#include "../kernel/kernelTypes.h"
#endif

//Buffers data offsets
//BUFF_CODE - contains offset of buffer in array of buffers
//BUFF_NEXT - pointer on next buffer in lists
//BUFF_PREV - pointer on previous buffer in double lists
//BUFF_TRACKING_DATA - used for tracking of buffers and preventing queueing buffer
//                     in two differnet queues
//BUFF_HEADER_LENGTH - length of header data
const signed   int BUFF_CODE          = - (signed int)(sizeof( uint8));

//#define __NO_PREV_PTR__
#ifdef __NO_PREV_PTR__
//without pointer to prevois buffer
const signed   int BUFF_NEXT          = - (signed int)(sizeof( uint8 *)) + BUFF_CODE;
#else  //with ptr to prevois buffer

const signed   int BUFF_PREV          = - (signed int)(sizeof( uint8 *)) + BUFF_CODE;
const signed   int BUFF_NEXT          = - (signed int)(sizeof( uint8 *)) + BUFF_PREV;
#endif

//#define __BUFFER_PROTECTION__
#ifdef __BUFFER_PROTECTION__

const signed   int BUFF_TRACKING_DATA = - (signed int)(sizeof( uint8 ))  + BUFF_NEXT;
const unsigned int BUFF_HEADER_LENGTH = - BUFF_TRACKING_DATA;

const uint8 USER_BUFFERS = 0x01;
const uint8 USER_MBX     = 0x02;
const uint8 USER_TIMER   = 0x03;
const uint8 USER_FSM     = 0x04;
const uint8 USER_FREE    = 0x05; //buffer taken, but user not known at the time

#else

const unsigned int BUFF_HEADER_LENGTH = - BUFF_NEXT;

#endif


// Kernel messages Id code (reserved)
// kodovi od 0x7F00 - 0x7FFF rezervisani za kernel
// 0x7fff - DISCARD, message to be descarded by the Kernel
enum {
 DISCARDED = 0x7FFF
};

//Message's header offsets
const uint8 MSG_FROM_AUTOMATE      = 0;  //code of autmate that sended message (BYTE)
const uint8 MSG_FROM_GROUP         = 1;  //code of object group that sended message (BYTE)
const uint8 MSG_TO_AUTOMATE        = 2;  //code of autmate that recive message (BYTE)
const uint8 MSG_TO_GROUP           = 3;  //code of object group that recive message (BYTE)
const uint8 MSG_CODE               = 4;  //code of message (WORD)
const uint8 MSG_OBJECT_ID_FROM     = 6;  //object ID of automate taht sended message (DWORD)
                                         //object ID is usualy index of object in array.
const uint8 MSG_OBJECT_ID_TO       = 10; //object ID of automate taht recive message (DWORD)
const uint8 CALL_ID                = 14; //unique call id, used for tracking of messages
const uint8 MSG_INFO_CODING        = 18; //type of message coding, zero by default
const uint8 MSG_LENGTH             = 19; //length of message data
const uint8 MSG_INFO               = 21; //start of message data
const uint8 MSG_HEADER_END         = MSG_INFO;  //
const uint8 MSG_HEADER_LENGTH = MSG_HEADER_END; //length of message header

const uint8 TIMER_MBX_ID           = MSG_FROM_GROUP;     //mail box id where to put timer when expired
const uint8 TIMER_COUNT            = MSG_OBJECT_ID_FROM; //ticks count until for timer


//kernel objects ID's
enum {
    TIMER, FREE_ID
};

//default number of mail boxes
const uint8 NUM_OF_MAILBOXES        = 30;

// Misc. invalid values (to locate errors easily)
#define INVALID_08        0xff //for  8bit values
#define INVALID_16      0xffff //for 16bit values
#define INVALID_32  0xffffffff //for 32bit values

#ifdef __ARM_CORE__
const uint8 MAX_BUFF_CLASS_NO =  5;
const uint8 MAX_MAILBOXES_NUM =  5;
#else
const uint8 MAX_BUFF_CLASS_NO =  32;
const uint8 MAX_MAILBOXES_NUM = 100;
#endif

#endif
