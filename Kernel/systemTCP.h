// **************************************************************************
//                        systemTCP.h  -  description
//                           -------------------
//  begin                : Tue Jun 27 2001
//  coded by             : Ivan Velikic
//  email                : ivelikic@yahoo.com
//  description:
//
// **************************************************************************

#ifndef __TCP_SYSTEM__
#define __TCP_SYSTEM__

#include "uniSock.h"

#ifndef __BUFFERS__
#include "../kernel/buffers.h"
#endif

#ifndef __POST_OFFICE
#include "../kernel/postOffice.h"
#endif

#ifndef __TIMER__
#include "../kernel/timer.h"
#endif

#ifndef __FSM__
#include "fsm.h"
#endif

#define MAX_NUM_OF_TCP_SERVERS  10
#define MAX_PENDING_CONNECTIONS 10

struct TCPConnectionData {
  SOCKET ConnectionSocket;
  HANDLE ThreadHandle;
  DWORD  ThreadId;
  BYTE   AutomateType;
  class  FiniteStateMachine *ClientObject;  
  bool   Used;
};

//dodao zbog prosirenja InitTCPServer-a sa mogucnoscu dodavanja parametara
//added for extension of InitTCPServer with functionality for adding parameters
struct exParam{
	int index;
	int length;
	unsigned char *param;

	exParam(unsigned char *paramIn,int len){
		index  = 0;
		length = len;
		param  = new unsigned char[length];
		memcpy(param,paramIn,length);
	}
	~exParam(){delete []param;}
};


class TCPSystemSupport {
  private:
    TCPConnectionData ServerData[MAX_NUM_OF_TCP_SERVERS];
    
  protected:
      FiniteStateMachine *GetClientObject(int automatType);        

  public:
    TCPSystemSupport();
    virtual ~TCPSystemSupport();

    //prosireno sa novim parametrom (default) da bi bilo podrzano prosledjivanje parametra kod InitTCPServer
	//i sacuvan dosadasnji interface ka korisnicima TCPServer-a
    int InitTCPServer(uint16 port, uint8 automateType, char *ipAddress = 0,  unsigned char *parm = 0, int length = 0);
	  int WaitConnection(int index,exParam *exPrm = 0);
	    
    int DeinitTCPServer(int index);
};
#endif

