#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

/** @brief Main function for Unit Test application 
 *
 * Creates all registered test suits automaticaly.
 * Refer to CPPUnit documentation for details
 * To run particular test suite: ./ut-fap3-lib [test_suite_name]
 * test_suite_name is stated by named registration macro:
 * CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_uri, "Ut_uri");
 */
int main(int argc, char* argv[])
{
    CPPUNIT_NS::TestResult controller;
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener( &progress );
    CPPUNIT_NS::TestRunner runner;
    if (argc == 1) {
	runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
    } else {
	for (int cnt = 1; cnt < argc; cnt++) {
	    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry(argv[cnt]);
	    runner.addTest(registry.makeTest());
	}
    }
    runner.run( controller );
    CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
    outputter.write(); 
    return result.wasSuccessful() ? 0 : 1;
}

