
#ifndef __FAP3_DESSDC_H
#define __FAP3_DESSDC_H

/** @brief System structure DES controller (SDC)
 * */


#include "mdes.h"
#include "mdata.h"
#include "msyst.h"
#include "unit.h"
#include "desadp.h"

/** @brief SDC base agent
 * */
class ASdc : public Unit, public MDesSyncable, public MDesObserver, public MDesInpObserver, public MAgent, public MDVarGet, public MObserver
{
    public:
	using TAgtCp = NCpOnp<MAgent, MAhost>;  /*!< Agent conn point */
	using TObserverCp = NCpOmnp<MObserver, MObservable>;
    public:
 	/** @brief Input access point base
	 * */
	class SdcIapb: public MDesInpObserver, public MDesSyncable {
	    public:
		SdcIapb(ASdc* aHost, const string& aInpUri): mHost(aHost), mInpUri(aInpUri), mUpdated(false), mActivated(false), mChanged(false) {}
		// From MDesInpObserver
		virtual void onInpUpdated() override { setActivated();}
		virtual string MDesInpObserver_Uid() const override {return MDesInpObserver::Type();}
		virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		// From MDesSyncable
		virtual string MDesSyncable_Uid() const override {return MDesSyncable::Type();} 
		virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		virtual void setUpdated() override { mUpdated = true; mHost->setUpdated();}
		virtual void setActivated() override { mActivated = true; mHost->setActivated();}
	    public:
		ASdc* mHost;
		string mInpUri;  /*!< Input URI */
		bool mActivated; /*!< Indication of data is active (to be updated) */
		bool mUpdated; /*!< Indication of data is updated */
		bool mChanged; /*!< Indication of data is changed */
	};

 	/** @brief Input access point
	 * */
	template <class T>
	class SdcIap: public SdcIapb {
	    public:
		SdcIap(ASdc* aHost, const string& aInpUri): SdcIapb(aHost, aInpUri) {}
		// From MDesSyncable
		virtual void update() override;
		virtual void confirm() override;
	    public:
		T mUdt;  /*!< Updated data */
		T mCdt;  /*!< Confirmed data */
	};

	/** @brief Parameter access point base
	 * Acts as a binder of MDVarGet request to method of host
	 * */
	class SdcPapb: public MDVarGet {
	    public:
		SdcPapb(ASdc* aHost, const string& aCpUri): mHost(aHost), mCpUri(aCpUri) {}
		// From MDVarGet
		virtual string MDVarGet_Uid() const override {return MDVarGet::Type();}
	    public:
		void NotifyInpsUpdated();
	    public:
		ASdc* mHost;
		string mCpUri;  /*!< Output URI */
	};

	/** @brief Parameter access point, using Sdata
	 * */
	template <typename T> class SdcPap: public SdcPapb, public MDtGet<Sdata<T>> {
	    public:
		SdcPap(ASdc* aHost, const string& aCpUri): SdcPapb(aHost, aCpUri) {}
		// From MDVarGet
		virtual string VarGetIfid() const override {return MDtGet<Sdata<T>>::Type();}
		virtual MIface* DoGetDObj(const char *aName) override;
		// From MDtGet
		virtual string MDtGet_Uid() const {return MDtGet<Sdata<T>>::Type();}
		virtual void DtGet(Sdata<T>& aData) override { mHost->getOut(aData);}
	    public:
		T mData;
	};

   public:
	static const char* Type() { return "ASdc";};
	ASdc(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~ASdc();
	// From Base
	virtual MIface* MNode_getLif(const char *aName) override;
	// From MUnit
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MAgent
	virtual string MAgent_Uid() const override {return getUid<MAgent>();}
	virtual MIface* MAgent_getLif(const char *aName) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const {return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
	virtual string VarGetIfid() const override {return string();}
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
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
	virtual MIface* MObserver_getLif(const char *aType) override;
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override;
	virtual void onObsChanged(MObservable* aObl) override;
	// From Node.MOwned
	virtual void onOwnerAttached() override;
    protected:
	bool rifDesIobs(SdcIapb& aIap, MIfReq::TIfReqCp* aReq);
	template<typename T> bool GetInpSdata(const string aInpUri, T& aRes);
	MNode* ahostNode();
	void getOut(Sdata<bool>& aData);
	// Utilities
	void addInput(const string& aName);
	virtual void addOutput(const string& aName);
	// Local
	/** @brief Gets status of the query */
	virtual bool getState() {return false;}
    protected:
	TObserverCp mObrCp;               /*!< Observer connpoint */
	TAgtCp mAgtCp;                   /*!< Agent connpoint */
	MNode* mMag; /*!< Managed agent */
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
	ASdc::SdcIap<bool> mIapEnb; /*!< "Enable" input access point */
	ASdc::SdcPap<bool> mOapOut; /*!< Comps count access point */
	bool mInpEnbUpdated = true;
};

template <typename T> MIface* ASdc::SdcPap<T>::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    string tt = MDtGet<Sdata<T> >::Type();
    if (tt.compare(aName) == 0) {
	res = dynamic_cast<MDtGet<Sdata<T> >*>(this);
    }
    return res;
}



/** @brief SDC agent "Component"
 * */
class ASdcComp : public ASdc
{
    public:
	static const char* Type() { return "ASdcComp";};
	ASdcComp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~ASdcComp();
	// From ASdc
	virtual bool getState();
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    protected:
	bool addComp();
    protected:
	ASdc::SdcIap<string> mIapName; /*!< "Name" input access point */
	ASdc::SdcIap<string> mIapParent; /*!< "Parent" input access point */
};

/** @brief SDC agent "Connect"
 * */
class ASdcConn : public ASdc
{
    public:
	static const char* Type() { return "ASdcConn";};
	ASdcConn(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~ASdcConn();
	// From ASdc
	virtual bool getState();
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    protected:
	bool doCtl();
    protected:
	ASdc::SdcIap<string> mIapV1; /*!< "V1" input access point */
	ASdc::SdcIap<string> mIapV2; /*!< "V2" input access point */
};


#endif // __FAP3_DESSDC_H
