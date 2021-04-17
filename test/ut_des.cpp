#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief This test of vertex/system layers
*/
class Ut_des : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_des);
    //CPPUNIT_TEST(test_des_1);
    CPPUNIT_TEST(test_des_dmc_1);
    //CPPUNIT_TEST(test_des_ifr_inval_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_des_1();
    void test_des_dmc_1();
    void test_des_ifr_inval_1();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_des );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_des, "Ut_des");


void Ut_des::setUp()
{
}

void Ut_des::tearDown()
{
    //    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/** @brief Test of creating of simple DES
 * */
void Ut_des::test_des_1()
{
    cout << endl << "=== Test of creating of simple DES ===" << endl;

    const string specn("ut_des_1");
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
    // Run 
    bool res = mEnv->RunSystem(4);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    // Verify the state
    MNode* stn = root->getNode("Launcher.Ds1.St1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn", stn);
    MDVarGet* vg = stn->lIf(vg);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn vg", vg);
    MDtGet<Sdata<int>>* dgi = vg->GetDObj(dgi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn dgi", dgi);
    Sdata<int> val;
    dgi->DtGet(val);
    CPPUNIT_ASSERT_MESSAGE("Wrong final state valud", val.mData == 4);
    
    delete mEnv;
}

/** @brief Test of creating of simple DES, DMC chromo
 * */
void Ut_des::test_des_dmc_1()
{
    cout << endl << "=== Test of creating of simple DES, DMC chromo ===" << endl;

    string specn("ut_des_dmc_1");
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
    // Run 
    bool res = mEnv->RunSystem(4);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    // Verify the state
    MNode* stn = root->getNode("Launcher.Ds1.St1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn", stn);
    MDVarGet* vg = stn->lIf(vg);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn vg", vg);
    MDtGet<Sdata<int>>* dgi = vg->GetDObj(dgi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn dgi", dgi);
    Sdata<int> val;
    dgi->DtGet(val);
    CPPUNIT_ASSERT_MESSAGE("Wrong final state valud", val.mData == 4);

    delete mEnv;


    // Test with saved chromo
    //
    cout << endl << "= Test with saved chromo =" << endl;

    specn = "ut_des_dmc_1_saved";
    spec = specn + string(".") + "chs";
    log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    root = mEnv->Root();
    eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    ruri;
    root->getUri(ruri);
    ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Run 
    res = mEnv->RunSystem(4);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    // Verify the state
    stn = root->getNode("Launcher.Ds1.St1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn", stn);
    vg = stn->lIf(vg);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn vg", vg);
    dgi = vg->GetDObj(dgi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn dgi", dgi);
    dgi->DtGet(val);
    CPPUNIT_ASSERT_MESSAGE("Wrong final state valud", val.mData == 4);

    delete mEnv;


}



/** @brief Test of ifr invalidation on DES reconfiguration
 * */
void Ut_des::test_des_ifr_inval_1()
{
    cout << endl << "=== Test of ifr invalidation on DES reconfiguration ===" << endl;

    const string specn("ut_des_if_inval_1");
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
    // Run 
    bool res = mEnv->RunSystem(2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    // Connect St1 to Add2.Inp and check the connection works
    MChromo* chr = mEnv->provider()->createChromo();
    chr->SetFromSpec("{ Add2.Inp ~ St1; }");
    MNode* ds1 = root->getNode("Launcher.Ds1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get da1", ds1);
    cout << endl << "Connecting Add2.Inp ~ St1" << endl;
    mEnv->Logger()->Write(EInfo, nullptr, "=== Connecting Add2.Inp ~ St1 ===");
    ds1->mutate(chr->Root(), false, MutCtx());
    delete chr;
    res = mEnv->RunSystem(2);
    // Verify the state
    MNode* stn = root->getNode("Launcher.Ds1.St2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn", stn);
    MDVarGet* vg = stn->lIf(vg);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn vg", vg);
    MDtGet<Sdata<int>>* dgi = vg->GetDObj(dgi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn dgi", dgi);
    Sdata<int> val;
    dgi->DtGet(val);
    CPPUNIT_ASSERT_MESSAGE("Wrong final state valud", val.mData == 20);

    delete mEnv;
}



