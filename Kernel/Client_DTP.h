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
#ifndef __CLIENTDTP_H__
#define __CLIENTDTP_H__

#include "Client.h"
#include "thread.h"
#include <list>

#define MAX_LENGTH_MESSAGE		40960
#define STATE0					0
#define STATE1					1
#define STATE2					2

#include "NetException.h"


using namespace std;


class NetFSM;

class Client_DTP : public Client
{
	public:
				
		Client_DTP(SOCKET &cl,NetFSM *net);
		~Client_DTP();

		void workToDo();

		
};
#endif