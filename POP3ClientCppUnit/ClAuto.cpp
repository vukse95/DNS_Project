#include <stdio.h>

#include "const.h"
#include "ClAuto.h"

#define StandardMessageCoding 0x00

ClAuto::ClAuto() : FiniteStateMachine(CL_AUTOMATE_TYPE_ID, CL_AUTOMATE_MBX_ID, 0, 10, 2) {
}

ClAuto::~ClAuto() {
}


uint8 ClAuto::GetAutomate() {
	return CL_AUTOMATE_TYPE_ID;
}

/* This function actually connnects the ClAutoe with the mailbox. */
uint8 ClAuto::GetMbxId() {
	return CL_AUTOMATE_MBX_ID;
}

uint32 ClAuto::GetObject() {
	return GetObjectId();
}

MessageInterface *ClAuto::GetMessageInterface(uint32 id) {
	return &StandardMsgCoding;
}

void ClAuto::SetDefaultHeader(uint8 infoCoding) {
	SetMsgInfoCoding(infoCoding);
	SetMessageFromData();
}

void ClAuto::SetDefaultFSMData() {
	SetDefaultHeader(StandardMessageCoding);
}

void ClAuto::NoFreeInstances() {
	printf("[%d] ClAuto::NoFreeInstances()\n", GetObjectId());
}

void ClAuto::Reset() {
	printf("[%d] ClAuto::Reset()\n", GetObjectId());
}


void ClAuto::Initialize() {

	

	SetState(FSM_Client_Idle);	
	
	//intitialization message handlers
	InitEventProc(FSM_Client_Idle, MSG_Client_Idle, (PROC_FUN_PTR)&ClAuto::FSM_Client_Idle_State );
	InitEventProc(FSM_Client_Input_From_User, MSG_User_To_Client_Data_Sent, (PROC_FUN_PTR)&ClAuto::FSM_Client_Get_Data_From_User );
	InitEventProc(FSM_Client_Check_Local_Table, MSG_Client_Check_Local_Table, (PROC_FUN_PTR)&ClAuto::FSM_Client_Check_In_Local_Table );
	InitEventProc(FSM_Client_DNS_Request, MSG_Client_To_Channel_DNS_Request_Sent, (PROC_FUN_PTR)&ClAuto::FSM_Client_Pass_DNS_Request_To_Channel );
	InitEventProc(FSM_Client_DNS_Recived, MSG_Channel_To_Client_DNS_Request_Recived, (PROC_FUN_PTR)&ClAuto::FSM_Client_Get_DNS_Request_From_Channel );
	InitEventProc(FSM_Client_DNS_Recived, MSG_Client_To_User_Return_Data_Recived, (PROC_FUN_PTR)&ClAuto::FSM_Client_Pass_DNS_Request_To_Client );
	InitEventProc(FSM_Client_DNS_Request, MSG_Timer_Expired, (PROC_FUN_PTR)&ClAuto::FSM_Client_Timer_Expired );

	InitTimerBlock(TIMER1_ID, TIMER1_COUNT, MSG_Timer_Expired);
}

void ClAuto::FSM_Client_Idle_State(){
	
	SetState(FSM_Client_Input_From_User);
	//TODO: Clean input output buffers
}

void ClAuto::FSM_Client_Get_Data_From_User(){
	// Get Data From Client
	
	uint8* buffer = GetParam(PARAM_DATA);
	uint16 size = buffer[2];

	memcpy(DNSRequestInput, buffer + 4,size);
	DNSRequestInput[size] = 0;

	PrepareNewMessage(0x00, MSG_Client_Check_Local_Table);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CL_AUTOMATE_MBX_ID);

	SetState(FSM_Client_Check_Local_Table);
}

void ClAuto::FSM_Client_Check_In_Local_Table(){

	// Check in table
	string line;
	string search;
	ifstream tableFile("table.txt");
	size_t pos;
	int foundFlag = 0;

	// Copy from char* to String
	search.copy(DNSRequestInput, (unsigned)strlen(DNSRequestInput));

	if (tableFile.is_open())
	{
		while (tableFile.good())
		{
			getline(tableFile, line); // get line from file
			pos = line.find(search); // search
			if (pos != string::npos) // string::npos is returned if string is not found
			{
				cout << "Found!";
				getline(tableFile, line); // hop to next line to get IP address
				strcpy(DNSRequestOutput, line.c_str()); // Copy from string to char* array
				foundFlag = 1;
				break;
			}
		}
		tableFile.close();
	}
	if (foundFlag)
	{
		// IP found in local table, skip sending part
		PrepareNewMessage(0x00, MSG_Client_To_User_Return_Data_Recived);
		SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(CL_AUTOMATE_MBX_ID);

		SetState(FSM_Client_DNS_Recived);

		//Send from FSM_Client_Pass_DNS_Request_To_Client function
	}
	else
	{
		PrepareNewMessage(0x00, MSG_Client_To_Channel_DNS_Request_Sent);
		SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
		SetMsgObjectNumberTo(0);
		SendMessage(CL_AUTOMATE_MBX_ID);

		SetState(FSM_Client_DNS_Request);
	}
}

void ClAuto::FSM_Client_Pass_DNS_Request_To_Channel(){
	// Sent MSG to Channel
	// Nije dobro treba da prosledi request ne samo poruku - ISPRAVLJENO

	PrepareNewMessage(0x00, MSG_Client_To_Channel_DNS_Request_Sent);
	SetMsgToAutomate(CH_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_DNS_INPUT, strlen(DNSRequestInput), (uint8*)DNSRequestInput);

	SendMessage(CH_AUTOMATE_MBX_ID);


	// Started timeout timer
	StartTimer(TIMER1_ID);

	SetState(FSM_Client_DNS_Recived);
}

void ClAuto::FSM_Client_Get_DNS_Request_From_Channel(){
	
	uint8* buffer = GetParam(PARAM_DATA);
	uint16 size = buffer[2];

	// Stoped Timer if data is recived
	StopTimer(TIMER1_ID);

	memcpy(DNSRequestOutput, buffer + 4, size);
	DNSRequestOutput[size] = 0;

	/*
	PrepareNewMessage(0x00, MSG_Client_To_User_Return_Data_Recived);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CL_AUTOMATE_MBX_ID);
	*/
}

void ClAuto::FSM_Client_Pass_DNS_Request_To_Client(){
	// Pass Recived data from channel to User fsm
	PrepareNewMessage(0x00, MSG_Client_To_User_Return_Data_Recived);
	SetMsgToAutomate(USER_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	AddParam(PARAM_DATA, strlen(DNSRequestOutput), (uint8*)DNSRequestOutput);

	SendMessage(USER_AUTOMATE_MBX_ID);

	SetState(FSM_Client_Idle);
}

void ClAuto::FSM_Client_Timer_Expired(){
	// If timer expires send request again

	PrepareNewMessage(0x00, MSG_Client_To_Channel_DNS_Request_Sent);
	SetMsgToAutomate(CL_AUTOMATE_TYPE_ID);
	SetMsgObjectNumberTo(0);
	SendMessage(CL_AUTOMATE_MBX_ID);

	SetState(FSM_Client_DNS_Request);
}