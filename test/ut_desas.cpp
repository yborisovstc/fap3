#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"
#include "rdatauri.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief This test of DES active subsystem
*/
class Ut_desas : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_desas);
    //CPPUNIT_TEST(test_Desas_1);
    CPPUNIT_TEST(test_Desas_2);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    MNode* constructSystem(const string& aFname);
    private:
    void test_Desas_1();
    void test_Desas_2();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_desas );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_desas, "Ut_desas");

MNode* Ut_desas::constructSystem(const string& aSpecn)
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

void Ut_desas::setUp()
{
}

void Ut_desas::tearDown()
{
    //    delete mEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/** @brief DESAS test - counter
 * */
void Ut_desas::test_Desas_1()
{
    printf("\n === Test of DES active subs: simple subs\n");
    MNode* root = constructSystem("ut_desas_1");

    bool res = mEnv->RunSystem(12, 2);

    delete mEnv;
}

/** @brief DESAS test - list
 * */
void Ut_desas::test_Desas_2()
{
    printf("\n === Test of DES active subs: list\n");
    MNode* root = constructSystem("ut_desas_2");

    bool res = mEnv->RunSystem(12, 2);

    MNode* asRes = root->getNode("Launcher.AsRes");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.AsRes", asRes);
    DGuri dres;
    GetGData(asRes, dres);
    CPPUNIT_ASSERT_MESSAGE("Wrong AsRes data", dres.mData == "Start");

    delete mEnv;
}



