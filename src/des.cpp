
#include <thread>

#include "mlink.h"
#include "des.h"
#include "data.h"
#include "prof_ids.h"

#define DES_RGS_VERIFY


const string KCont_Provided = "Provided";
const string KCont_Required = "Required";

#define STATE2_DATA_SWAP

CpState::CpState(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
}

void CpState::notifyInpsUpdated()
{
    auto* ifcs = getTIfs<MDesInpObserver>();
    for (auto* obs : *ifcs) {
	obs->onInpUpdated();
    }
}

void CpState::onConnected()
{
    ConnPointu::onConnected();
    notifyInpsUpdated();
}

void CpState::onDisconnected()
{
    ConnPointu::onDisconnected();
    notifyInpsUpdated();
}

void CpState::onIfpInvalidated(MIfProv* aProv)
{
    ConnPointu::onIfpInvalidated(aProv);
    notifyInpsUpdated();
}

/* Connection point - input of combined chain state AStatec */

#ifdef DES_CPS_IFC

CpStateInp::CpStateInp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv), mIop(this)
{
    bool res = setContent("Provided", "MDesInpObserver");
    res = setContent("Required", "MDVarGet");
    assert(res);
}

void CpStateInp::onInpUpdated()
{
    auto* ifcs = mIop.mIfr.ifaces();
    for (auto* ifc : *ifcs) {
	assert (ifc != dynamic_cast<MDesInpObserver*>(this));
	reinterpret_cast<MDesInpObserver*>(ifc)->onInpUpdated();
    }
}

MIface* CpStateInp::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesInpObserver>(aType));
    else res = CpState::MNode_getLif(aType);
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

MIface* CpStateInp::InpObsProvider::MIfProvOwner_getLif(const char *aType)
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

CpStateInp::CpStateInp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MDesInpObserver");
    res = setContent("Required", "MDVarGet");
    assert(res);
}

#endif // DES_CPS_IFC

/* Connection point - output of combined chain state AStatec */

CpStateOutp::CpStateOutp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MDVarGet");
    res = setContent("Required", "MDesInpObserver");
    assert(res);
}

/* Connection point - input of state */

CpStateMnodeInp::CpStateMnodeInp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MDesInpObserver");
    res = setContent("Required", "MLink");
    assert(res);
}

/* Connection point - output of combined chain state AStatec */

CpStateMnodeOutp::CpStateMnodeOutp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MLink");
    res = setContent("Required", "MDesInpObserver");
    assert(res);
}


/// CpStateInp direct extender

ExtdStateInp::ExtdStateInp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateOutp::Type(), Extd::KUriInt , mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

/// CpStateOutp direct extender 

ExtdStateOutp::ExtdStateOutp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), Extd::KUriInt , mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

/// CpStateOutp direct extender with ifaces impl

ExtdStateOutpI::ExtdStateOutpI(const string &aType, const string& aName, MEnv* aEnv): ExtdStateOutp(aType, aName, aEnv)
{
}

MIface* ExtdStateOutpI::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = ExtdStateOutp::MNode_getLif(aType);
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
    return inpvg ? inpvg->VarGetIfid() : nullptr;
}

void ExtdStateOutpI::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aName.c_str()); // Local
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    } else if (aName == MDesInpObserver::Type()) {
	// Enable MDesInpObserver resolution for input
	MVert* intcp = getExtd();
	MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
	MIfProvOwner* intcpo = intcpu ? intcpu->lIf(intcpo) : nullptr;
	if (intcpo && aReq->provided()->isRequestor(intcpo)) {
	    ifr = dynamic_cast<MDesInpObserver*>(this);
	    addIfpLeaf(ifr, aReq);
	}
    } else {
	ExtdStateOutp::resolveIfc(aName, aReq);
    }
}



/// CpStateMnodeOutp direct extender 

ExtdStateMnodeOutp::ExtdStateMnodeOutp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateMnodeInp::Type(), Extd::KUriInt , mEnv);
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
    MNode* cp = Provider()->createNode(CpStateInp::Type(), "Inp", mEnv);
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

MIface* State::MNode_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MDesInpObserver>(aType));
    else if (res = checkLif<MConnPoint>(aType));
    else if (res = checkLif<MDVarGet>(aType));
    else if (res = checkLif<MDVarSet>(aType));
    else res = Vertu::MNode_getLif(aType);
    return res;
}

MIface* State::MOwner_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType)); // ??
    else if(res = checkLif<MUnit>(aType));  // IFR from inputs
    else res = Vertu::MOwner_getLif(aType);
    return res;
}

MIface* State::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else res = Unit::MOwned_getLif(aType);
    return res;
}

void State::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == provName()) {
	MIface* ifr = MNode_getLif(aName.c_str());
	if (ifr && !aReq->binded()->provided()->findIface(ifr)) {
	    addIfpLeaf(ifr, aReq);
	}
    } else {
	Vertu::resolveIfc(aName, aReq);
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

MIface* State::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MVert_getLif(aType);
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
	    string prov = provName();
	    string req = reqName();
	    MConnPoint* mcp = cp->lIf(mcp);
	    if (mcp) {
		string pprov = mcp->provName();
		string preq = mcp->reqName();
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
	MUnit* ownu = Owner()->lIf(ownu);
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
    }
}

void State::update()
{
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
	}
    } else {
	mInpValid = false;
    }
#ifndef DES_LISTS_SWAP
    setUpdated();
#endif
}

void State::NotifyInpsUpdated()
{
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	auto ifcs = pe->getTIfs<MDesInpObserver>();
	if (ifcs) for (auto obs : *ifcs) {
	    obs->onInpUpdated();
	}
    }
}

void State::confirm()
{
    mUpdNotified = false;
    bool changed = false;
    PFL_DUR_STAT_START(PEvents::EDurStat_StConfirm);
    if (mCdata) {
	string old_value;
	if (isLogLevel(EDbg)) {
	    old_value = mCdata->ToString();
	}
	if (mInpValid) {
	    if (mPdata) {
		if (*mCdata != *mPdata) {
#ifdef STATE2_DATA_SWAP
		    // Swap the data
		    auto ptr = mCdata;
		    mCdata = mPdata;
		    mPdata = ptr;
#else
		    *mCdata = *mPdata;
#endif
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
	    refreshInpObsIfr();
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
	if (!mInp) mInp = getNode("Inp");
	MNode* inp = mInp;
	MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	mInpProv = inpu ? inpu->defaultIfProv(MDVarGet::Type()) : nullptr;
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

string State::provName() const
{
    return MDVarGet::Type();
}

string State::reqName() const
{
    return MDesInpObserver::Type();
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
    Vertu::onConnected();
    NotifyInpsUpdated();
}

void State::onDisconnected()
{
    Vertu::onDisconnected();
    NotifyInpsUpdated();
}

void State::onIfpInvalidated(MIfProv* aProv)
{
    Vertu::onIfpInvalidated(aProv);
    setActivated();
    NotifyInpsUpdated();
}

void State::refreshInpObsIfr()
{
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	// Don't add self to if request context to enable routing back to self
	MIfProv* ifp = pe->defaultIfProv(MDesInpObserver::Type());
	MIfProv* prov = ifp->first();
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
		//LOGN(EDbg, "Initialized:  " + mCdata->ToString(true) + "]");
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

MIface* Const::MNode_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MConnPoint>(aType));
    else if (res = checkLif<MDVarGet>(aType));
    else res = Vertu::MNode_getLif(aType);
    return res;
}

MIface* Const::MOwner_getLif(const char *aType)
{
    MIface* res = NULL;
    if(res = checkLif<MUnit>(aType));  // IFR from inputs
    else res = Vertu::MOwner_getLif(aType);
    return res;
}

MIface* Const::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    res = Unit::MOwned_getLif(aType);
    return res;
}

void Const::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == provName()) {
	MIface* ifr = MNode_getLif(aName.c_str());
	if (ifr && !aReq->binded()->provided()->findIface(ifr)) {
	    addIfpLeaf(ifr, aReq);
	}
    } else {
	Vertu::resolveIfc(aName, aReq);
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

MIface* Const::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MVert_getLif(aType);
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
	    string prov = provName();
	    string req = reqName();
	    MConnPoint* mcp = cp->lIf(mcp);
	    if (mcp) {
		string pprov = mcp->provName();
		string preq = mcp->reqName();
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

string Const::provName() const
{
    return MDVarGet::Type();
}

string Const::reqName() const
{
    return MDesInpObserver::Type();
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
    Vertu::onConnected();
    NotifyInpsUpdated();
}

void Const::onDisconnected()
{
    Vertu::onDisconnected();
    NotifyInpsUpdated();
}

void Const::onIfpInvalidated(MIfProv* aProv)
{
    Vertu::onIfpInvalidated(aProv);
    NotifyInpsUpdated();
}

void Const::refreshInpObsIfr()
{
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	// Don't add self to if request context to enable routing back to self
	MIfProv* ifp = pe->defaultIfProv(MDesInpObserver::Type());
	MIfProv* prov = ifp->first();
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

Des::Des(const string &aType, const string &aName, MEnv* aEnv): Syst(aType, aName, aEnv), mUpdNotified(false), mActNotified(false)
{
}

MIface* Des::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MDesObserver>(aType));
    else res = Syst::MNode_getLif(aType);
    return res;
}

void Des::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesCtxSpl::Type()) {
	// If requestor isn't comp then get local supplier
	// and propagate request to it, ref ds_dctx_dic_cs Solution_2
	MIfReq* ireq = aReq->provided()->tail(); // Initial requestor
	if (ireq) {
	    bool redirectedToSpl = false;
	    auto owdCp = owner()->firstPair();
	    while (owdCp) {
		MDesCtxSpl* spl = owdCp->provided()->lIf(spl);
		if (spl) {
		    MUnit* splu = spl->lIf(splu);
		    MIfProvOwner* splPo = splu ? splu->lIf(splPo) : nullptr;
		    bool isReq = splPo && aReq->provided()->isRequestor(splPo);
		    if (!isReq) {
			addIfpLeaf(spl, aReq);
			MUnit* splu = spl->lIf(splu);
			if (splu) {
			    splu->resolveIface(aName, aReq);
			    redirectedToSpl = true;
			}
		    }
		}
		owdCp = owner()->nextPair(owdCp);
	    }
	    if (!redirectedToSpl) {
		// Propagate request to owner
		MUnit* ownu = Owner()->lIf(ownu);
		if (ownu) {
		    ownu->resolveIface(aName, aReq);
		}
	    }
	} else { // Propagate request to owner
	    MUnit* ownu = Owner()->lIf(ownu);
	    if (ownu) {
		ownu->resolveIface(aName, aReq);
	    }
	}
    } else {
	Syst::resolveIfc(aName, aReq);
    }
}

#ifdef DES_LISTS_SWAP
void Des::update()
{
    mUpd = true;
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
    mActNotified = false;
    mUpd = false;
}
#else
void Des::update()
{
    while (!mActive.empty()) {
	auto comp = mActive.front();
	try {
	    comp->update();
	} catch (std::exception e) {
	    LOGN(EErr, "Error on update [" + comp->Uid() + "]");
	}
	mActive.pop_front();
    }
    mActNotified = false;
}
#endif

#ifdef DES_LISTS_SWAP
void Des::confirm()
{
    for (auto comp : *mUpdated) {
	comp->confirm();
    }
    mUpdNotified = false;
}
#else
void Des::confirm()
{
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
    }
    mUpdNotified = false;
}
#endif

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
	MUnit* ownu = Owner() ? Owner()->lIf(ownu) : nullptr;
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
    }
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

#ifdef DES_LISTS_SWAP
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
#else
void Des::onActivated(MDesSyncable* aComp)
{
    setActivated();
    if (aComp) {
#ifdef DES_RGS_VERIFY
	for (auto it = mActive.begin(); it !=  mActive.end(); it++) {
	    assert(aComp != *it);
	}
#endif
	mActive.push_back(aComp);
    }
}
#endif

#ifdef DES_LISTS_SWAP
void Des::onUpdated(MDesSyncable* aComp)
{
//    setUpdated();
}
#else
void Des::onUpdated(MDesSyncable* aComp)
{
    setUpdated();
    if (aComp) {
#ifdef DES_RGS_VERIFY
	for (auto it = mUpdated.begin(); it !=  mUpdated.end(); it++) {
	    assert(aComp != *it);
	}
#endif
	mUpdated.push_back(aComp);
    }
}
#endif

void Des::RmSyncable(TScblReg& aReg, MDesSyncable* aScbl)
{
    aReg.remove(aScbl);
}

void Des::onOwnedAttached(MOwned* aOwned)
{
    Syst::onOwnedAttached(aOwned);
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    if (os) {
	os->setActivated();
#ifndef DES_LISTS_SWAP
	os->setUpdated();
#endif
    }
}

void Des::onOwnedDetached(MOwned* aOwned)
{
    Syst::onOwnedDetached(aOwned);
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    if (os) {
#ifdef DES_LISTS_SWAP
	RmSyncable(*mActive, os);
	RmSyncable(*mUpdated, os);
#else
	RmSyncable(mActive, os);
	RmSyncable(mUpdated, os);
#endif
    }
}

MIface* Des::MOwned_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType));
    else res = Syst::MOwned_getLif(aType);
    return res;
}

MIface* Des::MOwner_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesObserver>(aType)); // Notifying from owned 
    else res = Syst::MOwner_getLif(aType);
    return res;
}

void Des::MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Opt1) {
	Ifu::offset(aIdt, aOs); aOs << "Active:" << endl;
#ifdef DES_LISTS_SWAP
	for (auto item : *mActive) {
#else
	for (auto item : mActive) {
#endif
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    } else {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
#ifdef DES_LISTS_SWAP
	for (auto item : *mUpdated) {
#else
	for (auto item : mUpdated) {
#endif
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
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


///// ADES

ADes::ADes(const string &aType, const string &aName, MEnv* aEnv): Unit(aType, aName, aEnv), mOrCp(this), mAgtCp(this), mUpdNotified(false), mActNotified(false)
{
}

ADes::~ADes()
{
}


MIface* ADes::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MAgent>(aType));
    else if (res = checkLif<MDesObserver>(aType));
    else res = Unit::MNode_getLif(aType);
    return res;
}

MIface* ADes::MAgent_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MUnit>(aType)); // To allow client to request IFR
    else if (res = checkLif<MDesObserver>(aType));
    return res;
}

void ADes::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesCtxSpl::Type()) {
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
	    auto owdCp = ahostNode()->owner()->firstPair();
	    while (owdCp) {
		MDesCtxSpl* spl = owdCp->provided()->lIf(spl);
		if (spl) {
		    MUnit* splu = spl->lIf(splu);
		    MIfProvOwner* splPo = splu ? splu->lIf(splPo) : nullptr;
		    bool isReq = splPo && aReq->provided()->isRequestor(splPo);
		    if (!isReq) {
			addIfpLeaf(spl, aReq);
			MUnit* splu = spl->lIf(splu);
			if (splu) {
			    splu->resolveIface(aName, aReq);
			    redirectedToSpl = true;
			}
		    }
		}
		owdCp = ahostNode()->owner()->nextPair(owdCp);
	    }
	    if (!redirectedToSpl) {
		// Propagate request to owner
		MUnit* ownu = mmo ? mmo->lIf(ownu) : nullptr;
		if (ownu) {
		    ownu->resolveIface(aName, aReq);
		}
	    }
	} else { // Propagate request to owner
	    MUnit* ownu = mmo ? mmo->lIf(ownu) : nullptr;
	    if (ownu) {
		ownu->resolveIface(aName, aReq);
	    }
	}
    } else {
	Unit::resolveIfc(aName, aReq);
    }
}

#ifdef DES_LISTS_SWAP
void ADes::update()
{
    mUpd = true;
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

    mActNotified = false;
    mUpd = false;
}
#else
void ADes::update()
{
    while (!mActive.empty()) {
	auto comp = mActive.front();
	try {
	    comp->update();
	} catch (std::exception e) {
	    LOGN(EErr, "Error on update [" + comp->Uid() + "]");
	}
	mActive.pop_front();
    }
    mActNotified = false;
}
#endif

#ifdef DES_LISTS_SWAP
void ADes::confirm()
{
    for (auto* comp : *mUpdated) {
	comp->confirm();
    }
    mUpdNotified = false;
}
#else
void ADes::confirm()
{
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
    }
    mUpdNotified = false;
}
#endif

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
	MDesObserver* obs = getDesObs();
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	} else {
	    //LOGN(EInfo, "setActivated, observer not found");
	    //obs = getDesObs();
	}
    }
}

#ifdef DES_LISTS_SWAP
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
#else
void ADes::onActivated(MDesSyncable* aComp)
{
    setActivated();
    if (aComp) {
#ifdef DES_RGS_VERIFY
	for (auto comp : mActive) {
	    assert(aComp != comp);
	}
#endif
	mActive.push_back(aComp);
    }
}
#endif

#ifdef DES_LISTS_SWAP
void ADes::onUpdated(MDesSyncable* aComp)
{
//    setUpdated();
}
#else
void ADes::onUpdated(MDesSyncable* aComp)
{
    setUpdated();
    if (aComp) {
	mUpdated.push_back(aComp);
    }
}
#endif

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

MIface* ADes::MOwned_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType));
    else res = Unit::MOwned_getLif(aType);
    return res;
}

MIface* ADes::MObserver_getLif(const char *aType)
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
#ifndef DES_LISTS_SWAP
	os->setUpdated();
#endif
    }
}

static void RmSyncable(list<MDesSyncable*>& aReg, MDesSyncable* aScbl)
{
    aReg.remove(aScbl);
}

void ADes::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    MDesSyncable* ss = MNode::lIf(ss); // self
    if (os && os != ss) {
#ifdef DES_LISTS_SWAP
	RmSyncable(*mActive, os);
	RmSyncable(*mUpdated, os);
#else
	RmSyncable(mActive, os);
	RmSyncable(mUpdated, os);
#endif
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
    auto owdCp = host->owner()->firstPair();
    while (owdCp) {
	MUnit* osu = owdCp->provided()->lIf(osu);
	MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
	if (os && os != this) {
	    os->setActivated();
	}
	owdCp = host->owner()->nextPair(owdCp);
    }
}

void ADes::MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Opt1) {
	Ifu::offset(aIdt, aOs); aOs << "Active:" << endl;
#ifdef DES_LISTS_SWAP
	for (auto item : *mActive) {
#else
	for (auto item : mActive) {
#endif
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    } else {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
#ifdef DES_LISTS_SWAP
	for (auto item : *mUpdated) {
#else
	for (auto item : mUpdated) {
#endif
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    }
}

MNode* ADes::ahostNode()
{
    auto pair = mAgtCp.firstPair();
    MAhost* ahost = pair ? pair->provided() : nullptr;
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    return hostn;
}

MNode* ADes::ahostGetNode(const GUri& aUri)
{
    auto pair = mAgtCp.firstPair();
    MAhost* ahost = pair ? pair->provided() : nullptr;
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    MNode* res = hostn ? hostn->getNode(aUri, this) : nullptr;
    return res;
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
#ifdef DES_LISTS_SWAP
	if (!mActive->empty()) {
#else
	if (!mActive.empty()) {
#endif
	    //updateCounter(cnt); // TODO Is it needed?
	    LOGN(EDbg, ">>> Update [" + to_string(cnt) + "], count: " + to_string(countOfActive()) +
		    ", dur: " + PFL_DUR_VALUE(PEvents::EDurStat_LaunchActive) +
		    ", inv: " + PFL_DUR_STAT_CNT(PEvents::EDurStat_UInvldIrm));
	    PFL_DUR_START(PEvents::EDurStat_LaunchActive);
	    PFL_DUR_STAT_START(PEvents::EDurStat_LaunchActive);
	    PFL_DUR_STAT_START(PEvents::EDurStat_LaunchUpdate);
	    update();
	    PFL_DUR_STAT_REC(PEvents::EDurStat_LaunchUpdate);
	    PFL_DUR_STAT_START(PEvents::EDurStat_LaunchConfirm);
#ifdef DES_LISTS_SWAP
	    if (isLogLevel(EDbg)) {
		LOGN(EDbg, ">>> Confirm [" + to_string(cnt) + "]");
	    }
	    //outputCounter(cnt); // TODO Is it needed?
	    confirm();
#else
	    if (!mUpdated.empty()) {
		LOGN(EDbg, ">>> Confirm [" + to_string(cnt) + "]");
		//outputCounter(cnt); // TODO Is it needed?
		confirm();
	    }
#endif
	    PFL_DUR_STAT_REC(PEvents::EDurStat_LaunchConfirm);
	    cnt++;
	    idlecnt = 0;
	    PFL_DUR_STAT_REC(PEvents::EDurStat_LaunchActive);
	    PFL_DUR_REC(PEvents::EDurStat_LaunchActive);
	} else {
	    if (idlecnt == 0) {
		LOGN(EInfo, "IDLE");
	    }
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

    MIface* DesLauncher::MOwned_getLif(const char *aType)
    {
	MIface* res = nullptr;
	if (res = checkLif<MLauncher>(aType));
	else res = Des::MOwned_getLif(aType);
	return res;
    }

    MIface* DesLauncher::MNode_getLif(const char *aType)
    {
	MIface* res = nullptr;
	if (res = checkLif<MLauncher>(aType));
	else res = Des::MNode_getLif(aType);
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


static const string K_SsInitUri = "Init";
static const string K_SsUri = "Subsys";

/// Active subsystem of DES

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
	/*
	MNode* ss = nullptr;
	auto owdCp = owner()->firstPair();
	while (owdCp) {
	    MNode* compn = owdCp->provided()->lIf(compn);
	    MDesSyncable* comps = compn ? compn->lIf(comps) : nullptr;
	    if (comps) {
		if (ss == nullptr) {
		    ss = compn;
		} else {
		    // TODO Is this limination really needed?
		    LOGN(EErr, "Subsystems number > 1");
		}
	    }
	    owdCp = owner()->nextPair(owdCp);
	}
	if (ss == nullptr) {
	    LOGN(EErr, "No subsystems found");
	    break;
	}
	// Initiate subsystem, ref ds_desas_sis_iph
	//MNode* ss = owner()->firstPair()->provided()->lIf(ss);
	*/
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
#ifdef DES_LISTS_SWAP
	if (!mActive->empty()) {
	    LOGN(EInfo, ">>> Init update");
	    Des::update();
	    if (!mUpdated->empty()) {
		LOGN(EInfo, ">>> Init confirm");
		Des::confirm();
	    }
	}
#else
	if (!mActive.empty()) {
	    LOGN(EInfo, ">>> Init update");
	    Des::update();
	    if (!mUpdated.empty()) {
		LOGN(EInfo, ">>> Init confirm");
		Des::confirm();
	    }
	}
#endif
	cntInit->setContent("", "SB false");
	// Run subsystem
	while (!mStop && (aCount == 0 || cnt < aCount) && (aIdleCount == 0 || idlecnt < aIdleCount)) {
#ifdef DES_LISTS_SWAP
	    if (!mActive->empty()) {
		updateCounter(cnt);
		LOGN(EInfo, ">>> Update [" + to_string(cnt) + "]");
		Des::update();
		if (!mUpdated->empty()) {
		    LOGN(EInfo, ">>> Confirm [" + to_string(cnt) + "]");
		    outputCounter(cnt);
		    Des::confirm();
		}
		cnt++;
	    } else {
		OnIdle();
		idlecnt++;
	    }

#else
	    if (!mActive.empty()) {
		updateCounter(cnt);
		LOGN(EInfo, ">>> Update [" + to_string(cnt) + "]");
		Des::update();
		if (!mUpdated.empty()) {
		    LOGN(EInfo, ">>> Confirm [" + to_string(cnt) + "]");
		    outputCounter(cnt);
		    Des::confirm();
		}
		cnt++;
	    } else {
		OnIdle();
		idlecnt++;
	    }
#endif
	}
    } while (false);
    return res;
}

void DesAs::update()
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
    if (!res) this->eHost()->logEmb(TLogRecCtg::EDbg, TLog(TP::mHost) + "Cannot get input [" + this->mUri + "]");
    else {
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

MIface* DesCtxSpl::MNode_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesCtxSpl>(aType));
    else res = Des::MNode_getLif(aType);
    return res;
}

MIface* DesCtxSpl::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesCtxSpl>(aType));
    else res = Des::MOwned_getLif(aType);
    return res;
}

MIface* DesCtxSpl::MDesCtxSpl_getLif(const char *aType)
{
    return checkLif<MUnit>(aType); // To enable IFR
}

void DesCtxSpl::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesCtxSpl::Type()) {
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
		ownu->resolveIface(aName, aReq);
	    }
	}
    } else {
	Des::resolveIfc(aName, aReq);
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

MIface* DesCtxCsm::MNode_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesCtxCsm>(aType));
    else res = Des::MNode_getLif(aType);
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
#ifndef DES_LISTS_SWAP
    if (!mInitialized) {
	mInitFailed = !init();
	if (mInitFailed) {
	    LOGN(EErr, "Init failed");
	}
	mInitialized = true;
    }
#endif
}

void DesCtxCsm::confirm()
{
    Des::confirm();
#ifdef DES_LISTS_SWAP
    if (!mInitialized) {
	mInitFailed = !init();
	if (mInitFailed) {
	    LOGN(EErr, "Init failed");
	}
	mInitialized = true;
    }
#endif
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
    assert(!mCsmCp.firstPair());
    bool res = mCsmCp.connect(aSpl);
    return res;
}

bool DesCtxCsm::bindCtxs()
{
    bool res = false;
    auto owdCp = owner()->firstPair();
    while (owdCp) {
	MNode* compn = owdCp->provided()->lIf(compn);
	MVert* compv = compn ? compn->lIf(compv) : nullptr;
	if (compv) {
	    MVert* extd = compv->getExtd();
	    if (extd) {
		res = mCsmCp.firstPair()->provided()->bindCtx(compn->name(), extd);
		if (!res) break;
	    }
	}
	owdCp = owner()->nextPair(owdCp);
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
    MNode* cp = Provider()->createNode(CpStateInp::Type(), aName, mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

void DesInpDemux::AddOutput(const string& aName)
{
    MNode* cp = Provider()->createNode(CpStateOutp::Type(), aName, mEnv);
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

void DesInpDemux::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDVarGet::Type()) {
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
    } else if (aName == MDesObserver::Type()) {
	MNode* inp = getNode(K_Cp_Inp);
	MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	MIfProvOwner* inppo = inpu ? inpu->lIf(inppo) : nullptr;
	if (inppo && aReq->provided()->isRequestor(inppo)) {
	    // Request from input, redirect to output
	    MNode* outp = getNode(K_Cp_Outp);
	    MUnit* outpu = outp ? outp->lIf(outpu) : nullptr;
	    outpu->resolveIface(aName, aReq);
	}
    } else {
	Des::resolveIfc(aName, aReq);
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
