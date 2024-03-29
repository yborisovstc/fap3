#include <stdlib.h>
#include <unit.h>
#include <syst.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test of MUnit interface main operations
 */


class Ut_unit : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_unit);
    CPPUNIT_TEST(test_nav_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_nav_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_unit );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_unit, "Ut_unit");


void Ut_unit::setUp()
{
}

void Ut_unit::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of navigation
 * */
void Ut_unit::test_nav_1()
{
    cout << endl << "=== Test of MUnit base operations ===" << endl;

    Unit* owner = new Unit(Unit::Type(), "Owner", nullptr);
    Unit* comp1 = new Unit(Unit::Type(), "Comp1", nullptr);
    Unit* comp2 = new Unit(Unit::Type(), "Comp2", nullptr);
    Unit* comp2_1 = new Unit(Unit::Type(), "Comp2_1", nullptr);
    owner->owner()->connect(comp1->owned());
    owner->owner()->connect(comp2->owned());
    comp2->owner()->connect(comp2_1->owned());

    GUri uri1("Comp2.Comp2_1");
    MNode* res1 = owner->getNode(uri1);
    CPPUNIT_ASSERT_MESSAGE("Failed getting Comp2_1", res1 == comp2_1);

    delete owner;
}
