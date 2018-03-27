#ifndef _User_AUTO_H_
#define _User_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>

#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"


class UserAuto : public FiniteStateMachine {
	
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
	
	// FSM States
	enum	UserStates {FSM_User_Idle, 
						FSM_User_Data_Input, 
						FSM_User_To_Client_Data,
						FSM_Client_To_User_Return_Data };

	
public:
	UserAuto();
	~UserAuto();
	
	void Initialize();
	
	void FSM_User_Idle_Set_All();
	void FSM_User_Data_Input_From_Console();
	void FSM_User_Pass_Data_To_Client();
	void FSM_User_Get_Data_From_Client();

	void Start();
	
protected:
	
	char DNSRequestInput[256];
	
};

#endif /* _User_AUTO_H */