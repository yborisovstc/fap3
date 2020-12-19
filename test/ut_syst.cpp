#include <stdlib.h>
#include <unit.h>
#include <syst.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test of vertex/system layers
 */


class Ut_syst : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_syst);
    CPPUNIT_TEST(test_vert_1);
    CPPUNIT_TEST(test_cp_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_vert_1();
    void test_cp_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_syst );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_syst, "Ut_syst");


void Ut_syst::setUp()
{
}

void Ut_syst::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of vertex base functionality
 * */
void Ut_syst::test_vert_1()
{
    cout << endl << "=== Test of vertex base functionality ===" << endl;
    MUnit* vu1 = new Vertu("V1", nullptr);
    MUnit* vu2 = new Vertu("V2", nullptr);
    MVert* v1 = vu1->lIf(v1);
    MVert* v2 = vu2->lIf(v2);
    bool res = MVert::connect(v1, v2);
    CPPUNIT_ASSERT_MESSAGE("Failed connecing v1 and v2", res);
    res = v1->isPair(v2);
    CPPUNIT_ASSERT_MESSAGE("Failed checking v1 pair", res);
    res = MVert::disconnect(v1, v2);
    CPPUNIT_ASSERT_MESSAGE("Failed disconnecing v1 and v2", res);
    res = v1->isPair(v2);
    CPPUNIT_ASSERT_MESSAGE("Failed checking v1 not pair", !res);

    
    delete vu1;
    delete vu2;
}

/** @brief Test of cp base functionality
 * */
void Ut_syst::test_cp_1()
{
    cout << endl << "=== Test of connpoint base functionality ===" << endl;
    MUnit* cpu1 = new ConnPointu("Cp1", nullptr);
    MUnit* cpu2 = new ConnPointu("Cp2", nullptr);
    MVert* cpv1 = cpu1->lIf(cpv1);
    MVert* cpv2 = cpu2->lIf(cpv2);
    cpu1->setContent("{ Provided:'Iface1' Required:'Iface2'}");
    cpu2->setContent("{ Provided:'Iface2' Required:'Iface1'}");
    MConnPoint* cp1 = cpu1->lIf(cp1);
    string cp1Prov = cp1->provName();
    bool res = cp1Prov == "Iface1";
    CPPUNIT_ASSERT_MESSAGE("Failed checking cp1 Provided", res);
    string cp1Req = cp1->reqName();
    res = cp1Req == "Iface2";
    CPPUNIT_ASSERT_MESSAGE("Failed checking cp1 Required", res);
    res = MVert::connect(cpv1, cpv2);
    CPPUNIT_ASSERT_MESSAGE("Failed connecing cpv1 and cpv2", res);
    res = cpv1->isPair(cpv2);
    CPPUNIT_ASSERT_MESSAGE("Failed checking cpv1 pair", res);
    res = MVert::disconnect(cpv1, cpv2);
    CPPUNIT_ASSERT_MESSAGE("Failed disconnecing cpv1 and cpv2", res);
    res = cpv1->isPair(cpv2);
    CPPUNIT_ASSERT_MESSAGE("Failed checking cpv1 not pair", !res);

    
    delete cpu1;
    delete cpu2;
}
