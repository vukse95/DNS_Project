#ifndef _CH_AUTO_H_
#define _CH_AUTO_H_

#include <fsm.h>
#include <fsmsystem.h>

#include "../kernel/stdMsgpc16pl16.h"
#include "NetFSM.h"


class ChAuto : public FiniteStateMachine {
	
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
	enum	ChStates {	FSM_Channel_Idle, 
						FSM_Channel_DNS_Request, 
						FSM_Channel_Connect,
						FSM_Channel_Connected,
						FSM_Channel_Disconnect,
						FSM_Channel_To_Client_DNS_Request_Sent };

	//FSM_Ch_Idle
	void	FSM_Channel_Idle_State();
	//FSM_Ch_Connecting
	void	FSM_Channel_Get_DNS_Request();
	void	FSM_Channel_Connect_State();
	//FSM_Ch_Connected
	void	FSM_Channel_Connected_State();
	void	FSM_Channel_Disconnect_State();
	void	FSM_Channel_Recive_DNS_Request_From_Server();
	//void	FSM_Channel_To_Client_DNS_Request_Pass();
		
public:
	ChAuto();
	~ChAuto();
	
	//bool FSMMsg_2_NetMsg();
	void FSM_Channel_To_Client_DNS_Request_Pass(const char* apBuffer, uint16 anBufferLength);

	void Initialize();

protected:
	static DWORD WINAPI ClientListener(LPVOID);
	
	SOCKET m_Socket;
	HANDLE m_hThread;
	DWORD m_nThreadID;
	uint16 m_nMaxMsgSize;

	char DNSRequestInput[256];

	int FirstStartFlag;
};

#endif /* _CH_AUTO_H */