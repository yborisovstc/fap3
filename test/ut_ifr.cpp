#include <stdlib.h>
#include <unit.h>
#include <syst.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test of interface resolution mechanism
 */


class Ut_ifr : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_ifr);
    CPPUNIT_TEST(test_base_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_base_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_ifr );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_ifr, "Ut_ifr");


void Ut_ifr::setUp()
{
}

void Ut_ifr::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of base functionality
 * */
void Ut_ifr::test_base_1()
{
    cout << endl << "=== Test of base functionality ===" << endl;
    MUnit* cpu1 = new ConnPointu("Cp1", nullptr);
    MUnit* cpu2 = new ConnPointu("Cp2", nullptr);
    MVert* cpv1 = cpu1->lIf(cpv1);
    MVert* cpv2 = cpu2->lIf(cpv2);
    cpu1->setContent("{ Provided:'Iface1' Required:'Iface2'}");
    cpu2->setContent("{ Provided:'Iface2' Required:'Iface1'}");
    MConnPoint* cp1 = cpu1->lIf(cp1);
    bool res = MVert::connect(cpv1, cpv2);
    MIfProv* ifp = cpu1->defaultIfProv("MConnPoint");
    MIfProv* prov = ifp->first();
    CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint provider", prov);
    MIface* iface = prov->iface();
    CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint iface", iface);
 
    
    delete cpu1;
    delete cpu2;
}
