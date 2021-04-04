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


class Ut_node : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_node);
//    CPPUNIT_TEST(test_nav_1);
//    CPPUNIT_TEST(test_cont_1);
//    CPPUNIT_TEST(test_cre_1);
    CPPUNIT_TEST(test_node_aul_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_nav_1();
    void test_cont_1();
    void test_cre_1();
    void test_node_aul_1();
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

    MNode* owner = new Node("Owner", nullptr);
    MNode* comp1 = new Node("Comp1", nullptr);
    MNode* cont1 = new Content("Cont1", nullptr);
    MNode* cont2 = new Content("Cont2", nullptr);
    MNode* cont2_1 = new Content("Cont2_1", nullptr);
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
    MNode* comp2 = new Node("Comp2", nullptr);
    MNode* comp2_1 = new Node("Comp2_1", nullptr);
    owner->owner()->connect(comp1->owned());
    owner->owner()->connect(cont1->owned());
    owner->owner()->connect(cont2->owned());
    owner->owner()->connect(comp2->owned());
    comp2->owner()->connect(comp2_1->owned());
    cont2->owner()->connect(cont2_1->owned());


    MContent* cont21c = cont2_1->lIf(cont21c);
    CPPUNIT_ASSERT_MESSAGE("Failed getting cont21c", cont1c);
    cwdata = "World";
    cres = cont21c->setData(cwdata);
    CPPUNIT_ASSERT_MESSAGE("Failed to set cont21c data", cres);

    GUri uri1("Comp2.Comp2_1");
    MNode* res1 = owner->getNode(uri1);
    CPPUNIT_ASSERT_MESSAGE("Failed getting Comp2_1", res1 == comp2_1);

    // testing getContent
    MContentOwner* cowner = owner->lIf(cowner);
    string c21data;
    cres = cowner->getContent(string("Cont2.Cont2_1"), c21data);
    CPPUNIT_ASSERT_MESSAGE("Failed getting owner Cont2.Cont2_1 data", cres);
    CPPUNIT_ASSERT_MESSAGE("Wrong owner Cont2.Cont2_1 data", c21data == "World");

    // Dump owner
    cout << "owner content dump:" << endl;
    cowner->dump(1,0);
    ostringstream ss;
    cowner->doDump(1,0,ss);
    string dump = ss.str();
    const string dumpsample("- Cont1: Hello\n- Cont2: nil\n   - Cont2_1: World\n");
    CPPUNIT_ASSERT_MESSAGE("Wrong conent dump of owner", dump == dumpsample);

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
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", mEnv);
    //mEnv->ImpsMgr()->ResetImportsPaths();
    //mEnv->ImpsMgr()->AddImportsPaths("../modules");
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
}
 
/** @brief Test of access to upper layers
 * */
void Ut_node::test_node_aul_1()
{
    cout << endl << "=== Test of access to upper layer ===" << endl;

    const string specn("ut_node_aul_1");
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
    // Save root chromoe
    eroot->Chromos().Save(specn + "_saved." + ext);
    MNode* n1_1 = root->getNode("N1.N1_1");
    CPPUNIT_ASSERT_MESSAGE("Fail to get n1_1", n1_1);
    // Try to access owner 
    MNode* n1 = n1_1->getNode(".MyRoot.N1");
    CPPUNIT_ASSERT_MESSAGE("Fail to deny access to n1", !n1);

}
 


