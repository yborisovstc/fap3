#include <stdlib.h>

#include "des.h"
#include "env.h"
#include "prov.h"

#include <cppunit/extensions/HelperMacros.h>


/** @brief This test of vertex/system layers
*/
class Ut_adp : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_adp);

    CPPUNIT_TEST(test_MnodeAdp_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_MnodeAdp_1();
    private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_adp );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_adp, "Ut_adp");


void Ut_adp::setUp()
{
}

void Ut_adp::tearDown()
{
    //    delete mEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/** @brief MNode DES adapter test
 * */
void Ut_adp::test_MnodeAdp_1()
{
    printf("\n === Test of MNode DES adapter\n");

    const string specn("ut_adp_node_1");
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

    // Verify target agent
    MNode* tag = root->getNode("test.Controller.Adapter.Targets.Target");
    CPPUNIT_ASSERT_MESSAGE("Fail to get target agent", tag);
    // Verify adapter
    MNode* adp = root->getNode("test.Controller.Adapter");
    CPPUNIT_ASSERT_MESSAGE("Fail to get adapter", adp);

    bool res = mEnv->RunSystem(7);

    // Verify comps count
    MNode* cmpCount = root->getNode("test.Controller.Adapter.CompsCount");
    CPPUNIT_ASSERT_MESSAGE("Fail to get adapters CompsCount", cmpCount);
    MUnit* cmpCountu = cmpCount->lIf(cmpCountu);
    MDVarGet* cmpCountVget = cmpCountu->getSif(cmpCountVget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get CompsCount VarGet iface", cmpCountVget);
    MDtGet<Sdata<int>>* cmpCountGsi = cmpCountVget->GetDObj(cmpCountGsi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cmpCountGsi ", cmpCountGsi);
    Sdata<int> cmpCountSi = 0;
    cmpCountGsi->DtGet(cmpCountSi);
    int cc = cmpCountSi.mData;
    CPPUNIT_ASSERT_MESSAGE("Incorrect CompsCount value", cc == 4);
    /*
    // Verify comp UID
    MNode* cmpUid = root->GetNode("./test/Controller/Adapter/CompUid");
    CPPUNIT_ASSERT_MESSAGE("Fail to get adapters CompUid", cmpUid != NULL);
    MDVarGet* cmpUidVget = cmpUid->GetSIfit(cmpUidVget);
    CPPUNIT_ASSERT_MESSAGE("Fail to get CompUid VarGet iface", cmpUidVget != NULL);
    MDtGet<Sdata<string>>* cmpUidGsi = cmpUidVget->GetDObj(cmpUidGsi);
    CPPUNIT_ASSERT_MESSAGE("Fail to get cmpUidGsi ", cmpUidGsi != NULL);
    Sdata<string> cmpUidSi;
    cmpUidGsi->DtGet(cmpUidSi);
    MNode* tagCmp = tag->GetComp(cnt);
    if (tagCmp) {
    CPPUNIT_ASSERT_MESSAGE("Incorrect CompUid", cmpUidSi.mData == tagCmp->Uid());
    }
    */


    delete mEnv;
}




