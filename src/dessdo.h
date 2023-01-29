
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
	template <typename T> class Inpg : public InpBase {
	    public:
		using Stype = T;
	    public:
		Inpg(SdoBase* aHost, const string& aName): InpBase(aHost, aName) {}
		bool getData(T& aData) {
		    bool res = false;
		    MNode* inp = mHost->getNode(mName);
		    if (inp) {
			res = GetGData(inp, aData);
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

/** @brief SDO "Parent"
 * */
class SdoParent : public Sdo<string>
{
    public:
	static const char* Type() { return "SdoParent";};
	SdoParent(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
};

/** @brief SDO "Comp Owner"
 * */
class SdoCompOwner : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoCompOwner";};
	SdoCompOwner(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void DtGet(Stype& aData) override;
    protected:
	Inpg<DGuri> mInpCompUri;  //<! Comp URI
};

/** @brief SDO "Comp Comp"
 * */
class SdoCompComp : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoCompComp";};
	SdoCompComp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void DtGet(Stype& aData) override;
    protected:
	Inpg<DGuri> mInpCompUri;  //<! Comp URI
	Inpg<DGuri> mInpCompCompUri;  //<! Comp comp URI
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

/** @brief SDO "Vertex pairs count"
 * */
class SdoPairsCount : public Sdo<int>
{
    public:
	static const char* Type() { return "SdoPairsCount";};
	SdoPairsCount(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
    protected:
	Inp<string> mInpVert;  //<! Vertex URI
};

/** @brief SDO "Vertex pair URI"
 * Invalid if the vertex is not connected
 * */
class SdoPair : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoPair";};
	SdoPair(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void DtGet(Stype& aData) override;
	virtual void onObsChanged(MObservable* aObl) override;
    protected:
	Inp<string> mInpTarg;  //<! Target Vertex URI
	MNode* mVertUe;        //<! Vertex under exploring 
};

/** @brief SDO "Single pair of targets comp"
 * Invalid if the vertex is not connected
 * It is usefull to explore subsystems connpoints pair
 * */
class SdoTcPair : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoTcPair";};
	SdoTcPair(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void DtGet(Stype& aData) override;
    protected:
	Inpg<DGuri> mInpTarg;  //<! Target URI
	Inpg<DGuri> mInpTargComp;  //<! Target comp vertex URI relative to target
};


/** @brief SDO "Pairs"
 * */
class SdoPairs : public Sdog<Vector<DGuri>>
{
    public:
	static const char* Type() { return "SdoPairs";};
	SdoPairs(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
};

/** @brief SDO "Target Pairs"
 * */
class SdoTPairs : public Sdog<Vector<DGuri>>
{
    public:
	static const char* Type() { return "SdoTPairs";};
	SdoTPairs(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
    protected:
	Inpg<DGuri> mInpTarg;  //<! Target URI
};

/** @brief SDO "Edges"
 * */
class SdoEdges : public Sdog<Vector<Pair<DGuri>>>
{
    public:
	static const char* Type() { return "SdoEdges";};
	SdoEdges(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MDtGet
	virtual void DtGet(Stype& aData) override;
};


#if 0 // NOT COMPLETED
/** @brief Inputs update indicator, ref ds_dcs_iui_sla
 * This is not SDO exactly but nevertheless acts as SDO observing the interior of system
 * The output is boolean indication of inputs update happened
 * */
class SdoUpdateInd: public CpStateOutp, public MDesSyncable, public MDesInpObserver, public MDVarGet, public MDtGet<Sdata<bool>>
{
    public:
	static const char* Type() { return "SdoUpdateInd";};
	SdoUpdateInd(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual void MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
    protected:
	Sdata<bool> mData;
};
#endif







#endif  // __FAP3_DESSDO_H


