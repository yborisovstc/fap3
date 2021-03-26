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
    MNode* cpu1 = new ConnPointu("Cp1", nullptr);
    MNode* cpu2 = new ConnPointu("Cp2", nullptr);
    MNode* cpu3 = new ConnPointu("Cp3", nullptr);
    MNode* cpu4 = new ConnPointu("Cp3", nullptr);
    MVert* cpv1 = cpu1->lIf(cpv1);
    MVert* cpv2 = cpu2->lIf(cpv2);
    MVert* cpv3 = cpu3->lIf(cpv3);
    MVert* cpv4 = cpu4->lIf(cpv4);
    cpu1->cntOw()->setContent("Provided", "Iface1");
    cpu1->cntOw()->setContent("Required", "MConnPoint");
    cpu2->cntOw()->setContent("Provided", "MConnPoint");
    cpu2->cntOw()->setContent("Required", "Iface1");
    cpu3->cntOw()->setContent("Provided", "MConnPoint");
    cpu3->cntOw()->setContent("Required", "Iface1");
    cpu4->cntOw()->setContent("Provided", "Iface1");
    cpu4->cntOw()->setContent("Required", "MConnPoint");
    MConnPoint* cp1 = cpu1->lIf(cp1);
    bool res = MVert::connect(cpv1, cpv2);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting cp1 - cp2", res);
    res = MVert::connect(cpv1, cpv3);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting cp1 - cp3", res);
    res = MVert::connect(cpv4, cpv3);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting cp4 - cp3", res);
    MUnit* cpu1u = cpu1->lIf(cpu1u);
    MIfProv* ifp = cpu1u->defaultIfProv("MConnPoint");
    MIfProv* prov = ifp->first();
    ifp->dump(0);
    CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint provider", prov);
    cout << endl << "== Iface resolved ==" << endl;
    do {
	MIface* iface = prov->iface();
	CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint iface", iface);
	cout << ">>  " << (iface ? iface->Uid() : "null") << endl;
	prov = prov->next();
    } while (prov);
    cout << endl;
    // Checking IRM update after disconnect
    cout << endl << "== cp1 disconnected from cp3 ==" << endl;
    res = MVert::disconnect(cpv1, cpv3);
    CPPUNIT_ASSERT_MESSAGE("Failed disconnecting cp1 - cp3", res);
    prov = ifp->first();
    ifp->dump(0);
    CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint provider", prov);
    cout << endl << "== Iface resolved ==" << endl;
    do {
	MIface* iface = prov->iface();
	CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint iface", iface);
	cout << ">>  " << (iface ? iface->Uid() : "null") << endl;
	prov = prov->next();
    } while (prov);
    cout << endl;
    // Checking IRM update after connect - disconnect - connect
    cout << endl << "== cp1, cp3 connected-disconnected-connected ==" << endl;
    res = MVert::connect(cpv1, cpv3);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting cp1 - cp3", res);
    res = MVert::disconnect(cpv1, cpv3);
    CPPUNIT_ASSERT_MESSAGE("Failed disconnecting cp1 - cp3", res);
    res = MVert::connect(cpv1, cpv3);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting cp1 - cp3", res);
    prov = ifp->first();
    ifp->dump(0);
    CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint provider", prov);
    cout << endl << "== Iface resolved ==" << endl;
    do {
	MIface* iface = prov->iface();
	CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint iface", iface);
	cout << ">>  " << (iface ? iface->Uid() : "null") << endl;
	prov = prov->next();
    } while (prov);
    cout << endl;



    delete cpu1;
    delete cpu2;
    delete cpu3;
    delete cpu4;
}
