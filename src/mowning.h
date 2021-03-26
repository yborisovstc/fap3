#ifndef __FAP3_MOWNING_H
#define __FAP3_MOWNING_H

#include "miface.h"

class MOwned;
class MutCtx;

/** @brief Native net owner interface
 * */
class MOwner : public MIface
{
    public:
	static const char* Type() { return "MOwner";};
	// From MIface
	virtual string Uid() const override { return MOwner_Uid();}
	virtual string MOwner_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MOwner_getLif(aType);}
	virtual MIface* MOwner_getLif(const char *aType) = 0;
	// Local
	virtual void getUri(GUri& aUri, MNode* aBase = nullptr) const = 0;
	virtual MNode* getNode(const GUri& aUri, const MNode* aOwned) const = 0;
	virtual MOwned* bindedOwned() = 0;
	virtual const MOwned* bindedOwned() const = 0;
	virtual void onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx) = 0;
};


/** @brief Native net owned interface
 * */
// TODO Do we reaaly need MOwned or better to directly use MUnit? 
class MOwned : public MIface
{
    public:
	static const char* Type() { return "MOwned";};
	// From MIface
	virtual string Uid() const override { return MOwned_Uid();}
	virtual string MOwned_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MOwned_getLif(aType);}
	virtual MIface* MOwned_getLif(const char *aType) = 0;
	// Local
	virtual string ownedId() const = 0;
	virtual void deleteOwned() = 0;
	virtual bool isOwner(const MOwner* aOwner) const = 0;
};



#endif //  __FAP3_MOWNING_H
