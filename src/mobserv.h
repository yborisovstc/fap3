
#ifndef __FAP3_MOBSERV_H
#define __FAP3_MOBSERV_H

#include "nconn.h"

class MObservable;
class MContent;

/** @brief Observer interface
 * */
class MObserver: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MObserver"sv;}
	inline static constexpr TIdHash idHash() { return 0x6162659973315528;}
    public:
	using TCp = MNcpp<MObserver, MObservable>;
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MObserver_Uid();}
	virtual string MObserver_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) { return MObserver_getLif(aTid);}
	virtual MIface* MObserver_getLif(TIdHash aTid) = 0;
	// Local
	virtual void onObsOwnerAttached(MObservable* aObl) = 0;
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) = 0;
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) = 0;
	virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) = 0;
	virtual void onObsChanged(MObservable* aObl) = 0;
};

/** @brief Observable interface
 * */
class MObservable: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MObservable"sv;}
	inline static constexpr TIdHash idHash() { return 0x1f3be8525c6d0537;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MObservable_Uid();}
	virtual string MObservable_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) { return MObservable_getLif(aTid);}
	virtual MIface* MObservable_getLif(TIdHash aTid) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MObservable_doDump(aLevel, aIdt, std::cout);}
	virtual void MObservable_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual bool addObserver(MObserver::TCp* aObs) = 0;
	virtual bool rmObserver(MObserver::TCp* aObs) = 0;
};


#endif
