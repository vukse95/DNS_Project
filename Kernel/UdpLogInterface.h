#ifndef __UdpLogInterface__
#define __UdpLogInterface __

#include "uniSock.h"
#include "logAutomate.h"
const uint16 RECEIVE_PORT = 4321;

class UdpLogInterface : public LogInterface 
{
protected:
	SOCKET sendingSocket;
	struct sockaddr_in to;
	void initUdp(const int8* ip, uint16 port);
	void deInitUdp(void);
public:
	UdpLogInterface();
	UdpLogInterface(const int8 *ip, uint16 port = RECEIVE_PORT);
	~UdpLogInterface();
	virtual void Write();
};
#endif 