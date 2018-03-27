
#ifdef __LINUX__

#include "uniSock.h"
#include "linuxThread.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

int OutputDebugString( const char *)	{ return 0;};
int OutputDebugString( char *)				{ return 0;};

//int atol(char*){ return 0;};

int WSAGetLastError(void)		{ return errno;};
int closesocket(SOCKET soc)	{return close(soc);};
SOCKET accept( SOCKET s, SOCKADDR *acc_sin, int *acc_sin_len){
	return accept( s, acc_sin, (size_t*)acc_sin_len);
}

HANDLE CreateThread(
  LPVOID lpThreadAttributes,							   // pointer to security attributes
  DWORD dwStackSize,                         // initial thread stack size
  LPTHREAD_START_ROUTINE lpStartAddress,     // pointer to thread function
  LPVOID lpParameter,                        // argument for new thread
  DWORD dwCreationFlags,                     // creation flags
  DWORD* lpThreadId													 // pointer to receive thread ID
	)
{
	pthread_create(lpThreadId, NULL, (LINUX_PTHREAD_START_ROUTINE)lpStartAddress, (pthread_t*)lpParameter);
	return 0;
}

int GetPrivateProfileString(char *Section, char *Keyname, char *Default, char *Data, int Size, char *IniFileName){
	FILE *iniFile = fopen(IniFileName, "r+");
	char section[40];
	char *offset;
	char line[300];
	
	//section search
	strcpy(Data, Default);
	sprintf(section, "[%s]", Section);
	while(1){
		if(NULL==fgets(line, 300, iniFile)) return strlen(Data);
		line[strlen(line)-2] = NULL;
		if(strcmp(line, section)==0) break;
	}
	
	//Keyname search
	while(1){
		if(NULL==fgets(line, 300, iniFile)) return strlen(Data);
    if(line[0]=='[') return strlen(Data);//check if it is end of section
		line[strlen(line)-2] = NULL;
		offset = strchr(line, '=');
		*offset = 0; 
		if(strcmp(line, Keyname)==0){
			strcpy(Data, offset+1);
			return strlen(Data);
		}
	}//end while
	return 0;
};
/*
int GetPrivateProfileInt(char *Section, char *Keyname, int Default, char *IniFileName){
	return 1;
}*/

int GetPrivateProfileInt(char *Section, char *Keyname, int Default, char *IniFileName){
	char section[40];
	char line[300];
	char *offset;
	char ini[100];
	int len;
	int res;


	FILE *iniFile = fopen(IniFileName, "r+");
	//return 1;
	//section search
	sprintf(section, "[%s]", Section);
	while(1){
		if(NULL==fgets(line, 300, iniFile)) return Default;
		len = strlen(line); line[len-2] = NULL;
		if(strcmp(line, section)==0) break;
	}	
	
	//Keyname search
	while(1){
		if(NULL==fgets(line, 300, iniFile)) return Default;
    if(line[0]=='[') return Default;//check if it is end of section
    if(line[0]==';') continue;
		line[strlen(line)-2] = NULL;
		offset = strchr(line, '=');
		*offset = 0; 
		if(strcmp(line, Keyname)==0){
			res = atol(offset+1);
			break;
			return res;
		}
	}//end while
	//return res;
}

void Sleep(DWORD ms){
	Tcl_Sleep(ms);
}

#endif