#include <stdlib.h>
#include <unit.h>

#include <cppunit/extensions/HelperMacros.h>

/*
 * This test of native net 
 */


class Ut_nn : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_nn);
    //CPPUNIT_TEST(test_owning_1);
    CPPUNIT_TEST(test_liter_1);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_owning_1();
    void test_liter_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_nn );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_nn, "Ut_nn");


void Ut_nn::setUp()
{
}

void Ut_nn::tearDown()
{
//    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Test of owner-owned native conn
 * */
void Ut_nn::test_owning_1()
{
    printf("\n === Test of native owner-owned conn\n");
    Unit* owner = new Unit(Unit::Type(), "Owner", nullptr);
    Unit* owned1 = new Unit(Unit::Type(), "Owned1", nullptr);
    Unit* owned2 = new Unit(Unit::Type(), "Owned2", nullptr);
    owner->owner()->connect(owned1->owned());
    owner->owner()->disconnect(owned1->owned());
    owner->owner()->connect(owned1->owned());
    owner->owner()->connect(owned2->owned());
    delete owner;
}

/** @brief Test of pairs iterator
 * */
void Ut_nn::test_liter_1()
{
    printf("\n === Test of leafs iterator\n");
    Unit* owner = new Unit(Unit::Type(), "Owner", nullptr);
    Unit* owned1 = new Unit(Unit::Type(), "Owned1", nullptr);
    Unit* owned1_1 = new Unit(Unit::Type(), "Owned1_1", nullptr);
    Unit* owned1_2 = new Unit(Unit::Type(), "Owned1_2", nullptr);
    Unit* owned2 = new Unit(Unit::Type(), "Owned2", nullptr);
    owner->owner()->connect(owned1->owned());
    owner->owner()->disconnect(owned1->owned());
    owner->owner()->connect(owned1->owned());
    owned1->owner()->connect(owned1_1->owned());
    owned1->owner()->connect(owned1_2->owned());
    owner->owner()->connect(owned2->owned());
    
    // Dump pairs
    cout <<  "Pairs: " << endl;
    auto* oor = owner->owner();
    for (auto it = oor->pairsBegin(); it != oor->pairsEnd(); it++) {
	cout << (*it)->provided()->Uid() << endl;
    }
    // Leafs iter begin
    auto* ood = owner->owned();
    auto lit1 = ood->leafsBegin();
    auto* cpn = *lit1;
    lit1++;
    auto* cpn2 = *lit1;
    lit1++;
    auto* cpn3 = *lit1;
    // Dump leafs
    cout <<  "Leafs: " << endl;
    for (auto lit = ood->leafsBegin(); lit != ood->leafsEnd(); lit++) {
	auto* cpn = *lit;
	cout << cpn->provided()->Uid() << endl;
    }
    // Empty tree iteration
    auto lit2b = owned2->owned()->leafsBegin();
    auto lit2e = owned2->owned()->leafsEnd();
    if (lit2b != lit2e) {
	auto* cpn = *lit2b;
	cout << cpn->provided()->Uid() << endl;
    }

    delete owner;
}
