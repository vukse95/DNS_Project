#include "UdpLogInterface.h"
#include "errorObject.h"

UdpLogInterface::UdpLogInterface()
{
	initUdp("192.168.0.89", RECEIVE_PORT);
}

UdpLogInterface::UdpLogInterface(const int8 *ip, uint16 port)
{
	initUdp(ip, port);
}

UdpLogInterface::~UdpLogInterface()
{
	deInitUdp();
#ifdef WIN32
	WSACleanup();
#endif
}

void UdpLogInterface::Write()
{
	uint16 len = GetNewMsgInfoLength();
	LogBuffer[GetNewMsgInfoLength()/*+ 1*/] = 0;
    SetMsgInfoLength((uint16)(GetNewMsgInfoLength() +1));
	//LogBuffer[2] = (uint16)ParamOffset;
	sendto(sendingSocket, 
		   (char*)LogBuffer, 
	 	   len+1, 
		   MSG_DONTROUTE, 
		   (struct sockaddr *)&to, 
		   sizeof(to));
	memset(LogBuffer,0,1024);
	//LogBuffer[0] = 0;
	ParamOffset = 0;
}

void UdpLogInterface::initUdp(const int8* ip, uint16 port)
{
#ifdef WIN32
	WSADATA WsaData;
	WORD wVersionRequested = MAKEWORD(2,1);
	if (WSAStartup (wVersionRequested, &WsaData) == SOCKET_ERROR)  
		throw TErrorObject( __LINE__, __FILE__, 0x01010200);	

	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons(0);
	sendingSocket = socket(AF_INET, SOCK_DGRAM,0); 
	
	if (sendingSocket == INVALID_SOCKET)
	{
		//deInitUdp();
		throw TErrorObject( __LINE__, __FILE__, 0x01010200);			
		return;
	}
	
	BOOL bReuse = true;
	setsockopt (sendingSocket,SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuse, sizeof(BOOL));
  	
	if (bind(sendingSocket, (struct sockaddr*)&local,sizeof(local)) == SOCKET_ERROR) 
	{
		//WSACleanup();
		int error = WSAGetLastError();
		throw TErrorObject( __LINE__, __FILE__, 0x01010200);			
	}
	
	to.sin_family = AF_INET;
	to.sin_port = htons(port);
	to.sin_addr.s_addr = inet_addr((const char*)ip);		
#endif WIN32
}

void UdpLogInterface::deInitUdp(void)
{
	closesocket(sendingSocket);
}