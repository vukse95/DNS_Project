
#include "../kernel/TransportInterface.h"

TransportInterface::TransportInterface( uint8 automateId1, uint8 mbxId1, uint16 numOfTimers, uint16 numOfState, uint16 maxNumOfProceduresPerState) : 
FiniteStateMachine( automateId1, mbxId1, numOfTimers, numOfState, maxNumOfProceduresPerState){
}


TransportInterface::~TransportInterface(){
}

void TransportInterface::setIP(char* ipnew){
	char* priv = ip;
	while((*(ipnew)) != '\0')
	{
		(*(ip)) = (*(ipnew));
		ip++;
		ipnew++;
	}
	(*(ip)) = '\0';
	ip = priv;
}

char *TransportInterface::getIP()
{
	return ip;
}

void TransportInterface::setPort(uint16 p){
	port = p;
}

