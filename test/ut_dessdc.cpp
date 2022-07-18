#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"
#include "rdatauri.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief This test of vertex/system layers
*/
class Ut_sdc : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_sdc);

    //CPPUNIT_TEST(test_Sdc_1);
    //CPPUNIT_TEST(test_Sdc_2);
    //CPPUNIT_TEST(test_Sdc_3);
    //CPPUNIT_TEST(test_Sdc_4);
    CPPUNIT_TEST(test_Sdo_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    MNode* constructSystem(const string& aFname);
    private:
    void test_Sdc_1();
    void test_Sdc_2();
    void test_Sdc_3();
    void test_Sdc_4();
    void test_Sdo_1();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_sdc );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_sdc, "Ut_sdc");

MNode* Ut_sdc::constructSystem(const string& aSpecn)
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

void Ut_sdc::setUp()
{
}

void Ut_sdc::tearDown()
{
    //    delete mEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/** @brief MNode DES adapter test
 * */
void Ut_sdc::test_Sdc_1()
{
    printf("\n === Test of SDC base controllers\n");

    const string specn("ut_sdc_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv != 0);
    mEnv->ImpsMgr()->ResetImportsPaths();
    mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root && eroot);

    bool res = mEnv->RunSystem(7, 2);

    // Verify the comp created
    MNode* comp = root->getNode("Launcher.Comp");
    CPPUNIT_ASSERT_MESSAGE("Comp hasn't been created", comp);

    delete mEnv;
}

/** @brief MNode DES adapter test
 * */
void Ut_sdc::test_Sdc_2()
{
    printf("\n === Test of SDC base controllers - list insertion/extraction\n");

    const string specn("ut_sdc_2");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv != 0);
    mEnv->ImpsMgr()->ResetImportsPaths();
    mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root && eroot);

    bool res = mEnv->RunSystem(7, 2);

    // Verify the comp created
    MNode* comp = root->getNode("Launcher.Comp");
    CPPUNIT_ASSERT_MESSAGE("Comp hasn't been created", comp);

    delete mEnv;
}

/** @brief MNode SDC test - create and remove node in cycle
 * */
void Ut_sdc::test_Sdc_3()
{
    printf("\n === Test of SDC: create and remove in cycle\n");

    const string specn("ut_sdc_3");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv != 0);
    mEnv->ImpsMgr()->ResetImportsPaths();
    mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root && eroot);

    bool res = mEnv->RunSystem(7, 2);

    // Verify the comp created
    MNode* comp = root->getNode("Launcher.Comp");
    CPPUNIT_ASSERT_MESSAGE("Comp hasn't been created", comp);

    delete mEnv;
}

/** @brief SDC test - connect/disconnect
 * */
void Ut_sdc::test_Sdc_4()
{
    printf("\n === Test of SDC: connect-disconnect\n");
    MNode* root = constructSystem("ut_sdc_4");

    bool res = mEnv->RunSystem(12, 2);

    delete mEnv;
}

/** @brief MNode SDO test - existence of component
 * */
void Ut_sdc::test_Sdo_1()
{
    printf("\n === Test of SDOs: comps, connections, etc.\n");
    MNode* root = constructSystem("ut_sdo_1");

    bool res = mEnv->RunSystem(5, 2);

    // Verify V1 observing
    MNode* cdbg = root->getNode("Launcher.Dbg_DcoComp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.Dbg_DcoComp", cdbg);
    bool dres = false;
    GetSData(cdbg, dres);
    CPPUNIT_ASSERT_MESSAGE("Wrong DCO result", dres);
    // Verify V1~V2 observing
    MNode* conndbg = root->getNode("Launcher.Dbg_DcoConn");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.Dbg_DcoConn", conndbg);
    dres = false;
    GetSData(conndbg, dres);
    CPPUNIT_ASSERT_MESSAGE("Wrong DCO V1~V2 result", dres);
    // Verify V1~V3 observing
    conndbg = root->getNode("Launcher.Dbg_DcoConn2");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.Dbg_DcoConn2", conndbg);
    dres = true;
    GetSData(conndbg, dres);
    CPPUNIT_ASSERT_MESSAGE("Wrong DCO V1~V3 result", !dres);
    // Verify V1 pairs count
    conndbg = root->getNode("Launcher.Dbg_DcoPairsCount");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.Dbg_DcoPairsCount", conndbg);
    int dresi = -1;
    GetSData(conndbg, dresi);
    CPPUNIT_ASSERT_MESSAGE("Wrong DCO V1 pairs count result", dresi == 2);
    // Verify V2 pair
    conndbg = root->getNode("Launcher.Dbg_DcoPair");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.Dbg_DcoPair", conndbg);
    DGuri dresu;
    GetGData(conndbg, dresu);
    CPPUNIT_ASSERT_MESSAGE("Wrong DCO V1 pairs count result", dresu.mData == "V1");
    // Verify V4_1 owner
    conndbg = root->getNode("Launcher.Dbg_DcoCompOwner");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.Dbg_DcoCompOwner", conndbg);
    dresu;
    GetGData(conndbg, dresu);
    CPPUNIT_ASSERT_MESSAGE("Wrong DCO V1 pairs count result", dresu.mData == "V4");
    // Verify V4 comp V4_1
    conndbg = root->getNode("Launcher.Dbg_DcoCompComp");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.Dbg_DcoCompComp", conndbg);
    dresu;
    GetGData(conndbg, dresu);
    CPPUNIT_ASSERT_MESSAGE("Wrong DCO V1 pairs count result", dresu.mData == "V4.V4_1");


    delete mEnv;
}

