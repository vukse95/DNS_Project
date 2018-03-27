// **************************************************************************
//                        thread.h  -  description
//                           -------------------
//  begin                : Mon Nov 5 2001
//  coded by             : Aleksander Stojivcevic
//  e-mail               : sasa@krt.neobee.net
//  Description:
//  This file contains declaration for thread class used for more
//  controlled work with threads.
// **************************************************************************
#ifndef __THREAD_H__
#define __THREAD_H__

#ifdef WIN32
#include "windows.h"
#endif
#include "kernelTypes.h"
#include "stdio.h"
 

class ThreadBase{
	public:
		int end;
		HANDLE threadHandle;
		DWORD  threadId;
    DWORD  param;
		
	public:
		void start();
		void stop();
		virtual void workToDo();
		int IsEnd(){return end;}
		ThreadBase();
		~ThreadBase();
};

#endif