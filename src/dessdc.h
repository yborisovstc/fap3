
#ifndef __FAP3_DESSDC_H
#define __FAP3_DESSDC_H

/** @brief System structure DES controller (SDC)
 * */


#include "mdes.h"
#include "mdata.h"
#include "msyst.h"
#include "unit.h"
#include "desadp.h"
#include "rmutdata.h"

/** @brief SDC base agent
 * NOTE: In normal cases MDesObserver iface is not used, so we just keep in for the rare case
 * for instance if the agent includes DES or states.
 * To avoid the agent providing MDesObserver iface to agent host and interfering main DES agent
 * the request from agent host is ignoring in resolveIfc()
 * */
class ASdc : public Unit, public MDesSyncable, public MDesObserver, public MDesInpObserver, public MAgent, public MDVarGet, public MObserver
{
    public:
	using TAgtCp = NCpOnp<MAgent, MAhost>;  /*!< Agent conn point */
	using TObserverCp = NCpOmnp<MObserver, MObservable>;
    public:
 	/** @brief Mag access point base
	 * */
	class SdcMapb: public MDesSyncable {
	    public:
		SdcMapb(const string& aName, ASdc* aHost);
		virtual void onMagUpdated();
		// From MDesSyncable
		virtual string MDesSyncable_Uid() const override {return MDesSyncable::Type();} 
		virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		virtual void setUpdated() override { mUpdated = true; mHost->setUpdated();}
		virtual void setActivated() override { mActivated = true; mHost->setActivated();}
	    public:
		ASdc* mHost;
		string mName;    /*!< Iap name */
		bool mActivated; /*!< Indication of data is active (to be updated) */
		bool mUpdated; /*!< Indication of data is updated */
		bool mChanged; /*!< Indication of data is changed */
	};

	/** @brief Mag access point
	 * */
	template <typename T> class SdcMap: public SdcMapb {
	    public:
		template<typename P> using TGetData = std::function<void (P&)>;
	    public:
		SdcMap(const string& aName, ASdc* aHost, TGetData<T> aGetData): SdcMapb(aName, aHost), mGetData(aGetData) {}
		// From MDesSyncable
		virtual void update() override;
		virtual void confirm() override;
	    public:
		TGetData<T> mGetData;
		T mUdt;  /*!< Updated data */
		T mCdt;  /*!< Confirmed data */
	};



 	/** @brief Input access point base
	 * */
	class SdcIapb: public MDesInpObserver, public MDesSyncable {
	    public:
		SdcIapb(const string& aName, ASdc* aHost, const string& aInpUri);
		string getInpUri() const { return mInpUri;}
		// From MDesInpObserver
		virtual void onInpUpdated() override { setActivated(); mHost->onInpUpdated();}
		virtual string MDesInpObserver_Uid() const override {return MDesInpObserver::Type();}
		virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		// From MDesSyncable
		virtual string MDesSyncable_Uid() const override {return MDesSyncable::Type();} 
		virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		virtual void setUpdated() override { mUpdated = true; mHost->setUpdated();}
		virtual void setActivated() override { mActivated = true; /*mHost->setActivated(); mHost->notifyMaps();*/}
	    public:
		ASdc* mHost;
		string mName;    /*!< Iap name */
		string mInpUri;  /*!< Input URI */
		bool mActivated; /*!< Indication of data is active (to be updated) */
		bool mUpdated; /*!< Indication of data is updated */
		bool mChanged; /*!< Indication of data is changed */
	};

 	/** @brief Input access point operating with Sdata
	 * @param  T  Sdata type 
	 * */
	template <class T>
	class SdcIap: public SdcIapb {
	    public:
		SdcIap(const string& aName, ASdc* aHost, const string& aInpUri): SdcIapb(aName, aHost, aInpUri) {}
		// From MDesSyncable
		virtual void update() override;
		virtual void confirm() override;
		// Local
		T& data() {return mCdt;}
	    public:
		T mUdt;  /*!< Updated data */
		T mCdt;  /*!< Confirmed data */
	};

 	/** @brief Input access point operating with generic data
	 * @param  T  data type 
	 * */
	template <class T>
	class SdcIapg: public SdcIapb {
	    public:
		SdcIapg(const string& aName, ASdc* aHost, const string& aInpUri): SdcIapb(aName, aHost, aInpUri) {}
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
		SdcPapb(const string& aName, ASdc* aHost, const string& aCpUri);
		string getCpUri() const { return mCpUri;}
		// From MDVarGet
		virtual string MDVarGet_Uid() const override {return MDVarGet::Type();}
	    public:
		void NotifyInpsUpdated();
	    public:
		const string& mName;
		ASdc* mHost;
		string mCpUri;  /*!< Output URI */
	};

	/** @brief Parameter access point, using Sdata
	 * */
	template <typename T> class SdcPap: public SdcPapb, public MDtGet<Sdata<T>> {
	    public:
		template<typename P> using TGetData = std::function<void (Sdata<P>&)>;
	    public:
		SdcPap(const string& aName, ASdc* aHost, const string& aCpUri, TGetData<T> aGetData): SdcPapb(aName, aHost, aCpUri), mGetData(aGetData) {}
		// From MDVarGet
		virtual string VarGetIfid() const override {return MDtGet<Sdata<T>>::Type();}
		virtual MIface* DoGetDObj(const char *aName) override;
		// From MDtGet
		virtual string MDtGet_Uid() const {return MDtGet<Sdata<T>>::Type();}
		//virtual void DtGet(Sdata<T>& aData) override { mHost->getOut(aData);}
		virtual void DtGet(Sdata<T>& aData) override { mGetData(aData);}
	    public:
		TGetData<T> mGetData;
		T mData;
	};

	/** @brief Parameter access point, using cached Sdata
	 * */
	template <typename T> class SdcPapc: public SdcPapb, public MDtGet<Sdata<T>> {
	    public:
		SdcPapc(const string& aName, ASdc* aHost, const string& aCpUri): SdcPapb(aName, aHost, aCpUri) {}
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


	/** @brief Managed agent observer
	 * */
	class MagObs : public MObserver {
	    public:
		MagObs(ASdc* aHost): mHost(aHost), mOcp(this) {}
		virtual ~MagObs() { }
		// From MObserver
		virtual string MObserver_Uid() const {return MObserver::Type();}
		virtual MIface* MObserver_getLif(const char *aName) override { return nullptr;}
		virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override {
		    mHost->notifyMaps();
		}
		virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override {
		    mHost->notifyMaps();
		}
		virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override {
		    mHost->notifyMaps();
		}
		virtual void onObsChanged(MObservable* aObl) override {
		    mHost->notifyMaps();
		}
	    public:
		TObserverCp mOcp;               /*!< Observer connpoint */
	    private:
		ASdc* mHost;
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
    public:
	bool registerIap(SdcIapb* aIap);
	bool registerPap(SdcPapb* aPap);
	bool registerMap(SdcMapb* aMap);
    protected:
	bool rifDesIobs(SdcIapb& aIap, MIfReq::TIfReqCp* aReq);
	bool rifDesPaps(SdcPapb& aPap, MIfReq::TIfReqCp* aReq);
	template<typename T> bool GetInpSdata(const string aInpUri, T& aRes);
	template<typename T> bool GetInpData(const string aInpUri, T& aRes);
	MNode* ahostNode();
	void getOut(Sdata<bool>& aData);
	// Utilities
	void addInput(const string& aName);
	virtual void addOutput(const string& aName);
	// Local
	virtual bool doCtl() { return false;}
	/** @brief Gets status of the query */
	virtual bool getState() {return false;}
	void notifyMaps();
	/** @brief Calculate control conditions */
	virtual void getCcd(bool& aData) {}
    protected:
	vector<SdcIapb*> mIaps; /*!< Input adapters registry */
	vector<SdcPapb*> mPaps; /*!< Param adapters registry */
	vector<SdcMapb*> mMaps; /*!< Mag adapters registry */
	TObserverCp mObrCp;               /*!< Observer connpoint */
	TAgtCp mAgtCp;                   /*!< Agent connpoint */
	MNode* mMag; /*!< Managed agent */
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
	ASdc::SdcIap<bool> mIapEnb; /*!< "Enable" input access point */
	ASdc::SdcPap<bool> mOapOut; /*!< Controlling status access point */
	MagObs mMagObs;             /*!< MAG observer */
	bool mCdone;               /*!<  Sign that controlling was completed, ref ds_dcs_sdc_dsgn_cc */
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

template <typename T>
MIface* ASdc::SdcPapc<T>::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    string tt = MDtGet<Sdata<T> >::Type();
    if (tt.compare(aName) == 0) {
	res = dynamic_cast<MDtGet<Sdata<T> >*>(this);
    }
    return res;
}

template <typename T>
void ASdc::SdcPapc<T>::updateData(const T& aData)
{
    if (aData != mData) {
	mData = aData;
	NotifyInpsUpdated();
    }
}

/** @brief SDC agent "Mutate"
 * Performs generic mutation
 * */
class ASdcMut : public ASdc
{
    public:
	static const char* Type() { return "ASdcMut";};
	ASdcMut(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<string> mIapTarg; /*!< "Target" input access point, URI */
	ASdc::SdcIapg<DChr2> mIapMut; /*!< "Mutation" input access point, Chromo2 */
	bool mMutApplied; /*!< Indicatio of mut has been applied successfully */
};


/** @brief SDC agent "Adding Component"
 * */
class ASdcComp : public ASdc
{
    public:
	static const char* Type() { return "ASdcComp";};
	ASdcComp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<string> mIapName; /*!< "Name" input access point */
	ASdc::SdcIap<string> mIapParent; /*!< "Parent" input access point */
	ASdc::SdcPapc<string> mOapName; /*!< Comps Name parameter point, Name pipelined, ref ds_dcs_sdc_dsgn_idp */
};

/** @brief SDC agent "Removing Component"
 * */
class ASdcRm : public ASdc
{
    public:
	static const char* Type() { return "ASdcRm";};
	ASdcRm(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<string> mIapName; /*!< "Name" input access point */
	ASdc::SdcPapc<string> mOapName; /*!< Comps Name parameter point, Name pipelined, ref ds_dcs_sdc_dsgn_idp */
};



/** @brief SDC agent "Connect"
 * */
class ASdcConn : public ASdc
{
    public:
	static const char* Type() { return "ASdcConn";};
	ASdcConn(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
	virtual void getCcd(bool& aData) override;
    protected:
	ASdc::SdcIap<string> mIapV1; /*!< "V1" input access point */
	ASdc::SdcIap<string> mIapV2; /*!< "V2" input access point */
};

/** @brief SDC agent "Disonnect"
 * */
class ASdcDisconn : public ASdc
{
    public:
	static const char* Type() { return "ASdcDisconn";};
	ASdcDisconn(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<string> mIapV1; /*!< "V1" input access point */
	ASdc::SdcIap<string> mIapV2; /*!< "V2" input access point */
};

/** @brief SDC agent "Insert"
 * Inserts system between given CP and its pair
 * There are improved version 2.
 * The current version is required because the ver.2 isn't compatible
 * with current version of vis container.
 * */
class ASdcInsert : public ASdc
{
    public:
	static const char* Type() { return "ASdcInsert";};
	ASdcInsert(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
	virtual void getCcd(bool& aData) override;
    protected:
	ASdc::SdcIap<string> mIapCp; /*!< "Given CP" input access point */
	ASdc::SdcIap<string> mIapIcp; /*!< "Inserted system CP conn to given CP" input access point */
	ASdc::SdcIap<string> mIapIcpp; /*!< "Inserted system CP conn to given CP pair" input access point */
	MVert* mCpPair;
};

/** @brief SDC agent "Insert node into list, ver. 2"
 * */
class ASdcInsert2 : public ASdc
{
    public:
	static const char* Type() { return "ASdcInsert2";};
	ASdcInsert2(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<string> mIapName; /*!< "Link name" input access point */
	ASdc::SdcIap<string> mIapPrev; /*!< "Prev CP" input access point */
	ASdc::SdcIap<string> mIapNext; /*!< "Next CP" input access point */
	ASdc::SdcIap<string> mIapPname; /*!< "Position - name" input access point */
	MVert* mCpPair;
};


/** @brief SDC agent "Extract, reverse to Insert"
 * Extract the link from the chain
 * */
class ASdcExtract : public ASdc
{
    public:
	static const char* Type() { return "ASdcExtract";};
	ASdcExtract(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState() override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<string> mIapName; /*!< "Link name" input access point */
	ASdc::SdcIap<string> mIapPrev; /*!< "Prev CP" input access point */
	ASdc::SdcIap<string> mIapNext; /*!< "Next CP" input access point */
	MVert* mCpPair;
};





#endif // __FAP3_DESSDC_H
