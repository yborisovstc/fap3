
#ifndef __FAP3_DESADP_H
#define __FAP3_DESADP_H

#include <functional> 

#include "mdes.h"
#include "mdata.h"
#include "msyst.h"
#include "unit.h"

/** @brief DES adapter base
 * Internal "access points" are used to create required topology instead of
 * using "true" DES with transitions. This is for optimization purpose.
 * */
// TODO There is design weakness here. The relation ADP - managed_agent is implicit and one way only. So removing managed agent
// will cause wrond ADB behaviour.
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
		virtual void onInpUpdated() override;
		virtual string MDesInpObserver_Uid() const {return MDesInpObserver::Type();}
		virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	    protected:
		MNode& mHost;
	};

	/** @brief Parameter access point, using Sdata
	 * Acts as a binder of MDVarGet request to method of host
	 * */
	template <typename T> class AdpPap: public MDVarGet, public MDtGet<Sdata<T>> {
	    public:
		template<typename P> using THandler = std::function<void (Sdata<P>&)>;
	    public:
		THandler<T> mHandler;
	    public:
		AdpPap(MNode& aHost, THandler<T> aHandler): mHost(aHost), mHandler(aHandler){}
		// From MDVarGet
		virtual string MDVarGet_Uid() const {return MDVarGet::Type();}
		virtual MIface* DoGetDObj(const char *aName) override;
		virtual string VarGetIfid() const override {return MDtGet<Sdata<T>>::Type();}
		// From MDtGet
		virtual string MDtGet_Uid() const {return MDtGet<Sdata<T>>::Type();}
		virtual void DtGet(Sdata<T>& aData) override { mHandler(aData);}
	    protected:
		MNode& mHost;
	};
    public:
	/** @brief Managed agent parameter access point, using generic data
	 * Acts as a binder of MDVarGet request to method of host
	 * */
	template <typename T> class AdpPapB: public MDVarGet, public MDtGet<T> {
	    public:
		template<typename P> using THandler = std::function<void (P&)>;
	    public:
		THandler<T> mHandler;
	    public:
		AdpPapB(const AdpPapB& aSrc): mHandler(aSrc.mHandler) {}
		AdpPapB(THandler<T> aHandler): mHandler(aHandler){}
		// From MDVarGet
		virtual string MDVarGet_Uid() const {return MDVarGet::Type();}
		virtual MIface* DoGetDObj(const char *aName) override;
		virtual string VarGetIfid() const override {return string();}
		// From MDtGet
		virtual void DtGet(T& aData) override { mHandler(aData);}
	};

	/** @brief Managed agent observer
	 * */
	// TODO TObserverCp is changed to multi-point CP, so no need to dedicated observer, to redesign 
	template <class T>
	    class AdpMagObs : public MObserver {
		public:
		    AdpMagObs(T* aHost): mHost(aHost), mOcp(this) {}
		    // From MObserver
		    virtual string MObserver_Uid() const {return MObserver::Type();}
		    virtual MIface* MObserver_getLif(const char *aName) override { return nullptr;}
		    virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override {
			mHost->onMagOwnedAttached(aObl, aOwned);
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
	virtual bool resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MAgent
	virtual string MAgent_Uid() const override {return getUid<MAgent>();}
	virtual MIface* MAgent_getLif(const char *aName) override;
	// From MDVarGet
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
	virtual string VarGetIfid() const override {return string();}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void update() override;
	virtual void confirm() override;
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
	virtual void onMagContentChanged(MObservable* aObl, const MContent* aCont) {}
	virtual void onMagChanged(MObservable* aObl) {}
	// Local
	virtual void OnMagUpdated() {}
	void NotifyInpUpdated(MNode* aCp);
	bool UpdateMagOwner(MNode* aMagOwner);
	bool UpdateMag(MNode* aMag);
	bool UpdateMag(const string& aMagUri);
	void UpdateMag();
    protected:
	/** @brief Notifies all states inputs of update **/
	virtual void NotifyInpsUpdated() {}
	// Local
	void OnInpMagUri();
	MNode* ahostGetNode(const GUri& aUri);
	MAhost* aHost();
	void setUpdated();
	void setActivated();
	MDesObserver* getDesObs();
	MNode* ahostNode();
    protected:
	bool mInpMagUriUpdated = true;
	MNode* mMagOwner; /*!< Owner of managed agents, ref DS_SN_AUL_LNK_SMA */
	MNode* mMag; /*!< Managed agent */
	string mMagUri; /*!< Managed agent URI */
	bool mMagUriValid; /*!< Validity sign of mMagUri */
	AdpIap mIapMagUri = AdpIap(*this, [this]() {OnInpMagUri();}); /*!< MAG URI input access point */
	TObserverCp mObrCp;               /*!< AHost Observer connpoint */
	AdpMagObs<AAdp> mMagObs = AdpMagObs<AAdp>(this); /*!< Managed agent observer */
	TAgtCp mAgtCp;                   /*!< Agent connpoint */
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
};

// Access point, using Sdata

template <typename T> MIface* AAdp::AdpPap<T>::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    string tt = MDtGet<Sdata<T> >::Type();
    if (tt.compare(aName) == 0) {
	res = dynamic_cast<MDtGet<Sdata<T> >*>(this);
    }
    return res;
}

// Access point, using generic data

template <typename T> MIface* AAdp::AdpPapB<T>::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    string tt = MDtGet<T>::Type();
    if (tt.compare(aName) == 0) {
	res = dynamic_cast<MDtGet<T>*>(this);
    }
    return res;
}



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
	virtual bool resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    protected:
	void GetCompsCount(Sdata<int>& aData);
	void GetCompNames(TCmpNames& aData);
	void GetOwner(Sdata<string>& aData);
	// From AAdp
	virtual void OnMagUpdated() override;
    protected:
	// Comps count param adapter. Even if the count can be get via comp names vector we support separate param for convenience
	AdpPap<int> mApCmpCount = AdpPap<int>(*this, [this](Sdata<int>& aData) {GetCompsCount(aData);}); /*!< Comps count access point */
	AdpPapB<TCmpNames> mApCmpNames = AdpPapB<TCmpNames>([this](TCmpNames& aData) {GetCompNames(aData);}); /*!< Comp names access point */
	// TODO Do we need owner?
	AdpPap<string> mPapOwner = AdpPap<string>(*this, [this](Sdata<string>& aData) {GetOwner(aData);}); /*!< Comps count access point */
	// From AAdp
	virtual void NotifyInpsUpdated() override;
	virtual void onMagOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
    protected:
	TCmpNames mCompNames;
	bool mCompNamesUpdated = true;
	bool mOwnerUpdated = true;
};

/** @brief MElem iface ADP agent
 * */
class AMelemAdp : public AAdp
{
    public:
	static const char* Type() { return "AMelemAdp";};
	AMelemAdp(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Unit.MIfProvOwner
	virtual bool resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
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




#endif
