
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
	CpStateInp(const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief Connection point - output of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateOutp: public ConnPointu
{
    public:
	static const char* Type() { return "CpStateOutp";};
	CpStateOutp(const string& aName = string(), MEnv* aEnv = NULL);
};

#if 0
class CpStateOutp: public Vertu, public MConnPoint
{
    public:
	static const char* Type() { return "CpStateOutp";};
	CpStateOutp(const string& aName = string(), MEnv* aEnv = NULL);
	// From MVert
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const {return getUid<MConnPoint>();}
	virtual string provName() const override;
	virtual string reqName() const override;
};
#endif


/** @brief State, non-inhritable, monolitic, using host unit base organs, combined chain
 *
 * Ref ds_uac for unit based orgars, ds_mae for monolitic agents, ds_mae_scc for combined chain design
 * TODO It is not obvious solution to expose MDVarSet iface, but I don't find more proper solution ATM. To redesign?
 * */
class State: public Vertu, public MConnPoint, public MDesSyncable, public MDesInpObserver, public MBdVarHost, public MDVarSet,
      public Cnt::Host
{
    public:
	static const char* Type() { return "State";};
	State(const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node
	virtual MIface* doMOwnerGetLif(const char *aType) override;
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
	// From Unit.MIfProvOwner
	virtual bool resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
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
    protected:
	/** @brief Notifies dependencies of input updated */
	void NotifyInpsUpdated();
	bool IsLogeventUpdate();
	// From MNode
	virtual MIface* MOwned_getLif(const char *aType) override;
	// Local
	void setUpdated();
	void setActivated();
    private:
	BdVar* mPdata;   //<! Preparing (updating) phase data
	BdVar* mCdata;   //<! Confirming phase data
	static const string KCont_Value;
	Cnt mValue = Cnt(*this, KCont_Value);
};


/** @brief DES system
 * Intended to be embedded in systems
* */
class Des: public Syst, public MDesSyncable, public MDesObserver
{
    public:
	static const char* Type() { return "Des";};
	Des(const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node
	virtual MIface* doMOwnerGetLif(const char *aType) override;
	virtual void onOwnedAttached(MOwned* aOwned) override;
	virtual MIface* MOwned_getLif(const char *aType);
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override {}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void update() override;
	virtual void confirm() override;
	// From MDesObserver
	virtual string MDesObserver_Uid() const override {return getUid<MDesObserver>();}
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
    protected:
	list<MDesSyncable*> mActive;     /*!< Active compoments */
	list<MDesSyncable*> mUpdated;     /*!< Updated compoments */
};



/** @brief Launcher of DES
 * Runs DES syncable owned
 * */
class DesLauncher: public Des, public MLauncher
{
    public:
	static const char* Type() { return "DesLauncher";};
	DesLauncher(const string& aName = string(), MEnv* aEnv = NULL);
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



#endif
