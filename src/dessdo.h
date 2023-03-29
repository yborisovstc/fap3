
#ifndef __FAP3_DESSDO_H
#define __FAP3_DESSDO_H

#include "dest.h"
#include "mobserv.h"
#include "mlink.h"

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
			mHost->Log(EDbg, TLog(mHost) + "Cannot get input [" + mName + "]");
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
			mHost->Log(EDbg, TLog(mHost) + "Cannot get input [" + mName + "]");
		    }
		    return res;
		}
	};
	class InpLink : public InpBase {
	    public:
		InpLink(SdoBase* aHost, const string& aName): InpBase(aHost, aName) {}
		bool getData(const MNode*& aData) {
		    bool res = false;
		    aData = nullptr;
		    MNode* inp = mHost->getNode(mName);
		    MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
		    if (inpu) {
			MLink* link = inpu->getSif(link);
			if (link) {
			    aData = link->pair();
			    res = true;
			} else {
			    mHost->Log(EDbg, TLog(mHost) + "Cannot get link via input [" + mName + "]");
			}
		    } else {
			mHost->Log(EDbg, TLog(mHost) + "Cannot get input [" + mName + "]");
		    }
		    return res;
		}
	};
	/** @brief Explored agent observer
	 * */
	class EagObs : public MObserver {
	    public:
		EagObs(SdoBase* aHost): mHost(aHost), mOcp(this) {}
		virtual ~EagObs() { }
		// From MObserver
		virtual string MObserver_Uid() const {return MObserver::Type();}
		virtual MIface* MObserver_getLif(const char *aName) override { return nullptr;}
		virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override {
		    mHost->onEagOwnedAttached(aOwned);
		}
		virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override {
		    mHost->onEagOwnedDetached(aOwned);
		}
		virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override {
		    mHost->onEagContentChanged(aCont);
		}
		virtual void onObsChanged(MObservable* aObl) override {
		    mHost->onEagChanged();
		}
	    public:
		TObserverCp mOcp;               /*!< Observer connpoint */
	    private:
		SdoBase* mHost;
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
	// Local
	virtual void onEagOwnedAttached(MOwned* aOwned) {}
	virtual void onEagOwnedDetached(MOwned* aOwned) {}
	virtual void onEagContentChanged(const MContent* aCont) {}
	virtual void onEagChanged() {}
    protected:
	TObserverCp mObrCp;               /*!< Observer connpoint */
	EagObs mEagObs;  /*!< Explored agent observer */
	MNode* mSue; /*!< System under exploring */
};

/** @brief SDO providing generic data
 * */
template <typename T> class Sdog : public SdoBase
{
    public:
	using Stype = T;
    public:
	Sdog(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): SdoBase(aType, aName, aEnv) {}
	// From MDVarGet
	virtual const DtBase* VDtGet(const string& aType) override { return &mRes;}
	virtual MIface* DoGetDObj(const char *aName) override {return nullptr;}
    	virtual string VarGetIfid() const override {return T::TypeSig();}
    protected:
	T mRes;
};

/** @brief SDO "Name"
 * */
class SdoName : public Sdog<Sdata<string>>
{
    public:
	static const char* Type() { return "SdoName";};
	SdoName(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
};

/** @brief SDO "URI of observed node"
 * */
class SdoUri : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoUri";};
	SdoUri(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
};


/** @brief SDO "Parent"
 * */
class SdoParent : public Sdog<Sdata<string>>
{
    public:
	static const char* Type() { return "SdoParent";};
	SdoParent(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
};

/** @brief SDO "Comp Owner"
 * */
class SdoCompOwner : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoCompOwner";};
	SdoCompOwner(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
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
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Inpg<DGuri> mInpCompUri;  //<! Comp URI
	Inpg<DGuri> mInpCompCompUri;  //<! Comp comp URI
};

/** @brief SDO "Comp Uri"
 * TODO NOT COMPLETED
 * */
class SdoCompUri : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoCompUri";};
	SdoCompUri(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override { return nullptr;}
    protected:
	InpLink mInpOwnerLink;  //<! Owner
};






/** @brief SDO "Component exists"
 * */
class SdoComp : public Sdog<Sdata<bool>>
{
    public:
	static const char* Type() { return "SdoComp";};
	SdoComp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Inp<string> mInpName;
};

/** @brief SDO "Component count"
 * */
class SdoCompsCount : public Sdog<Sdata<int>>
{
    public:
	static const char* Type() { return "SdoCompsCount";};
	SdoCompsCount(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
	virtual void onEagOwnedAttached(MOwned* aOwned) override;
	virtual void onEagOwnedDetached(MOwned* aOwned) override;
};

/** @brief SDO "Component count"
 * */
class SdoCompsNames : public Sdog<Vector<string>>
{
    public:
	static const char* Type() { return "SdoCompsNames";};
	SdoCompsNames(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
	virtual void onEagOwnedAttached(MOwned* aOwned) override;
	virtual void onEagOwnedDetached(MOwned* aOwned) override;
};



/** @brief SDO "Vertexes are connected"
 * */
class SdoConn : public Sdog<Sdata<bool>>
{
    public:
	static const char* Type() { return "SdoConn";};
	SdoConn(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
	virtual void onObsChanged(MObservable* aObl) override;
    protected:
	Inp<string> mInpVp;
	Inp<string> mInpVq;
	MNode* mVpUe;        //<! Vertex under exploring 
	MNode* mVqUe;        //<! Vertex under exploring 
};

/** @brief SDO "Vertex pairs count"
 * */
class SdoPairsCount : public Sdog<Sdata<int>>
{
    public:
	static const char* Type() { return "SdoPairsCount";};
	SdoPairsCount(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
	virtual void onObsChanged(MObservable* aObl) override;
    protected:
	Inp<string> mInpVert;  //<! Vertex URI
	MNode* mVertUe;        //<! Vertex under exploring 
};

/** @brief SDO "Vertex pair URI"
 * Invalid if the vertex is not connected
 * */
class SdoPair : public Sdog<DGuri>
{
    public:
	static const char* Type() { return "SdoPair";};
	SdoPair(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
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
	virtual const DtBase* VDtGet(const string& aType) override;
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
	virtual const DtBase* VDtGet(const string& aType) override;
};

/** @brief SDO "Target Pairs"
 * */
class SdoTPairs : public Sdog<Vector<DGuri>>
{
    public:
	static const char* Type() { return "SdoTPairs";};
	SdoTPairs(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
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
	virtual const DtBase* VDtGet(const string& aType) override;
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


