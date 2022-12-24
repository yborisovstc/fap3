#include <stdlib.h>

#include "chromo2.h"
#include "mvert.h"

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test is for chromo2
 */


class Ut_chromo2 : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_chromo2);
//    CPPUNIT_TEST(test_Chr1);
//    CPPUNIT_TEST(test_Chr2);
//    CPPUNIT_TEST(test_Chr3);
//    CPPUNIT_TEST(test_Chr4);
//    CPPUNIT_TEST(test_Chr5);
    CPPUNIT_TEST(test_Chr_Err_1);
    CPPUNIT_TEST(test_Ddmc);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Chr1();
    void test_Chr2();
    void test_Chr3();
    void test_Chr4();
    void test_Chr5();
    void test_Chr_Err_1();
    void test_Ddmc();
private:
    //Env* iEnv;
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_chromo2 );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_chromo2, "Ut_chromo2");


void Ut_chromo2::setUp()
{
}

void Ut_chromo2::tearDown()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

void Ut_chromo2::test_Chr1()
{
    printf("\n === Test of Chromo2 creation\n");
    Chromo2 chr;
    // Chromo as segment or mutation_create_chromo
    chr.SetFromSpec("{ }");
    cout << "Empty chromo: " << chr.Root().operator string() << endl;
    chr.Root().AddChild(TMut(ENt_Node, ENa_Id, "MyNode", ENa_Parent, "Node"));
    Chromo2 chr2;
    chr2 = chr;
    // Creation from file
    const string fileName("ut_chr2_1.chs");
    cout << "Chromo set from file [" << fileName << "]" << endl;
    chr.SetFromFile(fileName);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());
    ChromoNode croot = chr.Root();
    CPPUNIT_ASSERT_MESSAGE("Chromo root is empty", croot != ChromoNode());
    ChromoNode::Iterator beg = croot.Begin();
    ChromoNode cmp1 = *beg;
    CPPUNIT_ASSERT_MESSAGE("Chromo root comp1 is empty", cmp1 != ChromoNode());
    TNodeType cmp1t = cmp1.Type();
    CPPUNIT_ASSERT_MESSAGE("Wront type of root comp1", cmp1t == ENt_Cont);
    int cn = croot.Count();
    CPPUNIT_ASSERT_MESSAGE("Wrong root node comps number", cn == 5);
}

void Ut_chromo2::test_Chr2()
{
    printf("\n === Test of Chromo2 parsing\n");
    Chromo2 chr;
    const string fileName("ut_chr2_2.chs");
    cout << "Chromo set from file [" << fileName << "]" << endl;
    chr.SetFromFile(fileName);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());
    ChromoNode croot = chr.Root();
    CPPUNIT_ASSERT_MESSAGE("Chromo root is empty", croot != ChromoNode());
    ChromoNode::Iterator beg = croot.Begin();
    ChromoNode cmp1 = *beg;
    CPPUNIT_ASSERT_MESSAGE("Chromo root comp1 is empty", cmp1 != ChromoNode());
    TNodeType cmp1t = cmp1.Type();
    CPPUNIT_ASSERT_MESSAGE("Wront type of root comp1", cmp1t == ENt_Node);
    int cn = croot.Count();
    CPPUNIT_ASSERT_MESSAGE("Wrong root node comps number", cn == 3);
    
}

void Ut_chromo2::test_Chr3()
{
    printf("\n === Test of Chromo2 parsing\n");
    Chromo2 chr;
    const string fileName("ut_chr2_3.chs");
    cout << "Chromo set from file [" << fileName << "]" << endl;
    chr.SetFromFile(fileName);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());
    ChromoNode croot = chr.Root();
    CPPUNIT_ASSERT_MESSAGE("Chromo root is empty", croot != ChromoNode());
    ChromoNode::Iterator beg = croot.Begin();
    ChromoNode cmp1 = *beg;
    CPPUNIT_ASSERT_MESSAGE("Chromo root comp1 is empty", cmp1 != ChromoNode());
    TNodeType cmp1t = cmp1.Type();
    CPPUNIT_ASSERT_MESSAGE("Wront type of root comp1", cmp1t == ENt_Node);
    int cn = croot.Count();
    CPPUNIT_ASSERT_MESSAGE("Wrong root node comps number", cn == 3);
    
}

void Ut_chromo2::test_Chr4()
{
    printf("\n === Test of Chromo2 parsing\n");
    Chromo2 chr;
    const string fileName("ut_chr2_4.chs");
    cout << "Chromo set from file [" << fileName << "]" << endl;
    chr.SetFromFile(fileName);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());
    ChromoNode croot = chr.Root();
    CPPUNIT_ASSERT_MESSAGE("Chromo root is empty", croot != ChromoNode());
}

void Ut_chromo2::test_Chr5()
{
    printf("\n === Test of Chromo2 setting from spec\n");
    Chromo2 chr;

    cout << "Chromo set from spec #1" << endl;
    string spec = "{ V1 !~ V2; }";
    chr.SetFromSpec(spec);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());

    cout << "Chromo set from spec #2" << endl;
    spec = "{ + Module; }";
    chr.SetFromSpec(spec);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());


    cout << "Chromo set from spec #3" << endl;
    spec = "{ N1 : Node; N1 < Cnt = \"Value\"; N1 < BgColor : Content; }";
    chr.SetFromSpec(spec);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());
    spec = "{ Text = \"Button 3\";  BgColor < { R = \"0.0\"; G = \"0.0\"; B = \"1.0\"; } }";
    chr.SetFromSpec(spec);
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    CPPUNIT_ASSERT_MESSAGE("Chromo parsing error", !chr.IsError());
    chr.Root().Dump();
    int count = chr.Root().Count();
    CPPUNIT_ASSERT_MESSAGE("Wrong chromo nodes count", count == 2);

}

void Ut_chromo2::test_Chr_Err_1()
{
    cout << endl << "=== Test of Chromo2 error handling" << endl;
    Chromo2 chr;
   // chr.SetFromFile("ut_chr2_err_1.chs");
    chr.SetFromFile("ut_chr2_err_1.chs");
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
}

void Ut_chromo2::test_Ddmc()
{
    cout << endl << "=== Test of Chromo2 DDMC" << endl;
    Chromo2 chr;
    // Parse the chromo
    chr.SetFromFile("ut_chr2_ddmc.chs");
    if (chr.IsError()) {
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    cout << endl;
    chr.Save("ut_chr2_ddmc_saved.chs");

    // Trying second time parsing
    chr.SetFromFile("ut_chr2_ddmc_saved.chs");
    if (chr.IsError()) {
	cout << "Saved chromo error" << endl;
	cout << "Pos: " << chr.Error().mPos << " -- " << chr.Error().mText << endl;
    }
    chr.Root().Dump();
    cout << endl;

    // Trying convertion
    Chromo2 chr2;
    chr2.Convert(chr);
    chr2.Save("ut_chr2_ddmc_conv.chs");
}
