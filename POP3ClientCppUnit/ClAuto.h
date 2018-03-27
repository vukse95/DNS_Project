#ifndef _Cl_AUTO_H_
#define _Cl_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>
#include <cstring>
#include <fstream>

#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"


class ClAuto : public FiniteStateMachine {
	
	// for FSM
	StandardMessage StandardMsgCoding;
	
	MessageInterface *GetMessageInterface(uint32 id);
	void	SetDefaultHeader(uint8 infoCoding);
	void	SetDefaultFSMData();
	void	NoFreeInstances();
	void	Reset();
	uint8	GetMbxId();
	uint8	GetAutomate();
	uint32	GetObject();
	void	ResetData();
	

public:
	ClAuto();
	~ClAuto();
	
	void Initialize();
	
	void FSM_Client_Idle_State();
	void FSM_Client_Get_Data_From_User();
	void FSM_Client_Check_In_Local_Table();
	void FSM_Client_Pass_DNS_Request_To_Channel();
	void FSM_Client_Get_DNS_Request_From_Channel();
	void FSM_Client_Pass_DNS_Request_To_Client();
	void FSM_Client_Timer_Expired();


	
protected:

	char DNSRequestInput[256];
	char DNSRequestOutput[256];

};

#endif /*_Cl_AUTO_H */