#include <stdlib.h>
#include <unit.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test of native net 
 */


class Ut_cont : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_cont);
    CPPUNIT_TEST(test_cont_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_cont_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_cont );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_cont, "Ut_cont");


void Ut_cont::setUp()
{
}

void Ut_cont::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of owner-owned native conn
 * */
void Ut_cont::test_cont_1()
{
    printf("\n === Test of def content set\n");
    Unit* unit1 = new Unit("Owner", nullptr);
    string tcont1 = "test1";
    // Default content
    bool res = unit1->setContent(tcont1);
    CPPUNIT_ASSERT_MESSAGE("Failed setting content", res);
    string tcontr;
    res = unit1->getContent(tcontr);
    CPPUNIT_ASSERT_MESSAGE("Failed setting content", tcontr == tcont1);
    // Adding content
    cout << endl << "Adding [Debug] content node" << endl;
    res = unit1->addContent("Debug");
    unit1->dumpContent();
    CPPUNIT_ASSERT_MESSAGE("Adding content [Debug] failed", res);
    cout << endl << "Setting [Color] content leaf" << endl;
    res = unit1->setContent("Blue", "Color");
    CPPUNIT_ASSERT_MESSAGE("Adding content leaf [Color] failed", res);
    unit1->dumpContent();
    cout << endl << "Adding Debug.Update" << endl;
    res = unit1->addContent("Debug.Update");
    CPPUNIT_ASSERT_MESSAGE("Adding content [Debug.Update] failed", res);
    unit1->dumpContent();
    cout << endl << "Setting Debug.Trace.Enabled content leaf" << endl;
    res = unit1->setContent("Yes", "Debug.Trace.Enabled");
    CPPUNIT_ASSERT_MESSAGE("Setting content [Debug.Trace.Enabled] failed", res);
    unit1->dumpContent();
    cout << endl << "Setting complex content #1" << endl;
    res = unit1->setContent("{ Prov:'IfaceProv' Req:'IfaceReq'}", "ConnPoint");
    CPPUNIT_ASSERT_MESSAGE("Setting complex content #1 failed", res);
    unit1->dumpContent();

    
    delete unit1;
}
