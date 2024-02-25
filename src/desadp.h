
#ifndef __FAP3_DESADP_H
#define __FAP3_DESADP_H

#include <functional> 

#include "mdes.h"
#include "mdadp.h"
#include "mdata.h"
#include "msyst.h"
#include "mlink.h"
#include "unit.h"
#include "des.h"
#include "rdatauri.h"

/** @brief DES adapter base
 * Internal "access points" are used to create required topology instead of
 * using "true" DES with transitions. This is for optimization purpose.
 * */
// TODO There is design weakness here. The relation ADP - managed_agent is implicit and one way only. So removing managed agent
// will cause wrond ADB behaviour.
// TODO obsolete, to be replaced by composite adapter
class AAdp: public Unit, public MDesSyncable, public MDesObserver, public MDesInpObserver, public MAgent, public MDVarGet, public MObserver
{
    public:
	using TAgtCp = NCpOnp<MAgent, MAhost>;  /*!< Agent conn point */
	using TObserverCp = NCpOmnp<MObserver, MObservable>;
    public:
	/** @brief Input access point
	 * */
	class AdpIap: public MDesInpObserver {
	    public:
		using THandler = std::function<void()>;
		THandler mHandler;
	    public:
		AdpIap(MNode& aHost, THandler aHandler): mHost(aHost), mHandler(aHandler){}
		// From MDesInpObserver
		virtual void onInpUpdated() override { mHandler();}
		virtual string MDesInpObserver_Uid() const {return MDesInpObserver::Type();}
		virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	    protected:
		MNode& mHost;
	};

	/** @brief Parameter access point, using Sdata
	 * Acts as a binder of MDVarGet request to method of host
	 * */
	template <typename T> class AdpPap: public MDVarGet {
	    public:
		using THandler = std::function<const DtBase* ()>;
	    public:
		THandler mHandler;
	    public:
		AdpPap(MNode& aHost, THandler aHandler): mHost(aHost), mHandler(aHandler){}
		// From MDVarGet
		virtual string MDVarGet_Uid() const override {return MDVarGet::Type();}
		virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
		virtual string VarGetIfid() const override {return Sdata<T>::TypeSig();}
		virtual const DtBase* VDtGet(const string& aType) { return mHandler();}
	    protected:
		MNode& mHost;
	};
    public:
	/** @brief Managed agent parameter access point, using generic data
	 * Acts as a binder of MDVarGet request to method of host
	 * */
	template <typename T> class AdpPapB: public MDVarGet {
	    public:
		using THandler = std::function<const DtBase* ()>;
	    public:
		THandler mHandler;
	    public:
		AdpPapB(const AdpPapB& aSrc): mHandler(aSrc.mHandler) {}
		AdpPapB(THandler aHandler): mHandler(aHandler){}
		// From MDVarGet
		virtual string MDVarGet_Uid() const {return MDVarGet::Type();}
		virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
		virtual string VarGetIfid() const override {return T::TypeSig();}
		virtual const DtBase* VDtGet(const string& aType) { return mHandler();}
	};

	/** @brief Managed agent observer
	 * */
	// TODO TObserverCp is changed to multi-point CP, so no need to dedicated observer, to redesign 
	template <class T>
	    class AdpMagObs : public MObserver {
		public:
		    AdpMagObs(T* aHost): mHost(aHost), mOcp(this) {}
		    virtual ~AdpMagObs() { }
		    // From MObserver
		    virtual string MObserver_Uid() const {return MObserver::Type();}
		    virtual MIface* MObserver_getLif(const char *aName) override { return nullptr;}
		    virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override {
			mHost->onMagOwnedAttached(aObl, aOwned);
		    }
		    virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override {
			mHost->onMagOwnedDetached(aObl, aOwned);
		    }
		    virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override {
			mHost->onMagContentChanged(aObl, aCont);
		    }
		    virtual void onObsChanged(MObservable* aObl) override {
			mHost->onMagChanged(aObl);
		    }
		public:
		    TObserverCp mOcp;               /*!< Observer connpoint */
		private:
		    T* mHost;
	    };

    public:
	static const char* Type() { return "AAdp";};
	AAdp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~AAdp();
	// From Base
	virtual MIface* MNode_getLif(const char *aName) override;
	// From MUnit
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MAgent
	virtual string MAgent_Uid() const override {return getUid<MAgent>();}
	virtual MIface* MAgent_getLif(const char *aName) override;
	// From MDVarGet
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
	virtual string VarGetIfid() const override {return string();}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
	virtual void update() override;
	virtual void confirm() override;
	virtual int countOfActive(bool aLocal = false) const override { return 1;}
	// From MDesObserver
	virtual string MDesObserver_Uid() const override {return getUid<MDesObserver>();}
	virtual void MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
	// From MObserver
	virtual string MObserver_Uid() const {return getUid<MObserver>();}
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override;
	virtual void onObsChanged(MObservable* aObl) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const {return getUid<MDVarGet>();}
	// From MObserver
	virtual MIface* MObserver_getLif(const char *aType) override;
	// From Node.MOwned
	virtual void onOwnerAttached() override;
    protected:
	virtual void onMagOwnedAttached(MObservable* aObl, MOwned* aOwned) {}
	virtual void onMagOwnedDetached(MObservable* aObl, MOwned* aOwned) {}
	virtual void onMagContentChanged(MObservable* aObl, const MContent* aCont) {}
	virtual void onMagChanged(MObservable* aObl) {}
	// Local
	virtual void OnMagUpdated() {}
	void NotifyInpUpdated(MNode* aCp);
	bool UpdateMagOwner(MNode* aMagOwner);
	bool UpdateMag(MNode* aMag);
	bool UpdateMag(const string& aMagUri);
	void UpdateMag();
	bool ApplyMagBase();
    protected:
	/** @brief Notifies all states inputs of update **/
	virtual void NotifyInpsUpdated() {}
	// Local
	void OnInpMagUri();
	void OnInpMagBase();
	MNode* ahostGetNode(const GUri& aUri);
	MAhost* aHost();
	void setUpdated();
	void setActivated();
	MDesObserver* getDesObs();
	MNode* ahostNode();
	const DtBase* GetMagUri();
    protected:
	bool mInpMagUriUpdated = true;
	bool mInpMagBaseUpdated = true;
	MNode* mMagOwner; /*!< Owner of managed agents, ref DS_SN_AUL_LNK_SMA */
	MNode* mMag; /*!< Managed agent */
	Sdata<string> mMagUri; /*!< Managed agent URI */
	AdpIap mIapMagUri = AdpIap(*this, [this]() {OnInpMagUri();}); /*!< MAG URI input access point */
	AdpIap mIapMagBase = AdpIap(*this, [this]() {OnInpMagBase();}); /*!< Input access point: Managed agent base */
	AdpPap<string> mPapMagUri = AdpPap<string>(*this, [this]() -> const DtBase* { return GetMagUri();}); /*!< Mag URI access point */
	TObserverCp mObrCp;               /*!< AHost Observer connpoint */
	AdpMagObs<AAdp> mMagObs = AdpMagObs<AAdp>(this); /*!< Managed agent observer */
	TAgtCp mAgtCp;                   /*!< Agent connpoint */
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
	bool mSelfAsBase;  //<! Sign of that host is acts as MAG base
};



/** @brief MNode iface ADP agent
 * */
class AMnodeAdp : public AAdp
{
    public:
	using TCmpNames = Vector<string>;
    public:
	static const char* Type() { return "AMnodeAdp";};
	AMnodeAdp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDesSyncable
	virtual void confirm() override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    protected:
	const DtBase* GetCompsCount();
	const DtBase* GetCompNames();
	const DtBase* GetOwner();
	const DtBase* GetName();
	// From AAdp
	virtual void OnMagUpdated() override;
	// From MDesSyncable
	virtual void update() override;
	// Local
	void OnInpMut();
	void ApplyMut();
    protected:
	// Comps count param adapter. Even if the count can be get via comp names vector we support separate param for convenience
	AdpPap<int> mApCmpCount = AdpPap<int>(*this, [this]() -> const DtBase* { return GetCompsCount();}); /*!< Comps count access point */
	AdpPapB<TCmpNames> mApCmpNames = AdpPapB<TCmpNames>([this]() -> const DtBase* { return GetCompNames();}); /*!< Comp names access point */
	AdpPap<string> mPapOwner = AdpPap<string>(*this, [this]() -> const DtBase* { return GetOwner();}); /*!< Comps count access point */
	AdpPap<string> mPapName = AdpPap<string>(*this, [this]() -> const DtBase* { return GetName();}); /*!< Name access point */
	AdpIap mIapInpMut = AdpIap(*this, [this]() {OnInpMut();}); /*!< Mut Add Widget input access point */
	// From AAdp
	virtual void NotifyInpsUpdated() override;
	virtual void onMagOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
    protected:
	TCmpNames mCompNames;
	Sdata<int> mCompsCount;
	Sdata<string> mName;
	Sdata<string> mOwner;
	bool mCompNamesUpdated = true;
	bool mOwnerUpdated = true;
	bool mNameUpdated = true;
	bool mInpMutChanged = true;
};

/** @brief MElem iface ADP agent
 * */
class AMelemAdp : public AAdp
{
    public:
	static const char* Type() { return "AMelemAdp";};
	AMelemAdp(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    protected:
	void ApplyMut();
	void OnInpMut();
	// From MDesSyncable
	virtual void update() override;
    protected:
	AdpIap mIapInpMut = AdpIap(*this, [this]() {OnInpMut();}); /*!< Mut Add Widget input access point */
    protected:
	MChromo* mMagChromo; /*<! Managed agent chromo */
	bool mInpMutChanged = true;
};


/** @brief Composite DES adapter, ref ds_dcs_cda
 * It is the initial part of the approach opposite to AAdp
 * AAdp is a "monolitic" adapter obtaining link to managed node 
 * and providing it's control and observation
 * Dadp just provides its components with the link to managed or
 * observed node. All observing and control is performing by
 * Dadp components.
 * */
// TODO To intro Dese - DES with embedded elems, base DAdp on it
class DAdp : public Des, public IDesEmbHost
{
    public:
	// TODO intro "hard link" instead of MLink (w/o conn, disconn)
	class MagLink : public MLink {
	    public:
		MagLink(DAdp* aHost): mHost(aHost) {}
		// From MLink
		virtual string MLink_Uid() const { return mHost->getUid<MLink>();}
		virtual MIface* MLink_getLif(const char *aType) override;
		virtual void MLink_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		virtual bool connect(MNode* aPair) override { return false;}
		virtual bool disconnect(MNode* aPair) override { return false;}
		virtual MNode* pair() override { return mHost->mMag; }
	    private:
		DAdp* mHost;
	};
	/** @brief Managed agent observer
	 * */
	class MagObs : public MObserver {
	    using TObserverCp = NCpOmnp<MObserver, MObservable>;
	    public:
		MagObs(DAdp* aHost): mHost(aHost), mOcp(this) {}
		// From MObserver
		virtual string MObserver_Uid() const {return MObserver::Type();}
		virtual MIface* MObserver_getLif(const char *aName) override { return nullptr;}
		virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override { }
		virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override { }
		virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override { }
		virtual void onObsChanged(MObservable* aObl) override { }
	    public:
		TObserverCp mOcp;               /*!< Observer connpoint */
	    private:
		DAdp* mHost;
	};
    public:
	static const char* Type() { return "DAdp";};
	DAdp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    public:
	// From MNode
	virtual MIface* MNode_getLif(const char *aName) override;
	// From IDesEmbHost
	virtual void registerIb(DesEIbb* aIap) override;
	virtual void registerOst(DesEOstb* aItem) override;
	virtual void logEmb(int aCtg, const TLog& aRec) override { Log(aCtg, aRec);}
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
	// From MDesAdapter
	virtual string MDesAdapter_Uid() const override { return getUid<MDesAdapter>(); }
	virtual void MDesAdapter_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MNode* getMag() override;
	// From MOwner
	virtual MIface* MOwner_getLif(const char *aType) override;
    protected:
	// Local transitions
	bool UpdateMagBase();
	void UpdateMag();
	// Utils
	bool rifDesIobs(DesEIbb& aIap, MIfReq::TIfReqCp* aReq);
	bool rifDesOsts(DesEOstb& aItem, MIfReq::TIfReqCp* aReq);
    protected:
	vector<DesEIbb*> mIbs; /*!< Inputs buffered registry */
	vector<DesEOstb*> mOsts; /*!< Output states buffered registry */
	DesEIbd<DGuri> mIbMagUri;   //!< Buffered input "MagUri"
	DesEIbMnode mIbMagBase;   //!< Buffered input "Mag base"
	DesEOsts<string> mOstMagUri;   //!< Output state "Mag Uri"
	MNode* mMagBase;
	MNode* mMag;
	MagObs mMagObs;             /*!< MAG observer */
};

#endif
