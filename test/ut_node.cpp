#include <stdlib.h>
#include <unit.h>
#include <syst.h>
#include <content.h>

#include <cppunit/extensions/HelperMacros.h>

#include "env.h"
#include "melem.h"

/*
 * This test of MNode interface main operations
 */


class Ut_node : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_node);
    CPPUNIT_TEST(test_nav_1);
//    CPPUNIT_TEST(test_cont_1);
    CPPUNIT_TEST(test_cre_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_nav_1();
    void test_cont_1();
    void test_cre_1();
private:
    Env* mEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_node );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_node, "Ut_node");


void Ut_node::setUp()
{
}

void Ut_node::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of navigation
 * */
void Ut_node::test_nav_1()
{
    cout << endl << "=== Test of MNode base operations ===" << endl;

    Node* owner = new Node("Owner", nullptr);
    Node* comp1 = new Node("Comp1", nullptr);
    Node* comp2 = new Node("Comp2", nullptr);
    Node* comp2_1 = new Node("Comp2_1", nullptr);
    owner->mCpOwner.connect(&comp1->mCpOwned);
    owner->mCpOwner.connect(&comp2->mCpOwned);
    comp2->mCpOwner.connect(&comp2_1->mCpOwned);

    GUri uri1("Comp2.Comp2_1");
    MNode* res1 = owner->getNode(uri1);
    CPPUNIT_ASSERT_MESSAGE("Failed getting Comp2_1", res1 == comp2_1);

    uri1 = GUri(".Owner.Comp2");
    MNode* res2 = comp2_1->getNode(uri1);
    CPPUNIT_ASSERT_MESSAGE("Failed getting Comp2", res2 == nullptr);

    GUri uri_comp_2_1;
    comp2_1->getUri(uri_comp_2_1);
    string uris = uri_comp_2_1.toString();
    cout << "URI: "<< uris << endl;
    CPPUNIT_ASSERT_MESSAGE("Failed getting Comp2_1", uris == ".Owner.Comp2.Comp2_1");

    // Native agent Uri
    /*
    MNode* nag = owner->getNodeS("Node");
    CPPUNIT_ASSERT_MESSAGE("Failed getting nag", nag);
    GUri naguri;
    nag->getUri(naguri);
    cout << "NOde URI: "<< naguri.toString() << endl;
    */

    delete owner;
}

/** @brief Test of content
 * */
void Ut_node::test_cont_1()
{
    cout << endl << "=== Test of MNode base operations ===" << endl;

    Node* owner = new Node("Owner", nullptr);
    Node* comp1 = new Node("Comp1", nullptr);
    Node* cont1 = new Content("A", nullptr);
    MContent* cont1c = cont1->lIf(cont1c);
    CPPUNIT_ASSERT_MESSAGE("Failed getting cont1c", cont1c);
    cont1c->doDump(0, 0, cout);
    string cdata;
    bool cres = cont1c->getData(cdata);
    CPPUNIT_ASSERT_MESSAGE("Failed getting uninit content", !cres);
    string cwdata = "Hello";
    cres = cont1c->setData(cwdata);
    cont1c->dump(0);
    cres = cont1c->getData(cdata);
    CPPUNIT_ASSERT_MESSAGE("Wrong data", cres && cdata == cwdata);
    Node* comp2 = new Node("Comp2", nullptr);
    Node* comp2_1 = new Node("Comp2_1", nullptr);
    owner->mCpOwner.connect(&comp1->mCpOwned);
    owner->mCpOwner.connect(&cont1->mCpOwned);
    owner->mCpOwner.connect(&comp2->mCpOwned);
    comp2->mCpOwner.connect(&comp2_1->mCpOwned);

    GUri uri1("Comp2.Comp2_1");
    MNode* res1 = owner->getNode(uri1);
    CPPUNIT_ASSERT_MESSAGE("Failed getting Comp2_1", res1 == comp2_1);

    delete owner;
}

/** @brief Test of creating node based model
 * */
void Ut_node::test_cre_1()
{
    cout << endl << "=== Test of creating node based model ===" << endl;

    const string specn("ut_node_cre_1");
    string ext = "chs";
    string spec = specn + string(".") + "chs";
    string log = specn + "_" + ext + ".log";
    mEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv != 0);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
    mEnv->constructSystem();
    MNode* root = mEnv->Root();
    GUri ruri;
    root->getUri(ruri);
    string ruris = ruri.toString();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", eroot);
    root->dump(0x3,0);
}
 

