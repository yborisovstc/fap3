
#ifndef __FAP3_ELEM_H
#define __FAP3_ELEM_H

#include "melem.h"
#include "minh.h"
#include "unit.h"
#include "chromo.h"

/** @brief Mutable base agent 
 * */
class Elem: public Unit, public MElem, public MParent, public MChild
{
    public:
	using TInhTreeNode = NTnnp<MChild, MParent>; 
    public:
	static const char* Type() { return "Elem";}
	Elem(const string &aName, MEnv* aEnv);
	virtual ~Elem();
	// From MNode.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MElem.MIface
	virtual string MElem_Uid() const override { return getUid<MElem>();}
	virtual MIface* MElem_getLif(const char *aType) override;
	virtual void MElem_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MNode
	virtual void mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo = false) override;
	virtual MNode* createHeir(const string& aName) override;
	virtual bool attachHeir(MNode* aHeir) override;
	// From MElem
	virtual MChromo& Chromos() override { return *mChromo;}
	virtual void setParent(const string& aParent) override;
	virtual MChild* asChild() override;
	virtual MParent* asParent() override;
	// From MParent
	virtual string MParent_Uid() const override {return getUid<MParent>();}
	virtual MIface* MParent_getLif(const char *aType) override;
	virtual void MParent_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void onChildDeleting(MChild* aChild) override;
	virtual bool onChildRenaming(MChild* aChild, const string& aNewName) override;
	virtual MNode* createHeirPrnt(const string& aName) override;
	virtual bool attachChild(MChild* aChild) override;
	// From MChild
	virtual string MChild_Uid() const override {return getUid<MParent>();}
	virtual MIface* MChild_getLif(const char *aType) override;
	virtual void MChild_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void onParentDeleting(MParent* aParent) override;
	virtual TCp* cP() override;
    protected:
	MNode* getMowner(MNode* aNode);
	void notifyParentMutated(const TMut& aMut);
	void setCrAttr(const string& aEType, const string& aName);
	// From Node
	//virtual void mutSegment(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx) override;
	virtual MNode* mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	virtual void mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	virtual void onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx) override;
	MParent* parent();
    protected:
	MChromo* mChromo;     /*!< Chromo */
	TInhTreeNode mInode;  /*!< Inheritance tree node */
};

#endif

