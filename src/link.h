
#ifndef __FAP3_LINK_H
#define __FAP3_LINK_H

#include "mlink.h"
#include "unit.h"


/** @brief One-way relation 
 * */

class Link : public Unit, public MLink, public MObserver
{
    public:
	inline static constexpr std::string_view idStr() { return "Link"sv;}
    public:
	using TObserverCp = NCpOnp<MObserver, MObservable>;
    public:
	Link(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Link();
	// From MNode.MIface
	virtual MIface* MNode_getLif(TIdHash aTid) override;
	// From MLink.MIface
	virtual string MLink_Uid() const override { return getUid<MLink>();}
	virtual MIface* MLink_getLif(TIdHash aTid) override;
	virtual void MLink_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool connect(MNode* aPair) override;
	virtual bool disconnect(MNode* aPair) override;
	virtual MNode* pair() override;
	// From MObserver
	virtual string MObserver_Uid() const override { return getUid<MObserver>();}
	virtual MIface* MObserver_getLif(TIdHash aTid) override { return nullptr;}
        virtual void onObsOwnerAttached(MObservable* aObl) override {}
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override {}
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override {}
	virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override {}
	virtual void onObsChanged(MObservable* aObl) override {}
    protected:
	TObserverCp mOcp;  /*<! Observer native cp */
	MNode* mPair;
	MLink* mMLinkPtr = nullptr;
};

#endif
