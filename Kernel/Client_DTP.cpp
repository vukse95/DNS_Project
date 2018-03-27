#include "NetFSM.h"
#include "Client_DTP.h"


// Function: Constructor
// Parameters: 
//  (in) SOCKET cl		- SOCKET throght data arrive.
//  (in) NetFSM net		- instance of NetFSM, needed for queuing arrived messages in FSM system.
//  
// Return value: none
// Description: 
//  Initialization of object Client.
Client_DTP::Client_DTP(SOCKET &cl, NetFSM *net):Client(cl, net){
  
};

// Function: Destructor
// Parameters: none
//  
// Return value: none
// Description:
Client_DTP::~Client_DTP(){
   nrOfClients--;
	//if(nrOfClients == 0)
		//DeleteCriticalSection(&CriticalSection);
	// dodato naknadno mislim da nije potrebno releaseAll();
	delete[] ptrBuffer;
	delete[] priv;
}


// Function: workToDo()
// Parameters: none
// Return value: none
// Description: 
//  Here is used to handling arrived messages.
void Client_DTP::workToDo()
{

	int oldMessage = 0;//false
	int receivedBytes = 0;
	int receivedBytesMess = 0;
   char* BUFF;
   BUFF = new char[2048];
   int counter = 0;
   while(1){
	   receivedBytes = recv(client,ptrBuffer,2048,0);//poslednji parametar je nasumicno postavljen ne treba mi ni jedna od ponudjenih vrednosti
    
	   if(receivedBytes == 0)
	   {			
       netFSM->NetError();
		   releaseAll();
	   }
	   else if(receivedBytes > 0)
	   {
           memcpy(BUFF, ptrBuffer, receivedBytes);
             
		   netFSM->receivedMessageLength =  receivedBytes;
		   memcpy(netFSM->protocolMessageR,BUFF, receivedBytes);
		   
		   netFSM->protocolMessageR[receivedBytes] = 0;
           netFSM->currentMessageCode = netFSM->convertNetToFSMMessage();

		   //netFSM->workWhenReceive(netFSM->getProtocolInfoCoding());
		  
	   }

   }
}