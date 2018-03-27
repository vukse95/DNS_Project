
#include "Distribution.h"
#include "kernelTypes.h"
#include "linuxThread.h"
#include <stdio.h>

#ifdef WIN32
#define DISTRIBUTION_INI   "Distribution.ini"
#elif defined(__LINUX__)
#define DISTRIBUTION_INI   "/home/samba/savic/VccLinuxServer/Distribution.ini"
#endif

Distribution::Distribution(char* srvName, uint8 numOfAutomates, uint8 numberOfMbx) : FSMSystemWithTCP(numOfAutomates, numberOfMbx){
	//memcpy(serverName, srvName, strlen(srvName));
	strcpy(serverName, srvName);
	serverId = FindOwnNum();
#ifdef WIN32
	WSADATA WsaData;
	WORD wVersionRequested = MAKEWORD(2,1);
	if (WSAStartup(wVersionRequested, &WsaData) == SOCKET_ERROR){
		int i = WSAGetLastError();
		return;
	}
#endif
	InitLists();
	FillAutoStruct();
	ConnectToServers(srvName);
}

Distribution::~Distribution(){
	AutoSrv* as;
	for(int i=0; i<MAX_AUTO_TYPES_NUM; i++){
		as = RemoveFromListHead(i);
		while(as!=NULL){
			delete as;
			as = RemoveFromListHead(i);
		}
	}
#ifdef WIN32
	WSACleanup();
#endif
}

void Distribution::ConnectToServers(char* simbolicName){
	char *srvIp, *srvPort, *srvName;
	int connPort = 0;

	int serversNum = GetPrivateProfileInt((LPCWSTR)"SERVERS", (LPCWSTR)"NumberOfServers", 1, (LPCWSTR) DISTRIBUTION_INI);

	for(int i=0; i<serversNum; i++){
	  char message[50];
		char keyname[50];
    sprintf((char*)keyname, "%d", i );
	  GetPrivateProfileString( (LPCWSTR) "SERVERS", (LPCWSTR)keyname,	(LPCWSTR) "UNKNOWN",(LPWSTR) message, 50,	(LPCWSTR) DISTRIBUTION_INI);
		char* start = message;

		//get server ip
		char* offset = strchr(start, ',');
		*offset = 0; offset++;
		srvIp = start;
		start = offset;
		//get server port
		offset = strchr(start, ',');
		*offset = 0; offset++;
		srvPort = start;
		start = offset;
		//get server name
		offset = strchr(start, ',');
		*offset = 0; offset++;
		srvName = start;
		start = offset;

		if(memcmp(srvName, simbolicName, strlen(simbolicName))!=0)
			ConnectToServer(i, srvIp, atol(srvPort));
		else{
			server[i].init = true;
			serverId = i;
			connPort = atol(srvPort);
		}
	}
	WaitServerConn(connPort);
}

bool Distribution::ConnectToServer(int ServerNum, char* ServerIp, int ServerPort){
	struct sockaddr_in sockadd;
	server[ServerNum].init = false;

	memset(&sockadd,0,sizeof(sockadd));

	sockadd.sin_family = AF_INET;
	sockadd.sin_port = htons(ServerPort);
	//sockadd.sin_addr.S_un.S_addr = inet_addr(ServerIp);
  sockadd.sin_addr.s_addr = inet_addr(ServerIp);

	server[ServerNum].sock = socket(AF_INET, SOCK_STREAM, 0); // Open a socket 
	if (server[ServerNum].sock <0 ) {
		fprintf(stderr,"Client: Error Opening socket: Error %d\n", WSAGetLastError());
		return false;
	}

	if (connect(server[ServerNum].sock,(struct sockaddr*)&sockadd,sizeof(sockadd))
      		== SOCKET_ERROR) {
		fprintf(stderr,"connect() failed: %d\n",WSAGetLastError());
		closesocket(server[ServerNum].sock);
		return false;
	}
	send(server[ServerNum].sock, (char*)&serverId, 1, 0);

	DWORD threadId;
	ThreadInfo* ti = new ThreadInfo;
  if(ti == 0) throw;

	ti->dist       = this;
	ti->serverNum  = ServerNum;
	server[ServerNum].init = true;
	CreateThread( 0, 0, ReceiveThread, (LPVOID)ti, 0, &threadId);
	return true;
}

void Distribution::FillAutoStruct(){
	int srvsNum;
	char* offset;
	char message[300];
	char* start;
	char keyname[50];

	for(int i=0; i<MAX_AUTO_TYPES_NUM; i++){
		sprintf((char*)keyname, "%d", i);
		GetPrivateProfileString(  (LPCWSTR)"AUTOMATES", (LPCWSTR)keyname,(LPCWSTR)	"UNKNOWN",(LPWSTR) message, 300,(LPCWSTR)	DISTRIBUTION_INI);
		if(strcmp(message, "UNKNOWN")==0) continue;
		start = message;
		offset = strchr(start, ',');
		*offset = 0; offset++;
		srvsNum = atol(start);
		start = offset;
		for(int j=0; j<srvsNum; j++){
			AutoSrv* newEl = new AutoSrv;
      if(newEl == 0) throw;
			//start
			offset = strchr(start, ',');
			*offset = 0; offset++;
			newEl->startId = atol(start);
			start = offset;
			//last
			offset = strchr(start, ',');
			*offset = 0; offset++;
			newEl->lastId = atol(start);
			start = offset;
			//compId
			offset = strchr(start, ',');
			*offset = 0; offset++;
			int count = 0;
			while(*start != '#'){
				newEl->compId[count] = (uint8)atol(start);
				start = offset;
				offset = strchr(start, ',');
				*offset = 0; offset++;
				count++;
			}
			PutInListTail(i, newEl);
			start = offset;
		}
	}
}

void Distribution::WaitServerConn(int ConnPort){      
	SOCKADDR_IN local_sin;
	memset(&local_sin, 0, sizeof(local_sin));
	local_sin.sin_family=AF_INET;
	local_sin.sin_port = htons(ConnPort); 
	local_sin.sin_addr.s_addr =INADDR_ANY;


	connSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if (connSocket == SOCKET_ERROR)  {
		int i = WSAGetLastError();
		printf("sock error: %d", i);
		return;
	}

	if( bind(connSocket, (SOCKADDR *)&local_sin, sizeof(local_sin)) == SOCKET_ERROR)  {
		int i = WSAGetLastError();
		printf("sock error: %d", i);
		closesocket(connSocket);
		return;
	}
	DWORD threadId;
	CreateThread( 0, 0, AcceptConnThread, (LPVOID)this, 0, &threadId);
}

DWORD WINAPI AcceptConnThread(LPVOID lpParam){
  Distribution *dist = (Distribution*)lpParam;
	dist->AcceptConn();
  return 0;
}

void Distribution::AcceptConn(){
  SOCKET newSocket;
  SOCKADDR_IN acc_sin;
  int acc_sin_len = 0;  
  memset(&acc_sin, 0, sizeof(acc_sin));

  while(1){
    int retValue = listen(connSocket, DIST_MAX_PENDING_CONNECTIONS);
    if( retValue == SOCKET_ERROR) {
      int i = WSAGetLastError();
      printf("sock error: %d", i);
      return;
    }  

    acc_sin_len = sizeof(acc_sin);
    newSocket = accept(connSocket, (SOCKADDR *)&acc_sin, (int FAR *) &acc_sin_len);
    if( newSocket == INVALID_SOCKET)  {
      int i = WSAGetLastError(); 
      printf("sock error: %d", i);
      return;
    }  
		char recvIP[20];

		strcpy(recvIP, inet_ntoa(acc_sin.sin_addr));
		uint8 serverNum = FindServerNum(newSocket);
		DWORD threadId;
	
		ThreadInfo *ti = new ThreadInfo;
		ti->dist       = this;
		ti->serverNum  = serverNum;
		server[serverNum].init = true;
		server[serverNum].sock = newSocket;
		CreateThread( 0, 0, ReceiveThread, (LPVOID)ti, 0, &threadId);
	}//end while
}

DWORD WINAPI ReceiveThread(LPVOID lpParam){
	ThreadInfo* pTi    = (ThreadInfo*)lpParam;
	int serverNum      = pTi->serverNum;
	Distribution* dist = pTi->dist;
	delete pTi;
	dist->Receive(serverNum);
	return 1;
}

void Distribution::Receive(int ServerNum){
	uint8 msg[512];
	while(1){
		int ret = ReceiveMsg(ServerNum, msg);
		if(ret==0){	
			server[ServerNum].init = false;
			closesocket(server[ServerNum].sock);
			return;
		}
		uint8* sendMsg = GetBuffer(ret);
		memcpy(sendMsg, msg, ret);
		SendToMbx(sendMsg);
	}//end while

	return;
}

uint16 Distribution::ReceiveMsg(int ServerNum, uint8* msg){
	char buff[100];
	uint16 msgLen, currLen = 0;
	int recvLen;
	
	recvLen = recv(server[ServerNum].sock, (char*)&msgLen, 2, 0);
	if(recvLen<0 || recvLen==0){ return 0;}
	
	recvLen = recv(server[ServerNum].sock, buff, msgLen, 0);
	if(recvLen<0 || recvLen==0){ return 0;}

	while(recvLen <= msgLen-currLen){
		memcpy(msg+currLen, buff, recvLen);
		currLen += recvLen;

		if(msgLen == currLen){
			//memcpy(buff, temp, msgLen);
			return msgLen;
		}

		recvLen = recv(server[ServerNum].sock, buff, msgLen-currLen, 0);
		if(recvLen<0 || recvLen==0){ return 0;}
	}//end while
	return 0;
}

void Distribution::SendMsgToServer(int ServerNum, uint8* Msg){
	uint8 sendingMsg[512];
	uint16 len = (GetMsgInfoLength(Msg)+MSG_HEADER_LENGTH); 
	memcpy(sendingMsg, &len, 2);
	memcpy(sendingMsg+2, Msg, len);
	int sendLen = send(server[ServerNum].sock, (char*)sendingMsg, len+2, 0);
	if(sendLen<0 || sendLen==0){
		server[ServerNum].init = false;
		closesocket(server[ServerNum].sock);
	}
	DiscardMsg(Msg);
}

void Distribution::GetIP(char* ip){
  char localHost[255];
  gethostname(localHost, 255);
	struct hostent *host = gethostbyname(localHost);
	strcpy(ip, inet_ntoa (*(struct in_addr*)*host->h_addr_list));
}

uint8 Distribution::FindServerNum(SOCKET Sock){
	uint8 groupId = INVALID_08;
	recv(Sock, (char*)&groupId, 1, 0);
	return groupId;
}

uint8 Distribution::FindOwnNum(){
	int serversNum = GetPrivateProfileInt((LPCWSTR)"SERVERS", (LPCWSTR)"NumberOfServers", 1, (LPCWSTR) DISTRIBUTION_INI);
	for(int i=0; i<serversNum; i++){
	  char message[50];
		char keyname[50];
    sprintf((char*)keyname, "%d", i );
	  GetPrivateProfileString(  (LPCWSTR)"SERVERS",
															(LPCWSTR)keyname,
															(LPCWSTR) "UNKNOWN",
															(LPWSTR) message,
															50,
															(LPCWSTR) DISTRIBUTION_INI);
		char* offset = strchr(message, ',');
		char* start = strchr(++offset, ',');
		offset = strchr(++start, ',');
		*offset = 0;
		
		if(memcmp(start, serverName, strlen(serverName))==0) return i;
	}//end for
	return -1;
}

/*
uint8 Distribution::FindOwnNum(){
  char message[50];
	int serverNum;
	int i=0;
	serverNum = GetPrivateProfileInt("SERVERS", "NumberOfServers", 1, DISTRIBUTION_INI);
	for( i=0; i<serverNum; i++){
	  char message[50];
		char keyname[50];
    //strcpy((char*)keyname, "1");
    sprintf((char*)keyname, "%d", i );
	  GetPrivateProfileString(  "SERVERS",
															keyname,
															"UNKNOWN",
															message,
															50,
															DISTRIBUTION_INI);

//		char* offset = strchr(message, ',');
//		char* start = strchr(++offset, ',');
//		offset = strchr(++start, ',');
//		*offset = 0;
		
//		if(memcmp(start, serverName, strlen(serverName))==0) return i;
	}//end for
	return serverNum;
}*/

bool Distribution::IsSupported(uint8 AType, uint32 ObjId){
	AutoSrv* ai;
	uint8 compId;
	ai = GetFromListHead(AType);
	if(ObjId==INVALID_32)
		while(ai!=NULL){
			compId = GetActivSrv(ai);
			if(compId==serverId) return true;
			ai = (AutoSrv*)ai->next;
		}
	else
	while(ai!=NULL){
		compId = GetActivSrv(ai);
		if((compId==serverId) && (ai->startId<=ObjId) && (ObjId<=ai->lastId))	return true;
		ai = (AutoSrv*)ai->next;
	}
	return false;
}

void Distribution::ProcessMsg(uint8* msg){
	uint8  automateType = GetMsgToAutomate(msg);
	uint32 objNum = GetMsgObjectNumberTo(msg);
	uint8 group = GetMsgToGroup(msg);

	if( objNum == INVALID_32){
		ptrFiniteStateMachine object = FreeAutomates[automateType].Get();
		if( object != NULL) object->Process(msg);
		else if(group==INVALID_08) SendMsgToNextSrv(msg);
		return;
	}
	else 
		(Automates[automateType][objNum])->Process(msg);
}

void Distribution::SendMsgToNextSrv(uint8* msg){
	AutoSrv* as;
	uint8 compId;
	as = GetFromListHead(GetMsgToAutomate(msg));
	while(as!=NULL){
		compId = GetActivSrv(as); 
		if(compId==serverId){
			as = (AutoSrv*)as->next;
			break;
		}
		else
			as = (AutoSrv*)as->next;
	}
	//find activ srv till the end of list
	while(as!=NULL){
		compId = GetActivSrv(as);
		if(compId!=INVALID_08){
				SendMsgToServer(compId, msg);
				return;
		}
		as = (AutoSrv*)as->next;
	}//end while
	SendMsgToFirstSrv(msg);
}

void Distribution::SendMsgToFirstSrv(uint8* msg){
	AutoSrv* as;
	uint8 compId;
	as = GetFromListHead(GetMsgToAutomate(msg));

	while(as!=NULL){
		compId = GetActivSrv(as);
		//if(compId!=INVALID_08)
    if(compId!=INVALID_08 && compId!=serverId){
			SendMsgToServer(compId, msg);
      return;
    }
		as = (AutoSrv*)as->next;
	}
  DiscardMsg(msg);
}

uint8 Distribution::GetActivSrv(AutoSrv* as){
	uint8 compId;
	for(int i=0; i<MAX_SERVER_NUM; i++){
		compId = as->compId[i];
		if((compId!=INVALID_08) && (server[compId].init))
			return compId;
	}
	return INVALID_08;
}
// Function: Start 
// Parameters:   none
// Return value: none
// Description: 
// Monitor loop of autmate system. It takes message from queue, find which autmate should 
// process it and executes processing.
void Distribution::Start(){
  SystemWorking = true;
	uint8  automateType;
	uint32 objNum;
	uint8 group;

  while(SystemWorking) {

    for(uint8 i=0; i<NumberOfMbx; i++) {
      uint8 *msg = GetMsg(i);
      if(msg == NULL) {
#ifdef WIN32
		Sleep(10);
#endif
      continue;
      }
			automateType = GetMsgToAutomate(msg);
			objNum       = GetMsgObjectNumberTo(msg);
			group        = GetMsgToGroup(msg);

			if(group==serverId){
				if(IsSupported(automateType, objNum))
					ProcessMsg(msg);
				else 
					DiscardMsg(msg);
				continue;
			}
			if(group==INVALID_08){
				if(IsSupported(automateType, objNum))
					ProcessMsg(msg);
				else 
					SendMsgToFirstSrv(msg);
				continue;
			}
			SendMsgToServer(group, msg);
    }//end for
  }//end while
}
//list finction
void Distribution::InitLists(){
	for(int i=0; i<MAX_AUTO_TYPES_NUM; i++)
		automate[i].head = automate[i].tail = NULL;
}

void Distribution::PutInListTail(uint32 AType, AutoSrv *newEl){
  newEl->next = NULL;	/* obezbedi NULL terminator liste */
  if( automate[AType].head == NULL )
    automate[AType].head = automate[AType].tail = newEl;
  else
    automate[AType].tail = ((AutoSrv*)(automate[AType].tail))->next = newEl;
}

void Distribution::PutInListHead(uint32 AType, AutoSrv *newEl){
  if( automate[AType].head == NULL )
    automate[AType].head = automate[AType].tail = newEl;
  else  {
    newEl->next = automate[AType].head;
    automate[AType].head = newEl;
  }
}

AutoSrv* Distribution::GetFromListHead(uint32 AutoType){
  return (AutoSrv*)automate[AutoType].head;
}

AutoSrv* Distribution::RemoveFromListHead(uint32 AType){
  AutoSrv *pom;

  if( automate[AType].head == automate[AType].tail )  {
    pom = (AutoSrv *)automate[AType].head;
    automate[AType].head = automate[AType].tail = NULL;
  }
  else
    automate[AType].head = ((pom  = (AutoSrv *)automate[AType].head)->next);

  return pom;
}
//end list functions
