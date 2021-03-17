
#ifndef __FAP3_ELEM_H
#define __FAP3_ELEM_H

#include "melem.h"
#include "unit.h"
#include "chromo.h"

/** @brief Mutable base agent 
 * */
class Elem: public Unit, public MElem
{
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
	virtual void mutate(const ChromoNode& aMut, bool aUpdOnly /*EFalse*/, const MutCtx& aCtx) override;
	virtual MNode* createHeir(const string& aName, MNode* aContext) override;
	// From MElem
	virtual MChromo& Chromos() override { return *mChromo;}
	virtual void setParent(const string& aParent) override;
	virtual bool appendChild(MNode* aChild) override;
    protected:
	MNode* getMowner(MNode* aNode);
	MNode* mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	void notifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx);
	void notifyParentMutated(const TMut& aMut);
	void setCrAttr(const string& aEType, const string& aName);
    protected:
	MChromo* mChromo; /*!< Chromo */
};

#endif

