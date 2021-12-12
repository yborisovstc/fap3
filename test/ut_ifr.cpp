#include <stdlib.h>
#include <unit.h>
#include <syst.h>
#include "env.h"
#include "mdata.h"

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test of interface resolution mechanism
 */


class Ut_ifr : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_ifr);
    CPPUNIT_TEST(test_base_1);
    CPPUNIT_TEST(test_inval_sock_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_base_1();
    void test_inval_sock_1();
private:
    Env* mEnv;
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
    MNode* cpu1 = new ConnPointu(ConnPointu::Type(), "Cp1", nullptr);
    MNode* cpu2 = new ConnPointu(ConnPointu::Type(), "Cp2", nullptr);
    MNode* cpu3 = new ConnPointu(ConnPointu::Type(), "Cp3", nullptr);
    MNode* cpu4 = new ConnPointu(ConnPointu::Type(), "Cp3", nullptr);
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


/** @brief Test of IFR invalidation in sockets
 * */
void Ut_ifr::test_inval_sock_1()
{
    cout << endl << endl << "=== Test of IFR invalidation in sockets ===" << endl;

    const string specn("ut_ifr_inval_1");
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    // Verify if the sockets are connected
    MNode* sock1n = root->getNode("S1.Sock1");
    MVert* sock1v = sock1n->lIf(sock1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get sock1v", sock1v);
    MNode* sock2n = root->getNode("S1.Sock2");
    MVert* sock2v = sock2n->lIf(sock2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get sock2v", sock2v);
    bool conn = sock2v->isPair(sock1v);
    CPPUNIT_ASSERT_MESSAGE("Fail: sockets are not connected", conn);

    // Verify IFR from Sock2.Pin1
    MNode* s2p1n = root->getNode("S1.Sock2.Pin1");
    MUnit* s2p1u = s2p1n ? s2p1n->lIf(s2p1u) : nullptr;
    MDVarGet* ifc1 = s2p1u ? s2p1u->getSif(ifc1) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ifc1", ifc1);

    // Disconnect sockets
    MNode* s1n = root->getNode("S1");
    MChromo* chr = mEnv->provider()->createChromo();
    chr->Init(ENt_Node);
    chr->Root().AddChild(TMut(ENt_Disconn, ENa_P, "Sock1", ENa_Q, "Sock2"));
    s1n->mutate(chr->Root(), false, MutCtx(), true);
    delete chr;
    // Verify the sockets are disconnected
    conn = sock2v->isPair(sock1v);
    CPPUNIT_ASSERT_MESSAGE("Fail: sockets are still connected after disconnect", !conn);
    // Verify IFR from Sock2.Pin1
    MDVarGet* ifc2 = s2p1u ? s2p1u->getSif(ifc2) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("ifc2 is still resolvable", !ifc2);
    // Connect sockets again
    chr = mEnv->provider()->createChromo();
    chr->Init(ENt_Node);
    chr->Root().AddChild(TMut(ENt_Conn, ENa_P, "Sock1", ENa_Q, "Sock2"));
    s1n->mutate(chr->Root(), false, MutCtx(), true);
    delete chr;
    // Verify IFR from Sock2.Pin1
    ifc2 = s2p1u ? s2p1u->getSif(ifc2) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ifc2", ifc2);

    delete mEnv;
}
