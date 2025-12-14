
#include <thread>

#include "mlink.h"
#include "des.h"
#include "data.h"
#include "prof_ids.h"

// Enable verification of DES active registry. !Affect system performance
#define DES_RGS_VERIFY

const string KCont_Provided = "Provided";
const string KCont_Required = "Required";


CpState::CpState(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
}

void CpState::notifyInpsUpdated()
{
    //LOGN(EErr, "notifyInpsUpdated");
    auto* ifcs = getTIfs<MDesInpObserver>();
    for (auto* obs : *ifcs) {
	obs->onInpUpdated();
    }
}

void CpState::onConnected()
{
    ConnPointu::onConnected();
    // We don't need to notify inps. It is done in onIfpInvalidated
    //notifyInpsUpdated();
}

void CpState::onDisconnected()
{
    ConnPointu::onDisconnected();
    //notifyInpsUpdated();
}

void CpState::onIfpInvalidated(MIfProv* aProv)
{
    ConnPointu::onIfpInvalidated(aProv);
    if (aProv->ifId() == MDVarGet::idHash()) {
	// Notify DES inps only in case of MDVarGet invalidated, ref ds_asr_cbscpd_ahoi
	notifyInpsUpdated();
    }
}

/* Connection point - input of combined chain state AStatec */

#ifdef DES_CPS_IFC

CpStateInp::CpStateInp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv), mIop(this)
{
}

void CpStateInp::onInpUpdated()
{
    auto* ifcs = reinterpret_cast<MIfProv::TTIfaces<MDesInpObserver>*>(mIop.mIfr.ifaces());
    for (auto* ifc : *ifcs) {
	ifc->onInpUpdated();
    }
}

MIface* CpStateInp::MNode_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMDesInpObserverPtr));
    else res = CpState::MNode_getLif(aTid);
    return res;
}

void CpStateInp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	if (aReq->provided()->isRequestor(&mIop)) {
	    // Redirect the request to owner
	    auto owner = Owner();
	    MUnit* ownu = owner ? const_cast<MOwner*>(owner)->lIf(ownu): nullptr;
	    if (ownu) {
		ownu->resolveIface(aName, aReq);
	    }
	} else {
	    addIfpLeaf(dynamic_cast<MDesInpObserver*>(this), aReq);
	}
    } else {
	CpState::resolveIfc(aName, aReq);
    }
}

MIface* CpStateInp::InpObsProvider::MIfProvOwner_getLif(TIdHash aTid)
{
    return nullptr;
}

void CpStateInp::InpObsProvider::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    mHost->resolveIface(aName, aReq);
}

void CpStateInp::InpObsProvider::onIfpDisconnected(MIfProv* aProv)
{
}

void CpStateInp::InpObsProvider::onIfpInvalidated(MIfProv* aProv)
{
}


#else // DES_CPS_IFC

vector<GUri> CpStateInp::getParentsUri()
{
    auto p = CpState::getParentsUri();
    p.insert(p.begin(), string(idStr()));
    return p;
}

CpStateInp::CpStateInp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
}

#endif // DES_CPS_IFC

/* Connection point - output of combined chain state AStatec */

vector<GUri> CpStateOutp::getParentsUri()
{
    auto p = CpState::getParentsUri();
    p.insert(p.begin(), string(idStr()));
    return p;
}

CpStateOutp::CpStateOutp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
}

void CpStateOutp::onConnected()
{
    ConnPointu::onConnected();
}

void CpStateOutp::onDisconnected()
{
    ConnPointu::onDisconnected();
}


/* Connection point - input of state */

CpStateMnodeInp::CpStateMnodeInp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
}

MIface::TIdHash CpStateMnodeInp::idProvided() const
{
    return MDesInpObserver::idHash();
}

MIface::TIdHash CpStateMnodeInp::idRequired() const
{
    return MLink::idHash();
}


/* Connection point - output of combined chain state AStatec */

CpStateMnodeOutp::CpStateMnodeOutp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
}

MIface::TIdHash CpStateMnodeOutp::idProvided() const
{
    return MLink::idHash();
}

MIface::TIdHash CpStateMnodeOutp::idRequired() const
{
    return MDesInpObserver::idHash();
}



/// CpStateInp direct extender

vector<GUri> ExtdStateInp::getParentsUri()
{
    auto p = Extd::getParentsUri();
    p.insert(p.begin(), string(idStr()));
    return p;
}

ExtdStateInp::ExtdStateInp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(string(CpStateOutp::idStr()), Extd::KUriInt , mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

/// CpStateOutp direct extender

vector<GUri> ExtdStateOutp::getParentsUri()
{
    auto p = Extd::getParentsUri();
    p.insert(p.begin(), string(idStr()));
    return p;
}

ExtdStateOutp::ExtdStateOutp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(string(CpStateInp::idStr()), Extd::KUriInt , mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

/// CpStateOutp direct extender with ifaces impl

ExtdStateOutpI::ExtdStateOutpI(const string &aType, const string& aName, MEnv* aEnv): ExtdStateOutp(aType, aName, aEnv)
{
}

MIface* ExtdStateOutpI::MNode_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMDVarGetPtr));
    else res = ExtdStateOutp::MNode_getLif(aTid);
    return res;
}

void ExtdStateOutpI::onInpUpdated()
{
    // Rederect to call to pairs
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	auto* ifcs = pe ? pe->getTIfs<MDesInpObserver>() : nullptr;
	for (auto obs : *ifcs) {
	    // assert(obs);
	    obs->onInpUpdated();
	}
    }
}

const DtBase* ExtdStateOutpI::VDtGet(const string& aType)
{
    // Redirect to internal point
    MVert* intcp = getExtd();
    MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
    MDVarGet* inpvg = intcpu ? intcpu->getSif(inpvg) : nullptr;
    return inpvg ? inpvg->VDtGet(aType) : nullptr;
}

string ExtdStateOutpI::VarGetIfid() const
{
    // Redirect to internal point
    auto* self = const_cast<ExtdStateOutpI*>(this);
    MVert* intcp = self->getExtd();
    MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
    MDVarGet* inpvg = intcpu ? intcpu->getSif(inpvg) : nullptr;
    return inpvg ? inpvg->VarGetIfid() : string();
}

void ExtdStateOutpI::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aTid); // Local
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    } else if (aTid == MDesInpObserver::idHash()) {
	// Enable MDesInpObserver resolution for input
	MVert* intcp = getExtd();
	MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
	MIfProvOwner* intcpo = intcpu ? intcpu->lIf(intcpo) : nullptr;
	if (intcpo && aReq->provided()->isRequestor(intcpo)) {
	    ifr = mMDesInpObserverPtr ? mMDesInpObserverPtr : (mMDesInpObserverPtr = dynamic_cast<MDesInpObserver*>(this));
	    addIfpLeaf(ifr, aReq);
	}
    } else {
	ExtdStateOutp::resolveIfc(aTid, aReq);
    }
}


/// CpStateMnodeInp direct extender 

ExtdStateMnodeInp::ExtdStateMnodeInp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(string(CpStateMnodeOutp::idStr()), Extd::KUriInt , mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}


/// CpStateMnodeOutp direct extender 

ExtdStateMnodeOutp::ExtdStateMnodeOutp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(string(CpStateMnodeInp::idStr()), Extd::KUriInt , mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}



/* State base agent, monolitic, using unit base organs, combined data-observer chain */


// Debug logging levels
static const int KStatecDlog_Obs = 6;  // Observers
static const int KStatecDlog_ObsIfr = 7;  // Observers ifaces routing


// State, ver. 2, non-inhritable, monolitic, direct data, switching updated-confirmed

const string State::KCont_Value = "";
const string State::KInpName = "Inp";


vector<GUri> State::getParentsUri()
{
    auto p = Vertu::getParentsUri();
    p.insert(p.begin(), string(idStr()));
    return p;
}

bool State::SContValue::getData(string& aData) const
{
    aData = mHost.mCdata->ToString();
    return true;
}

bool State::SContValue::setData(const string& aData)
{
    return mHost.updateWithContValue(aData);
}

State::State(const string &aType, const string& aName, MEnv* aEnv): Vertu(aType, aName, aEnv),
    mPdata(NULL), mCdata(NULL), mUpdNotified(false), mActNotified(false), mInpProv(nullptr), mStDead(false), mInp(nullptr)
{
    MNode* cp = Provider()->createNode(string(CpStateInp::idStr()), KInpName, mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

State::~State()
{
    if (mPdata) {
	delete mPdata;
    }
    if (mCdata) {
	delete mCdata;
    }
    mStDead = true;
}

MIface* State::MNode_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMDesSyncablePtr));
    else if (res = checkLif2(aTid, mMDesInpObserverPtr));
    else if (res = checkLif2(aTid, mMConnPointPtr));
    else if (res = checkLif2(aTid, mMDVarGetPtr));
    else if (res = checkLif2(aTid, mMDVarSetPtr));
    else res = Vertu::MNode_getLif(aTid);
    return res;
}

MIface* State::MOwner_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMDesSyncablePtr)); // ??
    else if(res = checkLif2(aTid, mMUnitPtr));  // IFR from inputs
    else res = Vertu::MOwner_getLif(aTid);
    return res;
}

MIface* State::MOwned_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMDesSyncablePtr));
    else res = Unit::MOwned_getLif(aTid);
    return res;
}

void State::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    if (aTid == idProvided()) {
        MIface* ifr = MNode_getLif(aTid);
        if (ifr && !aReq->binded()->provided()->findIface(ifr)) {
            addIfpLeaf(ifr, aReq);
        }
#ifdef DES_IFR_DESOBS
    } else if (aTid == MDesObserver::idHash()) {
        // Redirect to owning
        MUnit* owru = Owner()->lIf(owru);
        if (owru) {
            owru->resolveIface(aTid, aReq);
        }
#endif
#ifdef DES_IFR_INPOBS
    } else if (aTid == MDesInpObserver::idHash()) {
        // For owned - self, for self - owning
        bool bndHasPairs = (aReq->binded()->pairsBegin() != aReq->binded()->pairsEnd());
        MIfReq::TIfReqCp* req = bndHasPairs ? *aReq->binded()->pairsBegin() : nullptr;
        const MIfProvOwner* reqo = req ? req->provided()->rqOwner() : nullptr;
        const MNode* reqn = reqo ? reqo->lIf(reqn) : nullptr; // Current requestor as node
        const MOwned* reqowd = reqn ? reqn->lIf(reqowd) : nullptr; // Requestor as owned
        if (reqowd && isOwned(reqowd)) {
            // Requestor from owned - resolve as self
            auto* ifc = MNode_getLif(aTid);
            if (ifc) {
                addIfpLeaf(ifc, aReq);
            }
	} else {
	    // Self requestor or no requestor - redirect to pairs
	    for (auto pair : mPairs) {
		MUnit* pe = pair->lIf(pe);
		pe->resolveIface(aTid, aReq);
	    }
	}
#endif
    } else {
	Vertu::resolveIfc(aTid, aReq);
    }
}

MContent* State::getCont(int aIdx)
{
    return const_cast<MContent*>(const_cast<const State*>(this)->getCont(aIdx));
}

const MContent* State::getCont(int aIdx) const
{
    const MContent* res = nullptr;
    if (aIdx == 0) res = &mValue;
    else if (aIdx == 1) res = Node::getCont(aIdx - 1);
    return res;
}

bool State::getContent(const GUri& aCuri, string& aRes) const
{
    bool res = true;
    string name = aCuri;
    if (name == KCont_Value)
	res = mValue.getData(aRes);
    else
	res = Vertu::getContent(aCuri, aRes);
    return res;
}

bool State::setContent(const GUri& aCuri, const string& aData)
{
    bool res = true;
    string name = aCuri;
    if (name == KCont_Value)
	res = mValue.setData(aData);
    else
	res = Vertu::setContent(aCuri, aData);
    return res;
}

void State::onContentChanged(const MContent* aCont)
{
    Vertu::onContentChanged(aCont);
}

MIface* State::MVert_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMConnPointPtr));
    else res = Vertu::MVert_getLif(aTid);
    return res;
}

bool State::isCompatible(MVert* aPair, bool aExt)
{
    bool res = false;
    bool ext = aExt;
    MVert* cp = aPair;
    // Checking if the pair is Extender
    if (aPair != this) {
	MVert* ecp = cp->getExtd(); 
	if (ecp) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp) {
	    // Check roles conformance
	    auto prov = idProvided();
	    auto req = idRequired();
	    MConnPoint* mcp = cp->lIf(mcp);
	    if (mcp) {
		auto pprov = mcp->idProvided();
		auto preq = mcp->idRequired();
		if (ext) {
		    res = prov == pprov && req == preq;
		} else {
		    res = prov == preq && req == pprov;
		}
	    }
	}
    } else {
	res = aExt;
    }
    return res;
}

void State::setActivated()
{
    if (!mActNotified) {
	// Propagate activation to owner
#ifndef DES_IFR_DESOBS
	MUnit* ownu = Owner()->lIf(ownu);
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
#else
        if (!mDobsIfProv) {
            mDobsIfProv = defaultIfProv(MDesObserver::idHash());
        }
        auto* ifcs = mDobsIfProv->ifaces();
        auto* obs = ifcs->size() ? reinterpret_cast<MDesObserver*>(ifcs->at(0)) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
#endif
    }
}

void State::update()
{
    PFL_DUR_STAT_START(PEvents::EDurStat_StUpdate);
    mActNotified = false;
    string dtype;
    if (mCdata) {
	dtype = mCdata->GetTypeSig();
    }
    MDVarGet* vget = GetInp();
    if (vget) {
	mInpValid = true;
	const DtBase* pdata = nullptr;
	try {
	    pdata = vget->VDtGet(dtype);
	} catch (std::exception e) {
	    Logger()->Write(EErr, this, "Unspecified error on update");
	}
	if (pdata) {
	    if (!mPdata) {
		mPdata = CreateData(pdata->GetTypeSig());
	    }
	    if (mPdata) {
		*mPdata = *pdata;
	    }
	} else {
            mInpValid = false;
	}
    } else {
	mInpValid = false;
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_StUpdate);
}

void State::NotifyInpsUpdated()
{
    //LOGN(EErr, "NotifyInpsUpdated");
#ifdef DES_IFR_INPOBS
    if (!mInpobsIfProv) {
	mInpobsIfProv = defaultIfProv(MDesInpObserver::idHash());
    }
    auto* ifcs = mInpobsIfProv->ifaces();
    for (auto ifc : *ifcs) {
	auto* obs = reinterpret_cast<MDesInpObserver*>(ifc);
	obs->onInpUpdated();
    }
#else
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	auto ifcs = pe->getTIfs<MDesInpObserver>();
	if (ifcs) for (auto obs : *ifcs) {
	    obs->onInpUpdated();
	}
    }
#endif
}

void State::confirm()
{
    mUpdNotified = false;
    bool changed = false;
    PFL_DUR_STAT_START(PEvents::EDurStat_StConfirm);
    if (mCdata) {
	string old_value;
	if (LOG_LEVEL(EDbg)) {
	    old_value = mCdata->ToString();
	}
	if (mInpValid) {
	    if (mPdata) {
		if (*mCdata != *mPdata) {
		    // Swap the data
		    auto ptr = mCdata;
		    mCdata = mPdata;
		    mPdata = ptr;
		    NotifyInpsUpdated();
		    changed = true;
		}
	    } else {
		if (mCdata->IsValid()) {
		    mCdata->mValid = false;
		    changed = true;
		}
	    }
	}
	if (changed) {
	    LOGN(EDbg, "Updated [" + mCdata->ToString() + " <- " + old_value + "]");
	} else {
	    // State is not changed. No need to notify connected inps.
	    // But we still need to make IFR paths to inps actual. Ref ds_asr.
	    // TODO PERF
#ifndef DES_IFR_INPOBS
	    PFL_DUR_STAT_START(PEvents::EDurStat_DesRfInpObs);
	    refreshInpObsIfr();
	    PFL_DUR_STAT_REC(PEvents::EDurStat_DesRfInpObs);
#endif
	}
    } else {
	if (mPdata) {
	    mCdata = CreateData(mPdata->GetTypeSig());
	}
	if (mCdata) {
	    *mCdata = *mPdata;
	    NotifyInpsUpdated();
	    LOGN(EInfo, "Updated [" + mCdata->ToString() + "]");
	} else {
	    LOGN(EInfo, "Not initialized");
	}
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_StConfirm);
}

void State::setUpdated()
{
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs && !mUpdNotified) {
	obs->onUpdated(this);
	mUpdNotified = true;
    }
}

void State::onInpUpdated()
{
    if (!mStDead) {
	setActivated();
    }
}

MDVarGet* State::GetInp()
{
    MDVarGet* res = nullptr;
    MIfProv::TIfaces* ifcs = nullptr;
    if (!mInpProv) {
	if (!mInp) mInp = getComp(KInpName);
	MNode* inp = mInp;
	MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	mInpProv = inpu ? inpu->defaultIfProv(MDVarGet::idHash()) : nullptr;
	ifcs = mInpProv ? mInpProv->ifaces() : nullptr;
    } else {
	ifcs = mInpProv->ifaces();
    }
    if (ifcs && ifcs->size() == 1) {
	res = reinterpret_cast<MDVarGet*>(ifcs->at(0));
    } else {
	LOGN(EDbg, "Cannot get input");
    }
    return res;
}

MIface::TIdHash State::idProvided() const
{
    return MDVarGet::idHash();
}

MIface::TIdHash State::idRequired() const
{
    return MDesInpObserver::idHash();
}

string State::VarGetIfid() const
{
    return mCdata ? mCdata->GetTypeSig() : string();
}

string State::VarGetSIfid()
{
    return mCdata ? mCdata->GetTypeSig() : string();
}

const bool State::VDtSet(const DtBase& aData)
{
    bool res = false;
    if (mCdata && mPdata) {
	*mPdata = aData;
	*mCdata = aData;
	if (mCdata->IsChanged()) {
	    NotifyInpsUpdated();
	}
    }
    return res;
}

DtBase* State::CreateData(const string& aType)
{
   return Provider()->createData(aType);
}

void State::onConnected()
{
    invalidateIrm(MDesInpObserver::idHash());
    notifyChanged();
    //Vertu::onConnected();
    //NotifyInpsUpdated();
}

void State::onDisconnected()
{
    invalidateIrm(MDesInpObserver::idHash());
    notifyChanged();
    //Vertu::onDisconnected();
    //NotifyInpsUpdated();
}

void State::onIfpInvalidated(MIfProv* aProv)
{
    Vertu::onIfpInvalidated(aProv);
    setActivated();
}

void State::refreshInpObsIfr()
{
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	// Don't add self to if request context to enable routing back to self
	MIfProv* ifp = pe->defaultIfProv(MDesInpObserver::idHash());
	//MIfProv* prov = ifp->first();
	ifp->ifaces();
    }
}

bool State::updateWithContValue(const string& aData)
{
    bool res = false;
    if (!mCdata) {
	string type;
	DtBase::ParseSigPars(aData, type);
	mCdata = CreateData(type);
	mPdata = CreateData(type);
    }
    if (mCdata) {
	res = true;
	mCdata->FromString(aData);
	mPdata->FromString(aData);
	if (mCdata->IsValid()) {
	    if (mCdata->IsChanged()) {
		LOGN(EDbg2, "Initialized:  " + mCdata->ToString(true) + "]");
		NotifyInpsUpdated();
	    }
	}  else {
	    if (!mCdata->IsValid() && mCdata->IsDsError()) {
		LOGN(EErr, "Error on applying content [" + mName + "] value [" + aData + "]");
		mCdata->FromString(aData);
	    }
	}
    }
    return res;
}

DtBase* State::VDtGet(const string& aType)
{
    // Enable getting base data
    return (mCdata && (aType == mCdata->GetTypeSig() || aType.empty())) ? mCdata : nullptr;
}






// Constant data

const string Const::KCont_Value = "";

vector<GUri> Const::getParentsUri()
{
    auto p = Vertu::getParentsUri();
    p.insert(p.begin(), string(idStr()));
    return p;
}

bool Const::SContValue::getData(string& aData) const
{
    aData = mHost.mData->ToString();
    return true;
}

bool Const::SContValue::setData(const string& aData)
{
    return mHost.updateWithContValue(aData);
}

Const::Const(const string &aType, const string& aName, MEnv* aEnv): Vertu(aType, aName, aEnv),
    mData(NULL), mInpProv(nullptr), mIsDead(false)
{
}

Const::~Const()
{
    if (mData) {
	delete mData;
    }
    mIsDead = true;
}

MIface* Const::MNode_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMConnPointPtr));
    else if (res = checkLif2(aTid, mMDVarGetPtr));
    else res = Vertu::MNode_getLif(aTid);
    return res;
}

MIface* Const::MOwner_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if(res = checkLif2(aTid, mMUnitPtr));  // IFR from inputs
    else res = Vertu::MOwner_getLif(aTid);
    return res;
}

MIface* Const::MOwned_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    res = Unit::MOwned_getLif(aTid);
    return res;
}

void Const::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    if (aTid == idProvided()) {
	MIface* ifr = MNode_getLif(aTid);
	if (ifr && !aReq->binded()->provided()->findIface(ifr)) {
	    addIfpLeaf(ifr, aReq);
	}
    } else {
	Vertu::resolveIfc(aTid, aReq);
    }
}

MContent* Const::getCont(int aIdx)
{
    return const_cast<MContent*>(const_cast<const Const*>(this)->getCont(aIdx));
}

const MContent* Const::getCont(int aIdx) const
{
    const MContent* res = nullptr;
    if (aIdx == 0) res = &mValue;
    else if (aIdx == 1) res = Node::getCont(aIdx - 1);
    return res;
}

bool Const::getContent(const GUri& aCuri, string& aRes) const
{
    bool res = true;
    string name = aCuri;
    if (name == KCont_Value)
	res = mValue.getData(aRes);
    else
	res = Vertu::getContent(aCuri, aRes);
    return res;
}

bool Const::setContent(const GUri& aCuri, const string& aData)
{
    bool res = true;
    string name = aCuri;
    if (name == KCont_Value)
	res = mValue.setData(aData);
    else
	res = Vertu::setContent(aCuri, aData);
    return res;
}

void Const::onContentChanged(const MContent* aCont)
{
    Vertu::onContentChanged(aCont);
}

MIface* Const::MVert_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMConnPointPtr));
    else res = Vertu::MVert_getLif(aTid);
    return res;
}

bool Const::isCompatible(MVert* aPair, bool aExt)
{
    bool res = false;
    bool ext = aExt;
    MVert* cp = aPair;
    // Checking if the pair is Extender
    if (aPair != this) {
	MVert* ecp = cp->getExtd(); 
	if (ecp) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp) {
	    // Check roles conformance
	    auto prov = idProvided();
	    auto req = idRequired();
	    MConnPoint* mcp = cp->lIf(mcp);
	    if (mcp) {
		auto pprov = mcp->idProvided();
		auto preq = mcp->idRequired();
		if (ext) {
		    res = prov == pprov && req == preq;
		} else {
		    res = prov == preq && req == pprov;
		}
	    }
	}
    } else {
	res = aExt;
    }
    return res;
}

void Const::NotifyInpsUpdated()
{
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	auto ifcs = pe->getIfs<MDesInpObserver>();
	if (ifcs) for (auto ifc : *ifcs) {
	    MDesInpObserver* obs = reinterpret_cast<MDesInpObserver*>(ifc);
	    obs->onInpUpdated();
	}
    }
}

MIface::TIdHash Const::idProvided() const
{
    return MDVarGet::idHash();
}

MIface::TIdHash Const::idRequired() const
{
    return MDesInpObserver::idHash();
}

string Const::VarGetIfid() const
{
    return mData ? mData->GetTypeSig() : string();
}

DtBase* Const::CreateData(const string& aType)
{
   return Provider()->createData(aType);
}

void Const::onConnected()
{
    invalidateIrm(MDesInpObserver::idHash());
    //Vertu::onConnected();
    //NotifyInpsUpdated();
}

void Const::onDisconnected()
{
    invalidateIrm(MDesInpObserver::idHash());
    notifyChanged();
    //Vertu::onDisconnected();
    //NotifyInpsUpdated();
}

void Const::onIfpInvalidated(MIfProv* aProv)
{
    Vertu::onIfpInvalidated(aProv);
    notifyChanged();
    //NotifyInpsUpdated();
}

void Const::refreshInpObsIfr()
{
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	// Don't add self to if request context to enable routing back to self
	MIfProv* ifp = pe->defaultIfProv(MDesInpObserver::idHash());
	//MIfProv* prov = ifp->first();
	ifp->ifaces();
    }
}

bool Const::updateWithContValue(const string& aData)
{
    bool res = false;
    if (!mData) {
	string type;
	DtBase::ParseSigPars(aData, type);
	mData = CreateData(type);
    }
    if (mData) {
	res = true;
	mData->FromString(aData);
	if (mData->IsValid()) {
	    if (mData->IsChanged()) {
		//LOGN(EDbg, "Initialized:  " + mCdata->ToString(true) + "]");
		NotifyInpsUpdated();
	    }
	}  else {
	    if (!mData->IsValid() && mData->IsDsError()) {
		LOGN(EErr, "Error on applying content [" + mName + "] value [" + aData + "]");
		mData->FromString(aData);
	    }
	}
    }
    return res;
}

DtBase* Const::VDtGet(const string& aType)
{
    return (mData && (aType == mData->GetTypeSig() || aType.empty())) ? mData : nullptr;
}





/// DES

const GUri Des::KControlledUri = "Controlled";

Des::Des(const string &aType, const string &aName, MEnv* aEnv): Syst(aType, aName, aEnv), mUpdNotified(false), mActNotified(false),
mPaused(false)
{
}

MIface* Des::MNode_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMDesSyncablePtr));
    else if (res = checkLif2(aTid, mMDesObserverPtr));
    else if (res = checkLif2(aTid, mMDesAdapterPtr));
    else if (res = checkLif2(aTid, mMDesManageablePtr));
    else res = Syst::MNode_getLif(aTid);
    return res;
}

void Des::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    if (aTid == MDesCtxSpl::idHash()) {
	// If requestor isn't comp then get local supplier
	// and propagate request to it, ref ds_dctx_dic_cs Solution_2
	MIfReq* ireq = aReq->provided()->tail(); // Initial requestor
	if (ireq) {
	    bool redirectedToSpl = false;
	    for (auto pitr = owner()->pairsBegin(); pitr != owner()->pairsEnd(); pitr++) {
		auto owdCp = *pitr;
		MDesCtxSpl* spl = owdCp->provided()->lIf(spl);
		if (spl) {
		    MUnit* splu = spl->lIf(splu);
		    MIfProvOwner* splPo = splu ? splu->lIf(splPo) : nullptr;
		    bool isReq = splPo && aReq->provided()->isRequestor(splPo);
		    if (!isReq) {
			addIfpLeaf(spl, aReq);
			MUnit* splu = spl->lIf(splu);
			if (splu) {
			    splu->resolveIface(aTid, aReq);
			    redirectedToSpl = true;
			}
		    }
		}
	    }
	    if (!redirectedToSpl) {
		// Propagate request to owner
		MUnit* ownu = Owner()->lIf(ownu);
		if (ownu) {
		    ownu->resolveIface(aTid, aReq);
		}
	    }
	} else { // Propagate request to owner
	    MUnit* ownu = Owner()->lIf(ownu);
	    if (ownu) {
		ownu->resolveIface(aTid, aReq);
	    }
	}
#ifdef DES_IFR_DESOBS
    } else if (aTid == MDesObserver::idHash()) {
        // For owned - self, for self - owning
        bool bndHasPairs = (aReq->binded()->pairsBegin() != aReq->binded()->pairsEnd());
        MIfReq::TIfReqCp* req = bndHasPairs ? *aReq->binded()->pairsBegin() : nullptr;
        const MIfProvOwner* reqo = req ? req->provided()->rqOwner() : nullptr;
        const MNode* reqn = reqo ? reqo->lIf(reqn) : nullptr; // Current requestor as node
        const MOwned* reqowd = reqn ? reqn->lIf(reqowd) : nullptr; // Requestor as owned
        //if (reqn && isNodeOwnedInd(reqn)) {
        if (reqowd && isOwned(reqowd)) {
            // Requestor from owned - resolve as self
            auto* ifc = MNode_getLif(aTid);
            if (ifc) {
                addIfpLeaf(ifc, aReq);
            }
        } else {
            // Self requestor or no requestor - redirect to owning
            MUnit* owru = Owner() ? Owner()->lIf(owru) : nullptr;
            if (owru) {
		owru->resolveIface(aTid, aReq);
            }
        }
#endif
    } else {
        Syst::resolveIfc(aTid, aReq);
    }
}

void Des::update()
{
    mUpd = true;
    if (!mPaused) {
        for (auto comp : *mActive) {
            try {
                comp->update();
            } catch (std::exception e) {
                LOGN(EErr, "Error on update [" + comp->Uid() + "]");
            }
        }
        // Swapping the lists
        auto upd = mUpdated;
        mUpdated = mActive;
        mActive = upd;
        mActive->clear();
    }
    mActNotified = false;
    mUpd = false;
}

void Des::confirm()
{
    for (auto comp : *mUpdated) {
	try {
	    comp->confirm();
	} catch (std::exception e) {
	    LOGN(EErr, "Error on confirm [" + comp->Uid() + "]");
	}
    }
    if (mIsActive && !mActNotified) {
	mIsActive = false;
	notifyChanged();
    }
    mUpdNotified = false;
}

void Des::setUpdated()
{
    if (!mUpdNotified) {
	MUnit* ownu = Owner() ? Owner()->lIf(ownu) : nullptr;
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	}
    }
}

void Des::setActivated()
{
    if (!mActNotified) {
	// Propagate activation to owner
#ifndef DES_IFR_DESOBS
	MUnit* ownu = Owner() ? Owner()->lIf(ownu) : nullptr;
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	    // TODO Improve notification design, ref ds_obsi
	    if (!mIsActive) {
		mIsActive = true;
		notifyChanged();
	    }
	}
#else
        if (!mDobsIfProv) {
            mDobsIfProv = defaultIfProv(MDesObserver::idHash());
        }
        auto* ifcs = mDobsIfProv->ifaces();
        auto* obs = ifcs->size() ? reinterpret_cast<MDesObserver*>(ifcs->at(0)) : nullptr;
        if (obs == this) {
            LOGN(EDbg, "setActivated, self observer");
        }
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	    if (!mIsActive) {
		mIsActive = true;
		notifyChanged();
	    }
	}
#endif
    }
}

bool Des::isActive() const
{
    return mIsActive;
}

int Des::countOfActive(bool aLocal) const
{
    int res = mActive->size();
    if (!aLocal) {
	for (auto item : *mActive) {
	    res += item->countOfActive(aLocal);
	}
    }
    return res;
}

void Des::onActivated(MDesSyncable* aComp)
{
    assert(!mUpd);
    setActivated();
    if (aComp) {
#ifdef DES_RGS_VERIFY
	for (auto comp : *mActive) {
	    assert(aComp != comp);
	}
#endif
	mActive->push_back(aComp);
    }
}

void Des::onUpdated(MDesSyncable* aComp)
{
//    setUpdated();
}

void Des::RmSyncable(TScblReg& aReg, MDesSyncable* aScbl)
{
    /*
    aReg.remove(aScbl);
    */
    for (auto it = aReg.begin(); it != aReg.end(); it++) {
	if (*it == aScbl) {
	    aReg.erase(it);
	    break;
	}
    }
}

void Des::onOwnedAttached(MOwned* aOwned)
{
    Syst::onOwnedAttached(aOwned);
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    if (os) {
	os->setActivated();
    }
}

void Des::onOwnedDetached(MOwned* aOwned)
{
    Syst::onOwnedDetached(aOwned);
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    if (os) {
	RmSyncable(*mActive, os);
	RmSyncable(*mUpdated, os);
    }
}

MIface* Des::MOwned_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMDesSyncablePtr));
    else res = Syst::MOwned_getLif(aTid);
    return res;
}

MIface* Des::MOwner_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMDesObserverPtr)); // Notifying from owned 
    else if (res = checkLif2(aTid, mMDesAdapterPtr));
    else res = Syst::MOwner_getLif(aTid);
    return res;
}

void Des::MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Opt1) {
	Ifu::offset(aIdt, aOs); aOs << "Active:" << endl;
	for (auto item : *mActive) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    }
    if (aLevel & Ifu::EDM_Opt2) {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
	for (auto item : *mUpdated) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    }
        if (aLevel & Ifu::EDM_Opt3) {
            Ifu::offset(aIdt, aOs); aOs << "Syncable:" << endl;
            auto self = const_cast<Des*>(this);
	    for (auto it = self->owner()->pairsBegin(); it != self->owner()->pairsEnd(); it++) {
		auto* owdCp = *it;
                MUnit* osu = owdCp->provided()->lIf(osu);
                MDesSyncable* item = osu ? osu->getSif(item) : nullptr;
                if (item && item != this) {
                    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
                    if (aLevel & Ifu::EDM_Recursive) {
                        item->MDesSyncable_doDump(Ifu::EDM_Opt3, aIdt + 4, aOs);
                    }
                }
	    }
        }
    }

    void Des::MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const
    {
    MDesSyncable_doDump(aLevel, aIdt, aOs);
    const MDesObserver* obs = Owner()->lIf(obs);
    if (obs && (aLevel & Ifu::EDM_Recursive)) {
	aIdt += 4;
	Ifu::offset(aIdt, aOs); aOs << ">> " << obs->Uid() << endl;
	obs->MDesObserver_doDump(aLevel, aIdt, aOs);
    }
}

void Des::pauseDes()
{
    mPaused = true;
}

void Des::resumeDes()
{
    mPaused = false;
    setActivated();
}

bool Des::isPaused() const
{
    return mPaused;
}


MNode* Des::getMag()
{
    return getNode(Des::KControlledUri);
}

void Des::onOwnerAttached()
{
    invalidateIrm(MDesObserver::idHash());
}

///// ADES



ADes::ADes(const string &aType, const string &aName, MEnv* aEnv): Unit(aType, aName, aEnv), mOrCp(this), mAgtCp(this), mUpdNotified(false), mActNotified(false),
    mPaused(false)
{
}

ADes::~ADes()
{
}

MIface* ADes::MNode_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMDesSyncablePtr));
    else if (res = checkLif2(aTid, mMAgentPtr));
    else if (res = checkLif2(aTid, mMDesObserverPtr));
    else if (res = checkLif2(aTid, mMDesManageablePtr));
    else if (res = checkLif2(aTid, mMDesAdapterPtr));
    else res = Unit::MNode_getLif(aTid);
    return res;
}

MIface* ADes::MAgent_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMDesSyncablePtr));
    else if (res = checkLif2(aTid, mMUnitPtr)); // To allow client to request IFR
    else if (res = checkLif2(aTid, mMDesObserverPtr));
    else if (res = checkLif2(aTid, mMDesManageablePtr));
    else if (res = checkLif2(aTid, mMDesAdapterPtr));
    return res;
}

void ADes::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    if (aTid == MDesCtxSpl::idHash()) {
	MOwner* mmo = ahostNode()->owned()->pcount() > 0 ? ahostNode()->owned()->pairAt(0)->provided() : nullptr; // Owner of owner
	// If requestor isn't comp then get local supplier
	// and propagate request to it, ref ds_dctx_dic_cs Solution_2
	MIfReq* ireq = aReq->provided()->tail(); // Initial requestor
	const MIfProvOwner* ireqPo = ireq ? ireq->rqOwner() : nullptr;
	const MNode* ireqn = ireqPo ? ireqPo->lIf(ireqn) : nullptr; 
	const MOwned* ireqOwd = ireqn ? ireqn->lIf(ireqOwd) : nullptr;
	MOwner* ahostOwr = ahostNode()->lIf(ahostOwr);
	//bool isRqLocal = ireqn == ahostNode() || ahostOwr->isOwned(ireqOwd); // Is init requestor local
	bool isRqLocal = ireqn == ahostNode();
	if (ireqn && !isRqLocal) {
	    bool redirectedToSpl = false;
	    auto* howner = ahostNode()->owner();
	    for (auto pitr = howner->pairsBegin(); pitr != howner->pairsEnd(); pitr++) {
		auto* owdCp = *pitr;
		MDesCtxSpl* spl = owdCp->provided()->lIf(spl);
		if (spl) {
		    MUnit* splu = spl->lIf(splu);
		    MIfProvOwner* splPo = splu ? splu->lIf(splPo) : nullptr;
		    bool isReq = splPo && aReq->provided()->isRequestor(splPo);
		    if (!isReq) {
			addIfpLeaf(spl, aReq);
			MUnit* splu = spl->lIf(splu);
			if (splu) {
			    splu->resolveIface(aTid, aReq);
			    redirectedToSpl = true;
			}
		    }
		}
	    }
	    if (!redirectedToSpl) {
		// Propagate request to owner
		MUnit* ownu = mmo ? mmo->lIf(ownu) : nullptr;
		if (ownu) {
		    ownu->resolveIface(aTid, aReq);
		}
	    }
	} else { // Propagate request to owner
	    MUnit* ownu = mmo ? mmo->lIf(ownu) : nullptr;
	    if (ownu) {
		ownu->resolveIface(aTid, aReq);
	    }
	}
#ifdef DES_IFR_DESOBS
    } else if (aTid == MDesObserver::idHash()) {
        // For owned - self, for self - owning
	MIfReq* req = aReq->provided()->tail(); // Initial requestor
        const MIfProvOwner* reqo = req ? req->rqOwner() : nullptr;
        const MNode* reqn = reqo ? reqo->lIf(reqn) : nullptr; // Current requestor as node
	bool isRqLocal = (reqn == ahostNode());
	if (reqn && !isRqLocal) {
            // Requestor from owned - resolve as self
            auto* ifc = MNode_getLif(aTid);
            if (ifc) {
                addIfpLeaf(ifc, aReq);
            }
        } else {
            // Self requestor or no requestor - redirect to owning
            MNode* ahn = ahostNode();
            if (ahn) {
                MOwner* ahno = ahn->owned()->pairAt(0) ? (*ahn->owned()->pairsBegin())->provided() : nullptr;
                MUnit* ahnou = ahno ? ahno->lIf(ahnou) : nullptr;
                if (ahnou) {
                    ahnou->resolveIface(aTid, aReq);
                }
            }
        }
#endif
    } else {
	Unit::resolveIfc(aTid, aReq);
    }
}

void ADes::update()
{
    mUpd = true;
    if (!mPaused) {
	for (auto* comp : *mActive) {
	    try {
		comp->update();
	    } catch (std::exception e) {
		LOGN(EErr, "Error on update [" + comp->Uid() + "]");
	    }
	}
	// Swapping the lists
	auto upd = mUpdated;
	mUpdated = mActive;
	mActive = upd;
	mActive->clear();
    }

    mActNotified = false;
    mUpd = false;
}

void ADes::confirm()
{
    if (!mPaused) {
	for (auto* comp : *mUpdated) {
	    comp->confirm();
	}
    }
    if (mIsActive && !mActNotified) {
	mIsActive = false;
	notifyChanged();
    }
    mUpdNotified = false;
}

MDesObserver* ADes::getDesObs()
{
    // Get access to owners owner via MAhost iface
    MDesObserver* obs = nullptr;
    MNode* ahn = ahostNode();
    if (ahn) {
	MOwner* ahno = ahn->owned()->pairAt(0) ? ahn->owned()->pairAt(0)->provided() : nullptr;
	MUnit* ahnou = ahno ? ahno->lIf(ahnou) : nullptr;
	obs = ahnou ? ahnou->getSif(obs) : nullptr;
    }
    return obs;
}

void ADes::setUpdated()
{
    if (!mUpdNotified) { // Notify owner
	MDesObserver* obs = getDesObs();
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	    //LOGN(EInfo, "setUpdated, observer: " + obs->Uid());
	    //obs = getDesObs();
	} else {
	    //LOGN(EInfo, "setUpdated, observer not found");
	}
    }
}

void ADes::setActivated()
{
    if (!mActNotified) { // Notify owner
#ifndef DES_IFR_DESOBS
	MDesObserver* obs = getDesObs();
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	    if (!mIsActive) {
		mIsActive = true;
		notifyChanged();
	    }
	} else {
	    //LOGN(EInfo, "setActivated, observer not found");
	    //obs = getDesObs();
	}
#else
        if (!mDobsIfProv) {
            mDobsIfProv = defaultIfProv(MDesObserver::idHash());
        }
        auto* ifcs = mDobsIfProv->ifaces();
        auto* obs = ifcs->size() ? reinterpret_cast<MDesObserver*>(ifcs->at(0)) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	    if (!mIsActive) {
		mIsActive = true;
		notifyChanged();
	    }
	} else {
	    //LOGN(EInfo, "setActivated, observer not found");
        }
#endif
    }
}

void ADes::onActivated(MDesSyncable* aComp)
{
    assert(!mUpd);
    setActivated();
    if (aComp) {
#ifdef DES_RGS_VERIFY
	for (auto comp : *mActive) {
	    assert(aComp != comp);
	}
#endif
	mActive->push_back(aComp);
    }
}

void ADes::onUpdated(MDesSyncable* aComp)
{
//    setUpdated();
}

bool ADes::isActive() const
{
    return mIsActive;
}

int ADes::countOfActive(bool aLocal) const
{
    int res = mActive->size();
    if (!aLocal) {
	for (auto item : *mActive) {
	    res += item->countOfActive(aLocal);
	}
    }
    return res;
}

MIface* ADes::MOwned_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMDesSyncablePtr));
    else res = Unit::MOwned_getLif(aTid);
    return res;
}

MIface* ADes::MObserver_getLif(TIdHash aTid)
{
    return nullptr;
}

void ADes::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    MDesSyncable* ss = MNode::lIf(ss); // self
    if (os && os != ss) {
	os->setActivated();
    }
}

void ADes::onObsOwnerAttached(MObservable* aObl)
{
    invalidateIrm(MDesObserver::idHash());
}


void ADes::RmSyncable(TScblReg& aReg, MDesSyncable* aScbl)
{
    /*
    aReg.remove(aScbl);
    */
    for (auto it = aReg.begin(); it != aReg.end(); it++) {
	if (*it == aScbl) {
	    aReg.erase(it);
	    break;
	}
    }

}

void ADes::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    MDesSyncable* ss = MNode::lIf(ss); // self
    if (os && os != ss) {
	LOGN(EInfo, "Removed syncable " + os->Uid());
	RmSyncable(*mActive, os);
	RmSyncable(*mUpdated, os);
    }
}

void ADes::onOwnerAttached()
{
    bool res = false;
    MObservable* obl = Owner()->lIf(obl);
    if (obl) {
	res = obl->addObserver(&mOrCp);
    }
    if (!res) {
	Logger()->Write(EErr, this, "Cannot attach to observer");
    }
    // Registering in agent host
    MActr* ac = Owner()->lIf(ac);
    res = ac->attachAgent(&mAgtCp);
    if (!res) {
	Logger()->Write(EErr, this, "Cannot attach to host");
    }
    // Activate all existing syncable components
    MNode* host = ahostNode();
    auto* howner = host->owner();
    for (auto pitr = howner->pairsBegin(); pitr != howner->pairsEnd(); pitr++) {
	auto* owdCp = *pitr;
	MUnit* osu = owdCp->provided()->lIf(osu);
	MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
	if (os && os != this) {
	    os->setActivated();
	}
    }
}

void ADes::MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Opt1) {
	Ifu::offset(aIdt, aOs); aOs << "Active:" << endl;
	for (auto item : *mActive) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    }
    if (aLevel & Ifu::EDM_Opt2) {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
	for (auto item : *mUpdated) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    }
        if (aLevel & Ifu::EDM_Opt3) {
            Ifu::offset(aIdt, aOs); aOs << "Syncable:" << endl;
            auto self = const_cast<ADes*>(this);
            MNode* host = self->ahostNode();
	    for (auto it = host->owner()->pairsBegin(); it != host->owner()->pairsEnd(); it++) {
		auto owdCp = *it;
                MUnit* osu = owdCp->provided()->lIf(osu);
                MDesSyncable* item = osu ? osu->getSif(item) : nullptr;
                if (item && item != this) {
	            Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	            if (aLevel & Ifu::EDM_Recursive) {
                        item->MDesSyncable_doDump(Ifu::EDM_Opt3, aIdt + 4, aOs);
                    }
                }
	    }
        }
    }

    MNode* ADes::ahostNode()
    {
        //auto pair = mAgtCp.firstPair();
        auto* pair = *mAgtCp.pairsBegin();
        MAhost* ahost = pair ? pair->provided() : nullptr;
        MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
        return hostn;
    }

    MNode* ADes::ahostGetNode(const GUri& aUri)
    {
	//auto pair = mAgtCp.firstPair();
        auto* pair = *mAgtCp.pairsBegin();
	MAhost* ahost = pair ? pair->provided() : nullptr;
	MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
	MNode* res = hostn ? hostn->getNode(aUri, this) : nullptr;
	return res;
}

void ADes::pauseDes()
{
    mPaused = true;
}

void ADes::resumeDes()
{
    mPaused = false;
    setActivated();
}

bool ADes::isPaused() const
{
    return mPaused;
}

MNode* ADes::getMag()
{
    auto host = ahostNode();
    return host ? host->getNode(Des::KControlledUri) : nullptr;
}


//// DesLauncher


const string KCont_TopDesUri = "Path";
const string KCont_Output = "OutpCount";
const string KUri_Counter = "Counter";


DesLauncher::DesLauncher(const string &aType, const string& aName, MEnv* aEnv): Des(aType, aName, aEnv), mStop(false)
{
}


bool DesLauncher::Run(int aCount, int aIdleCount)
{
    bool res = true;
    int cnt = 0;
    int idlecnt = 0;
    LOGN(EInfo, "START");
    PFL_DUR_STAT_START(PEvents::EDurStat_LaunchRun);
    while (!mStop && (aCount == 0 || cnt < aCount) && (aIdleCount == 0 || idlecnt < aIdleCount)) {
	mCounter++;
	if (!mActive->empty()) {
	    //updateCounter(cnt); // TODO Is it needed?
	    LOGN(EDbg, ">>> Update [" + to_string(mCounter) + "], count: " + to_string(countOfActive()) +
		    ", dur: " + PFL_DUR_VALUE(PEvents::EDur_LaunchActive) +
		    ", inv: " + PFL_DUR_STAT_CNT(PEvents::EDurStat_UInvldIrm));
	    PFL_DUR_START(PEvents::EDur_LaunchActive);
	    PFL_DUR_STAT_START(PEvents::EDurStat_LaunchUpdate);
	    update();
	    PFL_DUR_STAT_REC(PEvents::EDurStat_LaunchUpdate);
	    PFL_DUR_STAT_START(PEvents::EDurStat_LaunchConfirm);
	    if (isLogLevel(EDbg)) {
		LOGN(EDbg, ">>> Confirm [" + to_string(mCounter) + "]");
	    }
	    //outputCounter(cnt); // TODO Is it needed?
	    confirm();
	    PFL_DUR_STAT_REC(PEvents::EDurStat_LaunchConfirm);
	    cnt++;
	    idlecnt = 0;
	    PFL_DUR_REC(PEvents::EDur_LaunchActive);
	} else {
	    if (idlecnt == 0) {
		LOGN(EInfo, "IDLE");
	    }
            //LOGN(EInfo, "Idle [" + to_string(idlecnt) + "]");
	    OnIdle();
	    idlecnt++;
	}
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_LaunchRun);
    LOGN(EInfo, "END " + PFL_DUR_STAT_FIELD(PEvents::EDurStat_LaunchRun, PIndFId::EStat_SUM));
    return res;
}

bool DesLauncher::Stop()
{
    mStop = true;
    return true;
}

void DesLauncher::OnIdle()
{
    this_thread::sleep_for(std::chrono::milliseconds(100));
    //mStop = true;
}

MIface* DesLauncher::MOwned_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMLauncherPtr));
    else res = Des::MOwned_getLif(aTid);
    return res;
}

MIface* DesLauncher::MNode_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMLauncherPtr));
    else res = Des::MNode_getLif(aTid);
    return res;
}

void DesLauncher::updateCounter(int aCnt)
{
    MNode* ctrn = getNode(KUri_Counter);
    if (ctrn) {
        MContentOwner* cnto = ctrn ? ctrn->lIf(cnto) : nullptr;
        if (cnto) {
            cnto->setContent("", string("SS " + to_string(aCnt)));
        }
    }
}

void DesLauncher::outputCounter(int aCnt)
{
    if (getCont(KCont_Output)) {
        cout << "Count : " << aCnt << endl;
    }
}


/// Active subsystem of DES

static const string K_SsInitUri = "Init";
static const string K_SsUri = "Subsys";

DesAs::DesAs(const string &aType, const string& aName, MEnv* aEnv): DesLauncher(aType, aName, aEnv),
    mRunning(false)
{
}

bool DesAs::Run(int aCount, int aIdleCount)
{
    bool res = true;
    int cnt = 0;
    int idlecnt = 0;
    do {
        MNode* ss = getNode(K_SsUri);
        if (ss == nullptr) {
            LOGN(EErr, "No subsystem [" + K_SsUri + "] found");
            break;
        }
        MNode* ssinit = ss->getNode(K_SsInitUri);
        if (!ssinit) {
            LOGN(EErr, "Couldn't find Init state");
            res = false; break;
        }
        MContentOwner* cntInit = ssinit->lIf(cntInit);
        if (!cntInit) {
            LOGN(EErr, "Couldn't find Init state content");
            res = false; break;
        }
        cntInit->setContent("", "SB true");
        if (!mActive->empty()) {
            LOGN(EInfo, ">>> Init update");
            Des::update();
            if (!mUpdated->empty()) {
                LOGN(EInfo, ">>> Init confirm");
                Des::confirm();
            }
        }
        cntInit->setContent("", "SB false");
        // Run subsystem
	while (!mStop && (aCount == 0 || cnt < aCount) && !mActive->empty()) {
	    //updateCounter(cnt);
	    LOGN(EInfo, ">>> Update [" + to_string(cnt) + "]");
	    Des::update();
	    if (!mUpdated->empty()) {
		LOGN(EInfo, ">>> Confirm [" + to_string(cnt) + "]");
		outputCounter(cnt);
		Des::confirm();
	    }
	    cnt++;
	}
    } while (false);
    return res;
}

void DesAs::update()
{
}

// Running on confirm phase, ref solution ds_desas_nio_ric
void DesAs::confirm()
{
    PFL_DUR_STAT_START(PEvents::EDurStat_DesAsUpd);
    mRunning = true;
    bool res = Run(0, 1);
    mRunning = false;
    if (!res) {
        LOGN(EErr, "Failed run");
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_DesAsUpd);
}

void DesAs::setActivated()
{
    if (!mRunning) {
        DesLauncher::setActivated();
    }
}



/// Active subsystem of DES ver.2, ds_desas_nio_do

const GUri DesAs2::K_OutpUri = "Output";
const GUri DesAs2::K_SsysUri = "Subsys";
const GUri DesAs2::K_SsysInitUri = "Init";


DesAs2::DesAs2(const string &aType, const string& aName, MEnv* aEnv): DesLauncher(aType, aName, aEnv),
    mRunning(false)
{
}

bool DesAs2::Run(int aCount, int aIdleCount)
{
    bool res = true;
    int cnt = 0;
    int idlecnt = 0;
    do {
	MNode* ss = getNode(K_SsysUri);
	MDesSyncable* ssds = ss->lIf(ssds);
	if (ss == nullptr) {
	    LOGN(EErr, "No subsystem [" + K_SsysUri.toString() + "] found");
	    break;
	}
	MNode* ssinit = ss->getNode(K_SsysInitUri);
	if (!ssinit) {
	    LOGN(EErr, "Couldn't find Init state");
	    res = false; break;
	}
	MContentOwner* cntInit = ssinit->lIf(cntInit);
	if (!cntInit) {
	    LOGN(EErr, "Couldn't find Init state content");
	    res = false; break;
	}
	cntInit->setContent("", "SB true");
	if (!mActive->empty()) {
	    LOGN(EInfo, ">>> Init update");
	    ssds->update();
	    LOGN(EInfo, ">>> Init confirm");
	    ssds->confirm();
	}
	cntInit->setContent("", "SB false");
	// Run subsystem
	while (!mStop && (aCount == 0 || cnt < aCount) && ssds->isActive()) {
	    //updateCounter(cnt);
	    LOGN(EInfo, ">>> Subs Update [" + to_string(cnt) + "]");
	    ssds->update();
	    LOGN(EInfo, ">>> Subs Confirm [" + to_string(cnt) + "]");
	    outputCounter(cnt);
	    ssds->confirm();
	    cnt++;
	}
    } while (false);
    return res;
}

void DesAs2::update()
{
    PFL_DUR_STAT_START(PEvents::EDurStat_DesAsUpd);
    mRunning = true;
    bool res = Run(0, 1);
    mRunning = false;
    if (!res) {
	LOGN(EErr, "Failed run");
    }
    DesLauncher::update();
    PFL_DUR_STAT_REC(PEvents::EDurStat_DesAsUpd);
}

void DesAs2::confirm()
{
    LOGN(EInfo, "Confirm");
    DesLauncher::confirm();

    /*
       MNode* outp = getNode(K_OutpUri);
       MDesSyncable* outpds = outp->lIf(outpds);
       if (!outpds) {
       LOGN(EErr, "No outputs [" + K_OutpUri.toString() + "] found");
       } else {
       outpds->confirm();
       } 
       */
}

void DesAs2::onActivated(MDesSyncable* aComp)
{
    if (!mRunning) {
	DesLauncher::onActivated(aComp);
    }
}

void DesAs2::setActivated()
{
    if (!mRunning) {
	DesLauncher::setActivated();
    }
}







// Embedded elements support

/// Embedded Input buffered

void DesEIbMnode::update()
{
    bool res = false;
    DesEIbb::update();
    MNode* inp = TP::mHost->getNode(TP::mUri);
    MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
    if (inpu) {
	// Resolve MLink first to avoid MNode wrong resolution
	MLink* mmtl = inpu->getSif(mmtl);
	if (mmtl) {
	    mUdt = mmtl->pair(); res = true;
	}
    }
    if (!res) {
	//this->eHost()->logEmb(TLogRecCtg::EDbg, TLog(TP::mHost) + "Cannot get input [" + this->mUri + "]");
	LOGEMB(TLogRecCtg::EDbg, (TLog(mHost) + "Cannot get input [" + this->mUri + "]"));
    } else {
	this->mActivated = false;
	this->setUpdated();
    }
}





/// Embedded Output state

void DesEOstb::NotifyInpsUpdated()
{
    MNode* cp = mHost->getNode(mCpUri);
    MUnit* cpu = cp ? cp->lIf(cpu) : nullptr;
    auto ifaces = cpu->getIfs<MDesInpObserver>();
    if (ifaces) for (auto ifc : *ifaces) {
	MDesInpObserver* ifco = reinterpret_cast<MDesInpObserver*>(ifc);
	if (ifco) {
	    ifco->onInpUpdated();
	}
    }
}

//// DES affecting Parameter

void DesEParb::updatePar(const MContent* aCont)
{
}


/// DES context supplier

DesCtxSpl::DesCtxSpl(const string &aType, const string& aName, MEnv* aEnv): Des(aType, aName, aEnv),
    mSplCp(this)
{}

MIface* DesCtxSpl::MNode_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMDesCtxSplPtr));
    else res = Des::MNode_getLif(aTid);
    return res;
}

MIface* DesCtxSpl::MOwned_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMDesCtxSplPtr));
    else res = Des::MOwned_getLif(aTid);
    return res;
}

MIface* DesCtxSpl::MDesCtxSpl_getLif(TIdHash aTid)
{
    return checkLif2(aTid, mMUnitPtr); // To enable IFR
}

void DesCtxSpl::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    if (aTid == MDesCtxSpl::idHash()) {
	MIfReq* ireq = aReq->provided()->tail(); // Initial requestor
	if (ireq) {
	    MUnit* ownu = Owner()->lIf(ownu);
	    if (ownu) {
		auto ifaces = ownu->getIfs<MDesCtxSpl>();
		// Filter out same id suppliers, ref ds_dctx_dic_cs Solution_2
		if (ifaces) for (auto ifc : *ifaces) {
		    MDesCtxSpl* spl = reinterpret_cast<MDesCtxSpl*>(ifc);
		    if (spl->getSplId() != getSplId()) {
			addIfpLeaf(spl, aReq);
		    }
		}
	    }
	} else { // Propagate request to owner
	    MUnit* ownu = Owner()->lIf(ownu);
	    if (ownu) {
		ownu->resolveIface(aTid, aReq);
	    }
	}
    } else {
	Des::resolveIfc(aTid, aReq);
    }
}


MDesCtxSpl* DesCtxSpl::getSplsHead()
{
    MDesCtxSpl* res = nullptr;
    return res;
}

bool DesCtxSpl::registerCsm(MDesCtxCsm::TCp* aCsm)
{
    bool res = mSplCp.connect(aCsm);
    return res;
}

bool DesCtxSpl::bindCtx(const string& aCtxId, MVert* aCtx)
{
    bool res = false;
    MNode* ctxn =  getComp(aCtxId);
    MVert* ctxv = ctxn ? ctxn->lIf(ctxv) : nullptr;
    if (ctxv) {
	res = MVert::connect(ctxv, aCtx);
    } else {
	// Redirect to next supplier in the stack
	// To use dedicated iface provider instead of finding supplier here
	MUnit* ownu = Owner()->lIf(ownu);
	if (ownu) {
	    auto ifaces = ownu->getIfs<MDesCtxSpl>();
	    // Find same id supplier
	    if (ifaces) for (auto ifc : *ifaces) {
		MDesCtxSpl* spl = reinterpret_cast<MDesCtxSpl*>(ifc);
		if (spl->getSplId() == getSplId()) {
		    res = spl->bindCtx(aCtxId, aCtx);
		}
	    }
	}
    }
    return res;
}

bool DesCtxSpl::unbindCtx(const string& aCtxId)
{
    bool res = false;
    return res;
}




/// DES context consumer


const string KCnt_ID = "Id"; // Consumer Id

DesCtxCsm::DesCtxCsm(const string &aType, const string& aName, MEnv* aEnv): Des(aType, aName, aEnv),
    mInitialized(false), mInitFailed(false), mCsmCp(this)
{}

MIface* DesCtxCsm::MNode_getLif(TIdHash aTid)
{
    MIface* res = NULL;
    if (res = checkLif2(aTid, mMDesCtxCsmPtr));
    else res = Des::MNode_getLif(aTid);
    return res;
}

string DesCtxCsm::getCsmId() const
{
    string cont;
    bool res = getContent(KCnt_ID, cont);
    if (res) return cont;
    else return name();

}

void DesCtxCsm::onCtxAdded(const string& aCtxId)
{
}

void DesCtxCsm::onCtxRemoved(const string& aCtxId)
{
}

void DesCtxCsm::update()
{
    Des::update();
    // TODO init can cause des activation but it is prohibited on update phase, move to confirm
}

void DesCtxCsm::confirm()
{
    Des::confirm();
    if (!mInitialized) {
	mInitFailed = !init();
	if (mInitFailed) {
	    LOGN(EErr, "Init failed");
	}
	mInitialized = true;
    }
}


#ifndef SELF_IFR
bool DesCtxCsm::init()
{
    bool res = false;
    MUnit* ownu = Owner()->lIf(ownu);
    if (ownu) {
	auto ifaces = ownu->getIfs<MDesCtxSpl>();
	if (ifaces) for (auto ifc : *ifaces) {
	    MDesCtxSpl* spl = reinterpret_cast<MDesCtxSpl*>(ifc);
	    if (spl->getSplId() == getCsmId()) {
		res = spl->registerCsm(&mCsmCp);
		if (res) {
		    res = bindCtxs();
		}
		break;
	    }
	}
    }
    return res;
}
#else

bool DesCtxCsm::init()
{
    bool res = false;
    MDesCtxSpl* spl = getSif<MDesCtxSpl>(spl);
    if (spl) {
	res = spl->registerCsm(&mCsmCp);
	if (res) {
	    res = bindCtxs();
	}
    }
    return res;
}
#endif

bool DesCtxCsm::registerSpl(MDesCtxSpl::TCp* aSpl)
{
    //assert(!mCsmCp.firstPair());
    assert(mCsmCp.pairsBegin() == mCsmCp.pairsEnd());
    bool res = mCsmCp.connect(aSpl);
    return res;
}

bool DesCtxCsm::bindCtxs()
{
    bool res = false;
    for (auto it = owner()->pairsBegin(); it != owner()->pairsEnd(); it++) {
	auto owdCp = *it;
	MNode* compn = owdCp->provided()->lIf(compn);
	MVert* compv = compn ? compn->lIf(compv) : nullptr;
	if (compv) {
	    MVert* extd = compv->getExtd();
	    if (extd) {
		res = (*mCsmCp.pairsBegin())->provided()->bindCtx(compn->name(), extd);
		if (!res) break;
	    }
	}
    }
    return res;
}

#ifdef SELF_IFR
void DesCtxCsm::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesCtxSpl::Type()) {
	// Propagate request to owner
	MUnit* ownu = Owner()->lIf(ownu);
	if (ownu) {
	    auto ifaces = ownu->getIfs<MDesCtxSpl>();
	    if (ifaces) for (auto ifc : *ifaces) {
		MDesCtxSpl* spl = reinterpret_cast<MDesCtxSpl*>(ifc);
		if (spl->getSplId() == getCsmId()) {
		    addIfpLeaf(spl, aReq);
		}
	    }
	}
    } else {
	Syst::resolveIfc(aName, aReq);
    }
}
#endif


// DES Input demultiplexor

static const string K_Cp_Inp = "Inp";
static const string K_Cp_Outp = "Outp";

DesInpDemux::DesInpDemux(const string &aType, const string& aName, MEnv* aEnv): Des(aType, aName, aEnv)
{
    AddInput(K_Cp_Inp);
    AddInput("Done");
    AddOutput(K_Cp_Outp);
}

void DesInpDemux::AddInput(const string& aName)
{
    MNode* cp = Provider()->createNode(string(CpStateInp::idStr()), aName, mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

void DesInpDemux::AddOutput(const string& aName)
{
    MNode* cp = Provider()->createNode(string(CpStateOutp::idStr()), aName, mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

int DesInpDemux::getIfcCount()
{
    int res = 0;
    MNode* inp = getNode(K_Cp_Inp);
    MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
    auto ifaces = inpu->getIfs<MDVarGet>();
    if (ifaces && !ifaces->empty()) {
	res = ifaces->size();
    }
    return res;
}

void DesInpDemux::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    if (aTid == MDVarGet::idHash()) {
	MNode* outp = getNode(K_Cp_Outp);
	MUnit* outpu = outp ? outp->lIf(outpu) : nullptr;
	MIfProvOwner* outppo = outpu ? outpu->lIf(outppo) : nullptr;
	if (outppo && aReq->provided()->isRequestor(outppo)) {
	    // Request from output
	    MNode* inp = getNode(K_Cp_Inp);
	    MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	    auto ifaces = inpu->getIfs<MDVarGet>();
	    if (!ifaces || ifaces->empty() || ifaces->size() <= mIdx) {
		Logger()->Write(EErr, this, "Ifaces idx overflow");
	    } else {
		auto* ifc = reinterpret_cast<MDVarGet*>(ifaces->at(mIdx));
		addIfpLeaf(ifc, aReq);
	    }
	}
    } else if (aTid == MDesObserver::idHash()) {
	MNode* inp = getNode(K_Cp_Inp);
	MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	MIfProvOwner* inppo = inpu ? inpu->lIf(inppo) : nullptr;
	if (inppo && aReq->provided()->isRequestor(inppo)) {
	    // Request from input, redirect to output
	    MNode* outp = getNode(K_Cp_Outp);
	    MUnit* outpu = outp ? outp->lIf(outpu) : nullptr;
	    outpu->resolveIface(aTid, aReq);
	}
    } else {
	Des::resolveIfc(aTid, aReq);
    }
}


void DesInpDemux::confirm()
{
    Des::confirm();
    int ifcnt = getIfcCount();
    if (mIdx < (ifcnt - 1)) {
	mIdx++;
    }

}
