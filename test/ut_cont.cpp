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
    MUnit* unit1 = new Unit("Owner", nullptr);
    string tcont1 = "test1";
    // Default content
    bool res = unit1->setContent(tcont1);
    CPPUNIT_ASSERT_MESSAGE("Failed setting content", res);
    string tcontr;
    res = unit1->getContent(tcontr);
    CPPUNIT_ASSERT_MESSAGE("Failed setting content", tcontr == tcont1);
    // Adding content

    
    delete unit1;
}
