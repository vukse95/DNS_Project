#ifndef MainTestH
#define MainTestH

#include <cppunit/extensions/HelperMacros.h>

#include <stdio.h>
#include <windows.h>
#include <conio.h>

#include "const.h"
#include <fsmsystem.h>
#include "../kernel/logfile.h"

#include "ClAuto.h"
#include "UserAuto.h"

class MainTests : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(MainTests);
	printf("\n[I]--> POP3 Client User Test\n");
	CPPUNIT_TEST(UserTest);
    CPPUNIT_TEST_SUITE_END();
  
private:
    FSMSystem *pSys;
    UserAuto * pMF;
    ClAuto * POP3ClientFSM;
    LogFile *lf;
    
public:
    void setUp();
    void tearDown();
    
protected:
	void UserTest();
};

#endif