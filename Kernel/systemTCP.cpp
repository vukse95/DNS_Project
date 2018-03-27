#include "systemTCP.h"
#include "fsmSystem.h"

#ifdef __LINUX__
#include "linuxThread.h"
#endif

extern FSMSystemWithTCP SystemObject;

TCPSystemSupport *Server[MAX_NUM_OF_TCP_SERVERS];
#define MAX_MSG_LENGTH 1024
static char msg[MAX_MSG_LENGTH];


DWORD WINAPI ServerThread(LPVOID lpParameter){
  int index;
  index = (int)lpParameter;
  while( (Server[index]->WaitConnection(index) ==0 ));
  return 0;
}

//thread koji se pokrece kada se prosledjuju parametri kod InitTCPServer
DWORD WINAPI ServerThreadParam(LPVOID lpParameter){
  int index = 0;
  unsigned char *param = 0;
  exParam *priv = (exParam *)lpParameter;

  index = priv->index;
  param = priv->param;
  while( (Server[index]->WaitConnection(index,priv) == 0 ));
  if(priv != NULL){
	  delete priv;
  }
  return 0;
}

TCPSystemSupport::TCPSystemSupport(){
#ifdef WIN32
  WSADATA WsaData;
  WORD wVersionRequested = MAKEWORD( 2, 1 );
  if (WSAStartup (wVersionRequested, &WsaData) == SOCKET_ERROR)  {
    throw;
  }
#endif
  for(int i=0; i<MAX_NUM_OF_TCP_SERVERS; i++){
    Server[i] = 0;
    ServerData[i].Used = false;
    ServerData[i].ClientObject = 0;
  }
}
//
// Ja moram da pozivam ovu funkciju onoliko puta koliko imam servera !!
// 
/*
int TCPSystemSupport::InitTCPServer(uint16 port, uint8 automateType, char *ipAddress){
  SOCKADDR_IN local_sin;
  for(int i=0; i<MAX_NUM_OF_TCP_SERVERS; i++){
    if(ServerData[i].Used == false){
      memset(&local_sin, 0, sizeof(local_sin));
      local_sin.sin_family=AF_INET; // tip adrese
      local_sin.sin_port = htons(port); // port na kome se slusa, recimo 777
      if(ipAddress == 0) local_sin.sin_addr.s_addr =INADDR_ANY;
      else               local_sin.sin_addr.s_addr =inet_addr(ipAddress);

      ServerData[i].ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );
      if (ServerData[i].ConnectionSocket == SOCKET_ERROR)  {
        int i = WSAGetLastError();
        return -1;
      }
 
      if( bind( ServerData[i].ConnectionSocket, (SOCKADDR *)&local_sin, sizeof(local_sin)) == SOCKET_ERROR)  {
        int i = WSAGetLastError();
        closesocket(ServerData[i].ConnectionSocket);
        return -1;
      }
      ServerData[i].ClientObject = 0;
      Server[i] = this;
      ServerData[i].ThreadHandle = CreateThread( 0, 0, ServerThread, (LPVOID)i, 0, &ServerData[i].ThreadId);
      if(ServerData[i].ThreadHandle == 0) {
        return -1;
      }
      ServerData[i].AutomateType = automateType;
      ServerData[i].Used = true;
      return 0;
    } //end if
  } //end for
  return 0;
}
*/

SOCKADDR_IN local_sin;
int TCPSystemSupport::InitTCPServer(uint16 port, uint8 automateType, char *ipAddress, unsigned char *param, int length){
  for(int i=0; i<MAX_NUM_OF_TCP_SERVERS; i++){
    if(ServerData[i].Used == false){
      memset(&local_sin, 0, sizeof(local_sin));
      local_sin.sin_family=AF_INET; // tip adrese
      local_sin.sin_port = htons(port); // port na kome se slusa, recimo 777
      if(ipAddress == 0) local_sin.sin_addr.s_addr =INADDR_ANY;
      else               local_sin.sin_addr.s_addr =inet_addr(ipAddress);

      ServerData[i].ConnectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP );
      if (ServerData[i].ConnectionSocket == SOCKET_ERROR)  {
        int k = WSAGetLastError();
        return -1;
      }
 
      if( bind( ServerData[i].ConnectionSocket, (SOCKADDR *)&local_sin, sizeof(local_sin)) == SOCKET_ERROR)  {
        int k = WSAGetLastError();
        closesocket(ServerData[i].ConnectionSocket);
        return -1;
      }
      ServerData[i].ClientObject = 0;
      Server[i] = this;
	  if(param == NULL){
//#ifdef WIN32
		  ServerData[i].ThreadHandle = CreateThread( 0, 0, ServerThread, (LPVOID)i, 0, &ServerData[i].ThreadId);
//#else
//			pthread_create (&ServerData[i].ThreadId, NULL, &ServerThread, (pthread_t*)i);
//#endif
		  if(ServerData[i].ThreadHandle == 0) {
            return -1;
		  }
	  }
	  else{
		  exParam *priv = new exParam(param ,length);
		  priv->index = i;
		  ServerData[i].ThreadHandle = CreateThread( 0, 0, ServerThreadParam, (LPVOID)priv, 0, &ServerData[i].ThreadId);
		  if(ServerData[i].ThreadHandle == 0) {
            return -1;
		  }

	  }
      ServerData[i].AutomateType = automateType;
      ServerData[i].Used = true;
      return 0;
    } //end if
  } //end for
  return 0;
}

int TCPSystemSupport::WaitConnection(int index, exParam *exPrm){
  SOCKET newSocket;
  SOCKADDR_IN acc_sin;
  int acc_sin_len = 0;  
  
  memset(&acc_sin, 0, sizeof(acc_sin));
  int retValue = listen( ServerData[index].ConnectionSocket, MAX_PENDING_CONNECTIONS);
  if( retValue == SOCKET_ERROR) {
      int i = WSAGetLastError();
      return -1;
  }  
  // David add 12.06
  acc_sin_len = sizeof(acc_sin);
  newSocket = accept( ServerData[index].ConnectionSocket, (SOCKADDR *)&acc_sin, (int*) &acc_sin_len);
  if( newSocket == INVALID_SOCKET)  {
      int i = WSAGetLastError(); 
      return -1;
  }  
  // End of add
  
  if( (ServerData[index].ClientObject = GetClientObject(ServerData[index].AutomateType)) == 0){  
      closesocket( newSocket);
      return -1;
  }
  
  if(exPrm == NULL){
	  if( ((ServerData[index].ClientObject)->acceptConnection(newSocket)) < 0){      
		  closesocket( newSocket);
		  return -1;
	  }
  }
  else{
	  if( ((ServerData[index].ClientObject)->acceptConnection(newSocket, exPrm)) < 0){      
		  closesocket( newSocket);
		  return -1;
	  }
  }
  return 0;
}

int TCPSystemSupport::DeinitTCPServer(int index){
  if( closesocket( ServerData[index].ConnectionSocket) == SOCKET_ERROR)  {
    return -1;
  }
  ServerData[index].Used = false;
  //ExitThread(ServerData[index].ThreadId);
  return 0;
}

TCPSystemSupport::~TCPSystemSupport(){
  for(int i=0; i<MAX_NUM_OF_TCP_SERVERS; i++){
    if( ServerData[i].Used == true){
      DeinitTCPServer(i);
    }
  }
#ifdef WIN32
  WSACleanup();
#endif
}

// Function:     GetClientObject
// Parameters:   int automatType
// Return value: TCPFSMSupport *
// Description: 
// Return free automate
// Imam problem kako da odredim koji automat moram da vratim
// Primer: Kako da znam da SG mora da vrati AG !!???
// Drugo: Kako ja da pristupim FSMSystem klasi iz TCPSystemSupport !?
// Ovo sam resio sto sam stavio da je klasa TCPSystemSupport prijatelj
// klase FSMSystem.  

FiniteStateMachine *TCPSystemSupport::GetClientObject(int automatType) {
    return (SystemObject.FreeAutomates[automatType].Get());
}
