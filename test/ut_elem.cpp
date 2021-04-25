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
    CPPUNIT_TEST(test_elem_inh_1);
    //CPPUNIT_TEST(test_elem_imp_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_elem_inh_1();
    void test_elem_imp_1();
private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_elem );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_elem, "Ut_elem");


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
    // Save heir E2 chromo
    MNode* e2 = root->getNode("E2");
    MElem* e2e = e2 ? e2->lIf(eroot) : nullptr;
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
 

