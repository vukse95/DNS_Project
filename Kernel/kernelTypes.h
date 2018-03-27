// **************************************************************************
//                        kernelTypes.h  -  description
//                           -------------------
//  begin                : Mon Jun 19 2000
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//  Data types needed by system. all of this data types are put here in orther
//  to make it OS independent.
// **************************************************************************


#ifndef __KERNEL_TYPES__
#define __KERNEL_TYPES__


#ifdef WIN32
typedef unsigned char      uint8;
typedef unsigned short int uint16;
typedef unsigned long      uint32;
typedef char               int8;
typedef short int          int16;
typedef long               int32;

typedef uint8  BYTE;
typedef uint16 WORD;
typedef uint32 DWORD;

#elif defined(__LINUX__)

typedef unsigned char      uint8;
typedef unsigned short int uint16;
typedef unsigned long      uint32;
typedef char               int8;
typedef short int          int16;
typedef long               int32;

typedef uint8  BYTE;
typedef uint16 WORD;
typedef uint32 DWORD;

#define WINAPI
#define FAR

typedef void *HANDLE;
typedef void *LPVOID;
typedef void *HINSTANCE;

extern int OutputDebugString( const char *);
extern int OutputDebugString( char *);
extern int GetPrivateProfileInt(char*, char*, int, char*);
extern int GetPrivateProfileString(char*, char*, char*, char*, int, char*);
extern void Sleep(DWORD ms);
//extern int atol(char*);

#elif defined(__ARM_CORE__)

typedef unsigned char      uint8;
typedef unsigned short int uint16;
typedef unsigned long      uint32;
typedef char               int8;
typedef short int          int16;
typedef long               int32;


#ifndef BYTE
typedef uint8  BYTE;
#endif
#ifndef WORD
typedef uint16 WORD;
#endif
#ifndef DWORD
typedef uint32 DWORD;
#endif


#endif

#ifdef __ARM_CORE__
  #define __MICRO_CONTROLER_MEMORY_ACCESS__
  #define __NO_EXCEPTIONS__
  #define __NO_PREV_PTR__
  #define __NO_TCP_SUPPORT__
  #define __NO_ERROR_STREAM__
#else
  #define __MICRO_CONTROLER_MEMORY_ACCESS__
#endif

#ifdef __MICRO_CONTROLER_MEMORY_ACCESS__

#include <string.h>

void   SetUint16(uint8  *addr, uint16 value);
void   SetUint32(uint8  *addr, uint32 value);
uint16 GetUint16(uint8  *addr);
uint32 GetUint32(uint8  *addr);
void   SetMemPtr(uint8  *addr, uint8 *value);
uint8  *GetMemPtr(uint8 *addr);

#else
#define SetUint16(addr,value) *((uint16 *)(addr)) = value
#define SetUint32(addr,value) *((uint32 *)(addr)) = value
#define GetUint16(addr)   *((uint16 *)(addr))
#define GetUint32(addr)   *((uint32 *)(addr))

#define SetMemPtr(addr, newPtr)  *((uint8 **) (addr) ) = (newPtr)
#define GetMemPtr(addr)          *((uint8 **) (addr) )

#endif

#endif

