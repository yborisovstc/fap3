#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"
#include "rmutdata.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief This test of vertex/system layers
*/
class Ut_des : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_des);
//    CPPUNIT_TEST(test_des_1);
//    CPPUNIT_TEST(test_des_ades_1);
//    CPPUNIT_TEST(test_des_dmc_1);
//    CPPUNIT_TEST(test_des_ifr_inval_1);
    CPPUNIT_TEST(test_des_tr_1);
//    CPPUNIT_TEST(test_des_asr_1);
//    CPPUNIT_TEST(test_des_asr_2);
//    CPPUNIT_TEST(test_des_utl_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_des_1();
    void test_des_ades_1();
    void test_des_dmc_1();
    void test_des_ifr_inval_1();
    void test_des_tr_1();
    void test_des_asr_1();
    void test_des_asr_2();
    void test_des_utl_1();
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

    MNode* launcher = root->getNode("Launcher");
    MElem* lre = launcher->lIf(lre);
    MNode* ds1 = root->getNode("Launcher.Ds1");
    MElem* ds1e = ds1->lIf(ds1e);
    cout << endl << "= Ds1 chromo dump =" << endl;
    ds1e->Chromos().Root().Dump();

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


/** @brief Test of creating of simple DES system with ADES
 * */
void Ut_des::test_des_ades_1()
{
    cout << endl << "=== Test of creating of simple system managed by ADES ===" << endl;

    const string specn("ut_des_ades_1");
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

/** @brief Test of DES transitions
 * */
void Ut_des::test_des_tr_1()
{
    cout << endl << "=== Test of DES transisions ===" << endl;

    const string specn("ut_des_tr_1");
    string ext = "chs";
    string spec = specn + string(".") + ext;
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    // Verifiy chromo2 data
    cout << "Verifiy chromo2 data" << endl;
    MNode* chrdn = root->getNode("Launcher.Ds1.ChrD");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ChrD", chrdn);
    DChr2 data;
    GetGData(chrdn, data);
    Chromo2& cdc = data.mData;
    cout << "chromo2 data root dump:" << endl;
    cdc.Root().Dump();

    // Run 
    bool res = mEnv->RunSystem(3);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    // Verify the state
    MNode* stn = root->getNode("Launcher.Ds1.St1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn", stn);
    MDVarGet* vg = stn->lIf(vg);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn vg", vg);
    MDtGet<Sdata<bool>>* dgb = vg->GetDObj(dgb);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn dgb", dgb);
    Sdata<bool> val;
    dgb->DtGet(val);
    CPPUNIT_ASSERT_MESSAGE("Wrong final state valud", val.mData == false);
    // Verify getting item of vector
    MNode* st4 = root->getNode("Launcher.Ds1.St4");
    MDVarGet* st4vg = st4->lIf(st4vg);
    MDtGet<Sdata<string>>* st4dgs = st4vg->GetDObj(st4dgs);
    Sdata<string> st4val;
    st4dgs->DtGet(st4val);
    CPPUNIT_ASSERT_MESSAGE("Wrong value of St4", st4val.mData == "Item_3");
    
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
    // Verify DS1 chromo
    MNode* ds1n = root->getNode("Launcher.Ds1");
    MElem* ds1e = ds1n ? ds1n->lIf(ds1e) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get Ds1", ds1e);

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
    chr->SetFromSpec("e : Elem { Add2.Inp ~ St1; }");
    MNode* ds1 = root->getNode("Launcher.Ds1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get da1", ds1);
    cout << endl << "Connecting Add2.Inp ~ St1" << endl;
    mEnv->Logger()->Write(EInfo, nullptr, "=== Connecting Add2.Inp ~ St1 ===");
    ds1->mutate(chr->Root(), false, MutCtx(), true);
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

/** @brief Test of ifr activation on DES reconfiguration
 * */
void Ut_des::test_des_asr_1()
{
    cout << endl << "=== Test of activation on DES reconf: CP connection ===" << endl;

    const string specn("ut_des_asr_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    // Run 
    bool res = mEnv->RunSystem(2, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    MNode* ds1 = root->getNode("Launcher.Ds1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Ds1", ds1);
    MNode* stn = root->getNode("Launcher.Ds1.St1");

    // Disconnect Const_1 from Add.InpN and check the system activated again
    MChromo* chr = mEnv->provider()->createChromo();
    chr->Init(ENt_Node);
    chr->Root().AddChild(TMut(ENt_Disconn, ENa_P, "Add.InpN", ENa_Q, "Const_1"));
    cout << endl << "Disconnecting Add.InpN ~ Const_1" << endl;
    mEnv->Logger()->Write(EInfo, nullptr, "=== Disconnecting Add.InpN ~ Const_1 ===");
    ds1->mutate(chr->Root(), false, MutCtx(), true);
    delete chr;

    res = mEnv->RunSystem(2, 2);
    // Verify the state
    MDVarGet* vg = stn->lIf(vg);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn vg", vg);
    MDtGet<Sdata<int>>* dgi = vg->GetDObj(dgi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn dgi", dgi);
    Sdata<int> val;
    dgi->DtGet(val);
    CPPUNIT_ASSERT_MESSAGE("Wrong final state valud", val.mData == 2);

    delete mEnv;
}

/** @brief Test of ifr activation on DES reconfiguration
 * */
void Ut_des::test_des_asr_2()
{
    cout << endl << "=== Test of activation on DES reconf: sock connection ===" << endl;

    const string specn("ut_des_asr_2");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    // Run
    bool res = mEnv->RunSystem(2, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    MNode* ds1 = root->getNode("Launcher.Ds1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Ds1", ds1);
    MNode* stn = root->getNode("Launcher.Ds1.St1");

    // Disconnect Const_1 from Add.InpN and check the system activated again
    MChromo* chr = mEnv->provider()->createChromo();
    chr->Init(ENt_Node);
    chr->Root().AddChild(TMut(ENt_Disconn, ENa_P, "Sock1", ENa_Q, "Sock2"));
    cout << endl << "Disconnecting Sock1 ~ Sock2" << endl;
    mEnv->Logger()->Write(EInfo, nullptr, "=== Disconnecting Sock1 ~ Sock2 ===");
    ds1->mutate(chr->Root(), false, MutCtx(), true);
    delete chr;

    res = mEnv->RunSystem(2, 2);
    // Verify the state
    MDVarGet* vg = stn->lIf(vg);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn vg", vg);
    MDtGet<Sdata<int>>* dgi = vg->GetDObj(dgi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get stn dgi", dgi);
    Sdata<int> val;
    dgi->DtGet(val);
    CPPUNIT_ASSERT_MESSAGE("Wrong final state valud", val.mData == 2);

    delete mEnv;
}


/** @brief Test of ifr activation on DES reconfiguration
 * */
void Ut_des::test_des_utl_1()
{
    cout << endl << "=== Test of DES utilities  ===" << endl;

    const string specn("ut_des_utl_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    // Run
    bool res = mEnv->RunSystem(10, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    MNode* launcher = root->getNode("Launcher");
    CPPUNIT_ASSERT_MESSAGE("Failed getting launcher", launcher);
    MDesSyncable* ls = launcher->lIf(ls);
    CPPUNIT_ASSERT_MESSAGE("Failed getting launcher syncable", ls);

    delete mEnv;
}
