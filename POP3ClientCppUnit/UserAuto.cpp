#include <stdio.h>
#include <conio.h>
#include <time.h>

#include "const.h"
#include "UserAuto.h"

bool g_ProgramEnd = false;

#define StandardMessageCoding 0x00

UserAuto::UserAuto() : FiniteStateMachine(USER_AUTOMATE_TYPE_ID, USER_AUTOMATE_MBX_ID, 0, 4, 3) {
}

UserAuto::~UserAuto() {
}


uint8 UserAuto::GetAutomate() {
	return USER_AUTOMATE_TYPE_ID;
}

/* This function actually connnects the UserAuto with the mailbox. */
uint8 UserAuto::GetMbxId() {
	return USER_AUTOMATE_MBX_ID;
}

uint32 UserAuto::GetObject() {
	return GetObjectId();
}

MessageInterface *UserAuto::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void UserAuto::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void UserAuto::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void UserAuto::NoFreeInstances() {
	printf("[%d] UserAuto::NoFreeInstances()\n", GetObjectId());
}

void UserAuto::Reset() {
	printf("[%d] UserAuto::Reset()\n", GetObjectId());
}


void UserAuto::Initialize() {
	SetState(FSM_User_Idle);	
	
	//intitialization message handlers
	InitEventProc(FSM_User_Idle, MSG_User_Idle, (PROC_FUN_PTR)&UserAuto::FSM_User_Idle_Set_All);
	InitEventProc(FSM_User_Data_Input, MSG_User_Data_Input, (PROC_FUN_PTR)&UserAuto::FSM_User_Data_Input_From_Console);
	InitEventProc(FSM_User_To_Client_Data, MSG_User_To_Client_Data_Sent, (PROC_FUN_PTR)&UserAuto::FSM_User_Pass_Data_To_Client);
	InitEventProc(FSM_Client_To_User_Return_Data, MSG_Client_To_User_Return_Data_Recived, (PROC_FUN_PTR)&UserAuto::FSM_User_Get_Data_From_Client);
}

void UserAuto::Start(){

	PrepareNewMessage(0x00, MSG_User_Idle);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);
}

void UserAuto::FSM_User_Idle_Set_All(){
	// Set Client to IDLE
	PrepareNewMessage(0x00, MSG_Client_Idle);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CL_AUTOMATE_MBX_ID);

	// Set Channel to IDLE
	PrepareNewMessage(0x00, MSG_Channel_Idle);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CH_AUTOMATE_MBX_ID);

	// Set User to Next state
	PrepareNewMessage(0x00, MSG_User_Data_Input);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);

	SetState(FSM_User_Data_Input);

	//TODO: CLEAN DNSRequestInput buffer!!
}

void UserAuto::FSM_User_Data_Input_From_Console(){

	printf("\nEnter Domain Name[example: www.yahoo.com]: ");
	scanf("%s", &DNSRequestInput);

	PrepareNewMessage(0x00, MSG_User_To_Client_Data_Sent);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(USER_AUTOMATE_MBX_ID);

	SetState(FSM_User_To_Client_Data);
}

void UserAuto::FSM_User_Pass_Data_To_Client(){

	PrepareNewMessage(0x00, MSG_User_To_Client_Data_Sent);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_Name, strlen(DNSRequestInput), (uint8*)DNSRequestInput);
	
	SendMessage(CL_AUTOMATE_MBX_ID);


	SetState(FSM_Client_To_User_Return_Data);
	//Poruku setuje klijent
}

void UserAuto::FSM_User_Get_Data_From_Client(){

	//char* data = new char[255];
	uint8* buffer = GetParam(PARAM_DATA);
	uint16 size = buffer[2];

	memcpy(DNSRequestInput, buffer + 4,size);
	DNSRequestInput[size] = 0;

	printf("\nDNS_IP: %s", DNSRequestInput);
	
	FSM_User_Idle_Set_All(); // Set all FSM-s to IDLE

}