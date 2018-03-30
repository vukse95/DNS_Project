#include <stdio.h>

#include "const.h"
#include "ChAuto.h"

#define StandardMessageCoding 0x00

ChAuto::ChAuto() : FiniteStateMachine(CH_AUTOMATE_TYPE_ID, CH_AUTOMATE_MBX_ID, 1, 6, 3) {
}

ChAuto::~ChAuto() {
}

uint8 ChAuto::GetAutomate() {
	return CH_AUTOMATE_TYPE_ID;
}

/* This function actually connnects the ChAuto with the mailbox. */
uint8 ChAuto::GetMbxId() {
	return CH_AUTOMATE_MBX_ID;
}

uint32 ChAuto::GetObject() {
	return GetObjectId();
}

MessageInterface *ChAuto::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void ChAuto::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void ChAuto::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void ChAuto::NoFreeInstances() {
	printf("[%d] ChAuto::NoFreeInstances()\n", GetObjectId());
}

void ChAuto::Reset() {
	printf("[%d] ChAuto::Reset()\n", GetObjectId());
}


void ChAuto::Initialize() {

	FirstStartFlag = 0;

	SetState(FSM_Channel_Idle);
	
	//intitialization message handlers
	InitEventProc(FSM_Channel_Idle, MSG_Channel_Idle, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Idle_State);
	InitEventProc(FSM_Channel_DNS_Request, MSG_Client_To_Channel_DNS_Request_Sent, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Get_DNS_Request);
	InitEventProc(FSM_Channel_Connect, MSG_Channel_Create_Socket, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Connect_State);
	InitEventProc(FSM_Channel_Connected, MSG_Channel_DNS_Request_Send, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Connected_State);
	InitEventProc(FSM_Channel_Connected, MSG_Channel_DNS_Request_Recive, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Recive_DNS_Request_From_Server);
	InitEventProc(FSM_Channel_Disconnect, MSG_Channel_Disconnect, (PROC_FUN_PTR)&ChAuto::FSM_Channel_Disconnect_State);
	InitEventProc(FSM_Channel_To_Client_DNS_Request_Sent, MSG_Channel_To_Client_DNS_Request_Recived, (PROC_FUN_PTR)&ChAuto::FSM_Channel_To_Client_DNS_Request_Pass);

}

void ChAuto::FSM_Channel_Idle_State() {

	SetState(FSM_Channel_DNS_Request);
	FirstStartFlag++;
}

void ChAuto::FSM_Channel_Get_DNS_Request() {
	// Get DNS Request From Client FSM...

	uint8* buffer = GetParam(PARAM_DNS_INPUT);
	uint16 size = buffer[2];

	memcpy(DNSRequestInput, buffer + 4, size);
	DNSRequestInput[size] = 0;

	

	// Create socket only on first start... :D
	if (FirstStartFlag == 1)
	{
		PrepareNewMessage(0x00, MSG_Channel_Create_Socket);
		SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(CH_AUTOMATE_MBX_ID);

		SetState(FSM_Channel_Connect);
	}
	
}

void ChAuto::FSM_Channel_Connect_State(){
	
	WSAData wsaData;

	printf("\nConnecting to Server!");

	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0) 
	{
		return;
	} 

	/* Try to resolve the server name. */
	sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
	
	unsigned int addr = inet_addr(ADRESS);
	if (addr != INADDR_NONE) 
	{
        server_addr.sin_addr.s_addr	= addr;
        server_addr.sin_family		= AF_INET;
    }
    else 
	{
        hostent* hp = gethostbyname(ADRESS);
        if (hp != 0)  
		{
            memcpy( &(server_addr.sin_addr), hp->h_addr, hp->h_length );
            server_addr.sin_family = hp->h_addrtype;
        }
        else 
		{
			return ;
        }
    }
	server_addr.sin_port = htons(PORT);

	/* Create the socket. */ 
	m_Socket = socket(PF_INET, SOCK_STREAM, 0);
	if (m_Socket == INVALID_SOCKET) {
		return ;
	}

	/* Try to reach the server. */
	if (connect(m_Socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		/* Here some additional cleanup should be done. */
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return ;
	}
	printf("\nConnected to Server!");

	send(m_Socket, DNSRequestInput, strlen(DNSRequestInput), 0);

	/* Then, start the thread that will listen on the the newly created socket. */
	m_hThread = CreateThread(NULL, 0, ClientListener, (LPVOID) this, 0, &m_nThreadID); 
	if (m_hThread == NULL) {
		/* Cannot create thread.*/
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return ;
	}

	
	
}

void ChAuto::FSM_Channel_Connected_State(){
	// Connected
}
void ChAuto::FSM_Channel_Recive_DNS_Request_From_Server(){

}
void ChAuto::FSM_Channel_Disconnect_State(){
	PrepareNewMessage(0x00, MSG_Channel_Idle);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);

	SetState(FSM_Channel_Idle);
}

void ChAuto::FSM_Channel_To_Client_DNS_Request_Pass(const char* apBuffer, uint16 anBufferLength) {

	// SALJI KLIJENTU TODO
	PrepareNewMessage(0x00, MSG_Channel_To_Client_DNS_Request_Recived);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_DATA, anBufferLength, (uint8 *)apBuffer);
	SendMessage(CL_AUTOMATE_MBX_ID);

	// Posle slanja disconnectuj se
	PrepareNewMessage(0x00, MSG_Channel_Disconnect);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);

	SetState(FSM_Channel_Disconnect);

}

DWORD ChAuto::ClientListener(LPVOID param) {
	ChAuto* pParent = (ChAuto*)param;
	int nReceivedBytes = 0;
	char* buffer = new char[255];

	//Needs initialization
	//nReceivedBytes = recv(pParent->m_Socket, buffer, 255, 0);
	if (nReceivedBytes < 0) {
		DWORD err = WSAGetLastError();
	}
	else {
		pParent->FSM_Channel_Connected_State();

		/* Receive data from the network until the socket is closed. */
		do {
			nReceivedBytes = recv(pParent->m_Socket, buffer, 255, 0);
			if (nReceivedBytes == 0)
			{
				printf("\n\nDisconnected from server!\n");
				pParent->FSM_Channel_Disconnect_State();
				break;
			}
			if (nReceivedBytes < 0) {
				printf("error\n");
				DWORD err = WSAGetLastError();
				break;
			}
			pParent->FSM_Channel_To_Client_DNS_Request_Pass(buffer, nReceivedBytes);

			Sleep(1000);

		} while (1);

	}

	delete[] buffer;
	return 1;
}