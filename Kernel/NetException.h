// **************************************************************************
//                        NetException.h  -  description
//                           -------------------
//  begin                : Mon Nov 5 2001
//  coded by             : Aleksander Stojivcevic
//  e-mail               : sasa@krt.neobee.net
//  Description:
//  This file contains code for NetException class used for throwing exceptions
//  when something in work with network is wrong.
// **************************************************************************
#ifndef __NET_EXCEPTION__
#define __NET_EXCEPTION__

#include <stdexcept>
#include <stdio.h>

using namespace std;

class NetException : public runtime_error
{
	private:
		char text[100];
		

	public:
		//errorText is desired description for throwing exception.
		NetException(char* errorText) : runtime_error(errorText) {sprintf(text,"%s",errorText);}
		char* getText(){ return text;}
};

#endif