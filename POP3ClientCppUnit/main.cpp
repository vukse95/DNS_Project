
#include <stdio.h>
#include <conio.h>

#include "const.h"
#include "fsmsystem.h"
#include "logfile.h"
#include "ChAuto.h"
#include "ClAuto.h"
#include "UserAuto.h"

extern bool g_ProgramEnd;

#define AUTOMAT_COUNT 3
#define MSGBOX_COUNT 3


/* FSM system instance. */
static FSMSystem sys(AUTOMAT_COUNT, MSGBOX_COUNT);

DWORD WINAPI SystemThread(void *data) {
	ChAuto Channel;
	ClAuto Client;
	UserAuto User;

	/* Kernel buffer description block */
	/* number of buffer types */
	const uint8 buffClassNo =  4; 
	/* number of buffers of each buffer type */
	uint32 buffsCount[buffClassNo] = { 50, 50, 5000, 10 }; 
	/* buffer size for each buffer type */
 	uint32 buffsLength[buffClassNo] = { 128, 256, 512, 1024}; 
	
	/* Logging setting - to a file in this case */
	LogFile lf("log.log" /*log file name*/, "./log.ini" /* message translator file */);
	LogAutomateNew::SetLogInterface(&lf);

	/* Mandatory kernel initialization */
	printf("[*] Initializing system...\n");
	sys.InitKernel(buffClassNo, buffsCount, buffsLength, 3, Timer1s);

	/* Add automates to the system */
	sys.Add(&Channel, CH_AUTOMATE_TYPE_ID, 1, true);
	sys.Add(&Client, CL_AUTOMATE_TYPE_ID, 1, true);
	sys.Add(&User, USER_AUTOMATE_TYPE_ID, 1, true);

	/* Start the first automate - usually it sends the first message, 
	since only automates can send messages */
	User.Start();

	/* Starts the system - dispatches system messages */
	printf("[*] Starting system...\n");
	sys.Start();

	/* Finish thread */
	return 0;
}

void mainProgram(int argc, char* argv[]) {
	
	DWORD thread_id;
	HANDLE thread_handle;

	/* Start operating thread. */
	thread_handle = CreateThread(NULL, 0, SystemThread, NULL, 0, &thread_id);

	/* Wait for end. */
	while(!g_ProgramEnd){}
	getch();
	

	/* Notify the system to stop - this causes the thread to finish */
	printf("[*] Stopping system...\n");
	sys.StopSystem();

	/* Free the thread handle */
	CloseHandle(thread_handle);


}