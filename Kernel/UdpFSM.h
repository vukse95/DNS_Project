#ifndef _UdpFSM_H
#define _UdpFSM_H

#define MAX_PACKAGE_SIZE 1500

#include "../kernel/fsm.h"
#include "../kernel/fsmsystem.h"
#include "../kernel/TransportInterface.h"
#include "../kernel/stdMsgpc16pl16.h"
//#include "../constants.h"

#define MAX_LENGTH_MESSAGE 4096

#include <string>

using namespace std;

class UdpFSM : public TransportInterface {
private:
	DWORD thID;
	HANDLE hThread;
	HANDLE hThreadMulticast;
	string Err; // Error string 
	uint16 packageSize;

public:
	UdpFSM( uint8 automateId1, uint8 mbxId1);
	~UdpFSM();
	void Initialize();
	virtual uint16 convertNetToFSMMessage()=0;//povratna vrednost je message code
	virtual void convertFSMToNetMessage()=0;

	int sockToListen;
	int sockToListenMulticast;
	int listening;
	int listeningMulticast;
	uint16 portToListen;
	uint16 portToListenMulticast;

	void setPackageSize(uint16);

	int listen(int port);
	int listenMulticast(int port);
	unsigned int read(string &);
	unsigned int readMulticast(string &buffer);

	int startToListen(int port); 
	int startToListenMulticast(int port);
	void stopListeningMulticast();
	void stopListening();
	void closeListen(); // close socket for listening

	virtual void Work(string,unsigned int); 
	
	int sendToUDP(const string &message,const string &host, int port);
	int sendToUDP( const string &host, int port);
	int sendToUDPMulticast( const string &host, int port);
	int sendTo(const string &message);
	int sendTo( );
	int sendToMulticast( );

	
	void establishConnection(){ return;}//zbog kompatibilnosti sa netFSM
	void Disconnect(){ closeListen();}
	virtual uint8 getProtocolInfoCoding() = 0;

	int receivedMessageLength;

	string error();

	int getSock(); 

	StandardMessage StandardMsgCoding;
	MessageInterface *GetMessageInterface(uint32 id);
	void SetDefaultHeader(uint8 infoCoding);
	void SetDefaultFSMData();
	uint32 GetObject();
	void ResetData();

};
#endif /* _UdpFSM_H */