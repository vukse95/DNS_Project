// **************************************************************************
//                        thread.h  -  description
//                           -------------------
//  begin                : Mon Nov 5 2001
//  coded by             : Aleksander Stojivcevic
//  e-mail               : sasa@krt.neobee.net
//  Description:
//  This file contains code for thread class used for more controlled
//  work with threads.
// **************************************************************************
#include "thread.h"
#include "ThreadException.h"

#ifdef __LINUX__
#include "linuxThread.h"
#endif

// Function: Constructor
// Parameters: none.
// Return value: none
// Description: 
//  Initialization of object ThreadBase.
ThreadBase::ThreadBase(){
	end = 0;
}

// Function: Destructor
// Parameters: none
// Return value: none
// Description: 
//  Destruction of object ThreadBase.
ThreadBase::~ThreadBase(){
}


// Function: threadFunction
// Parameters: 
//  (in) LPVOID	lpParameter		- Parameters proceed to thred function.
// Return value: 
// Description: 
//  Defines thread function.
DWORD WINAPI threadFunction(LPVOID lpParameter){
	ThreadBase *priv;
	priv = (ThreadBase*)lpParameter;
	while(priv->end == 0)	{
		priv->workToDo();
	}
	return 0;
}

// Function: stop()
// Parameters: none.
// Return value: none.
// Description: 
//  Stops runned thread.
void ThreadBase::stop(){
	end = 1;
}

// Function: start()
// Parameters: none.
// Return value: none
// Description: 
//  Start thread.
void ThreadBase::start(){
	threadHandle = CreateThread(NULL,0,threadFunction,this,0,&threadId);
	
	if(threadHandle == NULL)
		throw ThreadException("CreateThread  failed - thread.cpp");
}

// Function: workToDo()
// Parameters: none
// Return value: none
// Description: 
//  Virtual function in which extended classes have implement thread function.
void ThreadBase::workToDo(){
}
