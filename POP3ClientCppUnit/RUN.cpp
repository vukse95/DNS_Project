#include <conio.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>    
#include "const.h"
#include "main.h"

int main(int argc, char* argv[])
{
#ifndef D_TESTING
    mainProgram(argc, argv);

#else
    printf("**********************************************************************");
    printf("\n");
    printf("**                             T E S T                              **");
    printf("\n");
    printf("**********************************************************************");
    printf("\n\n\n");

    /* Create the event manager and test controller. */
    CPPUNIT_NS::TestResult controller;

    /* Add a listener that colllects test resul. */
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );        

    /* Add a listener that print dots as test run. */
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener( &progress );      

    /* Add the top suite to the test runner. */
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
    runner.run( controller );

    /* Print test in a compiler compatible format. */
    CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
    outputter.write(); 

    _getch();
    return result.wasSuccessful() ? 0 : 1;
#endif
}
