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
    CPPUNIT_TEST(test_Chr4);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Chr1();
    void test_Chr2();
    void test_Chr3();
    void test_Chr4();
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
    CPPUNIT_ASSERT_MESSAGE("Wront type of root comp1", cmp1t == ENt_Node);
    int cn = croot.Count();
    CPPUNIT_ASSERT_MESSAGE("Wrong root node comps number", cn == 3);
    
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



