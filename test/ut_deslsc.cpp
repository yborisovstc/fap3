#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"
#include "rdatauri.h"

#include <cppunit/extensions/HelperMacros.h>

#include "ut_base.h"

/** @brief Test of Layered System control (LSC)
*/
class Ut_deslsc : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_deslsc);

    //CPPUNIT_TEST(test_DesLsc_1);
    ///CPPUNIT_TEST(test_DesLsc_2);
    //CPPUNIT_TEST(test_DesLsc_2a);
    //CPPUNIT_TEST(test_DesLsc_3);
    CPPUNIT_TEST(test_DesLsc_3a);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    string getStateDstr(const string& aUri) {return UtUtils::getStateDstr(mEnv, aUri);}
    MNode* constructSystem(const string& aFname) { return UtUtils::constructSystem(mEnv, aFname);}
    private:
    void test_DesLsc_1();
    void test_DesLsc_2();
    void test_DesLsc_2a();
    void test_DesLsc_3();
    void test_DesLsc_3a();
    private:
    Env* mEnv;
};


CPPUNIT_TEST_SUITE_REGISTRATION( Ut_deslsc );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_deslsc, "Ut_deslsc");


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
    printf("\n\n === Test of DES LSC: increasing the performance\n");
    printf("\n === Check/compare profiler metrics in profiler data files\n\n");
    printf("\n Running DES with LSC\n");
    MNode* root = constructSystem("ut_deslsc_2l");
    bool res = mEnv->RunSystem(8000, 2);
    printf("\n Run completed, deleting system\n");
    if (mEnv->profiler()) mEnv->profiler()->saveMetrics();

    printf("\n Running DES without LSC\n");
    root = constructSystem("ut_deslsc_2lo");
    res = mEnv->RunSystem(8000, 2);
    printf("\n Run completed, deleting system\n");
    if (mEnv->profiler()) mEnv->profiler()->saveMetrics();

    delete mEnv;
}

/** @brief DES LSC test 2
 * */
void Ut_deslsc::test_DesLsc_2a()
{
    printf("\n\n === Test of DES LSC with DES agt: increasing the performance\n");
    printf("\n === Check/compare profiler metrics in profiler data files\n\n");
    printf("\n Running DES with LSC\n");
    MNode* root = constructSystem("ut_deslsc_2la");
    bool res = mEnv->RunSystem(8000, 2);
    printf("\n Run completed, deleting system\n");
    if (mEnv->profiler()) mEnv->profiler()->saveMetrics();

    /*
    printf("\n Running DES without LSC\n");
    root = constructSystem("ut_deslsc_2lo");
    res = mEnv->RunSystem(8000, 2);
    printf("\n Run completed, deleting system\n");
    if (mEnv->profiler()) mEnv->profiler()->saveMetrics();
    */

    delete mEnv;
}

/** @brief DES LSC test 3: control on Controllable gets idle, ref ds_desopt_au
 * */
void Ut_deslsc::test_DesLsc_3()
{
    printf("\n\n === Test of DES LSC: control on Controllable gets idle, ref ds_desopt_au\n");
    printf("\n Running DES with LSC\n");
    MNode* root = constructSystem("ut_deslsc_3");
    bool res = mEnv->RunSystem(8000, 2);
    printf("\n Run completed, deleting system\n");
    if (mEnv->profiler()) mEnv->profiler()->saveMetrics();

    CPPUNIT_ASSERT_MESSAGE("Counter failed", getStateDstr("Launcher.System.Controlled.Counter") == "SI 100");
    CPPUNIT_ASSERT_MESSAGE("Idle_Dbg failed", getStateDstr("Launcher.System.Idle_Dbg") == "SB true");

    delete mEnv;
}

/** @brief DES LSC test 3a: control on Controllable (Syst+ADes) gets idle, ref ds_desopt_au
 * */
void Ut_deslsc::test_DesLsc_3a()
{
    printf("\n\n === Test of DES LSC: control on Controllable (Syst+ADes) gets idle, ref ds_desopt_au\n");
    printf("\n Running DES with LSC\n");
    MNode* root = constructSystem("ut_deslsc_3a");
    bool res = mEnv->RunSystem(8000, 2);
    printf("\n Run completed, deleting system\n");
    if (mEnv->profiler()) mEnv->profiler()->saveMetrics();

    CPPUNIT_ASSERT_MESSAGE("Counter failed", getStateDstr("Launcher.System.Controlled.Counter") == "SI 100");
    CPPUNIT_ASSERT_MESSAGE("Idle_Dbg failed", getStateDstr("Launcher.System.Idle_Dbg") == "SB true");

    delete mEnv;
}

