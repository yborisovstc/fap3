
#include <thread>

#include "mlink.h"
#include "des.h"
#include "data.h"

#define DES_RGS_VERIFY

const string KCont_Provided = "Provided";
const string KCont_Required = "Required";

CpState::CpState(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
}

void CpState::notifyInpsUpdated()
{
    MIfProv* ifp = defaultIfProv(MDesInpObserver::Type());
    MIfProv* prov = ifp->first();
    while (prov) {
	MDesInpObserver* obs = dynamic_cast<MDesInpObserver*>(prov->iface());
	if (obs) obs->onInpUpdated();
	prov = prov->next();
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

CpStateInp::CpStateInp(const string &aType, const string& aName, MEnv* aEnv): CpState(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MDesInpObserver");
    res = setContent("Required", "MDVarGet");
    assert(res);
}

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


/// CpStateOutp direct extender 

ExtdStateOutp::ExtdStateOutp(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), Extd::KUriInt , mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
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


const string State::KCont_Value = "";

#ifdef STATE_DIOR
void State::SDior::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	for (auto pair : mHost->mPairs) {
	    MUnit* pe = pair->lIf(pe);
	    pe->resolveIface(aName, aReq);
	}
    }
}

void State::SDior::onIfpInvalidated(MIfProv* aProv)
{
    Unit::onIfpInvalidated(aProv);
    mHost->setActivated();
    mHost->NotifyInpsUpdated();
}
#endif

State::State(const string &aType, const string& aName, MEnv* aEnv): Vertu(aType, aName, aEnv),
    mPdata(NULL), mCdata(NULL), mUpdNotified(false), mActNotified(false)
#ifdef STATE_DIOR
								    , mDior(nullptr)
#endif
{
    mPdata = new BdVar(this);
    mCdata = new BdVar(this);
    MNode* cp = Provider()->createNode(CpStateInp::Type(), "Inp", mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
#ifdef STATE_DIOR
    mDior = new SDior(SDior::Type(), "Dior", mEnv, this);
    assert(mDior);
    res = attachOwned(mDior);
    assert(res);
#endif
}

MIface* State::MNode_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MDesInpObserver>(aType));
    else if (res = checkLif<MConnPoint>(aType));
    else if (res = checkLifs<MDVarGet>(aType, dynamic_cast<MDVarGet*>(mCdata)));
    else if (res = checkLif<MDVarSet>(aType));
    else res = Vertu::MNode_getLif(aType);
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
    if (mPdata) {
	try {
	    mPdata->update(); // set updated in HOnDataChanged
	} catch (std::exception e) {
	    Logger()->Write(EErr, this, "Unspecified error on update");
	}
    }
}

void State::NotifyInpsUpdated()
{
#ifndef STATE_DIOR 
    // It would be better to request MDesInpObserver interface from self. But there is the problem
    // with using this approach because self implements this iface. So accoriding to iface resolution
    // rules UpdateIfi has to try local ifaces first, so self will be selected. Solution here could be
    // not following this rule and try pairs first for MDesObserver. But we choose another solution ATM -
    // to request pairs directly.
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	auto ifcs = pe->getIfs<MDesInpObserver>();
	if (ifcs) for (auto ifc : *ifcs) {
	    MDesInpObserver* obs = static_cast<MDesInpObserver*>(ifc);
	    obs->onInpUpdated();
	}

    }
#else
    auto ifcs = mDior->getIfs<MDesInpObserver>();
    if (ifcs) for (auto ifc : *ifcs) {
	MDesInpObserver* obs = static_cast<MDesInpObserver*>(ifc);
	if (obs) obs->onInpUpdated();
	else {
	    obs = ifc->lIf(obs);
	}
    }
#endif
}

void State::confirm()
{
    mUpdNotified = false;
    if (mCdata) {
	string old_value;
	if (isLogLevel(EDbg)) {
	    mCdata->ToString(old_value);
	}
	if (mCdata->update()) {
	    NotifyInpsUpdated();
	    if (isLogLevel(EDbg)) {
		string new_value;
		mCdata->ToString(new_value);
		Logger()->Write(EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
	    }
	} else {
	    // State is not changed. No need to notify connected inps. But we still need to make IFR paths to inps
	    // actual. Ref ds_asr.
	    // TODO PERF
	    refreshInpObsIfr();
	}
    }
}

void State::setUpdated()
{
    //MDesObserver* obs = Owner() ? Owner()->lIf(obs) : nullptr;
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs && !mUpdNotified) {
	obs->onUpdated(this);
	mUpdNotified = true;
    }
}

void State::onInpUpdated()
{
    setActivated();
}

MDVarGet* State::HGetInp(const void* aRmt)
{
    MDVarGet* res = NULL;
    if (aRmt == mPdata) {
	MNode* inpn = getNode("Inp");
	MUnit* inpu = inpn->lIf(inpu);
	MIfProv* difp = inpu->defaultIfProv(MDVarGet::Type());
	MIfProv* ifp = difp ? difp->first() : nullptr;
	if (ifp) {
	    res = dynamic_cast<MDVarGet*>(ifp->iface());
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input");
	}
    } else {
	res = mPdata->MDVar::lIf(res);
    }
    return res;
}

void State::HOnDataChanged(const void* aRmt)
{
    if (aRmt == mPdata) {
	setUpdated();
    }
}

string State::provName() const
{
    return MDVarGet::Type();
}

string State::reqName() const
{
    return MDesInpObserver::Type();
}
	
string State::VarGetSIfid()
{
    return mPdata == NULL ? string() : mPdata->VarGetSIfid();
}

MIface *State::DoGetSDObj(const char *aName)
{
    return mPdata == NULL ? NULL : mPdata->DoGetSDObj(aName);
}

MIface* State::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else res = Unit::MOwned_getLif(aType);
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

void State::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == provName()) {
	MIface* ifr = mCdata->MDVar_getLif(aName.c_str());
	if (ifr && !aReq->binded()->provided()->findIface(ifr)) {
	    addIfpLeaf(ifr, aReq);
	}
    } else {
	Vertu::resolveIfc(aName, aReq);
    }
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

string State::GetDvarUid(const MDVar* aComp) const
{
    string name =  (aComp == mPdata) ? "Pdata" : "Cdata";
    return getUid(name, MDVar::Type()); 
}


bool State::SContValue::getData(string& aData) const
{
    mHost.mCdata->ToString(aData);
    return true;
}

bool State::SContValue::setData(const string& aData)
{
    bool res = mHost.mPdata->FromString(aData);
    res = res && mHost.mCdata->FromString(aData);
    if (mHost.mPdata->IsValid() && mHost.mCdata->IsValid()) {
	if (res) {
	    mHost.NotifyInpsUpdated();
	}
    }  else {
	mHost.Log(TLog(EDbg2, &mHost) + "Error on applying content [" + mName + "] value [" + aData + "]");
	res = mHost.mPdata->FromString(aData);
    }
    return res;
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

void State::onConnected()
{
    Vertu::onConnected();
#ifdef STATE_DIOR
    mDior->onHostConnectionChange();
#endif
    NotifyInpsUpdated();
}

void State::onDisconnected()
{
    Vertu::onDisconnected();
#ifdef STATE_DIOR
    mDior->onHostConnectionChange();
#endif
    NotifyInpsUpdated();
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

void Des::update()
{
    while (!mActive.empty()) {
	auto comp = mActive.front();
	try {
	    comp->update();
	} catch (std::exception e) {
	    Log(TLog(EErr, this) + "Error on update [" + comp->Uid() + "]");
	}
	mActive.pop_front();
    }
    mActNotified = false;
}

void Des::confirm()
{
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
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
	MUnit* ownu = Owner() ? Owner()->lIf(ownu) : nullptr;
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
    }
}

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

static void RmSyncable(list<MDesSyncable*>& aList, MDesSyncable* aScbl)
{
    aList.remove(aScbl);
}

void Des::onOwnedAttached(MOwned* aOwned)
{
    Syst::onOwnedAttached(aOwned);
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    if (os) {
	os->setActivated();
	os->setUpdated();
    }
}

void Des::onOwnedDetached(MOwned* aOwned)
{
    Syst::onOwnedDetached(aOwned);
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    if (os) {
	RmSyncable(mActive, os);
	RmSyncable(mUpdated, os);
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
	for (auto item : mActive) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    } else {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
	for (auto item : mUpdated) {
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

void ADes::update()
{
    while (!mActive.empty()) {
	auto comp = mActive.front();
	try {
	    comp->update();
	} catch (std::exception e) {
	    Log(TLog(EErr, this) + "Error on update [" + comp->Uid() + "]");
	}
	mActive.pop_front();
    }
    mActNotified = false;
}

void ADes::confirm()
{
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
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
	    //Log(TLog(EInfo, this) + "setUpdated, observer: " + obs->Uid());
	    //obs = getDesObs();
	} else {
	    //Log(TLog(EInfo, this) + "setUpdated, observer not found");
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
	    //Log(TLog(EInfo, this) + "setActivated, observer not found");
	    //obs = getDesObs();
	}
    }
}

void ADes::onActivated(MDesSyncable* aComp)
{
    setActivated();
    if (aComp) {
	mActive.push_back(aComp);
    }
}

void ADes::onUpdated(MDesSyncable* aComp)
{
    setUpdated();
    if (aComp) {
	mUpdated.push_back(aComp);
    }
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
	os->setUpdated();
    }
}

void ADes::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    MDesSyncable* ss = MNode::lIf(ss); // self
    if (os && os != ss) {
	RmSyncable(mActive, os);
	RmSyncable(mUpdated, os);
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
	    onActivated(os);
	}
	owdCp = host->owner()->nextPair(owdCp);
    }
}

void ADes::MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Opt1) {
	Ifu::offset(aIdt, aOs); aOs << "Active:" << endl;
	for (auto item : mActive) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    } else {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
	for (auto item : mUpdated) {
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
    while (!mStop && (aCount == 0 || cnt < aCount) && (aIdleCount == 0 || idlecnt < aIdleCount)) {
	if (!mActive.empty()) {
	    updateCounter(cnt);
	    Log(TLog(EInfo, this) + ">>> Update [" + to_string(cnt) + "]");
	    update();
	    if (!mUpdated.empty()) {
		Log(TLog(EInfo, this) + ">>> Confirm [" + to_string(cnt) + "]");
		outputCounter(cnt);
		confirm();
	    }
	    cnt++;
	} else {
	    OnIdle();
	    idlecnt++;
	}
    }
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
    if (!res) this->eHost()->logEmb(TLog(TLogRecCtg::EDbg, TP::mHost) + "Cannot get input [" + this->mUri + "]");
    else { this->mActivated = false; this->setUpdated(); }
}


/// Embedded Output state

void DesEOstb::NotifyInpsUpdated()
{
    MNode* cp = mHost->getNode(mCpUri);
    MUnit* cpu = cp ? cp->lIf(cpu) : nullptr;
    auto ifaces = cpu->getIfs<MDesInpObserver>();
    if (ifaces) for (auto ifc : *ifaces) {
	MDesInpObserver* ifco = dynamic_cast<MDesInpObserver*>(ifc);
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
		    MDesCtxSpl* spl = dynamic_cast<MDesCtxSpl*>(ifc);
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
		MDesCtxSpl* spl = dynamic_cast<MDesCtxSpl*>(ifc);
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
    if (!mInitialized) {
	mInitFailed = !init();
	if (mInitFailed) {
	    Log(TLog(EErr, this) + "Init failed");
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
	    MDesCtxSpl* spl = dynamic_cast<MDesCtxSpl*>(ifc);
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
		MDesCtxSpl* spl = dynamic_cast<MDesCtxSpl*>(ifc);
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


