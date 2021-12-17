
#ifndef __FAP3_LINK_H
#define __FAP3_LINK_H

#include "mlink.h"
#include "unit.h"


/** @brief One-way relation 
 * */

class Link : public Unit, public MLink, public MObserver
{
    public:
	using TObserverCp = NCpOnp<MObserver, MObservable>;
    public:
	static const char* Type() { return "Link";}
	Link(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Link();
	// From MNode.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MLink.MIface
	virtual string MLink_Uid() const override { return getUid<MLink>();}
	virtual MIface* MLink_getLif(const char *aType) override;
	virtual void MLink_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool connect(MNode* aPair) override;
	virtual bool disconnect(MNode* aPair) override;
	virtual MNode* pair() override;
	// From MObserver
	virtual string MObserver_Uid() const override { return getUid<MObserver>();}
	virtual MIface* MObserver_getLif(const char *aType) override { return nullptr;}
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override {}
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override {}
	virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override {}
	virtual void onObsChanged(MObservable* aObl) override {}
    protected:
	TObserverCp mOcp;  /*<! Observer native cp */
	MNode* mPair;
};

#endif
