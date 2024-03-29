#ifndef __FAP3_MOWNING_H
#define __FAP3_MOWNING_H

#include "miface.h"
#include "nconn.h"

class MOwned;
class MutCtx;

/** @brief Native net owner interface
 * */
class MOwner : public MIface
{
    public:
	using TCp =  MNcpp<MOwner, MOwned>;
    public:
	static const char* Type() { return "MOwner";};
	// From MIface
	virtual string Uid() const override { return MOwner_Uid();}
	virtual string MOwner_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MOwner_getLif(aType);}
	virtual MIface* MOwner_getLif(const char *aType) = 0;
	// Local
	/** @brief Gets node by URI
	 * @param aReq  requesting node
	 * */
	virtual MNode* ownerGetNode(const GUri& aUri, const MNode* aReq) const = 0;
	virtual void ownerGetUri(GUri& aUri, MNode* aBase = nullptr) const = 0;
	virtual void onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx) = 0;
	virtual void onOwnedAttached(MOwned* aOwned) = 0;
	virtual void onOwnedDetached(MOwned* aOwned) = 0;
	/** @brief Gets the array of modules, the nearest the first */
	virtual void getModules(vector<MNode*>& aModules) = 0;
	/** @brief Provides parent node by given URI
	 * the variation of getNode with using parent access rule
	 * */
	virtual MNode* getParent(const GUri& aUri) = 0;
	virtual bool isOwned(const MOwned* mOwned) const = 0;
};


/** @brief Native net owned interface
 * */
class MOwned : public MIface
{
    public:
	using TCp =  MNcpp<MOwned, MOwner>;
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
	virtual void onOwnerAttached() = 0;
	virtual void onOwnerDetached() = 0;
	virtual bool isOwner(const MOwner* mOwner) const = 0;
};



#endif //  __FAP3_MOWNING_H
