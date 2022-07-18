#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"
#include "rmutdata.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief Test of DES contexts, ref ds_dctx
*/
class Ut_dctx : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_dctx);
    //CPPUNIT_TEST(test_des_ctx_1);
    //CPPUNIT_TEST(test_des_ctx_2);
    //CPPUNIT_TEST(test_des_ctx_3);
    CPPUNIT_TEST(test_des_ctx_4);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    MNode* constructSystem(const string& aFname);
    private:
    void test_des_ctx_1();
    void test_des_ctx_2();
    void test_des_ctx_3();
    void test_des_ctx_4();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_dctx );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_dctx, "Ut_dctx");

MNode* Ut_dctx::constructSystem(const string& aSpecn)
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

void Ut_dctx::setUp()
{
}

void Ut_dctx::tearDown()
{
    //    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/** @brief Test of DES context #1
 * */
void Ut_dctx::test_des_ctx_1()
{
    cout << endl << "=== Test of DES context 1 ===" << endl;
    MNode* root = constructSystem("ut_des_ctx_1");
    bool res = mEnv->RunSystem(4, 2);
   
    delete mEnv;
}

/** @brief Test of DES context #2
 * */
void Ut_dctx::test_des_ctx_2()
{
    cout << endl << "=== Test of DES context 2 ===" << endl;
    MNode* root = constructSystem("ut_des_ctx_2");
    bool res = mEnv->RunSystem(4, 2);
   
    delete mEnv;
}

/** @brief Test of DES context #3
 * */
void Ut_dctx::test_des_ctx_3()
{
    cout << endl << "=== Test of DES context 3 ===" << endl;
    MNode* root = constructSystem("ut_des_ctx_3");
    bool res = mEnv->RunSystem(4, 2);
   
    delete mEnv;
}

/** @brief Test of DES context #4 - upward data flow
 * */
void Ut_dctx::test_des_ctx_4()
{
    cout << endl << "=== Test of DES context 4 - upward data flow ===" << endl;
    MNode* root = constructSystem("ut_des_ctx_4");
    bool res = mEnv->RunSystem(4, 2);

    // Verify
    MNode* dbg = root->getNode("Launcher.System1.S1Add_Dbg");
    CPPUNIT_ASSERT_MESSAGE("Fail to get Launcher.System1.S1Add_Dbg", dbg);
    int dres;
    GetSData(dbg, dres);
    CPPUNIT_ASSERT_MESSAGE("Wrong System1.S1Add result", dres == 9);

    delete mEnv;
}
