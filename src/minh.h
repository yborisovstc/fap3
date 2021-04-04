
#ifndef __FAP3_MINH_H
#define __FAP3_MINH_H

#include "miface.h"
#include "nconn.h"


class MChild;
class MNode;

/** @brief Native net Parent interface
 * */
class MParent : public MIface
{
    public:
	static const char* Type() { return "MParent";};
	// From MIface
	virtual string Uid() const override { return MParent_Uid();}
	virtual string MParent_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MParent_getLif(aType);}
	virtual MIface* MParent_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MParent_doDump(aLevel, aIdt, std::cout);}
	virtual void MParent_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual void onChildDeleting(MChild* aChild) = 0;
	virtual bool onChildRenaming(MChild* aChild, const string& aNewName) = 0;
	/** @brief Create heir as parent */
	virtual MNode* createHeirPrnt(const string& aName, MNode* aContext) = 0;
	virtual bool attachChild(MChild* aChild) = 0;
};


/** @brief Native net Child interface
 * */
class MChild : public MIface
{
    public:
	using TCp =  MNcpp<MChild, MParent>;
    public:
	static const char* Type() { return "MChild";};
	// From MIface
	virtual string Uid() const override { return MChild_Uid();}
	virtual string MChild_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MChild_getLif(aType);}
	virtual MIface* MChild_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MChild_doDump(aLevel, aIdt, std::cout);}
	virtual void MChild_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual void onParentDeleting(MParent* aParent) = 0;
	virtual TCp* cP() = 0;
};



#endif
