#include <stdlib.h>
#include <unit.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test of native net 
 */


class Ut_nn : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_nn);
    CPPUNIT_TEST(test_owning_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_owning_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_nn );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_nn, "Ut_nn");


void Ut_nn::setUp()
{
}

void Ut_nn::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of owner-owned native conn
 * */
void Ut_nn::test_owning_1()
{
    printf("\n === Test of native owner-owned conn\n");
    Unit* owner = new Unit("Owner", nullptr);
    Unit* owned1 = new Unit("Owned1", nullptr);
    Unit* owned2 = new Unit("Owned2", nullptr);
    owner->owner()->connect(owned1->owned());
    owner->owner()->disconnect(owned1->owned());
    owner->owner()->connect(owned1->owned());
    owner->owner()->connect(owned2->owned());
    delete owner;
}
