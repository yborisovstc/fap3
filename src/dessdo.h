
#ifndef __FAP3_DESSDO_H
#define __FAP3_DESSDO_H

#include "dest.h"
#include "mobserv.h"

/** @brief SDO base
 * */
class SdoBase : public CpStateOutp, public MDVarGet, public MObserver
{
    public:
	using TObserverCp = NCpOmnp<MObserver, MObservable>;
    protected:
	class InpBase {
	    public:
		InpBase(SdoBase* aHost, const string& aName);
	    public:
		SdoBase* mHost;
		const string mName;
	};
	template <typename T> class Inp : public InpBase {
	    public:
		using Stype = Sdata<T>;
	    public:
		Inp(SdoBase* aHost, const string& aName): InpBase(aHost, aName) {}
		bool getData(T& aData) {
		    bool res = false;
		    MNode* inp = mHost->getNode(mName);
		    if (inp) {
			res = GetSData(inp, aData);
		    } else {
			mHost->Log(TLog(EDbg, mHost) + "Cannot get input [" + mName + "]");
		    }
		    return res;
		}

	};
    public:
	static const char* Type() { return "SdoBase";};
	SdoBase(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual void MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From Node.MOwned
	virtual void onOwnerAttached() override;
	// From MObserver
	virtual string MObserver_Uid() const {return getUid<MObserver>();}
	virtual MIface* MObserver_getLif(const char *aType) override {return nullptr;}
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override;
	virtual void onObsChanged(MObservable* aObl) override;
    protected:
	void UpdateMag();
	void NotifyInpsUpdated();
    protected:
	TObserverCp mObrCp;               /*!< Observer connpoint */
	MNode* mSue; /*!< System under exploring */
};

/** @brief SDO providing Sdata
 * */
template <typename T> class Sdo : public SdoBase, public MDtGet<Sdata<T>>
{
    public:
	using Stype = Sdata<T>;
    public:
	Sdo(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): SdoBase(aType, aName, aEnv) {}
	// From MDVarGet
	virtual MIface* DoGetDObj(const char *aName) override;
    	virtual string VarGetIfid() const override {return MDtGet<Stype>::Type();}
	// From MDtGet
	virtual void MDtGet_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
    protected:
	T mRes;
};

template <typename T> MIface* Sdo<T>::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<Stype>::Type()) == 0) {
	res = dynamic_cast<MDtGet<Stype>*>(this);
    }
    return res;
}

/** @brief SDO providing generic data
 * */
template <typename T> class Sdog : public SdoBase, public MDtGet<T>
{
    public:
	using Stype = T;
    public:
	Sdog(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): SdoBase(aType, aName, aEnv) {}
	// From MDVarGet
	virtual MIface* DoGetDObj(const char *aName) override;
    	virtual string VarGetIfid() const override {return MDtGet<Stype>::Type();}
	// From MDtGet
	virtual void MDtGet_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
    protected:
	T mRes;
};

template <typename T> MIface* Sdog<T>::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<Stype>::Type()) == 0) {
	res = dynamic_cast<MDtGet<Stype>*>(this);
    }
    return res;
}


/** @brief SDO "Name"
 * */
class SdoName : public Sdo<string>
{
    public:
	static const char* Type() { return "SdoName";};
	SdoName(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
};


/** @brief SDO "Component exists"
 * */
class SdoComp : public Sdo<bool>
{
    public:
	static const char* Type() { return "SdoComp";};
	SdoComp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
    protected:
	Inp<string> mInpName;
};

/** @brief SDO "Component count"
 * */
class SdoCompsCount : public Sdo<int>
{
    public:
	static const char* Type() { return "SdoCompsCount";};
	SdoCompsCount(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
};

/** @brief SDO "Component count"
 * */
class SdoCompsNames : public Sdog<Vector<string>>
{
    public:
	static const char* Type() { return "SdoCompsNames";};
	SdoCompsNames(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
};



/** @brief SDO "Vertexes are connected"
 * */
class SdoConn : public Sdo<bool>
{
    public:
	static const char* Type() { return "SdoConn";};
	SdoConn(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
    protected:
	Inp<string> mInpVp;
	Inp<string> mInpVq;
};


#endif  // __FAP3_DESSDO_H


