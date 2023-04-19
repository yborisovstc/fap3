
#ifndef __FAP3_DES_H
#define __FAP3_DES_H

#include "mlog.h"
#include "mdata.h"
#include "mdes.h"
#include "mlauncher.h"

#include "nconn.h"
#include "unit.h"
#include "vert.h"
#include "syst.h"
#include "content.h"
#include "rdatauri.h"

// Experimental oprimization of DES cycle, ref ds_mdc_sw
#define DES_LISTS_SWAP

// Experimantal. Cp connpoints with ifaces impl. IRM prb solution ds_irm_wprc_uic. Not completed, errors happen.
//#define DES_CPS_IFC

/** @brief State Connection point
 * */
class CpState: public ConnPointu
{
    public:
	static const char* Type() { return "CpState";};
	CpState(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From Vertu
	virtual void onConnected() override;
	virtual void onDisconnected() override;
	// From MUnit
	virtual void onIfpInvalidated(MIfProv* aProv) override;
	// Local
	void notifyInpsUpdated();
};

#ifdef DES_CPS_IFC
/** @brief Connection point - input of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateInp: public CpState, public MDesInpObserver
{
    protected:
	/** @brief Ifc provider to redirect IRM request to owner.  Trans as owner require RQ context
	 * for IRM algo. So this provider makes the context required
	 * */
	struct InpObsProvider : public MIfProvOwner {
	    InpObsProvider(CpStateInp* aHost): mHost(aHost), mIfr(this, MDesInpObserver::Type())  {}
	    // From MIfProvOwner
	    virtual string MIfProvOwner_Uid() const override { return mHost->getUid<MIfProvOwner>() + "~Iop";}
	    virtual MIface* MIfProvOwner_getLif(const char *aType) override;
	    virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	    virtual void onIfpDisconnected(MIfProv* aProv) override;
	    virtual void onIfpInvalidated(MIfProv* aProv) override;
	    CpStateInp* mHost;
	    IfrNodeRoot mIfr;
	};
    public:
	static const char* Type() { return "CpStateInp";};
	CpStateInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From Node
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
    protected:
	InpObsProvider mIop;
};

#else // DES_CPS_IFC
/** @brief Connection point - input of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateInp: public CpState
{
    public:
	static const char* Type() { return "CpStateInp";};
	CpStateInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};
#endif // DES_CPS_IFC

#ifdef DES_CPS_IFC
/** @brief Connection point - output of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateOutp: public CpState
{
    public:
	static const char* Type() { return "CpStateOutp";};
	CpStateOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

#else // DES_CPS_IFC
/** @brief Connection point - output of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateOutp: public CpState
{
    public:
	static const char* Type() { return "CpStateOutp";};
	CpStateOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};
#endif // DES_CPS_IFC

/** @brief Connection point - input of state required MNode iface
 * Just ConnPointu with pre-configured prepared/required
 * */
class CpStateMnodeInp: public CpState
{
    public:
	static const char* Type() { return "CpStateMnodeInp";};
	CpStateMnodeInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief CpStateInp direct extender (extd as inp)
 * */
class ExtdStateInp : public Extd
{
    public:
	static const char* Type() { return "ExtdStateInp";};
	ExtdStateInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief CpStateOutp direct extender (extd as outp)
 * */
class ExtdStateOutp : public Extd
{
    public:
	static const char* Type() { return "ExtdStateOutp";};
	ExtdStateOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief CpStateOutp direct extender (extd as outp)
 * */
class ExtdStateMnodeOutp : public Extd
{
    public:
	static const char* Type() { return "ExtdStateMnodeOutp";};
	ExtdStateMnodeOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief CpStateOutp direct extender (extd as outp) with ifaces impl, ref ds_irm_wprc_blc
 * !! Supports only single MDVarGet input
 * */
class ExtdStateOutpI : public ExtdStateOutp, public MDVarGet, protected MDesInpObserver
{
    public:
	static const char* Type() { return "ExtdStateOutpI";};
	ExtdStateOutpI(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
	virtual const DtBase* VDtGet(const string& aType) override;
	virtual string VarGetIfid() const override;
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
};



/** @brief Connection point - output of state provided MNode iface
 * Just ConnPointu with pre-configured prepared/required
 * */
class CpStateMnodeOutp: public CpState
{
    public:
	static const char* Type() { return "CpStateMnodeOutp";};
	CpStateMnodeOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};


/** @brief State, ver. 2, non-inhritable, monolitic, direct data, switching updated-confirmed
 * */
class State: public Vertu, public MConnPoint, public MDesSyncable, public MDesInpObserver, public MDVarGet, public MDVarSet, public Cnt::Host
{
    public:
	/** @brief Pseudo content */
	class SCont : public MContent {
	    public:
	    SCont(State& aHost, const string& aName) : mName(aName), mHost(aHost), mUpdated(false) {}
	    // From MContent
	    virtual string MContent_Uid() const override { return mHost.getCntUid(mName, MContent::Type());}
	    virtual MIface* MContent_getLif(const char *aType) override { return nullptr;}
	    virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	    virtual string contName() const override { return mName;}
	    public:
	    bool mUpdated;
	    string mValue;
	    protected:
	    string mName;
	    State& mHost;
	};
	/** @brief Value content */
	class SContValue : public SCont {
	    public:
	    SContValue(State& aHost, const string& aName): SCont(aHost, aName) {}
	    // From MContent
	    virtual bool getData(string& aData) const override;
	    virtual bool setData(const string& aData) override;
	};
    public:
	static const char* Type() { return "State";};
	State(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~State();
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node
	virtual MIface* MOwner_getLif(const char *aType) override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
	// From MVert
	virtual MIface *MVert_getLif(const char *aType) override;
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual TDir getDir() const override { return EOut;}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
	virtual int countOfActive(bool aLocal = false) const override { return 0;}
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const {return getUid<MDesInpObserver>();}
	virtual string provName() const override;
	virtual string reqName() const override;
	// From Cnt.Host
	virtual string getCntUid(const string& aName, const string& aIfName) const override { return getUid(aName, aIfName);}
	virtual MContentOwner* cntOwner() override { return this;}
	// From Node.MContentOwner
	virtual int contCount() const override { return 1 + Node::contCount();}
	virtual MContent* getCont(int aIdx) override;
	virtual const MContent* getCont(int aIdx) const override;
	virtual bool getContent(const GUri& aCuri, string& aRes) const override;
	virtual bool setContent(const GUri& aCuri, const string& aData) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override {return getUid<MDVarGet>();}
	virtual string VarGetIfid() const override;
	virtual MIface* DoGetDObj(const char *aName) override {return nullptr;}
	virtual DtBase* VDtGet(const string& aType) override;
	// From MDVarSet
	virtual string MDVarSet_Uid() const override {return getUid<MDVarSet>();}
	virtual string VarGetSIfid();
	virtual const bool VDtSet(const DtBase& aData) override;
    public:
	static const string KCont_Value;
    protected:
	MDVarGet* GetInp();
	bool updateWithContValue(const string& aCnt);
    protected:
	/** @brief Notifies dependencies of input updated */
	void NotifyInpsUpdated();
	// From MNode
	virtual MIface* MOwned_getLif(const char *aType) override;
	// From MUnit
	virtual void onIfpInvalidated(MIfProv* aProv) override;
	// From Vertu
	virtual void onConnected() override;
	virtual void onDisconnected() override;
	// Local
	void refreshInpObsIfr();
	DtBase* CreateData(const string& aSrc);
    protected:
	DtBase* mPdata;   //<! Preparing (updating) phase data
	DtBase* mCdata;   //<! Confirming phase data
	SContValue mValue = SContValue(*this, KCont_Value);
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
	MIfProv* mInpProv;
	bool mInpValid;
	bool mStDead;     //<! Sign of Stated destructed, needs to avoid callbacks initialted by bases */
	MNode* mInp;      //<! Input cached
};


/** @brief Constant data
 * */
class Const: public Vertu, public MConnPoint, public MDVarGet, public Cnt::Host
{
    public:
	/** @brief Pseudo content */
	class SCont : public MContent {
	    public:
	    SCont(Const& aHost, const string& aName) : mName(aName), mHost(aHost), mUpdated(false) {}
	    // From MContent
	    virtual string MContent_Uid() const override { return mHost.getCntUid(mName, MContent::Type());}
	    virtual MIface* MContent_getLif(const char *aType) override { return nullptr;}
	    virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	    virtual string contName() const override { return mName;}
	    public:
	    bool mUpdated;
	    string mValue;
	    protected:
	    string mName;
	    Const& mHost;
	};
	/** @brief Value content */
	class SContValue : public SCont {
	    public:
	    SContValue(Const& aHost, const string& aName): SCont(aHost, aName) {}
	    // From MContent
	    virtual bool getData(string& aData) const override;
	    virtual bool setData(const string& aData) override;
	};
    public:
	static const char* Type() { return "Const";};
	Const(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~Const();
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node
	virtual MIface* MOwner_getLif(const char *aType) override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
	// From MVert
	virtual MIface *MVert_getLif(const char *aType) override;
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual TDir getDir() const override { return EOut;}
	// From MConnPoint
	virtual string MConnPoint_Uid() const {return getUid<MDesInpObserver>();}
	virtual string provName() const override;
	virtual string reqName() const override;
	// From Cnt.Host
	virtual string getCntUid(const string& aName, const string& aIfName) const override { return getUid(aName, aIfName);}
	virtual MContentOwner* cntOwner() override { return this;}
	// From Node.MContentOwner
	virtual int contCount() const override { return 1 + Node::contCount();}
	virtual MContent* getCont(int aIdx) override;
	virtual const MContent* getCont(int aIdx) const override;
	virtual bool getContent(const GUri& aCuri, string& aRes) const override;
	virtual bool setContent(const GUri& aCuri, const string& aData) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override {return getUid<MDVarGet>();}
	virtual string VarGetIfid() const override;
	virtual MIface* DoGetDObj(const char *aName) override {return nullptr;}
	virtual DtBase* VDtGet(const string& aType) override;
    public:
	static const string KCont_Value;
    protected:
	MDVarGet* GetInp();
	bool updateWithContValue(const string& aCnt);
    protected:
	/** @brief Notifies dependencies of input updated */
	void NotifyInpsUpdated();
	// From MNode
	virtual MIface* MOwned_getLif(const char *aType) override;
	// From MUnit
	virtual void onIfpInvalidated(MIfProv* aProv) override;
	// From Vertu
	virtual void onConnected() override;
	virtual void onDisconnected() override;
	// Local
	void refreshInpObsIfr();
	DtBase* CreateData(const string& aSrc);
    protected:
	DtBase* mData;   //<! Data
	SContValue mValue = SContValue(*this, KCont_Value);
	MIfProv* mInpProv;
	bool mIsDead;     //<! Sign of instance destructed, needs to avoid callbacks initialted by bases */
};




/** @brief DES system
* */
class Des: public Syst, public MDesSyncable, public MDesObserver
{
    public:
    using TScblReg = list<MDesSyncable*>;
    public:
	static const char* Type() { return "Des";};
	Des(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node
	virtual MIface* MOwner_getLif(const char *aType) override;
	virtual void onOwnedAttached(MOwned* aOwned) override;
	virtual void onOwnedDetached(MOwned* aOwned) override;
	virtual MIface* MOwned_getLif(const char *aType);
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
	virtual int countOfActive(bool aLocal = false) const override;
	// From MDesObserver
	virtual string MDesObserver_Uid() const override {return getUid<MDesObserver>();}
	virtual void MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    protected:
	static void RmSyncable(TScblReg& aReg, MDesSyncable* aScbl);
    protected:
#ifdef DES_LISTS_SWAP
	TScblReg mSP;     /*!< Active compoments */
	TScblReg mSQ;    /*!< Updated compoments */
	TScblReg* mActive = &mSP;
	TScblReg* mUpdated = &mSQ;
#else
	TScblReg mActive;     /*!< Active compoments */
	TScblReg mUpdated;    /*!< Updated compoments */
#endif
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
	bool mUpd = false;
};

/** @brief DES agent
 * Intended to be embedded into host system
 * TODO Do we really need it? It cannot be embedded to ordinary system, but to DES
 * (it needs owner to be MDesObserver or resolve this iface). Consider to remove.
* */
class ADes: public Unit, public MAgent, public MDesSyncable, public MDesObserver, public MObserver
{
    public:
	using TAgtCp = NCpOnp<MAgent, MAhost>;  /*!< Agent conn point */
	using TObserverCp = NCpOnp<MObserver, MObservable>;
    public:
	static const char* Type() { return "ADes";};
	ADes(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~ADes();
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node
	virtual MIface* MOwned_getLif(const char *aType);
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
	virtual int countOfActive(bool aLocal = false) const override;
	// From MDesObserver
	virtual string MDesObserver_Uid() const override {return getUid<MDesObserver>();}
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
	virtual void MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	// From MAgent
	virtual string MAgent_Uid() const override {return getUid<MAgent>();}
	virtual MIface* MAgent_getLif(const char *aType) override;
	// From MObserver
	virtual string MObserver_Uid() const  override {return getUid<MObserver>();}
	virtual MIface* MObserver_getLif(const char *aType) override;
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override {}
	virtual void onObsChanged(MObservable* aObl) override {}
	// From Node.MOwned
	virtual void onOwnerAttached() override;
    protected:
	MNode* ahostGetNode(const GUri& aUri);
	MNode* ahostNode();
	MDesObserver* getDesObs();
    protected:
#ifdef DES_LISTS_SWAP
	list<MDesSyncable*> mSP;     /*!< Active compoments */
	list<MDesSyncable*> mSQ;    /*!< Updated compoments */
	list<MDesSyncable*>* mActive = &mSP;
	list<MDesSyncable*>* mUpdated = &mSQ;
#else
	list<MDesSyncable*> mActive;     /*!< Active compoments */
	list<MDesSyncable*> mUpdated;    /*!< Updated compoments */
#endif
	TObserverCp mOrCp;               /*!< Observer connpoint */ 
	TAgtCp mAgtCp;                   /*!< Agent connpoint */ 
	bool mUpdNotified;               //<! Sign of that State notified observers on Update
	bool mActNotified;               //<! Sign of that State notified observers on Activation
	bool mUpd = false;
};



/** @brief Launcher of DES
 * Runs DES syncable owned
 * */
class DesLauncher: public Des, public MLauncher
{
    public:
	static const char* Type() { return "DesLauncher";};
	DesLauncher(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MOwned
	virtual MIface* MOwned_getLif(const char *aType) override;
	// From MLauncher
	virtual string MLauncher_Uid() const override {return getUid<MLauncher>();}
	virtual bool Run(int aCount = 0, int aIdleCount = 0) override;
	virtual bool Stop() override;
	// Local
	virtual void OnIdle();
    protected:
	void updateCounter(int aCnt);
	void outputCounter(int aCnt);
    protected:
	int mCounter = 0;
	bool mStop;
};

/** @brief Active subsystem of DES
 * Runs on master DES update
 * */
class DesAs: public DesLauncher
{
    public:
	static const char* Type() { return "DesAs";};
	DesAs(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MLauncher
	virtual bool Run(int aCount = 0, int aIdleCount = 0) override;
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override {}
	virtual void setUpdated() override {}
	virtual void setActivated() override;
    protected:
	bool mRunning;
};




// Helpers

template <typename T> bool GetSData(MNode* aDvget, T& aData)
{
    bool res = false;
    MUnit* vgetu = aDvget->lIf(vgetu);
    MDVarGet* vget = vgetu ? vgetu->getSif(vget) : nullptr;
    if (vget) {
	const Sdata<T>* data = vget->DtGet(data);
	if (data) {
	    aData = data->mData;
	    res = data->IsValid();
	}
    }
    return res;
}

// TODO get rid of returned data
template <typename T> bool GetGData(MNode* aDvget, T& aData)
{
    bool res = false;
    MUnit* vgetu = aDvget->lIf(vgetu);
    MDVarGet* vget = vgetu->getSif(vget);
    if (vget) {
	const T* data = vget->DtGet(data);
	if (data) {
	    aData = *data;
	    res = data->IsValid();
	}
    }
    return res;
}

///// Embedded DES elements support

class DesEIbb;
class DesEOstb;
class DesEParb;

/** @brief Local iface of the host of DES embedded elements
 * */
class IDesEmbHost
{
    public:
	virtual void registerIb(DesEIbb* aIap) = 0;
	virtual void registerOst(DesEOstb* aOst) = 0;
	virtual void registerPar(DesEParb* aPar) { assert(false);}
	virtual void logEmb(int aCtg, const TLog& aRec) =0;
};

/** @brief Double buffering Input base, ref ds_dee_ssi
 * This is lightweight solution for embedding states into some DES syncable 
 * Another option of embedding the state would be separate simple input access point and pseudo state
 * but this option is more complicated.
 * Note: this embedding approach "erodes" strong DES design, so needs to be used carefully
 *
 * @parem Th  type of host
 * */
class DesEIbb: public MDesInpObserver, public MDesSyncable
{
    public:
	DesEIbb(MNode* aHost, const string& aInpUri, const string& aCpType = CpStateInp::Type()):
	    mHost(aHost), mUri(aInpUri), mCpType(aCpType), mUpdated(false), mActivated(true),
	    mChanged(false), mValid(false) { eHost()->registerIb(this);}
	string getUri() const { return mUri;}
	// From MDesInpObserver
	virtual void onInpUpdated() override { setActivated();}
	virtual string MDesInpObserver_Uid() const override { return MDesInpObserver::Type();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return MDesSyncable::Type();} 
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
	virtual void update() override { mChanged = false;}
	virtual void setUpdated() override { mUpdated = true; sHost()->setUpdated();}
	virtual void setActivated() override { mActivated = true; sHost()->setActivated();}
	virtual int countOfActive(bool aLocal = false) const override { return 1;}
    protected:
	template <typename S> string toStr(const S& aData) { return to_string(aData); }
	string toStr(const string& aData) { return aData; }
	string toStr(MNode*& aData) { return (aData ? aData->Uid() : "nil"); }
	string toStr(const DtBase& aData) { return aData.ToString(true); }
	string toStr(const DGuri& aData) { return aData.ToString(true); }
	MDesSyncable* sHost() { MDesSyncable* ss = mHost->lIf(ss); return ss;}
	IDesEmbHost* eHost() { return dynamic_cast<IDesEmbHost*>(mHost);}
    public:
	MNode* mHost;  /*!< Host */
	string mUri;  /*!< Input URI */
	string mCpType; /*!< Type of input connpoint */
	bool mActivated; /*!< Indication of data is active (to be updated) */
	bool mUpdated; /*!< Indication of data is updated */
	bool mChanged; /*!< Indication of data is changed */
	bool mValid;  /*!< Indication of data validity */
};

/** @brief Input buffered with typed data
 * @param  T  data type 
 * */
template <typename T> class DesEIbt: public DesEIbb
{
    public:
	DesEIbt(MNode* aHost, const string& aInpUri, const string& aCpType = CpStateInp::Type()): DesEIbb(aHost, aInpUri, aCpType) {}
	// From MDesSyncable
	virtual void confirm() override;
	// Local
	T& data() {return mCdt;}
    public:
	T mUdt, mCdt;  /*!< Updated and confirmed data */
};

template <typename T> void DesEIbt<T>::confirm()
{
    if (mUdt != mCdt) {
	eHost()->logEmb(TLogRecCtg::EDbg, TLog(mHost) + "[" + mUri + "] Updated: [" + toStr(mCdt) + "] -> [" + toStr(mUdt) + "]");
	mCdt = mUdt; mChanged = true;
    } else mChanged = false;
    mUpdated = false;
}

/** @brief Input buffered base data DtBase
 * @param  T  data type
 * */
// TODO to migrate to base data buffered input
template <typename T> class DesEIbd: public DesEIbt<T>
{
    public:
	using TP = DesEIbb;
	DesEIbd(MNode* aHost, const string& aInpUri): DesEIbt<T>(aHost, aInpUri) {}
	// From MDesSyncable
	virtual void update() override;
};

template <typename T> void DesEIbd<T>::update()
{
    DesEIbb::update();
    MNode* inp = TP::mHost->getNode(TP::mUri);
    if (inp) TP::mValid = GetGData(inp, this->mUdt);
    if (!TP::mValid) this->eHost()->logEmb(TLogRecCtg::EDbg, TLog(TP::mHost) + "Cannot get input [" + this->mUri + "]");
    else { this->mActivated = false; this->setUpdated(); }
}


/** @brief Input buffered Sdata based
 * @param  T  data type 
 * */
// TODO to migrate to base data buffered input 
template <typename T> class DesEIbs: public DesEIbt<T>
{
    public:
	using TP = DesEIbb;
	DesEIbs(MNode* aHost, const string& aInpUri): DesEIbt<T>(aHost, aInpUri) {}
	// From MDesSyncable
	virtual void update() override;
};

template <typename T> void DesEIbs<T>::update()
{
    DesEIbb::update();
    MNode* inp = TP::mHost->getNode(TP::mUri);
    if (inp) TP::mValid = GetSData(inp, this->mUdt);
    if (!TP::mValid) this->eHost()->logEmb(TLogRecCtg::EDbg, TLog(TP::mHost) + "Cannot get input [" + this->mUri + "]");
    else { this->mActivated = false; this->setUpdated(); }
}


/** @brief Input buffered MNode based
 * @param  T  data type 
 * */
class DesEIbMnode: public DesEIbt<MNode*>
{
    public:
	using TP = DesEIbb;
	using TPT = DesEIbt<MNode*>;
	DesEIbMnode(MNode* aHost, const string& aUri): DesEIbt<MNode*>(aHost, aUri, CpStateMnodeInp::Type())
	{ TPT::mCdt = nullptr; TPT::mUdt = nullptr; }
	// From MDesSyncable
	virtual void update() override;
};

/** @brief Output state - embedded pseudo-state "connnected" to host output
 * */
class DesEOstb: public MDVarGet {
    public:
	DesEOstb(MNode* aHost, const string& aCpUri, const string& aCpType = CpStateOutp::Type()):
	    mHost(aHost), mCpUri(aCpUri), mCpType(aCpType) { eHost()->registerOst(this);}
	string getCpUri() const { return mCpUri;}
	// From MDVarGet
	virtual string MDVarGet_Uid() const override {return MDVarGet::Type();}
    public:
	void NotifyInpsUpdated();
    protected:
	MDesSyncable* sHost() { MDesSyncable* ss = mHost->lIf(ss); return ss;}
	IDesEmbHost* eHost() { return dynamic_cast<IDesEmbHost*>(mHost);}
    public:
	MNode* mHost;
	string mCpUri;  /*!< Output URI */
	string mCpType; /*!< Type of input connpoint */
};


/** @brief Output state with Sdata
 * */
template <typename T> class DesEOsts: public DesEOstb {
    public:
	using Tdata = Sdata<T>;
	DesEOsts(MNode* aHost, const string& aCpUri): DesEOstb(aHost, aCpUri) {}
	// From MDVarGet
	virtual string VarGetIfid() const override {return Tdata::TypeSig();}
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
	virtual const DtBase* VDtGet(const string& aType) override { return &mData;}
	// Local
	void updateData(const T& aData);
    public:
	Tdata mData;
};

// TODO to use Tdata as arg
template <typename T>
void DesEOsts<T>::updateData(const T& aData)
{
    if (aData != mData.mData || !mData.IsValid()) {
	Tdata newData(aData);
	eHost()->logEmb(TLogRecCtg::EDbg, TLog(mHost) + "[" + mCpUri + "] Updated: [" + mData.ToString() + "] -> [" + newData.ToString() + "]");
	mData.mData = aData; mData.mValid = true;
	NotifyInpsUpdated();
    }
}

/** @brief Output state with generic data
 * */
template <typename T> class DesEOst: public DesEOstb {
    public:
	using Tdata = T;
	DesEOst(MNode* aHost, const string& aCpUri): DesEOstb(aHost, aCpUri) {}
	// From MDVarGet
	virtual string VarGetIfid() const override {return Tdata::TypeSig();}
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
	virtual const DtBase* VDtGet(const string& aType) override { return &mData;}
	// Local
	void updateData(const T& aData);
	void updateInvalid();
    public:
	Tdata mData;
};

template <typename T>
void DesEOst<T>::updateData(const T& aData)
{
    if (aData != mData) {
	eHost()->logEmb(TLogRecCtg::EDbg, TLog(mHost) + "[" + mCpUri + "] Updated: [" + mData.ToString() + "] -> [" + aData.ToString() + "]");
	mData = aData;
	NotifyInpsUpdated();
    }
}

template <typename T>
void DesEOst<T>::updateInvalid()
{
    if (mData.IsValid()) {
	T prevData = mData;
	mData.mValid = false;
	eHost()->logEmb(TLogRecCtg::EDbg, TLog(mHost) + "[" + mCpUri + "] Updated: [" + prevData.ToString() + "] -> [" + mData.ToString() + "]");
	NotifyInpsUpdated();
    }
}




/** @brief DES affecting Parameter base (not completed)
 * */
class DesEParb
{
    public:
	DesEParb(MNode* aHost, const string& aUri): mHost(aHost), mUri(aUri) { eHost()->registerPar(this);}
    protected:
	IDesEmbHost* eHost() { return dynamic_cast<IDesEmbHost*>(mHost);}
	void updatePar(const MContent* aCont);
    protected:
	MNode* mHost;
	const string mUri;  //!< Paremeter's URI
};


/* @brief DES context supplier
 * */
class DesCtxSpl : public Des, public MDesCtxSpl
{
    public:
	using TSplCp = NCpOmnp<MDesCtxSpl, MDesCtxCsm>;  /*!< Supplier connpoint */
    public:
	static const char* Type() { return "DesCtxSpl";};
	DesCtxSpl(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MNode.MOwned
	virtual MIface* MOwned_getLif(const char *aType) override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MDesCtxSpl
	virtual string MDesCtxSpl_Uid() const override {return getUid<MDesCtxSpl>();}
	virtual void MDesCtxSpl_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MIface* MDesCtxSpl_getLif(const char *aType) override;
	// Local
	virtual string getSplId() const { return name(); }
	virtual MDesCtxSpl* getSplsHead() override;
	virtual bool registerCsm(MDesCtxCsm::TCp* aCsm) override;
	virtual bool bindCtx(const string& aCtxId, MVert* aCtx) override;
	virtual bool unbindCtx(const string& aCtxId) override;
	virtual MDesCtxSpl::TCp* splCp() override { return &mSplCp; }
    protected:
	TSplCp mSplCp;  /*!< Ctx supplier CP */
};


/** @brief DES context consumer
 * */
// TODO is DES inheritance reasonable?
class DesCtxCsm : public Des, public MDesCtxCsm
{
    public:
	using TCsmCp = NCpOnp<MDesCtxCsm, MDesCtxSpl>;
    public:
	static const char* Type() { return "DesCtxCsm";};
	DesCtxCsm(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
#ifdef SELF_IFR
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
#endif
	// From MDesCtxCsm
	virtual string MDesCtxCsm_Uid() const override {return getUid<MDesCtxCsm>();}
	virtual void MDesCtxCsm_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual string getCsmId() const override;
	virtual void onCtxAdded(const string& aCtxId) override;
	virtual void onCtxRemoved(const string& aCtxId) override;
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
    protected:
	bool init();
	// TODO not used, to delete?
	bool registerSpl(MDesCtxSpl::TCp* aSpl);
	bool bindCtxs();
    protected:
	bool mInitialized;
	bool mInitFailed;
	TCsmCp mCsmCp;  /*!< Consumer Cp */
};

/** @brief DES Input demultiplexor, ref ds_des_idmux
 * */
class DesInpDemux : public Des
{
    public:
	static const char* Type() { return "DesInpDemux";};
	DesInpDemux(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MDesSyncable
	virtual void confirm() override;
    protected:
	void AddInput(const string& aName);
	void AddOutput(const string& aName);
	int getIfcCount();
    protected:
	int mIfcCount; //!< Ifaces count
	int mIdx; //!< Index of input iface
};

#endif
