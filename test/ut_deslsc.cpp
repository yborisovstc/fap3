#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"
#include "rdatauri.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief Test of Layered System control (LSC)
*/
class Ut_deslsc : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_deslsc);

    //CPPUNIT_TEST(test_DesLsc_1);
    CPPUNIT_TEST(test_DesLsc_2);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    string getStateDstr(const string& aUri);
    MNode* constructSystem(const string& aFname);
    private:
    void test_DesLsc_1();
    void test_DesLsc_2();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_deslsc );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_deslsc, "Ut_deslsc");

string Ut_deslsc::getStateDstr(const string& aUri)
{
    MNode* st = mEnv->Root()->getNode(aUri);
    MDVarGet* stg = st ? st->lIf(stg) : nullptr;
    const DtBase* data = stg ? stg->VDtGet(string()) : nullptr;
    return data ? data->ToString(true) : string();
}

MNode* Ut_deslsc::constructSystem(const string& aSpecn)
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

void Ut_deslsc::setUp()
{
}

void Ut_deslsc::tearDown()
{
    //    delete mEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/** @brief DES LSC test 1
 * */
void Ut_deslsc::test_DesLsc_1()
{
    printf("\n === Test of DES LSC: simple system\n");
    MNode* root = constructSystem("ut_deslsc_1");
    bool res = mEnv->RunSystem(9, 2);

    // Verify that incrementors have same status
    CPPUNIT_ASSERT_MESSAGE("Wrong Incr1.Count", getStateDstr("Launcher.Controller.Manageable.Incr1.Count") == "SI 5");
    CPPUNIT_ASSERT_MESSAGE("Wrong Incr1.Count", getStateDstr("Launcher.Controller.Manageable.Incr2.Count") == "SI 5");

    delete mEnv;
}

/** @brief DES LSC test 2
 * */
void Ut_deslsc::test_DesLsc_2()
{
    printf("\n === Test of DES LSC: increasing the performance\n");
    MNode* root = constructSystem("ut_deslsc_2");
    bool res = mEnv->RunSystem(1120, 2);
    mEnv->profiler()->saveMetrics();

    delete mEnv;
}

