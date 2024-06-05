#include <stdlib.h>
#include <unit.h>
#include <syst.h>
#include <content.h>

#include <cppunit/extensions/HelperMacros.h>

#include "env.h"
#include "melem.h"
#include "mchromo.h"

/*
 * This test of MNode interface main operations
 */


class Ut_elem : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_elem);
    //CPPUNIT_TEST(test_elem_inh_1);
    //CPPUNIT_TEST(test_elem_imp_1);
    //CPPUNIT_TEST(test_elem_dmc_1);
    CPPUNIT_TEST(test_elem_mutperf_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    MNode* constructSystem(const string& aFname);
private:
    void test_elem_inh_1();
    void test_elem_imp_1();
    void test_elem_dmc_1();
    void test_elem_mutperf_1();
private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_elem );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_elem, "Ut_elem");


MNode* Ut_elem::constructSystem(const string& aSpecn)
{
    string ext = "chs";
    string spec = aSpecn + string(".") + "chs";
    string log = aSpecn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv != 0);
    mEnv->ImpsMgr()->ResetImportsPaths();
    mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->ImpsMgr()->AddImportsPaths("../../fap3/modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root && eroot);
    return root;
}

void Ut_elem::setUp()
{
}

void Ut_elem::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of inheritance
 * */
void Ut_elem::test_elem_inh_1()
{
    cout << endl << "=== Test of inheritance ===" << endl;

    string specn("ut_elem_inh_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    GUri ruri;
    root->getUri(ruri);
    string ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromo
    eroot->Chromos().Save(specn + "_saved." + ext);
    // Checking E1 chromo forming, ref issue DS_ISS_001
    MNode* e1 = root->getNode("E1");
    MElem* e1e = e1 ? e1->lIf(e1e) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get E1", e1e);
    string cnt_e1_about;
    bool res = e1->cntOw()->getContent("About", cnt_e1_about);
    //!!CPPUNIT_ASSERT_MESSAGE("Fail to get E1 about", res && cnt_e1_about == "E1");
    // Checking that local mutation goes to owners chromo
    MChromo* chr = mEnv->provider()->createChromo();
    chr->SetFromSpec("E1_1_1 : Elem;");
    MNode* e1_n1_1 = e1->getNode("N1_1");
    e1_n1_1->mutate(chr->Root(), false, MutCtx(), false);
    delete chr;
    chr = mEnv->provider()->createChromo();
    chr->SetFromSpec("E4 : E1;");
    root->mutate(chr->Root(), false, MutCtx(), false);
    delete chr;
    MNode* e4_e1_1_1 = root->getNode("E4.N1_1.E1_1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get N1_1.E1_1_1", e4_e1_1_1);


    // Save heir E2 chromo
    MNode* e2 = root->getNode("E2");
    MElem* e2e = e2 ? e2->lIf(e2e) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get e2e", e2e);
    e2e->Chromos().Save(specn + "_e2_saved." + ext);
    // Verifying E2 parent name in E2 chromo
    const string e2pname = e2e->Chromos().Root().Attr(ENa_Parent);
    CPPUNIT_ASSERT_MESSAGE("Wrong E2 parents name", e2pname == "E1");
    // Verifying if E1 chromo was properly applied to E3
    MNode* e3_n111 = root->getNode("E3.N1_1.N1_1_1");
    CPPUNIT_ASSERT_MESSAGE("Failed getting  e3_n111", e3_n111);


    // Dump Elem parent
    MNode* en = root->getNode("Elem");
    MElem* ee = en ? en->lIf(ee) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ee", ee);
    cout << endl << "=== Elem dump as parent ===" << endl;
    ee->asParent()->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive);

    delete mEnv;

    // Create model from saved chromo
    cout << endl << "<< Creating model from saved chromo >>" << endl;

    specn = "ut_elem_inh_1_saved";
    ext = "chs";
    spec = specn + string(".") + "chs";
    log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    root = mEnv->Root();
    eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    ruri;
    root->getUri(ruri);
    ruris = ruri.toString();
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromo
    eroot->Chromos().Save(specn + "_saved." + ext);

    delete mEnv;
}
 

/** @brief Test of import of module
 * */
void Ut_elem::test_elem_imp_1()
{
    cout << endl << "=== Test of module import ===" << endl;

    const string specn("ut_elem_imp_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    mEnv->ImpsMgr()->AddImportsPaths("./modules");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
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
    // Check module comps inheritance
    MNode* me1n = root->getNode("E1.Me1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get me1n", me1n);
    MNode* e11n = root->getNode("E1.Me1.El1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get e11n", e11n);
    // Dump Elem parent
    /*
    MNode* en = root->getNode("Elem");
    MElem* ee = en ? en->lIf(ee) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get ee", ee);
    cout << endl << "=== Elem dump as parent ===" << endl;
    ee->asParent()->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive);
    */

    delete mEnv;
}
 
/** @brief Test of node mut propagation
 * */
void Ut_elem::test_elem_dmc_1()
{
    cout << endl << "=== Test of node mut propagation ===" << endl;

    const string specn("ut_elem_dmc_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    mEnv->ImpsMgr()->AddImportsPaths("./modules");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    // Mutate node
    MNode* n11n = root->getNode("E1.N1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get n11n", n11n);
    MChromo* chr = mEnv->provider()->createChromo();
    bool res1 = chr->SetFromSpec("NewNode : Node");
    MutCtx mc(nullptr);
    n11n->mutate(chr->Root(), true, mc);
    // Dump root
    root->dump(Ifu::EDM_Base | Ifu::EDM_Comps | Ifu::EDM_Recursive,0);
    // Save root chromo
    eroot->Chromos().Save(specn + "_saved." + ext);

    delete mEnv;
}
  
/** @brief Test of node mut propagation
 * */
void Ut_elem::test_elem_mutperf_1()
{
    cout << endl << "=== Test of elem mut performance ===" << endl;

    string ssname = "ut_elem_mutperf_1";
    MNode* root = constructSystem(ssname);
    CPPUNIT_ASSERT_MESSAGE("Fail to construct system", root);
    cout << "Construct duration: " << PROF_FIELD(mEnv->profiler(), PROF_DUR, PEvents::EDur_Construct, PIndFId::EInd_VAL) << endl;
    if (mEnv->profiler()) mEnv->profiler()->saveMetrics();

    // Profiler calibration
    PROF_DUR_START(mEnv->profiler(), PROF_DUR, PEvents::EDur_Tst1);
    for (int i = 0; i < 1000000; i++) {
	PFL_DUR_STAT_START(PEvents::EDurStat_Clbr);
	PFL_DUR_STAT_REC(PEvents::EDurStat_Clbr);
    }
    PROF_DUR_REC(mEnv->profiler(), PROF_DUR, PEvents::EDur_Tst1);
    cout << "Profiler calibration (1000000 durstate cycles): " << PROF_FIELD(mEnv->profiler(), PROF_DUR, PEvents::EDur_Tst1, PIndFId::EInd_VAL) << ", "
        << PROF_FIELD(mEnv->profiler(), PROF_DUR_STAT, PEvents::EDurStat_Clbr, PIndFId::EStat_SUM) << endl;

    delete mEnv;
}
 


