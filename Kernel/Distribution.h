
#ifndef _DISTRIBUTION_
#define _DISTRIBUTION_

//#include <conio.h>
//#include <winsock2.h>
#include "uniSock.h"
#include "kernelTypes.h"
#include "fsmSystem.h"
#ifdef __LINUX__
#include <stdlib.h>
#endif

#define DISTRIBUTION_PORT							8888
#define DIST_MAX_PENDING_CONNECTIONS  10
#define MAX_AUTO_TYPES_NUM						MAX_NUM_OF_AUTOMATES_TYPES
#define MAX_SERVER_NUM								10


DWORD WINAPI AcceptConnThread(LPVOID lpParam);
DWORD WINAPI ReceiveThread(LPVOID lpParam);

class Distribution;

typedef struct ThreadInfo_tag{
	int serverNum;
	Distribution *dist;
} ThreadInfo;

struct AutoSrv{
	void  *next;
	uint32 startId;
	uint32 lastId;
	uint8  compId[MAX_SERVER_NUM];
	AutoSrv(){ 
    memset(compId, INVALID_08, MAX_SERVER_NUM);
//		for(int i=0; i<MAX_SERVER_NUM; i++)
//			compId[i] = INVALID_08;
	};
};

typedef struct queue{
	void *head;
	void *tail;
} AHDR;

typedef struct Server_tag{
	bool   init;
	SOCKET sock;
} SERVER;

class Distribution : public FSMSystemWithTCP{
  protected:
	  SERVER server[MAX_SERVER_NUM];
	  SOCKET connSocket;
	  char	 serverName[20];
	  AHDR   automate[MAX_AUTO_TYPES_NUM];

	  void ConnectToServers(char* simbolicName);
	  bool ConnectToServer(int ServerNum, char* ServerIp, int ServerPort);
	  void FillAutoStruct();
	  void WaitServerConn(int ConnPort);
		  
	  uint16 ReceiveMsg(int ServerNum, uint8* msg);
	  void   SendMsgToServer(int ServerNum, uint8* Msg);

	  bool  IsSupported(uint8 AType, uint32 ObjId);
	  uint8 GetActivSrv(AutoSrv* as);
	  void  SendMsgToNextSrv(uint8* msg);
	  void  SendMsgToFirstSrv(uint8* msg);
	  void  ProcessMsg(uint8* msg);

	  void  GetIP(char* ip);
	  uint8 FindServerNum(SOCKET Sock);
	  uint8 FindOwnNum();

	  //list elements
	  void InitLists();
	  void PutInListTail(uint32 AType, AutoSrv *newEl);
	  void PutInListHead(uint32 AType, AutoSrv *newEl);
	  AutoSrv *GetFromListHead(uint32 AType);
	  AutoSrv *RemoveFromListHead(uint32 AType);
	  //list elements end

  public:
	  void Receive(int ServerNum);
	  void AcceptConn();
	  virtual void Start();

	  Distribution(char* srvName, uint8 numOfAutomates, uint8 numberOfMbx);
	  ~Distribution();
};

#endif //_DISTRIBUTION_