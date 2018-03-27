// **************************************************************************
//                        NetFSM.cpp  -  description
//                           -------------------
//  begin                : Mon Nov 5 2001
//  copyright            : (C) 2001 by Aleksander Stojicevic
//  email                : sasa@krt.neobee.net
//  Description:
//  This class is superclass for automates whose need to send and receive
//  messages to/from network.
// **************************************************************************
#include "NetFSM.h"
#include "NetException.h"
#include "ThreadException.h"
#include "Client_DTP.h"

// Function: Constructor
// Parameters: 
//  (in) uint16 numOfTimers - max. number of timers used (per FSM entity)
//  (in) uint16 numOfStates - max. number of FSM states
//  (in) uint16 maxNumOfProceduresPerState - max. number of procedures per one state
// Return value: none
// Description: 
//  Gets memory and initializes data structures..
//  Throw exceptions if something wrong with network - NetException.
NetFSM::NetFSM( uint8 automateId1, uint8 mbxId1, uint16 numOfTimers, uint16 numOfState, uint16 maxNumOfProceduresPerState) : 
TransportInterface( automateId1, mbxId1, numOfTimers, numOfState, maxNumOfProceduresPerState)
{
	priv = new unsigned char[MAX_LENGTH_MESSAGE];

	parameters = NULL;
	receivingSide = NULL;//02.2007

	ip = new char[16];
	fsmMessageR = new uint8[MAX_LENGTH_MESSAGE];
	protocolMessageR = new uint8[MAX_LENGTH_MESSAGE];
	fsmMessageS = new uint8[MAX_LENGTH_MESSAGE];
	protocolMessageS = new uint8[MAX_LENGTH_MESSAGE];

	uint16 error;
	OutputDebugString( (LPCWSTR) "Initialize sockets.....");
#ifdef WIN32
	error = WSAStartup(MAKEWORD(2,2),&wsaData);
	if(error != 0 )
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.*/
		OutputDebugString((LPCWSTR) "  !OK\n");
		throw NetException("Can't find usable WinSock.dll");
	}
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	if(LOBYTE(wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.*/
		OutputDebugString((LPCWSTR) "  !OK\n");
		WSACleanup();
		throw NetException("WinSock DLL supports 2.2");
	}
#endif
}

// Function: Destructor
// Parameters: none
// Return value: none
// Description: 
//  Release alocated resources.
NetFSM::~NetFSM()
{
	if(parameters != NULL)
		delete parameters;
	if(receivingSide != NULL)
		receivingSide->stop();
#ifdef WIN32
	WSACleanup();
#endif
	delete[] priv;
	delete[] ip;
	delete[] fsmMessageR;
	delete[] protocolMessageR;
	delete[] fsmMessageS;
	delete[] protocolMessageS;

}

// Function: acceptConnection(SOCKET newSocket)
// Parameters: 
//  (in) SOCKET newSocket		- Socket of TCP connection.
// Return value: If succesfuly accepted return 0 else return -1.
// Description: 
//  Accepting socket for TCP connection, create and start thread
//  for accepting data whose arrive on that socket.
int NetFSM::acceptConnection(SOCKET newSocket,void *exParam)
{
	parameters = (struct exParam *)exParam;
	try
	{
		receivingSide = new Client_DTP(newSocket,this);
		receivingSide->start();
		client = newSocket;
	}
	catch(...)
	{
		return -1;
	}
	return 0;
}

// Function: establishConnection()
// Parameters: none
// Return value: none.
// Description: 
//  Establish TCP connection. Before calling this method is 
//  desirable to put right port and ip address for TCP connection.
//  Throw exceptions if something wrong with network - NetException.
void NetFSM::establishConnection()
{
	unsigned long privIp;
	if((*ip <= '9') && (*ip >= '0'))
	{
		if((privIp = inet_addr(ip)) == INADDR_NONE)
			printf("invalid host ip given");
	}
	else
	{
		hostent* ent = gethostbyname(ip);
		if(!ent)
			printf("Windows Sockets error %d: Couldn't bind socket.\n",WSAGetLastError());
		privIp = *(unsigned long*)(ent->h_addr);
	}


	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr = *(in_addr*)&privIp;



	if((cli_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP))==INVALID_SOCKET)
		throw NetException("Windows Sockets error.\n");



	uint32 iOptval = 1;
	if(setsockopt( cli_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&iOptval, sizeof(uint32)) )
	{
		throw NetException("Windows Sockets error. Setsockopt.\n");
	}

	cli_addr.sin_family=AF_INET;
	cli_addr.sin_addr.s_addr=htonl(INADDR_ANY);        
	cli_addr.sin_port=htons(0);                //no specific port req'd
	//Bind client socket to any local interface and port
	if(bind(cli_sock,(LPSOCKADDR)&cli_addr,sizeof(cli_addr))==SOCKET_ERROR)
	{
		throw NetException("Windows Sockets error. Couldn't bind socket.\n");
	}

	if(SOCKET_ERROR == connect(cli_sock,(sockaddr*)&name,sizeof(sockaddr))){
		int error = WSAGetLastError();
		throw NetException("Windows Sockets error. Connect.\n");
	}
	if(client != 0)
		closesocket(client);
	client = cli_sock;

	try
	{
		receivingSide = new Client_DTP(client,this);
		receivingSide->start();
	}
	catch(...)
	{
		throw ThreadException("Can't create and start receivingSide.\n");
	}

}

// Function: killConnection()
// Parameters: none
// Return value: none.
// Description: 
//  Kill TCP connection. 
void NetFSM::killConnection()
{
	NetError();
	receivingSide->releaseAll();	
}

// Function: sendToTCP()
// Parameters: none
// Return value: none.
// Description: 
//  Sends message to another computer throught TCP connection.
//  Throw exceptions if something wrong with network - NetException. 
void NetFSM::sendToTCP() {
	uint8 *newMsg;
	fsmMessageSInfoLength = GetNewMsgInfoLength();

	fsmMessageSLength = fsmMessageSInfoLength + MSG_HEADER_LENGTH;
	//in fsmMessageS is all fsm message = header + info
	newMsg = GetNewMessage();

	memcpy(fsmMessageS, newMsg, GetNewMsgInfoLength() + MSG_HEADER_LENGTH);
	RetBuffer(newMsg);
	//memcpy(fsmMessageS, (GetNewMessage()+MSG_HEADER_LENGTH), GetNewMsgInfoLength());
	SetNewMessage(NULL);
	convertFSMToNetMessage();
	uint16 sendedBytes = -1;
	if(client == 0)
		throw NetException("Connection is broken!");

	//begining - tcp stream control
	memcpy(priv,protocolMessageS,MAX_LENGTH_MESSAGE);

	sendedBytes = send(client, (const char *)protocolMessageS, strlen((const char*)protocolMessageS) ,0);

	if(sendedBytes > 0)
	{
		//successfuly sended message
	}
	else if(sendedBytes == 0)
	{
		throw NetException("Message length is 0 in send call. NetFSM::SendToTCP()");
	}
	else
	{
		throw NetException("Sending message caused error - socket error. NetFSM::SendToTCP()");
	}

}

// Function: workWhenReceive(uint16 msgCoding)
// Parameters: 
//  (in) uint16 msgCoding		- Message coding for FSM system.
// Return value: none.
// Description: 
//  Prepare message from fsmMessageR to sending and sending her to own "mailbox". 
void NetFSM::workWhenReceive(uint16 msgCoding) {
	//PrepareNewMessage(receivedMessageLength,INVALID_16);
	//	PrepareNewMessage(fsmMessageRLength+MSG_HEADER_LENGTH,INVALID_16);
	uint8 *msg = GetBuffer(fsmMessageRLength+MSG_HEADER_LENGTH);
	//memcpy(GetNewMessage(),fsmMessageR,receivedMessageLength);
	memcpy(msg+MSG_HEADER_LENGTH,fsmMessageR,fsmMessageRLength);

	SetMsgToAutomate(GetAutomate(), msg);
	SetMsgToGroup(GetGroup(), msg);
	SetMsgObjectNumberTo(GetObjectId(), msg);
	SetMsgInfoCoding(msgCoding, msg);
	SetMsgCode(currentMessageCode, msg);
	SetMsgInfoLength(fsmMessageRLength, msg);

	SendMessage(GetMbxId(), msg);
	//	SetNewMessage(NULL);
}

// Function: getMsgCodeFSMMessageS()
// Parameters: none
// Return value: Message code from fsmMessageS.
// Description: 
//  Gives message code from fsmMessageS message. 
uint16 NetFSM::getMsgCodeFSMMessageS()
{
	return *(fsmMessageS+MSG_CODE);
}

// Function: getMsgCodingInfoFSMMessageS()
// Parameters: none
// Return value: Message coding info from fsmMessageS.
// Description: 
//  Gives message coding info from fsmMessageS message. 
uint8 NetFSM::getMsgCodingInfoFSMMessageS()
{
	return *(fsmMessageS+MSG_INFO_CODING);
}

int NetFSM::sendTo(const string &message){
	return -1; //not implemented
}


