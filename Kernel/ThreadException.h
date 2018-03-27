// **************************************************************************
//                        NetException.h  -  description
//                           -------------------
//  begin                : Mon Nov 5 2001
//  coded by             : Aleksander Stojivcevic
//  e-mail               : sasa@krt.neobee.net
//  Description:
//  This file contains code for ThreadException class used for throwing exceptions
//  when something in work with threads is wrong.
// **************************************************************************
#ifndef __THREAD_EXCEPTION__
#define __THREAD_EXCEPTION__


#include <stdexcept>
#include <stdio.h>

using namespace std;

class ThreadException : public runtime_error
{
	private:
		char text[100];

	public:
		//errorText is desired description for throwing exception.
		ThreadException(char* errorText) : runtime_error(errorText) {sprintf(text,"%s",errorText);}
		char* getText(){ return text;}
};

#endif