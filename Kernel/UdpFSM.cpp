#include "UdpFSM.h"
#include <string>

struct sData {
	string message;
	unsigned int from;
	UdpFSM* pUdpFSM;
};


DWORD WINAPI processConnection(LPVOID lpParam){
	sData *ps=(sData*)lpParam;
	if (ps->pUdpFSM->listening)
           ps->pUdpFSM->Work(ps->message,ps->from);
	return 1;
}


DWORD WINAPI Listener(LPVOID lpParam){
//	DWORD thID;
//	HANDLE hTh;
	UdpFSM *p=(UdpFSM *)lpParam;
/*	if(!p->listen(p->portToListen)){
		return 0;
	}*/
	while(p->listening){
		sData data;
		data.pUdpFSM = p;
 		data.from = p->read(data.message);
		/*if(!(hTh=CreateThread(NULL, 0, processConnection, (void*)&data, THREAD_PRIORITY_NORMAL, &thID))){
			return 0;
		}*/
		p->Work(data.message,data.from);
		Sleep(0);
	}
//	WaitForSingleObject(hTh,INFINITE);
	return 1;
}

DWORD WINAPI ListenerMulticast(LPVOID lpParam){
//	DWORD thID;
//	HANDLE hTh;
	UdpFSM *p=(UdpFSM *)lpParam;
/*	if(!p->listen(p->portToListen)){
		return 0;
	}*/
	while(p->listeningMulticast){
		sData data;
		data.pUdpFSM = p;
 		data.from = p->readMulticast(data.message);
		/*if(!(hTh=CreateThread(NULL, 0, processConnection, (void*)&data, THREAD_PRIORITY_NORMAL, &thID))){
			return 0;
		}*/
		p->Work(data.message,data.from);
		Sleep(0);
	}
//	WaitForSingleObject(hTh,INFINITE);
	return 1;
}

UdpFSM::UdpFSM( uint8 automateId1, uint8 mbxId1) : 
TransportInterface(  automateId1, mbxId1, 100, 100, 100){
	ip = new char[16];
	sockToListen = 0;
	portToListen = 4000;
	packageSize = 1024;
	fsmMessageR = new uint8[MAX_LENGTH_MESSAGE];
	protocolMessageR = new uint8[MAX_LENGTH_MESSAGE];
	fsmMessageS = new uint8[MAX_LENGTH_MESSAGE];
	protocolMessageS = new uint8[MAX_LENGTH_MESSAGE];
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 0), &wsd);
}



UdpFSM::~UdpFSM(){
	Disconnect();
    WSACleanup ();
}

void UdpFSM::Initialize() {
}


void UdpFSM::Work(string message,unsigned int from) {
	uint8 *msg = GetBuffer(receivedMessageLength+MSG_HEADER_LENGTH);
	memcpy(msg+MSG_HEADER_LENGTH,message.c_str(),receivedMessageLength);	
	//memcpy(GetNewMessage(),fsmMessageR,receivedMessageLength);

	SetMsgToAutomate(GetAutomate(), msg);
	SetMsgToGroup(INVALID_08, msg);
	SetMsgObjectNumberTo(GetObjectId(), msg);
	SetMsgInfoCoding(getProtocolInfoCoding(), msg);
	memcpy(protocolMessageR,message.c_str(),receivedMessageLength);
	uint16 currentMessageCode = convertNetToFSMMessage();
	SetMsgCode(currentMessageCode, msg);
	SetMsgInfoLength(receivedMessageLength, msg);
	SendMessage(GetMbxId(), msg);
}	


int UdpFSM::startToListen(int port) {
	Sleep(500);
	portToListen = port;
	if(!listen(portToListen)){
		return 0;
	}

	if(!(hThread=CreateThread(NULL, 0, Listener, (void *)this, THREAD_PRIORITY_NORMAL, &thID))){
		return 0;
	}
	listening = 1;
	return 1;
}

int UdpFSM::startToListenMulticast(int port) {
	Sleep(500);
	portToListenMulticast = port;
	if(!listenMulticast(portToListenMulticast)){
		return 0;
	}

	if(!(hThreadMulticast=CreateThread(NULL, 0, ListenerMulticast, (void *)this, THREAD_PRIORITY_NORMAL, &thID))){
		return 0;
	}
	listeningMulticast = 1;
	return 1;
}


void UdpFSM::stopListening(){
    listening = 0;
	listeningMulticast = 0;
	closeListen();
	WaitForSingleObject(hThread,INFINITE);
	WaitForSingleObject(hThreadMulticast,INFINITE);
}
void UdpFSM::stopListeningMulticast(){
	
	listeningMulticast = 0;
//	WaitForSingleObject(hThreadMulticast,INFINITE);
	CloseHandle(hThreadMulticast);
	if(sockToListenMulticast){
		::closesocket(sockToListenMulticast);
		sockToListenMulticast=0;
	}
}

void UdpFSM::setPackageSize(uint16 p){
	packageSize = p;
}


void UdpFSM::closeListen(){
	if(sockToListen){
		::closesocket(sockToListen);
		sockToListen=0;
	}
	if(sockToListenMulticast){
		::closesocket(sockToListenMulticast);
		sockToListenMulticast=0;
	}
}

int UdpFSM::sendToUDP(const string &s,const string &host, int port){
	//int sock;
	struct sockaddr_in sa;
	struct hostent *hstEnt;

	/*if((sock=socket(AF_INET, SOCK_DGRAM, 0))==-1){
		Err="Unable to create socket.";
		return 0;
	}*/

	if(!(hstEnt=gethostbyname(host.c_str()))){
		switch(h_errno){
			case HOST_NOT_FOUND:
				Err="The specified host is unknown.";
				break;
			case NO_ADDRESS:
				Err="The requested host name is valid but does not have an IP address.";
				break;
			case NO_RECOVERY:
				Err="A non-recoverable name server error occurred.";
				break;
			case TRY_AGAIN:
				Err="A temporary error occurred on an authoritative name server.";
				break;
			default:
				Err="Unknown error in gethostbyname()";
		}
	//	closesocket(sock);
		return 0;
	}

	memset(&sa, '\0', sizeof(sa));
	memcpy(&(sa.sin_addr), hstEnt->h_addr, hstEnt->h_length);
	sa.sin_family=AF_INET;
	sa.sin_port=htons(port);

	if(::connect(sockToListen, (struct sockaddr*) &sa, sizeof(sa))==-1){
		Err="Cannot connect to the host: "+host;
	//	closesocket(sockToListen);
		return 0;
	}

	send(sockToListen, s.c_str(), packageSize, 0);
	//closesocket(sock);

	return 1;
}


string UdpFSM::error(){
	return Err;
}

int UdpFSM::sendToUDP( const string &host, int port){
//	int sock;
	struct sockaddr_in sa;
	struct hostent *hstEnt;

	/*if((sock=socket(AF_INET, SOCK_DGRAM, 0))==-1){
		Err="Unable to create socket.";
		return 0;
	}*/
    //sock = sockToListen;
	if(!(hstEnt=gethostbyname(host.c_str()))){
		switch(h_errno){
			case HOST_NOT_FOUND:
				Err="The specified host is unknown.";
				break;
			case NO_ADDRESS:
				Err="The requested host name is valid but does not have an IP address.";
				break;
			case NO_RECOVERY:
				Err="A non-recoverable name server error occurred.";
				break;
			case TRY_AGAIN:
				Err="A temporary error occurred on an authoritative name server.";
				break;
			default:
				Err="Unknown error in gethostbyname()";
		}
	//	closesocket(sock);
		return 0;
	}

	memset(&sa, '\0', sizeof(sa));
	memcpy(&(sa.sin_addr), hstEnt->h_addr, hstEnt->h_length);
	sa.sin_family=AF_INET;
	sa.sin_port=htons(port);

	if(::connect(sockToListen, (struct sockaddr*) &sa, sizeof(sa))==-1){
		Err="Cannot connect to the host: "+host;
	//	closesocket(sock);
		return 0;
	}


  uint8 *newMsg;
	fsmMessageSInfoLength = GetNewMsgInfoLength();

	fsmMessageSLength = fsmMessageSInfoLength;
	//in fsmMessageS is all fsm message = header + info
  newMsg = GetNewMessage();
  
  memcpy(fsmMessageS, newMsg + MSG_HEADER_LENGTH, fsmMessageSLength);
  RetBuffer(newMsg);
	//memcpy(fsmMessageS, (GetNewMessage()+MSG_HEADER_LENGTH), GetNewMsgInfoLength());
	SetNewMessage(NULL);
	convertFSMToNetMessage();
	uint16 sendedBytes = -1;
    sendedBytes = send(sockToListen, (char*)fsmMessageS, fsmMessageSLength, 0);
	//sendedBytes = send(sock, (char*)fsmMessageS, fsmMessageSLength, 0);
//	closesocket(sock);

	if(sendedBytes > 0)
	{
		//successfuly sended message
	}
	else if(sendedBytes == 0)
	{
		Err = "Message length is 0 in send call.";
	}
	else
	{
		Err = "Sending message caused error - socket error.";
	}
	return 1;
}

int UdpFSM::sendToUDPMulticast( const string &host, int port){

//	int iOptVal = 64;                                                     
  //char szMessage[] = "Multicasting message!";                           
                                    // Sent message string              
  TCHAR szError[100];               // Error message string             
  //SOCKET Sock = INVALID_SOCKET;     // Datagram window socket           
                                                                        
  SOCKADDR_IN dest_sin;             // Destination socket address       
                                                                        
/*  WSADATA WSAData;                  // Contains details of the          
                                    // Winsock implementation           
                                                                        
  // Initialize Winsock.                                                
  if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0)                        
  {                                                                     
    wsprintf (szError, TEXT("WSAStartup failed! Error: %d"),            
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    return 0;                                                       
  }                                                                     
                                                                        
  // Create a datagram window socket, Sock.                             
  if ((Sock = socket (AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)       
  {                                                                     
    wsprintf (szError, TEXT("Allocating socket failed! Error: %d"),     
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    return 0;                                                       
  }                                                                     
                                                                        
  // Fill out source socket's address information.                      
  source_sin.sin_family = AF_INET;                                      
  source_sin.sin_port = htons (3000);                            
  source_sin.sin_addr.s_addr = htonl (INADDR_ANY);                      
                                                                        
  // Associate the source socket's address with the socket, Sock.       
  if (bind (Sock,                                                       
            (struct sockaddr FAR *) &source_sin,                        
            sizeof (source_sin)) == SOCKET_ERROR)                       
  {                                                                     
    wsprintf (szError, TEXT("Binding socket failed! Error: %d"),        
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    closesocket (Sock);                                                 
    return 0;                                                       
  }                                                                     
                                                                        
  // Set the Time-to-Live of the multicast.                             
  if (setsockopt (Sock,                                                 
                  IPPROTO_IP,                                           
                  IP_MULTICAST_TTL,                                     
                  (char FAR *)&iOptVal,                                 
                  sizeof (int)) == SOCKET_ERROR) 
  				  
  {                                                                     
    wsprintf (szError, TEXT("setsockopt failed! Error: %d"),            
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    closesocket (Sock);                                                 
    return 0;                                                       
  }    */                                                                 
 //  struct hostent *hstEnt;

	/*if((sock=socket(AF_INET, SOCK_DGRAM, 0))==-1){
		Err="Unable to create socket.";
		return 0;
	}*/
    //sock = sockToListen;
/*	if(!(hstEnt=gethostbyname(host.c_str()))){
		switch(h_errno){
			case HOST_NOT_FOUND:
				Err="The specified host is unknown.";
				break;
			case NO_ADDRESS:
				Err="The requested host name is valid but does not have an IP address.";
				break;
			case NO_RECOVERY:
				Err="A non-recoverable name server error occurred.";
				break;
			case TRY_AGAIN:
				Err="A temporary error occurred on an authoritative name server.";
				break;
			default:
				Err="Unknown error in gethostbyname()";
		}
	//	closesocket(sock);
		return 0;
	}*/
  uint8 *newMsg;
	fsmMessageSInfoLength = GetNewMsgInfoLength();

	fsmMessageSLength = fsmMessageSInfoLength;
	//in fsmMessageS is all fsm message = header + info
  newMsg = GetNewMessage();
  
  memcpy(fsmMessageS, newMsg + MSG_HEADER_LENGTH, fsmMessageSLength);
  RetBuffer(newMsg);
	//memcpy(fsmMessageS, (GetNewMessage()+MSG_HEADER_LENGTH), GetNewMsgInfoLength());
	SetNewMessage(NULL);
	convertFSMToNetMessage();
	uint16 sendedBytes = -1;

  // Fill out the desination socket's address information.              
  dest_sin.sin_family = AF_INET;                                        
  dest_sin.sin_port = htons (port);
  
/*  memset(&dest_sin, '\0', sizeof(dest_sin));
  memcpy(&(dest_sin.sin_addr), hstEnt->h_addr, hstEnt->h_length);*/
  
//  memcpy(ip,host.c_str(),host.size());
  dest_sin.sin_addr.s_addr = inet_addr (host.c_str());                    
                                                                        
  // Send a message to the multicasting address.                        
  if (sendto (sockToListenMulticast, 
	          (char*)fsmMessageS, 
			  fsmMessageSLength,
              //szMessage,                                                
              //strlen (szMessage) + 1,                                   
              0,                                                        
              (struct sockaddr FAR *) &dest_sin,                        
              sizeof (dest_sin)) == SOCKET_ERROR)                       
  {                                                                     
    wsprintf (szError, TEXT("sendto failed! Error: %d"),                
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    closesocket (sockToListenMulticast);                                                 
    return 0;                                                       
  }                                                                     
  /*else                                                                  
    MessageBox (NULL, TEXT("Sending data succeeded!"), TEXT("Info"),    
                MB_OK);*/                                                 
                                                                        
 /* // Disable sending on Sock before closing it.                         
  shutdown (Sock, 0x01);                                                
                                                                        
  // Close Sock.                                                        
  closesocket (Sock);                                                   
                                                                        
  WSACleanup ();      */                                                  
                                                                        
  return 1;                             
}

unsigned int  UdpFSM::read(string &buffer){
	struct sockaddr_in sad;
	int sockln=sizeof(sad);
	char temp[MAX_PACKAGE_SIZE];
	memset(&sad, 0, sizeof(sad));
	if ((receivedMessageLength = recvfrom(sockToListen, temp, packageSize, 0,(sockaddr*)&sad, &sockln)) == SOCKET_ERROR){
	    Err="Error in reading!";
	}
	
	for(int i =0;i<receivedMessageLength;i++)
	{
       buffer+= temp[i];
	}
	//buffer = temp;
//	memcpy(&buffer,temp,receivedMessageLength);
	return  (unsigned int)sad.sin_addr.S_un.S_addr;
}

unsigned int  UdpFSM::readMulticast(string &buffer){
	struct sockaddr_in sad;
	int sockln=sizeof(sad);
	char temp[MAX_PACKAGE_SIZE];
	memset(&sad, 0, sizeof(sad));
	if ((receivedMessageLength = recvfrom(sockToListenMulticast, temp, packageSize, 0,(sockaddr*)&sad, &sockln)) == SOCKET_ERROR){
	    Err="Error in reading!";
	}
	
	for(int i =0;i<receivedMessageLength;i++)
	{
       buffer+= temp[i];
	}
	//buffer = temp;
//	memcpy(&buffer,temp,receivedMessageLength);
	return  (unsigned int)sad.sin_addr.S_un.S_addr;
}



int UdpFSM::listen(int port){
	if(sockToListen)
		return 0;
    struct sockaddr_in addr;
	int addrlen=sizeof(addr);
	const char *flag="1";

	sockToListen=socket(AF_INET, SOCK_DGRAM, 0);
	
	if(setsockopt(sockToListen, SOL_SOCKET, SO_REUSEADDR, flag, sizeof(flag))<0){
		Err="Cannot set socket options!";
		return 0;
	}
	
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=INADDR_ANY;

	// bind
	if(bind(sockToListen, (struct sockaddr *)&addr, addrlen)<0){
		Err="Cannot bind!";
		return 0;
	}
	return 1;
}
int UdpFSM::listenMulticast(int port){
    	int iOptVal = 64;                                                     
  //char szMessage[] = "Multicasting message!";                           
                                    // Sent message string              
  TCHAR szError[100];               // Error message string             
 // SOCKET Sock = INVALID_SOCKET;     // Datagram window socket           
                                                                        
  SOCKADDR_IN source_sin;           // Source socket address            
             // dest_sin;             // Destination socket address       
                                                                        
  WSADATA WSAData;                  // Contains details of the          
                                    // Winsock implementation           
                                                                        
  // Initialize Winsock.                                                
  if (WSAStartup (MAKEWORD(1,1), &WSAData) != 0)                        
  {                                                                     
    wsprintf (szError, TEXT("WSAStartup failed! Error: %d"),            
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    return 0;                                                       
  }                                                                     
                                                                        
  // Create a datagram window socket, Sock.                             
  if ((sockToListenMulticast = socket (AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)       
  {                                                                     
    wsprintf (szError, TEXT("Allocating socket failed! Error: %d"),     
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    return 0;                                                       
  }                                                                     
                                                                        
  // Fill out source socket's address information.                      
  source_sin.sin_family = AF_INET;                                      
  source_sin.sin_port = htons (portToListenMulticast);                            
  source_sin.sin_addr.s_addr = htonl (INADDR_ANY);                      
                                                                        
  // Associate the source socket's address with the socket, Sock.       
  if (bind (sockToListenMulticast,                                                       
            (struct sockaddr FAR *) &source_sin,                        
            sizeof (source_sin)) == SOCKET_ERROR)                       
  {                                                                     
    wsprintf (szError, TEXT("Binding socket failed! Error: %d"),        
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    closesocket (sockToListenMulticast);                                                 
    return 0;                                                       
  }                                                                     
                                                                        
  // Set the Time-to-Live of the multicast.                             
  if (setsockopt (sockToListenMulticast,                                                 
                  IPPROTO_IP,                                           
                  IP_MULTICAST_TTL,                                     
                  (char FAR *)&iOptVal,                                 
                  sizeof (int)) == SOCKET_ERROR) 
  				  
  {                                                                     
    wsprintf (szError, TEXT("setsockopt failed! Error: %d"),            
              WSAGetLastError ());                                      
    MessageBox (NULL, szError, TEXT("Error"), MB_OK);                   
    closesocket (sockToListenMulticast);                                                 
    return 0;                                                       
  }  
	return 1;
}

int UdpFSM::sendTo(const string &message){
	sendToUDP( ip, port);
	return 0;
}

int UdpFSM::sendTo(){
	sendToUDP( ip, port);
	return 0;
}

int UdpFSM::sendToMulticast(){
	sendToUDPMulticast("224.0.1.41",1718);//privremeno za H.323 gatekeeper discovery 
	return 0;
}

uint8 UdpFSM::getProtocolInfoCoding(){
	return 1;
}

MessageInterface *UdpFSM::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void UdpFSM::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void UdpFSM::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

