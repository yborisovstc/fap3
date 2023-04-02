#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"
#include "rmutdata.h"
#include "rdatauri.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief This test of vertex/system layers
*/
class Ut_des : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_des);
    //CPPUNIT_TEST(test_des_data);
    CPPUNIT_TEST(test_des_1);
    CPPUNIT_TEST(test_des_ades_1);
    CPPUNIT_TEST(test_des_dmc_1);
    CPPUNIT_TEST(test_des_ifr_inval_1);
    CPPUNIT_TEST(test_des_tr_1);
    CPPUNIT_TEST(test_des_tr_tres_1);
    CPPUNIT_TEST(test_des_asr_1);
    CPPUNIT_TEST(test_des_asr_2);
    CPPUNIT_TEST(test_des_utl_1);
    CPPUNIT_TEST(test_des_utl_2);
    /*
    CPPUNIT_TEST(test_des_sp_1);
    */
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    MNode* constructSystem(const string& aFname);
    /** @brief Helper. Get state data string 
     * */
    string getStateDstr(const string& aUri);
    private:
    void test_des_data();
    void test_des_1();
    void test_des_ades_1();
    void test_des_dmc_1();
    void test_des_ifr_inval_1();
    void test_des_tr_1();
    void test_des_tr_tres_1();
    void test_des_asr_1();
    void test_des_asr_2();
    void test_des_utl_1();
    void test_des_utl_2();
    void test_des_sp_1();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_des );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_des, "Ut_des");

string Ut_des::getStateDstr(const string& aUri)
{
    MNode* st = mEnv->Root()->getNode(aUri);
    MDVarGet* stg = st ? st->lIf(stg) : nullptr;
    const DtBase* data = stg ? stg->VDtGet(string()) : nullptr;
    return data ? data->ToString(true) : string();
}

MNode* Ut_des::constructSystem(const string& aSpecn)
{
    string ext = "chs";
    string spec = aSpecn + string(".") + "chs";
    string log = aSpecn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv != 0);
    mEnv->ImpsMgr()->ResetImportsPaths();
    mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root && eroot);
    return root;
}



void Ut_des::setUp()
{
}

void Ut_des::tearDown()
{
    //    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/** @brief Test of creating of simple DES system with ADES
 * */
void Ut_des::test_des_ades_1()
{
    cout << endl << "=== Test of creating of simple system managed by ADES ===" << endl;
    const string specn = "ut_des_ades_1";
    MNode* root = constructSystem(specn);
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    // Dump root
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved.chs");
    // Run 
    bool res = mEnv->RunSystem(4);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);

    // Verify the state
    CPPUNIT_ASSERT_MESSAGE("St1 failed", getStateDstr("Launcher.Ds1.St1") == "SI 4");

    delete mEnv;
}

/** @brief Test of DES transitions
 * */
void Ut_des::test_des_tr_1()
{
    cout << endl << "=== Test of DES transisions ===" << endl;

#if 0
    DChr2 sc;
    sc.FromString("CHR2 { Text = \\\"Button 3\\\";  BgColor < { R = \\\"0.0\\\"; G = \\\"0.0\\\"; B = \\\"1.0\\\"; } }");
    cout << sc.ToString() << endl;

    Sdata<string> ss;
    istringstream iss("SS 'H \\'ello\\' dude!'");
    iss >> ss;
    ostringstream oss;
    oss << ss << endl;
    cout << oss.str();
#endif

    MNode* root = constructSystem("ut_des_tr_1");
    MElem* eroot = root ? root->lIf(eroot) : nullptr;

#if 0
    // Verifiy chromo2 data
    cout << "Verifiy chromo2 data" << endl;
    MNode* chrdn = root->getNode("Launcher.Ds1.ChrD");
    CPPUNIT_ASSERT_MESSAGE("Fail to get ChrD", chrdn);
    DChr2 data;
    GetGData(chrdn, data);
    Chromo2& cdc = data.mData;
    cout << "chromo2 data root dump:" << endl;
    cdc.Root().Dump();
#endif

    // Run
    bool res = mEnv->RunSystem(3, 3);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    // Verify
    CPPUNIT_ASSERT_MESSAGE("St1 failed", getStateDstr("Launcher.Ds1.St1") == "SB false");
    CPPUNIT_ASSERT_MESSAGE("StSS failed", getStateDstr("Launcher.Ds1.StSS") == "SS 'Hello World!'");
    CPPUNIT_ASSERT_MESSAGE("StPS failed", getStateDstr("Launcher.Ds1.StPS") == "PS (First,Second)");
    CPPUNIT_ASSERT_MESSAGE("StPSS failed", getStateDstr("Launcher.Ds1.StPSS") == "PSS (SS 'First elem',SS 'Second elem')");
    CPPUNIT_ASSERT_MESSAGE("StPU failed", getStateDstr("Launcher.Ds1.StPU") == "PDU (URI first1.first2,URI second1.second2)");
    CPPUNIT_ASSERT_MESSAGE("StVS failed", getStateDstr("Launcher.Ds1.StVS") == "VS (elem1,elem2,elem3)");
    CPPUNIT_ASSERT_MESSAGE("StVDU failed", getStateDstr("Launcher.Ds1.StVDU") == "VDU (URI e1_f1.e1_f2,URI e2_f1.e2_f2)");
    CPPUNIT_ASSERT_MESSAGE("StVPU failed", getStateDstr("Launcher.Ds1.StVPU") == "VPDU (PDU (URI e1f1.e1f2,URI e1s1.e1s2),PDU (URI e2f1.e2f2,URI e2s1.e2s2))");
    CPPUNIT_ASSERT_MESSAGE("StVPS failed", getStateDstr("Launcher.Ds1.StVPS") == "VPS (PS (e1_first,e1_second),PS (e2_first,e2_second))");
    CPPUNIT_ASSERT_MESSAGE("StUri failed", getStateDstr("Launcher.Ds1.StUri") == "URI State1.State2");
    CPPUNIT_ASSERT_MESSAGE("SSizeOfVect failed", getStateDstr("Launcher.Ds1.SSizeOfVect") == "SI 3");
    CPPUNIT_ASSERT_MESSAGE("SSwitch failed", getStateDstr("Launcher.Ds1.SSwitch") == "SI 2");
    CPPUNIT_ASSERT_MESSAGE("SSwitchUsInv failed", getStateDstr("Launcher.Ds1.SSwitchUsInv") == "SI 2");
    CPPUNIT_ASSERT_MESSAGE("SAddInt failed", getStateDstr("Launcher.Ds1.SAddInt") == "SI 7");
    CPPUNIT_ASSERT_MESSAGE("SMplInt failed", getStateDstr("Launcher.Ds1.SMplInt") == "SI 10");
    CPPUNIT_ASSERT_MESSAGE("SDivInt failed", getStateDstr("Launcher.Ds1.SDivInt") == "SI 5");
    CPPUNIT_ASSERT_MESSAGE("SMinInt failed", getStateDstr("Launcher.Ds1.SMinInt") == "SI 1");
    CPPUNIT_ASSERT_MESSAGE("SMaxInt failed", getStateDstr("Launcher.Ds1.SMaxInt") == "SI 10");
    CPPUNIT_ASSERT_MESSAGE("SAnd failed", getStateDstr("Launcher.Ds1.SAnd") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("SOr failed", getStateDstr("Launcher.Ds1.SOr") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("SNeg failed", getStateDstr("Launcher.Ds1.SNeg") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("SUri failed", getStateDstr("Launcher.Ds1.SUri") == "URI elem1.elem2.elem3");
    CPPUNIT_ASSERT_MESSAGE("SMutConn failed", getStateDstr("Launcher.Ds1.SMutConn") == "MUT conn,p:Cp1,q:Cp2");
    CPPUNIT_ASSERT_MESSAGE("SMutDisconn failed", getStateDstr("Launcher.Ds1.SMutDisconn") == "MUT disconn,p:Cp1,q:Cp2");
    CPPUNIT_ASSERT_MESSAGE("SVectItm failed", getStateDstr("Launcher.Ds1.SVectItm") == "SS 'Item_3'");
    CPPUNIT_ASSERT_MESSAGE("SAtgVdu failed", getStateDstr("Launcher.Ds1.SAtgVdu") == "URI b1.b2");
    CPPUNIT_ASSERT_MESSAGE("TupleRes failed", getStateDstr("Launcher.Ds1.TupleRes") == "TPL,SS:name,SI:value 'Test_name' 24");
    CPPUNIT_ASSERT_MESSAGE("TupleRes2 failed", getStateDstr("Launcher.Ds1.TupleRes2") == "TPL,SI:data,SI:value 5 7");
    CPPUNIT_ASSERT_MESSAGE("TupleRes3 failed", getStateDstr("Launcher.Ds1.TupleRes3") == "TPL,SI:data,SI:value <ERR> 2");
    CPPUNIT_ASSERT_MESSAGE("SCmpLt failed", getStateDstr("Launcher.Ds1.SCmpLt") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("SCmpNeqInv failed", getStateDstr("Launcher.Ds1.SCmpNeqInv") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("TupleSelIfaceG failed", getStateDstr("Launcher.Ds1.TupleSelIfaceG") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("InpCntRes failed", getStateDstr("Launcher.Ds1.InpCntRes") == "SI 3");
    CPPUNIT_ASSERT_MESSAGE("InpSelRes failed", getStateDstr("Launcher.Ds1.InpSelRes") == "SS 'Inp3'");
    string sr = "CHR2 {\n    Text = \"Button 3\"\n    BgColor <  {\n        R = \"0.0\"\n        G = \"0.0\"\n        B = \"1.0\"\n    }\n}";
    CPPUNIT_ASSERT_MESSAGE("SChrD failed", getStateDstr("Launcher.Ds1.SChrD") == sr);
    sr = "CHR2 {\n    Test_Node : Node\n    Test_Node2 : Test_Node\n}";
    CPPUNIT_ASSERT_MESSAGE("ChromoRes failed", getStateDstr("Launcher.Ds1.ChromoRes") == sr);
    sr = "CHR2 {\n    {\n        Node : TestNode\n    }\n    {\n        Node2 : TestNode\n    }\n}";
    CPPUNIT_ASSERT_MESSAGE("StrApndRes failed", getStateDstr("Launcher.Ds1.StrApndRes") == "SS 'Part1_Part2'");
    CPPUNIT_ASSERT_MESSAGE("UriApndRes failed", getStateDstr("Launcher.Ds1.UriApndRes") == "URI Node1.Node2.Node3");
    CPPUNIT_ASSERT_MESSAGE("IntToStr failed", getStateDstr("Launcher.Ds1.IntToStr") == "SS '34'");
    CPPUNIT_ASSERT_MESSAGE("SUriToStr failed", getStateDstr("Launcher.Ds1.SUriToStr") == "SS 'bla1.bla2.bla3'");
    CPPUNIT_ASSERT_MESSAGE("SUriToStr2 failed", getStateDstr("Launcher.Ds1.SUriToStr2") == "SS 'part1.part2'");
    sr = "CHR2 {\n    SModelUri < = \"test.elem1\"\n}";
    CPPUNIT_ASSERT_MESSAGE("SMutCont2 failed", getStateDstr("Launcher.Ds1.SMutCont2") == sr);
    CPPUNIT_ASSERT_MESSAGE("SIsValid1 failed", getStateDstr("Launcher.Ds1.SIsValid1") == "SB false");
    CPPUNIT_ASSERT_MESSAGE("SIsValid2 failed", getStateDstr("Launcher.Ds1.SIsValid2") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("SSelValid1 failed", getStateDstr("Launcher.Ds1.SSelValid1") == "URI Hello");
    CPPUNIT_ASSERT_MESSAGE("SSelValid2 failed", getStateDstr("Launcher.Ds1.SSelValid2") == "URI Hello");
    CPPUNIT_ASSERT_MESSAGE("SSelValidS1 failed", getStateDstr("Launcher.Ds1.SSelValidS1") == "SS 'Hello'");
    CPPUNIT_ASSERT_MESSAGE("SSelValidS2 failed", getStateDstr("Launcher.Ds1.SSelValidS2") == "SS 'Hello'");
    CPPUNIT_ASSERT_MESSAGE("SUriTail failed", getStateDstr("Launcher.Ds1.SUriTail") == "URI elem2.elem3");
    CPPUNIT_ASSERT_MESSAGE("SUriHead failed", getStateDstr("Launcher.Ds1.SUriHead") == "URI elem1.elem2");
    CPPUNIT_ASSERT_MESSAGE("SUriTailn failed", getStateDstr("Launcher.Ds1.SUriTailn") == "URI elem3");
    CPPUNIT_ASSERT_MESSAGE("SHashStr failed", getStateDstr("Launcher.Ds1.SHashStr") == "SI 877266171");
    CPPUNIT_ASSERT_MESSAGE("SPair1 failed", getStateDstr("Launcher.Ds1.SPair1") == "PSI (SI 11,SI 22)");
    CPPUNIT_ASSERT_MESSAGE("SPair2 failed", getStateDstr("Launcher.Ds1.SPair2") == "SI 11");
    CPPUNIT_ASSERT_MESSAGE("SPair2 failed", getStateDstr("Launcher.Ds1.SPair2") == "SI 11");

    delete mEnv;
}



/** @brief Test of creating of simple DES, DMC chromo
 * */
void Ut_des::test_des_dmc_1()
{
    cout << endl << "=== Test of creating of simple DES, DMC chromo ===" << endl;

    string specn("ut_des_dmc_1");
    string ext = "chs";
    MNode* root = constructSystem(specn);
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Verify DS1 chromo
    MNode* ds1n = root->getNode("Launcher.Ds1");
    MElem* ds1e = ds1n ? ds1n->lIf(ds1e) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get Ds1", ds1e);
    // Save ds1e chromo
    ds1e->Chromos().Save(specn + "ds1n_chromo_saved." + ext);
    // Save root chromo
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Run 
    bool res = mEnv->RunSystem(4);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);

    // Verify the state
    CPPUNIT_ASSERT_MESSAGE("St1 failed", getStateDstr("Launcher.Ds1.St1") == "SI 4");

    delete mEnv;


    // Test with saved chromo
    //
    cout << endl << "= Test with saved chromo =" << endl;
    root = constructSystem(specn);
    eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Run 
    res = mEnv->RunSystem(4);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);

    // Verify the state
    CPPUNIT_ASSERT_MESSAGE("St1 failed", getStateDstr("Launcher.Ds1.St1") == "SI 4");

    delete mEnv;
}



/** @brief Test of ifr invalidation on DES reconfiguration
 * */
void Ut_des::test_des_ifr_inval_1()
{
    cout << endl << "=== Test of ifr invalidation on DES reconfiguration ===" << endl;

    const string specn("ut_des_if_inval_1");
    string ext = "chs";
    MNode* root = constructSystem(specn);
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);

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
    CPPUNIT_ASSERT_MESSAGE("Fail to get ds1", ds1);
    cout << endl << "Connecting Add2.Inp ~ St1" << endl;
    mEnv->Logger()->Write(EInfo, nullptr, "=== Connecting Add2.Inp ~ St1 ===");
    ds1->mutate(chr->Root(), false, MutCtx(), true);
    delete chr;

    res = mEnv->RunSystem(2);
    // Verify the state
    CPPUNIT_ASSERT_MESSAGE("St1 failed", getStateDstr("Launcher.Ds1.St2") == "SI 20");

    delete mEnv;
}

/** @brief Test of ifr activation on DES reconfiguration
 * */
void Ut_des::test_des_asr_1()
{
    cout << endl << "=== Test of activation on DES reconf: CP connection ===" << endl;

    const string specn("ut_des_asr_1");
    string ext = "chs";
    MNode* root = constructSystem(specn);
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
    CPPUNIT_ASSERT_MESSAGE("St1 failed", getStateDstr("Launcher.Ds1.St1") == "SI 2");

    delete mEnv;
}

/** @brief Test of ifr activation on DES reconfiguration
 * */
void Ut_des::test_des_asr_2()
{
    cout << endl << "=== Test of activation on DES reconf: sock connection ===" << endl;

    const string specn("ut_des_asr_2");
    string ext = "chs";
    MNode* root = constructSystem(specn);
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
    CPPUNIT_ASSERT_MESSAGE("St1 failed", getStateDstr("Launcher.Ds1.St1") == "SI 2");

    delete mEnv;
}

/** @brief Test of transitions type resolution #1
 * */
void Ut_des::test_des_tr_tres_1()
{
    cout << endl << "=== Test of transitions type resolution  ===" << endl;

    MNode* root = constructSystem("ut_des_tr_tres_1");
    MNode* launcher = root->getNode("Launcher");
    CPPUNIT_ASSERT_MESSAGE("Failed getting launcher", launcher);

    // Run
    bool res = mEnv->RunSystem(15, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", res);

    // Verify
    CPPUNIT_ASSERT_MESSAGE("S1 failed", getStateDstr("Launcher.S1") == "SB false");

    delete mEnv;
}



/** @brief Test of ifr activation on DES reconfiguration
 * */
void Ut_des::test_des_utl_1()
{
    cout << endl << "=== Test of DES utilities  ===" << endl;

    const string specn("ut_des_utl_1");
    string ext = "chs";
    MNode* root = constructSystem(specn);
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    // Run
    bool res = mEnv->RunSystem(1, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);

    // Verifying #1
    CPPUNIT_ASSERT_MESSAGE("BChange_Dbg failed", getStateDstr("Launcher.BChange_Dbg") == "SB true");
    CPPUNIT_ASSERT_MESSAGE("SDPulse failed", getStateDstr("Launcher.SDPulse") == "URI Hello.World");

    res = mEnv->RunSystem(1, 2);
    // Verifying #2
    CPPUNIT_ASSERT_MESSAGE("SDPulse failed", getStateDstr("Launcher.SDPulse") == "URI <ERR>");
    CPPUNIT_ASSERT_MESSAGE("VectIter.SIdx failed", getStateDstr("Launcher.VectIter.SIdx") == "SI 2");

    delete mEnv;
}

void Ut_des::test_des_utl_2()
{
    cout << endl << "=== Test of DES utilities: reset  ===" << endl;

    MNode* root = constructSystem("ut_des_utl_2");
    MNode* launcher = root->getNode("Launcher");
    CPPUNIT_ASSERT_MESSAGE("Failed getting launcher", launcher);

    // Run
    bool res = mEnv->RunSystem(15, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", res);

    // Verifying
    CPPUNIT_ASSERT_MESSAGE("Max failed", getStateDstr("Launcher.Max") == "SI 10");

    delete mEnv;
}

void Ut_des::test_des_1()
{
    cout << endl << "=== Test of simple DES ===" << endl;

    MNode* root = constructSystem("ut_des_1");
    MNode* launcher = root->getNode("Launcher");
    CPPUNIT_ASSERT_MESSAGE("Failed getting launcher", launcher);

    // Run
    // Verifying iface cache update, ref ds_desopt_uic 
    bool res = mEnv->RunSystem(5, 2);
    CPPUNIT_ASSERT_MESSAGE("Ds1.St1 failed on phase 1", getStateDstr("Launcher.Ds1.St1") == "SI 5");
    mEnv->profiler()->saveMetrics();


    MNode* c1n = launcher->getNode("Ds1.Const_1");
    MVert* c1v = c1n->lIf(c1v);
    MNode* ainpn = launcher->getNode("Ds1.Add.Inp");
    MVert* ainpv = ainpn->lIf(ainpv);
    c1v->disconnect(ainpv);
    ainpv->disconnect(c1v);
    res = mEnv->RunSystem(5, 2);

    CPPUNIT_ASSERT_MESSAGE("Ds1.St1 failed on phase 2", getStateDstr("Launcher.Ds1.St1") == "SI 5");
    c1v->connect(ainpv);
    ainpv->connect(c1v);

    res = mEnv->RunSystem(5, 2);
    CPPUNIT_ASSERT_MESSAGE("Ds1.St1 failed on phase 3", getStateDstr("Launcher.Ds1.St1") == "SI 10");

    // Set data to St1
    MNode* ds1n = root->getNode("Launcher.Ds1.St1");
    MDVarSet* ds1s = ds1n ? ds1n->lIf(ds1s) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Failed to get Ds1.St1 MDVarSet", ds1s);
    Sdata<int> data;
    data.FromString("SI 20");
    ds1s->VDtSet(data);
    res = mEnv->RunSystem(2, 2);
    CPPUNIT_ASSERT_MESSAGE("Ds1.St1 failed on phase 3", getStateDstr("Launcher.Ds1.St1") == "SI 22");


    // Benchmarking
    //bool res = mEnv->RunSystem(10000, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", res);

    delete mEnv;
}

/** @brief Test of DES data
 * */
void Ut_des::test_des_data()
{
    cout << endl << "=== Test of DES data  ===" << endl;

    DGuri du1;
    du1.FromString("URI  ");

    /*
    const string specn("ut_des_utl_1");
    string ext = "chs";
    MNode* root = constructSystem(specn);
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    // Run
    bool res = mEnv->RunSystem(1, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", eroot);
    */

//    delete mEnv;
}

void Ut_des::test_des_sp_1()
{
    cout << endl << "=== Test of DES Service point ===" << endl;

    MNode* root = constructSystem("ut_des_sp_1");
    MNode* launcher = root->getNode("Launcher");
    CPPUNIT_ASSERT_MESSAGE("Failed getting launcher", launcher);

    // Run
    bool res = mEnv->RunSystem(15, 2);
    CPPUNIT_ASSERT_MESSAGE("Failed running system", res);

    // Verifying

    delete mEnv;
}


