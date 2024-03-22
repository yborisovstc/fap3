#include <stdlib.h>
#include <unit.h>
#include <syst.h>

#include "env.h"
#include "prov.h"
#include "mlink.h"

#include <cppunit/extensions/HelperMacros.h>



/** @brief Test interface 1 */
class MTIf1: public MIface{
    public:
	static const char* Type() { return "MTIf1";}
	// From MIface
	virtual string Uid() const override { return MTIf1_Uid();}
	virtual string MTIf1_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MTIf1_getLif(aType);}
	virtual MIface* MTIf1_getLif(const char *aType) = 0;
};


/** @brief Test interface 2 */
class MTIf2: public MIface{
    public:
	static const char* Type() { return "MTIf2";}
	// From MIface
	virtual string Uid() const override { return MTIf2_Uid();}
	virtual string MTIf2_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MTIf2_getLif(aType);}
	virtual MIface* MTIf2_getLif(const char *aType) = 0;
};



/**
 * Test agent providing the ifaces
 */
class TstAgt: public Unit, public MAgent
{
    public:
	class TestIface1: public MTIf1 {
	    public:
		TestIface1(TstAgt& aHost): mHost(aHost) {}
		virtual ~TestIface1() {}
		virtual string MTIf1_Uid() const override { return mHost.getUid<MTIf1>();}
		virtual MIface* MTIf1_getLif(const char *aType) override {return this;}
	    private:
		TstAgt& mHost;
	};

	class TestIface2: public MTIf2 {
	    public:
		TestIface2(TstAgt& aHost): mHost(aHost) {}
		virtual ~TestIface2() {}
		virtual string MTIf2_Uid() const override { return mHost.getUid<MTIf2>();}
		virtual MIface* MTIf2_getLif(const char *aType) override {return this;}
	    private:
		TstAgt& mHost;
	};

    public:
	static const char* Type() { return "TstAgt";};
	TstAgt(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): Unit(aType, aName, aEnv) { }
	virtual ~TstAgt() {}
	virtual string MAgent_Uid() const { return getUid<MAgent>();}
	MIface* MNode_getLif(const char *aType) override {
	    MIface* res = nullptr;
	    if (res = checkLif<MAgent>(aType));
	    else if (res = checkLif<MNode>(aType));
	    else res = Unit::MNode_getLif(aType);
	    return res;
	}
	MIface* MAgent_getLif(const char *aType) override {
	    MIface* res = nullptr;
	    if (res = checkLif<MAgent>(aType));
	    else res = checkLif<MUnit>(aType);
	    return res;
	}
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override {
	    if (aName == MTIf1::Type()) {
		IfrLeaf* lf = new IfrLeaf(this, &mIface1);
		aReq->connect(lf);
	    } else if (aName == MTIf2::Type()) {
		IfrLeaf* lf = new IfrLeaf(this, &mIface2);
		aReq->connect(lf);
	    }
	}
    private:
	TestIface1 mIface1 = TestIface1(*this);
	TestIface2 mIface2 = TestIface2(*this);
};

/**
 * Agent provider for test agents
 */
class TstProv: public ProvBase
{
    public:
	TstProv(const string& aName, MEnv* aEnv): ProvBase(aName, aEnv) {}
	virtual ~TstProv() {}
	// From ProvBase
	virtual const TFReg& FReg() const override {return mReg;}
	virtual const TDtFReg& FDtReg() const override {return mDtReg;}
	virtual void setChromoRslArgs(const string& aRargs) override {}
	virtual void getChromoRslArgs(string& aRargs) override {}
    private:
	static const TFReg mReg;
	static const TDtFReg mDtReg;
};


const ProvBase::TFReg TstProv::mReg ({ Item<TstAgt>() });
const ProvBase::TDtFReg TstProv::mDtReg;




/** @brief This test of vertex/system layers
*/
class Ut_syst : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_syst);
    //    CPPUNIT_TEST(test_vert_1);
    //    CPPUNIT_TEST(test_cp_1);
    //CPPUNIT_TEST(test_syst_1);
    //CPPUNIT_TEST(test_syst_link);
    //CPPUNIT_TEST(test_cp_2);
    //CPPUNIT_TEST(test_syst_cp_3);
    //CPPUNIT_TEST(test_syst_sock_1);
    CPPUNIT_TEST(test_syst_sock_2);
    CPPUNIT_TEST(test_syst_sock_3);
    CPPUNIT_TEST(test_syst_sock_4);
    CPPUNIT_TEST(test_syst_cpe_1);
    //CPPUNIT_TEST(test_syst_dn); // Not completed
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_vert_1();
    void test_cp_1();
    void test_syst_1();
    void test_syst_link();
    void test_cp_2();
    void test_syst_cp_3();
    void test_syst_sock_1();
    void test_syst_sock_2();
    void test_syst_sock_3();
    void test_syst_sock_4();
    void test_syst_cpe_1();
    void test_syst_dn();
    private:
    MNode* constructSystem(const string& aFname);
    private:
    Env* mEnv;
    TstProv* mProv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_syst );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_syst, "Ut_syst");

MNode* Ut_syst::constructSystem(const string& aSpecn)
{
    string ext = "chs";
    string spec = aSpecn + string(".") + "chs";
    string log = aSpecn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv != 0);
    mEnv->ImpsMgr()->ResetImportsPaths();
    mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mProv = new TstProv("TestProv", mEnv);
    bool res = mEnv->addProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to add provider", res);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root && eroot);
    return root;
}

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
    MUnit* vu1 = new Vertu(Vertu::Type(), "V1", nullptr);
    MUnit* vu2 = new Vertu(Vertu::Type(), "V2", nullptr);
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
    MNode* cpu1 = new ConnPointu(ConnPointu::Type(), "Cp1", nullptr);
    MNode* cpu2 = new ConnPointu(ConnPointu::Type(), "Cp2", nullptr);
    MVert* cpv1 = cpu1->lIf(cpv1);
    MVert* cpv2 = cpu2->lIf(cpv2);
    cpu1->cntOw()->setContent("Provided", "Iface1");
    cpu1->cntOw()->setContent("Required", "Iface2");
    cpu2->cntOw()->setContent("Provided", "Iface2");
    cpu2->cntOw()->setContent("Required", "Iface1");
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

/** @brief Test of connecting link
 * */
void Ut_syst::test_syst_link()
{
    cout << endl << "=== Test of connecting link ===" << endl;

    const string specn("ut_syst_link");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    // Verify linked node
    MNode* l1n = root->getNode("S1.L1");
    MLink* l1l = l1n ? l1n->lIf(l1l) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1", l1l);
    MNode* l1_pairn = l1l->pair();
    CPPUNIT_ASSERT_MESSAGE("Fail to get L1 pair", l1_pairn);

    // Unlink
    MNode* s1n = root->getNode("S1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get S1", s1n);
    MChromo* chr = mEnv->provider()->createChromo();
    chr->Init(ENt_Node);
    chr->Root().AddChild(TMut(ENt_Disconn, ENa_P, "L1", ENa_Q, "N1"));
    s1n->mutate(chr->Root(), false, MutCtx(), true);
    delete chr;
    l1_pairn = l1l->pair();
    CPPUNIT_ASSERT_MESSAGE("Fail to disconnect L1 pair", !l1_pairn);
 


    delete mEnv;
}



/** @brief Test of creating simple system containing connected vert
 * */
void Ut_syst::test_syst_1()
{
    cout << endl << "=== Test of creating simple system ===" << endl;

    const string specn("ut_syst_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    GUri ruri;
    root->getUri(ruri);
    string ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);

    // Verify S1 chromo parent name
    MNode* s1n = root->getNode("S1");
    MElem* s1e = s1n ? s1n->lIf(s1e) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1e", s1e);
    const string s1pname = s1e->Chromos().Root().Attr(ENa_Parent);
    CPPUNIT_ASSERT_MESSAGE("Wrong S1 parents name", s1pname == "Syst");

    // Check connection
    MNode* v1n = root->getNode("S1.V1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1n", v1n);
    MVert* v1v = v1n->lIf(v1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1v", v1v);
    MNode* v2n = root->getNode("S1.V2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v2n", v2n);
    MVert* v2v = v2n->lIf(v2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get v2v", v2v);
    bool res = v1v->isPair(v2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of v1v", res);

    // Disconnect V1 V2
    MChromo* chr = mEnv->provider()->createChromo(); chr->Init(ENt_Node);
    chr->Root().AddChild(TMut(ENt_Disconn, ENa_P, "V1", ENa_Q, "V2"));
    s1n->mutate(chr->Root(), false, MutCtx(), true);
    delete chr;

    // Check V3, V4 disconnection
    MNode* v3n = root->getNode("S1.V3");
    MVert* v3v = v3n ? v3n->lIf(v3v) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get v3v", v3v);
    MNode* v4n = root->getNode("S1.V4");
    MVert* v4v = v4n ? v4n->lIf(v4v) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get v4v", v4v);
    res = v3v->isPair(v4v);
    CPPUNIT_ASSERT_MESSAGE("V3 isn't disconnected from V4", !res);

    delete mEnv;
}

/** @brief Test of connecting connpoints
 * */
void Ut_syst::test_cp_2()
{
    cout << endl << "=== Test of connecting connpoints ===" << endl;

    const string specn("ut_cp_2");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    GUri ruri;
    root->getUri(ruri);
    string ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check connection
    MNode* cp1n = root->getNode("S1.Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1n", cp1n);
    MVert* cp1v = cp1n->lIf(cp1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1v", cp1v);
    MNode* cp2n = root->getNode("S1.Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp2n", cp2n);
    MVert* cp2v = cp2n->lIf(cp2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp2v", cp2v);
    bool res = cp1v->isPair(cp2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of cp1v", res);
}


/** @brief Test of resolving interface via connecting connpoints
 * */
void Ut_syst::test_syst_cp_3()
{
    cout << endl << "=== Test of resolving interface via connecting connpoints ===" << endl;

    const string specn("ut_syst_cp_3");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mProv = new TstProv("TestProv", mEnv);
    bool res = mEnv->addProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to add provider", res);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    GUri ruri;
    root->getUri(ruri);
    string ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check connection
    MNode* cp1n = root->getNode("SS.S1.Cp1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1n", cp1n);
    MVert* cp1v = cp1n->lIf(cp1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp1v", cp1v);
    MNode* cp2n = root->getNode("SS.Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp2n", cp2n);
    MVert* cp2v = cp2n->lIf(cp2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp2v", cp2v);
    res = cp1v->isPair(cp2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get pair of cp1v", res);
    // Check MAgent resolution by Syst
    MNode* s1n = root->getNode("SS.S1");
    MUnit* s1u = s1n ? s1n->lIf(s1u) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1u", s1u);
    MIfProv* ifp = s1u->defaultIfProv("MAgent");
    MIfProv* maprov = ifp->first();
    CPPUNIT_ASSERT_MESSAGE("Failed getting MAgent provider", maprov);
    cout << endl << "=== Magents resolved by S1:" << endl;
    while (maprov) {
	cout << maprov->iface()->Uid() << endl;
	maprov = maprov->next();
    }
    cout << endl;
    cout << endl << "=== S1 MAgent default IFP dump:" << endl;
    ifp->dump(0);
    // Check iface resolution via connpoint
    MUnit* cp2u = cp2n->lIf(cp2u);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp2u", cp2u);
    ifp = cp2u->defaultIfProv("MTIf1");
    MIfProv* prov = ifp->first();
    cout << endl << "=== CP2 MTIf1 default IFP dump:" << endl;
    ifp->dump(0);
    CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint provider", prov);
}


/** @brief Test of resolving interface via extending chain
 * */
void Ut_syst::test_syst_cpe_1()
{
    cout << endl << "=== Test of resolving interface via extending chain ===" << endl;

    const string specn("ut_syst_cpe_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mProv = new TstProv("TestProv", mEnv);
    bool res = mEnv->addProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to add provider", res);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    GUri ruri;
    root->getUri(ruri);
    string ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Check MAgent resolution by Syst
    MNode* s1n = root->getNode("SS.S1");
    MUnit* s1u = s1n ? s1n->lIf(s1u) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1u", s1u);
    MIfProv* ifp = s1u->defaultIfProv("MAgent");
    MIfProv* maprov = ifp->first();
    CPPUNIT_ASSERT_MESSAGE("Failed getting MAgent provider", maprov);
    cout << endl << "=== Magents resolved by S1:" << endl;
    while (maprov) {
	cout << maprov->iface()->Uid() << endl;
	maprov = maprov->next();
    }
    cout << endl;
    cout << endl << "=== S1 MAgent default IFP dump:" << endl;
    ifp->dump(0);
    // Check iface resolution via connpoint
    MNode* cp2n = root->getNode("SS.Cp2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp2n", cp2n);
    MUnit* cp2u = cp2n->lIf(cp2u);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cp2u", cp2u);
    ifp = cp2u->defaultIfProv("MTIf1");
    MIfProv* prov = ifp->first();
    cout << endl << "=== CP2 MTIf1 default IFP dump:" << endl;
    ifp->dump(0);
    CPPUNIT_ASSERT_MESSAGE("Failed getting MConnPoint provider", prov);

    delete mEnv;
}



/** @brief Test 1 of socket: compatible sockets
 * */
void Ut_syst::test_syst_sock_1()
{
    cout << endl << "=== Test of socket: compatible sockets ===" << endl;

    const string specn("ut_syst_sock_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mProv = new TstProv("TestProv", mEnv);
    bool res = mEnv->addProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to add provider", res);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    GUri ruri;
    root->getUri(ruri);
    string ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);

    // Verify if the sockets are connected
    MNode* s1n = root->getNode("S1.Sock1");
    MVert* s1v = s1n->lIf(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1v", s1v);
    MNode* s2n = root->getNode("S1.Sock2");
    MVert* s2v = s2n->lIf(s2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s2v", s2v);
    bool conn = s2v->isPair(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail: sockets are not connected", conn);
    // Verify IFR
    MNode* s1p1n = root->getNode("S1.Sock1.Pin1");
    MUnit* s1p1u = s1p1n->lIf(s1p1u);
    MTIf1* ifc1 = s1p1u->getSif(ifc1);
    CPPUNIT_ASSERT_MESSAGE("Fail to get ifc1", ifc1);
    // Verify IFR for iface that doesn't provided by pins
    MUnit* s1u = s1n->lIf(s1u);
    MSyst* s1s = s1u ? s1u->getSif(s1s) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Resolved iface not-provided", !s1s);

    delete mEnv;
}

/** @brief Test 2 of socket: incompatible sockets
 * */
void Ut_syst::test_syst_sock_2()
{
    cout << endl << "=== Test of socket: incompatible sockets ===" << endl;

    const string specn("ut_syst_sock_2");
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mProv = new TstProv("TestProv", mEnv);
    bool res = mEnv->addProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to add provider", res);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    // Verify if the sockets are not connected
    MNode* s1n = root->getNode("S1.Sock1");
    MVert* s1v = s1n->lIf(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1v", s1v);
    MNode* s2n = root->getNode("S1.Sock2");
    MVert* s2v = s2n->lIf(s2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s2v", s2v);
    bool conn = s2v->isPair(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail: Sock1 and Sock2 are connected", !conn);

    delete mEnv;
}



/** @brief Test 3 of socket: connecting thru internal socket
 * */
void Ut_syst::test_syst_sock_3()
{
    cout << endl << endl << "=== Test 3: connecting thru internal sockets ===" << endl;

    const string specn("ut_syst_sock_3");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mProv = new TstProv("TestProv", mEnv);
    bool res = mEnv->addProvider(mProv);
    CPPUNIT_ASSERT_MESSAGE("Fail to add provider", res);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    // Verify if the sockets are connected
    MNode* s1n = root->getNode("S1.Sock1");
    MVert* s1v = s1n->lIf(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1v", s1v);
    MNode* s2n = root->getNode("S1.Sock2");
    MVert* s2v = s2n->lIf(s2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s2v", s2v);
    bool conn = s2v->isPair(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail: sockets are not connected", conn);

    // Verify IFR for MTIf1
    MNode* s1p1n = root->getNode("S1.Sock1.PinS1.Pin1");
    MUnit* s1p1u = s1p1n ? s1p1n->lIf(s1p1u) : nullptr;
    MTIf1* ifc1 = s1p1u ? s1p1u->getSif(ifc1) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ifc1", ifc1);

    // Verify IFR for MTIf2
    MNode* s12cpn = root->getNode("S1.S1_2.Cp");
    MUnit* s12cpu = s12cpn ? s12cpn->lIf(s12cpu) : nullptr;
    MTIf2* ifc2 = s12cpu ? s12cpu->getSif(ifc2) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ifc2", ifc2);


    // Verify IFR from Sock2.SockB to S1_2.TstAgt
    MNode* s2ps1p1n = root->getNode("S1.Sock2.PinS1.Pin1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get S1.Sock2.PinS1.Pin1", s2ps1p1n);
    MUnit* s2ps1p1u = s2ps1p1n ? s2ps1p1n->lIf(s2ps1p1u) : nullptr;
    MTIf2* s2ps1p1_ifc2 = s2ps1p1u ? s2ps1p1u ->getSif(s2ps1p1_ifc2) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get MTIf2 from S1.Sock2.PinS1.Pin1", s2ps1p1_ifc2);

    delete mEnv;
}


/** @brief Test 4 of socket: complex loopback
 * */
void Ut_syst::test_syst_sock_4()
{
    cout << endl << endl << "=== Socket test 4: complex loopback ===" << endl;
    MNode* root = constructSystem("ut_syst_sock_4");

    // Verify if the sockets are connected
    MNode* s1n = root->getNode("S1.Sock1");
    MVert* s1v = s1n->lIf(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s1v", s1v);
    MNode* s2n = root->getNode("S1.Sock2");
    MVert* s2v = s2n->lIf(s2v);
    CPPUNIT_ASSERT_MESSAGE("Fail to get s2v", s2v);
    bool conn = s2v->isPair(s1v);
    CPPUNIT_ASSERT_MESSAGE("Fail: sockets are not connected", conn);

    // Verify IFR for MTIf1
    MNode* s1p1n = root->getNode("S1.Sock1.PinS1.Pin1");
    MUnit* s1p1u = s1p1n ? s1p1n->lIf(s1p1u) : nullptr;
    MTIf1* ifc1 = s1p1u ? s1p1u->getSif(ifc1) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ifc1", ifc1);

    // Verify IFR for MTIf2
    MNode* s12cpn = root->getNode("S1.S1_2.Cp");
    MUnit* s12cpu = s12cpn ? s12cpn->lIf(s12cpu) : nullptr;
    MTIf2* ifc2 = s12cpu ? s12cpu->getSif(ifc2) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ifc2", ifc2);

    delete mEnv;
}

/** @brief Test of chromo runtime dependency node, ref ds_cli_pi_dn
 * */
void Ut_syst::test_syst_dn()
{
    cout << endl << endl << "=== Test of chromo runtime dependency node ===" << endl;
    string spec = "ut_syst_dn";
    MNode* root = constructSystem("ut_syst_dn");
    CPPUNIT_ASSERT_MESSAGE("Fail to construct system", root);
    MElem* eroot = root->lIf(eroot);
    CPPUNIT_ASSERT_MESSAGE("Fail to get eroot", eroot);
    eroot->Chromos().Save(spec + "_saved.chs");
    cout << "system native hier dump:" << endl;
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);

    // Q-dep on mutation_create
    MNode* v1n = root->getNode("S1.V1");
    MVert* v1v = v1n ? v1n->lIf(v1v) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get v1v", v1v);
    MNode* v2n = root->getNode("S1.Vert_211");
    CPPUNIT_ASSERT_MESSAGE("Fail to get v2n", v2n);
    MVert* v2v = v2n ? v2n->lIf(v2v) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get v2v", v2v);
    CPPUNIT_ASSERT_MESSAGE("v1v isn't connected to v2v", v2v->isPair(v1v));

    // Q-dep on segment_target
    MNode* v3n = root->getNode("S1.V3");
    MVert* v3v = v3n ? v3n->lIf(v3v) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get v3v", v2v);
    CPPUNIT_ASSERT_MESSAGE("v1v isn't connected to v3v", v3v->isPair(v1v));
    MElem* v3e = v3n->lIf(v3e);
    CPPUNIT_ASSERT_MESSAGE("Failed to get v3e", v3e);
    cout << "V3 chromo:" << endl;
    v3e->Chromos().Root().Dump();
    // Q-dep on segment_target
    MNode* vs1n = root->getNode("S1.VS1");
    MVert* vs1v = vs1n ? vs1n->lIf(vs1v) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get vs1v", vs1v);
    CPPUNIT_ASSERT_MESSAGE("v1v isn't connected to vs1v", vs1v->isPair(v1v));

    // Q-dep on segment_namespace
    MNode* v4n = root->getNode("S1.V4");
    MVert* v4v = v4n ? v4n->lIf(v4v) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get v4v", v1v);
    CPPUNIT_ASSERT_MESSAGE("v4v isn't connected to v3v", v3v->isPair(v4v));

    delete mEnv;
}

