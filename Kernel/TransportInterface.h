#ifndef _TransportInterface_H
#define _TransportInterface_H

#include "../kernel/fsm.h"
#include "../kernel/fsmsystem.h"
#include <string>

using namespace std;

class TransportInterface : public FiniteStateMachine{

protected:
	uint16  port;
	char   *ip;

	uint8 *fsmMessageR;
	uint8 *protocolMessageR;
	uint8 *fsmMessageS;
	uint8 *protocolMessageS;
	uint16 sendMsgLength;
	uint16 fsmMessageSInfoLength;
	uint16 fsmMessageSLength;
	uint16 receivedMessageLength;
	uint16 fsmMessageRLength;

	uint16 currentMessageCode;

public:
	virtual uint16 convertNetToFSMMessage()=0;//povratna vrednost je message code
	virtual void convertFSMToNetMessage()=0;

	virtual int sendTo(const string &message) = 0;

	virtual void establishConnection() = 0;
	virtual void setPort(uint16 port);
	virtual void setIP(char* ip);
	virtual char *getIP();
    virtual void Disconnect() = 0;


	TransportInterface( uint8 automateId1, uint8 mbxId1, uint16 numOfTimers, uint16 numOfState, uint16 maxNumOfProceduresPerState);
	~TransportInterface();

};

#define StandardMessage stdMsg_pc16_pl16

#define StandardMessageCoding 0x00

#endif /* _TransportInterface_H */