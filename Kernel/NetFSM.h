// **************************************************************************
//                        NetFSM.h  -  description
//                           -------------------
//  begin                : Wed Oct 24 2001
//  coded by             : Aleksander Stojivcevic
//  e-mail               : sasa@krt.neobee.net
//  Description:
//  This class is superclass for automates whose need to send and receive
//  messages to/from network.
// **************************************************************************
//
// DECRIPTION OF abstract FUNCTIONS for class NetFSM
//
// Function:  convertNetToFSMMessage
// Parameters: none
// Return value:
//  uint16 - message code of received message accepted from network
// Description: 
//  Converts accepted message from protocol format to FSM system format.
//
// Function:  convertFSMToNetMessage
// Parameters: none
// Return value: none
// Description: 
//  Converts current message from FSM system format to protocol format.
//
// Function:  getProtocolInfoCoding
// Parameters: none
// Return value:
//  uint8 - kind of coding for info part of protocol message, used when
//			prepare message for sending to own mailbox
// Description: 
//  Gives message coding information used by arrived protocol message.
//
// 
#if !defined(__NET_FSM__)
#define __NET_FSM__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../kernel/fsm.h"
#include "../kernel/systemTCP.h"
#include "TransportInterface.h"

class Client;
class Client_DTP;


class NetFSM : public TransportInterface{
	friend class Client;
	friend class Client_DTP;
public:

	struct exParam *parameters;

	virtual void SetDefaultHeader(uint8 infoCoding)=0;
	virtual void NoFreeInstances(){};
	virtual MessageInterface *GetMessageInterface(uint32 id)=0;
	//	  virtual char  *LogGetAutomateName()=0;
	//    virtual int    LogGetAutomateType()=0;
	//    virtual char  *LogGetStateName()=0;
	//    virtual int    LogGetState()=0;
	//    virtual uint32 LogGetCallId()=0;


	NetFSM( uint8 automateId1, uint8 mbxId1, uint16 numOfTimers, uint16 numOfState, uint16 maxNumOfProceduresPerState);
	~NetFSM();

	virtual int sendTo(const string &message);//not implemented

	virtual uint16 convertNetToFSMMessage()=0;//povratna vrednost je message code
	virtual void convertFSMToNetMessage()=0;
	virtual uint8 getProtocolInfoCoding()=0;//dobijam info coding protocola koji kasnije postavljam u pouku pre slanaj u svoj mbx
	virtual void NetError(){};

	virtual uint32 getTPKTLen()=0;
	virtual uint32 getLenFromTPKT(unsigned char *buff,int oldMessage=0)=0;
	virtual void setTPKT(uint16 sendMsgLen,unsigned char *place)=0;

	void workWhenReceive(uint16 msgCode);

	//sprega sa TCP support-om
	virtual int acceptConnection(SOCKET newSocket,void *exParam);


	//privremeno public zbog testa bila je protected
	virtual void establishConnection();

	void Disconnect(){return;}//zbog kompatibilnosti sa udpFSM
	sockaddr_in name;
	sockaddr_in cli_addr;


protected:

	void killListenerSide();
	void killConnection();
	void sendToTCP();

	uint16 getMsgCodeFSMMessageS();
	uint8  getMsgCodingInfoFSMMessageS();

	Client *receivingSide;
	SOCKET  client; //for establishing connection
	SOCKET cli_sock;


private:

	SOCKET srv_sock; //for receiving connection



	//Client *receivingSide;

	SOCKET  server;//for receiving connection
	HANDLE  acceptConnectionHandle;
#ifdef WIN32
	WSADATA wsaData;
#endif
	unsigned char *priv;

};


#endif 