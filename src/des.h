
#ifndef __FAP3_DES_H
#define __FAP3_DES_H

#include "mlog.h"
#include "mdata.h"
#include "mdes.h"
#include "mlauncher.h"
#include "vert.h"
#include "syst.h"
#include "content.h"

class BdVar;

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

/** @brief Connection point - input of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateInp: public CpState
{
    public:
	static const char* Type() { return "CpStateInp";};
	CpStateInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief Connection point - output of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateOutp: public CpState
{
    public:
	static const char* Type() { return "CpStateOutp";};
	CpStateOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief Connection point - input of state required MNode iface
 * Just ConnPointu with pre-configured prepared/required
 * */
class CpStateMnodeInp: public CpState
{
    public:
	static const char* Type() { return "CpStateMnodeInp";};
	CpStateMnodeInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
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



/** @brief State, non-inhritable, monolitic, using host unit base organs, combined chain
 *
 * Ref ds_uac for unit based orgars, ds_mae for monolitic agents, ds_mae_scc for combined chain design
 * TODO It is not obvious solution to expose MDVarSet iface, but I don't find more proper solution ATM. To redesign?
 * */
class State: public Vertu, public MConnPoint, public MDesSyncable, public MDesInpObserver, public MBdVarHost, public MDVarSet,
      public Cnt::Host
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
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node
	virtual MIface* MOwner_getLif(const char *aType) override;
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MVert
	virtual MIface *MVert_getLif(const char *aType) override;
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual TDir getDir() const override { return EOut;}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const {return getUid<MDesInpObserver>();}
	virtual string provName() const override;
	virtual string reqName() const override;
	// From MBdVarHost
	virtual MDVarGet* HGetInp(const void* aRmt) override;
	virtual void HOnDataChanged(const void* aRmt) override;
	virtual string GetDvarUid(const MDVar* aComp) const;
	// From MDVarSet
	virtual string MDVarSet_Uid() const override {return getUid<MDVarSet>();}
	virtual string VarGetSIfid();
	virtual MIface *DoGetSDObj(const char *aName) override;
	// From Cnt.Host
	virtual string getCntUid(const string& aName, const string& aIfName) const override { return getUid(aName, aIfName);}
	virtual MContentOwner* cntOwner() override { return this;}
	// From Node.MContentOwner
	virtual int contCount() const override { return 1 + Node::contCount();}
	virtual MContent* getCont(int aIdx) override;
	virtual const MContent* getCont(int aIdx) const override;
	virtual bool getContent(const GUri& aCuri, string& aRes) const override;
	virtual bool setContent(const GUri& aCuri, const string& aData) override;
    public:
	static const string KCont_Value;
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
    protected:
	BdVar* mPdata;   //<! Preparing (updating) phase data
	BdVar* mCdata;   //<! Confirming phase data
	SContValue mValue = SContValue(*this, KCont_Value);
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
};


/** @brief DES system
* */
class Des: public Syst, public MDesSyncable, public MDesObserver
{
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
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
	// From MDesObserver
	virtual string MDesObserver_Uid() const override {return getUid<MDesObserver>();}
	virtual void MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
    protected:
	list<MDesSyncable*> mActive;     /*!< Active compoments */
	list<MDesSyncable*> mUpdated;     /*!< Updated compoments */
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
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
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override {}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
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
	list<MDesSyncable*> mActive;     /*!< Active compoments */
	list<MDesSyncable*> mUpdated;    /*!< Updated compoments */
	TObserverCp mOrCp;               /*!< Observer connpoint */ 
	TAgtCp mAgtCp;                   /*!< Agent connpoint */ 
	bool mUpdNotified;               //<! Sign of that State notified observers on Update
	bool mActNotified;               //<! Sign of that State notified observers on Activation
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



// Helpers
template <typename T> bool GetSData(MNode* aDvget, T& aData)
{
    bool res = false;
    MUnit* vgetu = aDvget->lIf(vgetu);
    MDVarGet* vget = vgetu ? vgetu->getSif(vget) : nullptr;
    if (vget) {
	MDtGet<Sdata<T>>* gsd = vget->GetDObj(gsd);
	if (gsd) {
	    Sdata<T> st;
	    gsd->DtGet(st);
	    aData = st.mData;
	    res = true;
	}
    }
    return res;
}

template <typename T> bool GetGData(MNode* aDvget, T& aData)
{
    bool res = false;
    MUnit* vgetu = aDvget->lIf(vgetu);
    MDVarGet* vget = vgetu->getSif(vget);
    if (vget) {
	MDtGet<T>* gsd = vget->GetDObj(gsd);
	if (gsd) {
	    gsd->DtGet(aData);
	    res = true;
	}
    }
    return res;
}


#if 0
// Embedded DES elements support

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
	DesEIbb(const string& aInpUri): mUri(aInpUri), mUpdated(false), mActivated(true), mChanged(false) {}
	string getUri() const { return mUri;}
	// From MDesInpObserver
	virtual void onInpUpdated() override { setActivated();}
	virtual string MDesInpObserver_Uid() const override {return MDesInpObserver::Type();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return MDesSyncable::Type();} 
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
    public:
	template <typename S> string toStr(const S& aData) { return to_string(aData); }
	string toStr(const string& aData) { return aData; }
    public:
	string mUri;  /*!< Input URI */
	bool mActivated; /*!< Indication of data is active (to be updated) */
	bool mUpdated; /*!< Indication of data is updated */
	bool mChanged; /*!< Indication of data is changed */
};

/** @brief Input buffered hosted
 * @param  Th  host type 
 * */
template <typename Th>
class DesEIbh: public DesEIbb
{
    public:
	DesEIbh(Th* aHost, const string& aInpUri): DesEIbb(aInpUri), mHost(aHost) { mHost->registerIb(this);}
	// From MDesSyncable
	virtual void setUpdated() override { mUpdated = true; mHost->setUpdated();}
	virtual void setActivated() override { mActivated = true; mHost->setUpdated();}
    public:
	Th* mHost;
};

#if 0
/** @brief Input state Sdata based
 * @param  T  Sdata type 
 * */
template <typename Th, typename T>
class DesEIbs: public DesEIbb
{
    public:
	DesEIbs(Th* aHost, const string& aInpUri): DesEIbb(aInpUri), mHost(aHost) {}
	// From MDesSyncable
	virtual void setUpdated() override { mUpdated = true; mHost->setUpdated();}
	virtual void setActivated() override { mActivated = true; mHost->setUpdated();}
	virtual void update() override;
	virtual void confirm() override;
	// Local
	T& data() {return mCdt;}
    public:
	Th* mHost;
	T mUdt;  /*!< Updated data */
	T mCdt;  /*!< Confirmed data */
};
#endif

/** @brief Input buffered Sdata based
 * @param  Th  host type 
 * @param  T  Sdata type 
 * */
template <typename Th, typename T>
class DesEIbs: public DesEIbh<Th>
{
    public:
	using TP = DesEIbh<Th>;
    public:
	DesEIbs(Th* aHost, const string& aInpUri): TP(aHost, aInpUri) {}
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
	// Local
	T& data() {return mCdt;}
    public:
	T mUdt;  /*!< Updated data */
	T mCdt;  /*!< Confirmed data */
};

template <typename Th, typename T>
void DesEIbs<Th, T>::update()
{
    bool res = false;
    MNode* inp = TP::mHost->getNode(TP::mUri);
    if (inp) res = GetSData(inp, mUdt);
    if (!res)  TP::mHost->Log(TLog(TLogRecCtg::EDbg, TP::mHost) + "Cannot get input [" + TP::mUri + "]");
    else { TP::mActivated = false; TP::setUpdated(); }
}

template <typename Th, typename T>
void DesEIbs<Th, T>::confirm()
{
    if (mUdt != mCdt) {
	TP::mHost->Log(TLog(TLogRecCtg::EDbg, TP::mHost) + "[" + TP::mUri + "] Updated: [" + toStr(mUdt) + "] -> [" + toStr(mCdt) + "]");
	mCdt = mUdt; TP::mChanged = true;
    } else TP::mChanged = false;
    TP::mUpdated = false;
}

#endif

class DesEIbb;
class DesEOstb;

/** @brief Local iface of the host of DES embedded elements
 * */
class IDesEmbHost
{
    public:
	virtual void registerIb(DesEIbb* aIap) = 0;
	virtual void registerOst(DesEOstb* aOst) = 0;
	virtual void logEmb(const TLog& aRec) =0;
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
	virtual string MDesInpObserver_Uid() const override {return MDesInpObserver::Type();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return MDesSyncable::Type();} 
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void update() override { mChanged = false;}
	virtual void setUpdated() override { mUpdated = true; sHost()->setUpdated();}
	virtual void setActivated() override { mActivated = true; sHost()->setActivated();}
    protected:
	template <typename S> string toStr(const S& aData) { return to_string(aData); }
	string toStr(const string& aData) { return aData; }
	string toStr(MNode*& aData) { return (aData ? aData->Uid() : "nil"); }
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
	eHost()->logEmb(TLog(TLogRecCtg::EDbg, mHost) + "[" + mUri + "] Updated: [" + toStr(mCdt) + "] -> [" + toStr(mUdt) + "]");
	mCdt = mUdt; mChanged = true;
    } else mChanged = false;
    mUpdated = false;
}

/** @brief Input buffered Sdata based
 * @param  T  data type 
 * */
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
    if (!TP::mValid) this->eHost()->logEmb(TLog(TLogRecCtg::EDbg, TP::mHost) + "Cannot get input [" + this->mUri + "]");
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
	    mHost(aHost), mCpUri(aCpUri), mCpType(aCpType), mValid(false) { eHost()->registerOst(this);}
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
	bool mValid;  /*!< Indication of data validity */
};


/** @brief Output state with Sdata
 * */
template <typename T> class DesEOsts: public DesEOstb, public MDtGet<Sdata<T>> {
    public:
	DesEOsts(MNode* aHost, const string& aCpUri): DesEOstb(aHost, aCpUri) {}
	// From MDVarGet
	virtual string VarGetIfid() const override {return MDtGet<Sdata<T>>::Type();}
	virtual MIface* DoGetDObj(const char *aName) override;
	// From MDtGet
	virtual string MDtGet_Uid() const {return MDtGet<Sdata<T>>::Type();}
	virtual void DtGet(Sdata<T>& aData) override { aData.mData = mData; aData.mValid = true;}
	// Local
	void updateData(const T& aData);
    public:
	T mData;
};

template <typename T>
MIface* DesEOsts<T>::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    string tt = MDtGet<Sdata<T> >::Type();
    if (tt.compare(aName) == 0) {
	res = dynamic_cast<MDtGet<Sdata<T> >*>(this);
    }
    return res;
}

template <typename T>
void DesEOsts<T>::updateData(const T& aData)
{
    if (aData != mData) {
	mData = aData;
	NotifyInpsUpdated();
    }
}




#endif
