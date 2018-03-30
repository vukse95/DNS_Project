#ifndef _CONST_H_
#define _CONST_H_

#include "./kernelTypes.h"

//#define D_TESTING

const uint8 CH_AUTOMATE_TYPE_ID = 0x00;
const uint8 CL_AUTOMATE_TYPE_ID = 0x01;
const uint8 USER_AUTOMATE_TYPE_ID = 0x02;
const uint8 TEST_AUTOMATE_TYPE_ID = 0x03;

const uint8 CH_AUTOMATE_MBX_ID = 0x00;
const uint8 CL_AUTOMATE_MBX_ID = 0x01;
const uint8 USER_AUTOMATE_MBX_ID = 0x02;
const uint8 TEST_MBX_ID = 0x03;

//	Client states
enum ClStates {	FSM_Client_Idle,
				FSM_Client_Check_Local_Table,
				FSM_Client_Input_From_User, 
				FSM_Client_DNS_Request,
				FSM_Client_DNS_Recived };

// User messages
const uint16 MSG_User_Idle							= 0x0001;
const uint16 MSG_User_Data_Input					= 0x0002;
const uint16 MSG_User_To_Client_Data_Sent			= 0x0003;
const uint16 MSG_Client_To_User_Return_Data_Recived	= 0x0004;

// Client messages
const uint16 MSG_Client_Idle							= 0x0005;
const uint16 MSG_Client_To_Channel_DNS_Request_Sent		= 0x0006;
const uint16 MSG_Channel_To_Client_DNS_Request_Recived	= 0x0007;
const uint16 MSG_Timer_Expired							= 0x0008;
const uint16 MSG_Client_Check_Local_Table				= 0x0009;

// Channel messages
const uint16 MSG_Channel_Idle				= 0x000a;
const uint16 MSG_Channel_Create_Socket		= 0x000b;
const uint16 MSG_Channel_Connect			= 0x000c;
const uint16 MSG_Channel_DNS_Request_Send	= 0x000d;
const uint16 MSG_Channel_DNS_Request_Recive	= 0x000f;
const uint16 MSG_Channel_Disconnect			= 0x0010;


#define ADRESS "localhost"
//#define ADRESS "mail.spymac.com"
//#define ADRESS "krtlab8"
#define PORT 111

#define TIMER1_ID 0
#define TIMER1_COUNT 10
#define TIMER1_EXPIRED 0x20

#define TIMER2_ID 1
#define TIMER2_COUNT 10 //NOT SURE WHAT TIME - NOT IMPORTANT FOR NOW
#define TIMER2_EXPIRED 0x30

#define PARAM_DATA 0x01
#define PARAM_Name 0x02
#define PARAM_DNS_INPUT 0x03


#endif //_CONST_H_