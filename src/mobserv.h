
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
	using TCp = MNcpp<MObserver, MObservable>;
    public:
	static const char* Type() { return "MObserver";};
	// From MIface
	virtual string Uid() const override { return MObserver_Uid();}
	virtual string MObserver_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MObserver_getLif(aType);}
	virtual MIface* MObserver_getLif(const char *aType) = 0;
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
	static const char* Type() { return "MObservable";};
	// From MIface
	virtual string Uid() const override { return MObservable_Uid();}
	virtual string MObservable_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MObservable_getLif(aType);}
	virtual MIface* MObservable_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MObservable_doDump(aLevel, aIdt, std::cout);}
	virtual void MObservable_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual bool addObserver(MObserver::TCp* aObs) = 0;
	virtual bool rmObserver(MObserver::TCp* aObs) = 0;
};


#endif
