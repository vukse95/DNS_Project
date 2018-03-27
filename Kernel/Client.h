// **************************************************************************
//                        Client.h  -  description
//                           -------------------
//  begin                : Mon Nov 5 2001
//  coded by             : Aleksander Stojivcevic
//  e-mail               : sasa@krt.neobee.net
//  Description:
//  This file contains declaration for Client class used for receiving messages from net. 
//  Is used by some automate extended from NetFSM
// **************************************************************************
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "thread.h"
#include <list>

#define MAX_LENGTH_MESSAGE		1024
#define STATE0					0
#define STATE1					1
#define STATE2					2

#include "NetException.h"


using namespace std;


class NetFSM;

static int nrOfClients;//potrebno kod raspustanja kriticne sekcije
//static CRITICAL_SECTION CriticalSection;

class Client : public ThreadBase
{
	public:
		SOCKET client;
		char FAR* ptrBuffer;
		
		
		Client(SOCKET &cl,NetFSM *nFSM);
		~Client();

		virtual void workToDo();

		void getError();
		void releaseAll();
		void workWithTable();

		int currentState;
		unsigned short currentMsgLength;
		unsigned char* priv ;//koristi se ko tcp security control-a za privremeno cuvanje poruke kada je jedna
							 //poruka poslata u vise delova rpeko tcp-a
		unsigned short recvMsgLength;
		
		bool send;
		bool haveMore;

		bool readFromNET;



	protected:
		NetFSM *netFSM;
};
#endif