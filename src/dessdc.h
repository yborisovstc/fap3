
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
class ASdc : public Unit, public MDesSyncable, public MDesObserver, public MObserver
{
    public:
	using TObserverCp = NCpOmnp<MObserver, MObservable>;
    public:

 	/** @brief Mag access point base
	 * */
	// TODO unused?
	class SdcMapb: public MDesSyncable {
	    public:
		SdcMapb(const string& aName, ASdc* aHost);
		virtual void onMagUpdated();
		// From MDesSyncable
		virtual string MDesSyncable_Uid() const override {return MDesSyncable::Type();} 
		virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
#ifdef DES_LISTS_SWAP
		virtual void setUpdated() override { mUpdated = true; }
#else
		virtual void setUpdated() override { mUpdated = true; mHost->setUpdated();}
#endif
		virtual void setActivated() override { mActivated = true; mHost->setActivated();}
		virtual int countOfActive(bool aLocal = false) const override { return 1;}
	    public:
		ASdc* mHost;
		string mName;    /*!< Iap name */
		bool mActivated; /*!< Indication of data is active (to be updated) */
		bool mUpdated; /*!< Indication of data is updated */
		bool mChanged; /*!< Indication of data is changed */
	};

	/** @brief Mag access point
	 * */
	// TODO unused?
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
		virtual void onInpUpdated() override { setActivated(); }
		virtual string MDesInpObserver_Uid() const override {return mHost->getUidC<MDesInpObserver>(mName); }
		virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
		// From MDesSyncable
		virtual string MDesSyncable_Uid() const override {return mHost->getUidC<MDesSyncable>(mName);} 
		virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
#ifdef DES_LISTS_SWAP
		virtual void setUpdated() override { mUpdated = true; }
#else
		virtual void setUpdated() override { mUpdated = true; mHost->setUpdated();}
#endif
		virtual void setActivated() override { mActivated = true; mHost->setActivated();}
		virtual int countOfActive(bool aLocal = false) const override { return 1;}
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
		virtual bool updateData();
		T& data(bool aConf = true);
	    public:
		T mUdt;  /*!< Updated data */
		T mCdt;  /*!< Confirmed data */
	};

 	/** @brief Input "Enable" access point
	 * @param  Specific is that boolean AND is applied for inp ifaces
	 * */
	class SdcIapEnb: public SdcIap<Sdata<bool>> {
	    public:
		SdcIapEnb(const string& aName, ASdc* aHost, const string& aInpUri): SdcIap<Sdata<bool>>(aName, aHost, aInpUri) {}
		// Local
		virtual bool updateData();
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
		// Local
		virtual bool updateData();
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
		virtual string MDVarGet_Uid() const override {return mHost->getUidC<MDVarGet>(mName);}
	    public:
		void NotifyInpsUpdated();
	    public:
		string mName;
		ASdc* mHost;
		string mCpUri;  /*!< Output URI */
	};

	/** @brief Parameter access point, using Sdata
	 * */
	template <typename T> class SdcPap: public SdcPapb {
	    public:
		template<typename P> using TGetData = std::function<void (P&)>;
	    public:
		SdcPap(const string& aName, ASdc* aHost, const string& aCpUri, TGetData<T> aGetData): SdcPapb(aName, aHost, aCpUri), mGetData(aGetData) {}
		// From MDVarGet
		virtual string VarGetIfid() const override {return T::TypeSig();}
		virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
		virtual const DtBase* VDtGet(const string& aType) override { mGetData(mData); return &mData;}
	    public:
		TGetData<T> mGetData;
		T mData;
	};

	/** @brief Parameter access point, using cached Sdata
	 * */
	template <typename T> class SdcPapc: public SdcPapb {
	    public:
		SdcPapc(const string& aName, ASdc* aHost, const string& aCpUri): SdcPapb(aName, aHost, aCpUri) {}
		// From MDVarGet
		virtual string VarGetIfid() const override {return T::TypeSig();}
		virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
		virtual const DtBase* VDtGet(const string& aType) override { return &mData;}
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

	/** @brief Mag data observer
	 * */
	class MagDobs : public MObserver {
	    public:
		/** @brief Event to be observed */
		enum EObs{
		    EO_ATCH = 0x01,
		    EO_DTCH = 0x02,
		    EO_CNT = 0x04,
		    EO_CHG = 0x08,
		    EO_ALL = EO_ATCH | EO_DTCH | EO_CNT | EO_CHG,
		};
	    public:
		MagDobs(ASdc* aHost, int aMask = EO_ALL): mHost(aHost), mNuo(nullptr), mMask(aMask), mOcp(this) {}
		virtual ~MagDobs() { }
		void updateNuo(MNode* aNuo);
		// From MObserver
		virtual string MObserver_Uid() const {return mNuo->name() + "%" + MObserver::Type();}
		virtual MIface* MObserver_getLif(const char *aName) override { return nullptr;}
		virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override {
		    if (mMask & EO_ATCH) mHost->notifyOutp();
		}
		virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override;
		virtual void onObsContentChanged(MObservable* aObl, const MContent* aCont) override {
		    if (mMask & EO_CNT) mHost->notifyOutp();
		}
		virtual void onObsChanged(MObservable* aObl) override {
		    if (mMask & EO_CHG) mHost->notifyOutp();
		}
	    public:
		MNode* mNuo;                   /*!< Node under observation */
		int mMask;                     /*!< Events mask, EObs */
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
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MIface* MDesSyncable_getLif(const char *aType) override { return nullptr; }
	virtual void update() override;
	virtual void confirm() override;
	virtual void setUpdated() override;
	virtual void setActivated() override;
	virtual int countOfActive(bool aLocal = false) const override { return 1;}
	// From MDesObserver
	virtual string MDesObserver_Uid() const override {return getUid<MDesObserver>();}
	virtual void MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
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
	void getOut(Sdata<bool>& aData);
	// Utilities
	void addInput(const string& aName);
	virtual void addOutput(const string& aName);
	// Local
	virtual bool doCtl() { return false;}
	/** @brief Gets status of the query */
	virtual bool getState(bool aConf = false) {return false;}
	void notifyMaps();
	void notifyOutp();
	void UpdateMag();
    protected:
	vector<SdcIapb*> mIaps; /*!< Input adapters registry */
	vector<SdcPapb*> mPaps; /*!< Param adapters registry */
	vector<SdcMapb*> mMaps; /*!< Mag adapters registry */
	TObserverCp mObrCp;               /*!< Observer connpoint */
	MNode* mMag; /*!< Managed agent */
	bool mUpdNotified;  //<! Sign of that State notified observers on Update
	bool mActNotified;  //<! Sign of that State notified observers on Activation
	ASdc::SdcIapEnb mIapEnb; /*!< "Enable" input access point */
	ASdc::SdcPap<Sdata<bool>> mOapOut; /*!< Controlling status access point */
	MagObs mMagObs;             /*!< MAG observer */
	bool mCdone;               /*!<  Sign that controlling was completed, ref ds_dcs_sdc_dsgn_cc */
	bool mOutCInv = true;      //!< Sign of output data cache invalidated
};

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
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<Sdata<string>> mIapTarg; /*!< "Target" input access point, URI */
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
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
	// From MObserver
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override;
    protected:
	ASdc::SdcIap<Sdata<string>> mIapName; /*!< "Name" input access point */
	ASdc::SdcIap<Sdata<string>> mIapParent; /*!< "Parent" input access point */
	// TODO not used
	ASdc::SdcPapc<Sdata<string>> mOapName; /*!< Comps Name parameter point, Name pipelined, ref ds_dcs_sdc_dsgn_idp */
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
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
	// From MObserver
	virtual void onObsOwnedAttached(MObservable* aObl, MOwned* aOwned) override;
	virtual void onObsOwnedDetached(MObservable* aObl, MOwned* aOwned) override;
    protected:
	ASdc::SdcIap<Sdata<string>> mIapName; /*!< "Name" input access point */
	// TODO not used
	ASdc::SdcPapc<Sdata<string>> mOapName; /*!< Comps Name parameter point, Name pipelined, ref ds_dcs_sdc_dsgn_idp */
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
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<Sdata<string>> mIapV1; /*!< "V1" input access point */
	ASdc::SdcIap<Sdata<string>> mIapV2; /*!< "V2" input access point */
};

/** @brief SDC agent "Disconnect"
 * */
class ASdcDisconn : public ASdc
{
    public:
	static const char* Type() { return "ASdcDisconn";};
	ASdcDisconn(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<Sdata<string>> mIapV1; /*!< "V1" input access point */
	ASdc::SdcIap<Sdata<string>> mIapV2; /*!< "V2" input access point */
};

/** @brief SDC agent "Insert node into list, ver. 2. Insertion before the chain given node"
 * TODO The curret approach for insertion is wrong: the element is inserted BEFORE
 * given chain node (position node). But the insertion causes the change of position of this given chain node
 * so getting status of the control operation results to false. We need to use opposite approach -
 * insert AFTER given chain node. In this case the position of this given node is not changed.
 * Actually even the current approach works ok if the given chan node is specified basing on the position from end.
 * But in many cases this is rather inconvenient. For instance in the colums chain in column layout the position of
 * the columnn is calculated from start to end.
 * */
class ASdcInsert2 : public ASdc
{
    public:
	static const char* Type() { return "ASdcInsert2";};
	ASdcInsert2(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From MObserver
	virtual void onObsChanged(MObservable* aObl) override;
	// From ASdc
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	bool isBindedToEnd(MVert* aCurPrevv, const MVert* aEndNextv, const GUri& aUriP, const GUri& aUriN);
    protected:
	ASdc::SdcIap<Sdata<string>> mIapName; /*!< "Link name" input access point */
	ASdc::SdcIap<Sdata<string>> mIapPrev; /*!< "Prev CP" input access point */
	ASdc::SdcIap<Sdata<string>> mIapNext; /*!< "Next CP" input access point */
	ASdc::SdcIap<Sdata<string>> mIapPname; /*!< "Position - name" input access point */
	ASdc::MagDobs mDobsNprev; /*!< "Link prev CP" observation */
	MVert* mCpPair;
};

#if 0
/** @brief SDC agent "Insert node into list, ver. 3. DS_ISS_013 fixed.
 * TODO Migrate to DES based SDC
 * */
class ASdcInsert3 : public ASdc
{
    public:
	static const char* Type() { return "ASdcInsert3";};
	ASdcInsert3(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From MObserver
	virtual void onObsChanged(MObservable* aObl) override;
	// From ASdc
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<GUri> mIapStartUri; /*!< "List start" input access point */
	ASdc::SdcIap<GUri> mIapEndUri; /*!< "List end" input access point */
	ASdc::SdcIap<GUri> mIapName; /*!< "Link name" input access point */
	ASdc::SdcIap<GUri> mIapPrev; /*!< "Prev CP" input access point */
	ASdc::SdcIap<GUri> mIapNext; /*!< "Next CP" input access point */
	ASdc::SdcIap<GUri> mIapPname; /*!< "Position - name" input access point */
	ASdc::MagDobs mDobsNprev; /*!< "Link prev CP" observation */
	MVert* mCpPair;
};
#endif

/** @brief SDC agent "Insert node into list AFTER a the chain given node"
 * */
class ASdcInsertN : public ASdc
{
    public:
	static const char* Type() { return "ASdcInsertN";};
	ASdcInsertN(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From MObserver
	virtual void onObsChanged(MObservable* aObl) override;
	// From ASdc
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<Sdata<string>> mIapName; /*!< "Link name" input access point */
	ASdc::SdcIap<Sdata<string>> mIapPrev; /*!< "Prev CP" input access point */
	ASdc::SdcIap<Sdata<string>> mIapNext; /*!< "Next CP" input access point */
	ASdc::SdcIap<Sdata<string>> mIapPname; /*!< "Position - name" input access point */
	ASdc::MagDobs mDobsNprev; /*!< "Link prev CP" observation */
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
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	ASdc::SdcIap<Sdata<string>> mIapName; /*!< "Link name" input access point */
	ASdc::SdcIap<Sdata<string>> mIapPrev; /*!< "Prev CP" input access point */
	ASdc::SdcIap<Sdata<string>> mIapNext; /*!< "Next CP" input access point */
	MVert* mCpPair;
};


/** @brief SDC agent "Pause manageable"
 * */
class ASdcPause : public ASdc
{
    public:
	static const char* Type() { return "ASdcPause";};
	ASdcPause(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	MDesManageable* getDesManageable();
};

/** @brief SDC agent "Resume manageable"
 * */
class ASdcResume : public ASdc
{
    public:
	static const char* Type() { return "ASdcResume";};
	ASdcResume(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From ASdc
	virtual bool getState(bool aConf = false) override;
	bool doCtl() override;
    protected:
	MDesManageable* getDesManageable();
};






#endif // __FAP3_DESSDC_H
