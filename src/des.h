
#ifndef __FAP3_DES_H
#define __FAP3_DES_H

#include "mdata.h"
#include "mdes.h"
#include "mlauncher.h"
#include "vert.h"
#include "syst.h"
#include "content.h"

class BdVar;

/** @brief Connection point - input of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateInp: public ConnPointu
{
    public:
	static const char* Type() { return "CpStateInp";};
	CpStateInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief Connection point - output of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateOutp: public ConnPointu
{
    public:
	static const char* Type() { return "CpStateOutp";};
	CpStateOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
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
	    SCont(State& aHost, const string& aName) : mName(aName), mHost(aHost) {}
	    // From MContent
	    virtual string MContent_Uid() const override { return mHost.getCntUid(mName, MContent::Type());}
	    virtual MIface* MContent_getLif(const char *aType) override { return nullptr;}
	    virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	    virtual string contName() const override { return mName;}
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
	bool IsLogeventUpdate();
	// From MNode
	virtual MIface* MOwned_getLif(const char *aType) override;
	// Local
	void setUpdated();
	void setActivated();
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
	virtual MIface* MOwned_getLif(const char *aType);
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override {}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void update() override;
	virtual void confirm() override;
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
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override {} // TODO implement
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
	virtual bool Run(int aCount = 0) override;
	virtual bool Stop() override;
	// Local
	virtual void OnIdle();
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



#endif
