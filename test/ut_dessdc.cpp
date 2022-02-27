#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief This test of vertex/system layers
*/
class Ut_sdc : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_sdc);

    CPPUNIT_TEST(test_Sdc_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_Sdc_1();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_sdc );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_sdc, "Ut_sdc");


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

