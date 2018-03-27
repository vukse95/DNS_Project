#include "fsm.h"
#include "MainTest.h"
#include "const.h"

CPPUNIT_TEST_SUITE_REGISTRATION( MainTests );
uint8 * msg;
uint16 msgcode;


void MainTests::setUp()
{
    pSys = new FSMSystem(4, 4);
    pMF  = new UserAuto();

    POP3ClientFSM = new ClAuto();

   
    uint8 buffClassNo = 4;
    uint32 buffsCount[8] = { 100, 50, 50, 50 };
    uint32 buffsLength[8] = { 1025, 1025, 1025, 1025};

    pSys->InitKernel(buffClassNo, buffsCount, buffsLength, 1);

    lf = new LogFile("log.log", "log.ini");
    LogAutomateNew::SetLogInterface(lf);	

   
    pSys->Add(POP3ClientFSM, CL_AUTOMATE_TYPE_ID, 1, true);
    pSys->Add(pMF, TEST_AUTOMATE_TYPE_ID, 1, true);
}

void MainTests::tearDown()
{
}

void MainTests::UserTest()
{
	/*
	pMF->FSM_User_Connecting_User_Connected();

	POP3ClientFSM->FSM_Cl_Connecting_Cl_Connectiong_Accept();

	msg = POP3ClientFSM->GetMsg(CL_AUTOMATE_MBX_ID);
	POP3ClientFSM->Process(msg);

	msgcode = POP3ClientFSM->GetState();
	CPPUNIT_ASSERT_EQUAL((uint16)FSM_Cl_User_Check, msgcode);
	*/
}
