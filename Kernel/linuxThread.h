#ifndef _LINUXTHREAD_
#define _LINUXTHREAD_

#ifdef __LINUX__

#include "kernelTypes.h"
#include <pthread.h>

typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef void* (WINAPI *LINUX_PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter);

extern HANDLE CreateThread(
  LPVOID lpThreadAttributes,							   // pointer to security attributes
  DWORD dwStackSize,                         // initial thread stack size
  LPTHREAD_START_ROUTINE lpStartAddress,     // pointer to thread function
  LPVOID lpParameter,                        // argument for new thread
  DWORD dwCreationFlags,                     // creation flags
  DWORD* lpThreadId													 // pointer to receive thread ID
	);
 
#endif __LINUX__
#endif//_LINUXTHREAD_