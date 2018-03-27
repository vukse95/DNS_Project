#ifndef _UNISOCK_
#define _UNISOCK_

#include "kernelTypes.h"

#ifdef WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif 

//types
#ifdef __LINUX__
typedef int SOCKET;
typedef struct sockaddr SOCKADDR;
typedef SOCKADDR *LPSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN;
#endif

//consts
#ifdef __LINUX__
#define SOCKET_ERROR			-1
#define INVALID_SOCKET		-1
#endif

//functions
#ifdef __LINUX__
extern int errno;
extern int WSAGetLastError(void);
extern int closesocket(SOCKET soc);
extern SOCKET accept( SOCKET s, SOCKADDR *acc_sin, int *acc_sin_len);
#endif


#endif//_UNISOCK_