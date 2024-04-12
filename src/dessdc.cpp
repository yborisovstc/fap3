#include "des.h"
#include "dessdc.h"
#include "prof_ids.h"


ASdc::NodeCreationObserver::NodeCreationObserver(ASdc* aHost):
    mHost(aHost), mOcp(this)
{
}

void ASdc::NodeCreationObserver::startObserving(const GUri& aTargUri)
{
    mTargUri = aTargUri;
    GUri targOwrUri;
    mTargOwrLevel = mTargUri.size() - 1;
    MNode* owner = nullptr;
    do {
	targOwrUri = mTargUri.head(mTargOwrLevel);
	owner = mHost->mMag->getNode(targOwrUri);
	if (!owner) mTargOwrLevel--;
    } while (!owner);
    if (owner != mTargOwr) {
	LOGNN(mHost, EDbg, "Owner [" + targOwrUri.toString() + "] to be observing, level: " + to_string(mTargOwrLevel));
	if (mTargOwr) {
	    MObservable* obl = mTargOwr->lIf(obl);
	    bool res = obl->rmObserver(&mOcp);
	    if (!res || !obl) {
		LOGNN(mHost, EErr, "Failed deattaching VertUeOwr from observable");
	    }
	}
	mTargOwr = owner;
	MObservable* obl = mTargOwr->lIf(obl);
	bool res = obl ? obl->addObserver(&mOcp) : false;
	if (!res || !obl) {
            LOGNN(mHost, EErr, "Cannot attach VertUeOwr to observer");
	} else {
            LOGNN(mHost, EDbg, "Owner [" + mTargOwr->getUriS(mHost->mMag) + "] observing, level: " + to_string(mTargOwrLevel));
	}
    }
}

void ASdc::NodeCreationObserver::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    LOGNN(mHost, EDbg, "onObsOwnedAttached, owned: " + aOwned->Uid());
    GUri owdUri = mTargUri.head(mTargOwrLevel + 1);
    auto* targOwrOwd = mHost->mMag->getNode(owdUri);
    MOwned* targOwd = targOwrOwd ? targOwrOwd->lIf(targOwd) : nullptr;
    if (targOwd && aOwned == targOwd) {
        LOGNN(mHost, EDbg, "[" + mTargOwr->getUriS(mHost->mMag) + "] owned [" + targOwrOwd->getUriS(mHost->mMag) + "] attached");
        LOGNN(mHost, EDbg, "Targ URI: " + mTargUri.toString());
        // Checking if target got attached
        MNode* targ = mHost->mMag->getNode(mTargUri);
        if (targ) {
            // Yes, attached. Stop observing the attaching
            LOGNN(mHost, EDbg, "Target [" + targ->getUriS(mHost->mMag) + "] got attached");
            mHost->setActivated();
        } else {
            // Not attached yet, proceed
            startObserving(mTargUri);
        }
    }
}



/* SDC base agent */

const string K_CpUri_Enable = "Enable";
const string K_CpUri_Outp = "Outp";

void ASdc::MagDobs::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    if (mMask & EO_DTCH) mHost->notifyOutp();
}

void ASdc::MagDobs::updateNuo(MNode* aNuo)
{
    assert(aNuo);
    /*
       if (aNuo != mNuo) {
       MObservable* nuo = mNuo ? mNuo->lIf(nuo) : nullptr;
       if (nuo) nuo->rmObserver(&mOcp);
       mNuo = aNuo;
       nuo = mNuo->lIf(nuo);
       if (nuo) nuo->addObserver(&mOcp);
       }
       */
    mOcp.disconnectAll();
    MObservable* nuo = aNuo->lIf(nuo);
    if (nuo) nuo->addObserver(&mOcp);
}

ASdc::SdcIapb::SdcIapb(const string& aName, ASdc* aHost, const string& aInpUri):
    mName(aName), mHost(aHost), mInpUri(aInpUri)
{
    mHost->registerIap(this);
}

ASdc::SdcPapb::SdcPapb(const string& aName, ASdc* aHost, const string& aCpUri):
    mName(aName), mHost(aHost), mCpUri(aCpUri)
{
    mHost->registerPap(this);
}

template <typename T> string toStr(const T& aData) { return to_string(aData); }

string toStr(const string& aData) { return aData;}

    template <class T>
T& ASdc::SdcIap<T>::data(bool aConf)
{ 
    if (aConf) return mCdt;
    else {
	updateData();
	return mUdt;
    }
}

    template <class T>
bool ASdc::SdcIap<T>::updateData()
{
    T old_data;
    if (mHost->isLogLevel(EDbg)) {
        old_data = mUdt;
    }
    bool res = mHost->GetInpData<T>(mInpUri, mUdt);
    if (mHost->isLogLevel(EDbg)) {
        if (mUdt != old_data) {
            LOGNN(mHost, EDbg, "[" + mName + "] Updated: [" + toStr(old_data.mData) + "] -> [" + toStr(mUdt.mData) + "]");
        }
    }
    return res;
}

bool ASdc::SdcIapEnb::updateData()
{
    bool res = false;
    MNode* inp = mHost->getNode(mInpUri);
    MVert* inpv = inp ? inp->lIf(inpv) : nullptr;
    MUnit* vgetu = inp->lIf(vgetu);
    auto ifaces = vgetu->getIfs<MDVarGet>();
    // We need to interpret not-connecting "enable" in favor of "disable"
    // So using workaround here
    Sdata<bool> old_data;
    if (mHost->isLogLevel(EDbg)) {
        old_data = mUdt;
    }
    if (ifaces && ifaces->size() >= inpv->pairsCount()) {
	bool first = true;
	if (ifaces) for (auto ifc : *ifaces) {
	    MDVarGet* vget = reinterpret_cast<MDVarGet*>(ifc);
	    const Sdata<bool>* st = vget->DtGet(st);
	    if (st) {
		if (first) mUdt = *st;
		else {
		    mUdt.mData &= st->mData;
		    mUdt.mValid &= st->mValid;
		}
		first = false;
		res = true;
		if (!st->IsValid()) {
		    break;
		}
	    }
	}
    } else {
	// Disconnected "enable"
	mUdt.mData = false;
	mUdt.mValid = true;
    }
    if (mHost->isLogLevel(EDbg)) {
        if (mUdt != old_data) {
            LOGNN(mHost, EDbg, "[" + mName + "] Updated: [" + toStr(old_data.mData) + "] -> [" + toStr(mUdt.mData) + "]");
        }
    }
    return res;
}

    template <class T>
bool ASdc::SdcIapg<T>::updateData()
{
    T old_data = mUdt;
    bool res = mHost->GetInpData<T>(mInpUri, mUdt);
    if (mUdt != old_data) {
        LOGNN(mHost, EDbg, "[" + mName + "] Updated: [" + old_data.ToString() + "] -> [" + mUdt.ToString() + "]");
    }
    return res;
}

    template <class T>
T& ASdc::SdcIapg<T>::data(bool aConf)
{ 
    if (aConf) return mCdt;
    else {
	updateData();
	return mUdt;
    }
}


ASdc::ASdc(const string &aType, const string& aName, MEnv* aEnv): Unit(aType, aName, aEnv),
    mIaps(), mMag(NULL), mUpdNotified(false), mActNotified(false), mObrCp(this), mIapEnb("Enb", this, K_CpUri_Enable),
    mOapOut("Outp", this, K_CpUri_Outp, [this](Sdata<bool>& aData) {getOut(aData);}),
    mMagObs(this), mCdone(false)
{
    mIapEnb.mUdt = false;
    mIapEnb.mCdt = false;
}

ASdc::~ASdc()
{
}

MIface* ASdc::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));

    else if (res = checkLif<MDesInpObserver>(aType));
    else res = Unit::MNode_getLif(aType);
    return res;
}

bool ASdc::rifDesIobs(SdcIapb& aIap, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
    MNode* cp = getNode(aIap.mInpUri);
    if (isRequestor(aReq, cp)) {
	MIface* iface = dynamic_cast<MDesInpObserver*>(&aIap);
	addIfpLeaf(iface, aReq);
	res = true;
    }
    return res;
}

bool ASdc::rifDesPaps(SdcPapb& aPap, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
    MNode* cp = getNode(aPap.getCpUri());
    if (isRequestor(aReq, cp)) {
	MIface* iface = dynamic_cast<MDVarGet*>(&aPap);
	addIfpLeaf(iface, aReq);
	res = true;
    }
    return res;
}


void ASdc::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	for (auto iap : mIaps) {
	    rifDesIobs(*iap, aReq);
	}
#ifndef DES_IFR_DESOBS
    } else if (aName == MDesObserver::Type()) {
	MIface* iface = MNode_getLif(MDesObserver::Type());
	addIfpLeaf(iface, aReq);
#else
    } else if (aName == MDesObserver::Type()) {
        // For owned - self, for self - owning
        bool bndHasPairs = (aReq->binded()->pairsBegin() != aReq->binded()->pairsEnd());
        MIfReq::TIfReqCp* req = bndHasPairs ? *aReq->binded()->pairsBegin() : nullptr;
        const MIfProvOwner* reqo = req ? req->provided()->rqOwner() : nullptr;
        const MNode* reqn = reqo ? reqo->lIf(reqn) : nullptr; // Current requestor as node
        const MOwned* reqowd = reqn ? reqn->lIf(reqowd) : nullptr; // Requestor as owned
        //if (reqn && isNodeOwnedInd(reqn)) {
        if (reqowd && isOwned(reqowd)) {
            // Requestor from owned - resolve as self
            auto* ifc = MNode_getLif(aName.c_str());
            if (ifc) {
                addIfpLeaf(ifc, aReq);
            }
        } else {
            // Self requestor or no requestor - redirect to owning
            MUnit* owru = Owner() ? Owner()->lIf(owru) : nullptr;
            if (owru) {
		owru->resolveIface(aName, aReq);
            }
        }
#endif
    } else if (aName == MDVarGet::Type()) {
	for (auto pap : mPaps) {
	    rifDesPaps(*pap, aReq);
	}
    } else {
	Unit::resolveIfc(aName, aReq);
    }
}

void ASdc::addInput(const string& aName)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), aName, mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

void ASdc::addOutput(const string& aName)
{
    MNode* cp = Provider()->createNode(CpStateOutp::Type(), aName, mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

void ASdc::update()
{
    for (auto iap : mIaps) {
	    iap->updateData();
	    /*
	if (iap->mActivated) {
	    iap->update();
	}
	*/
    }
    mActNotified = false;
#ifndef DES_LISTS_SWAP
    setUpdated();
#endif
}

bool ASdc::areInpsValid() const
{
    bool res = true;
    for (auto iap : mIaps) {
	if (!iap->isDataValid()) {
	    res = false; break;
	}
    }
    return res;
}

void ASdc::confirm()
{
    PFL_DUR_STAT_START(PEvents::EDurStat_ASdcConfirm);
    if (mMag && areInpsValid() && mIapEnb.data(true).mData) { // Ref ds_dcs_sdc_dsgn_oin Solution#1
        PFL_DUR_STAT_START(PEvents::EDurStat_ASdcConfState);
        bool state = getState(true);
        PFL_DUR_STAT_REC(PEvents::EDurStat_ASdcConfState);
        if (!state) { // Ref ds_dcs_sdc_dsgn_cc Solution#2
            PFL_DUR_STAT_START(PEvents::EDurStat_ASdcConfCtl);
            bool res = doCtl();
            PFL_DUR_STAT_REC(PEvents::EDurStat_ASdcConfCtl);
            if (!res) {
                LOGN(EErr, "Failed controlling managed agent");
            } else {
                mCdone = true;
                notifyOutp();
            }
        }
    }
    mUpdNotified = false;
    PFL_DUR_STAT_REC(PEvents::EDurStat_ASdcConfirm);
}

void ASdc::setActivated()
{
    if (!mActNotified) {
#ifndef DES_IFR_DESOBS
	MUnit* ownu = Owner()->lIf(ownu);
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
	// System is activated, this means some inputs are notified of change
	// Status transition potentially depends on any inputs so we need to
	// propagate notification to output
	notifyOutp();
#else
        if (!mDobsIfProv) {
            mDobsIfProv = defaultIfProv(MDesObserver::Type());
        }
        auto* ifcs = mDobsIfProv->ifaces();
        auto* obs = ifcs->size() ? reinterpret_cast<MDesObserver*>(ifcs->at(0)) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
            // System is activated, this means some inputs are notified of change
            // Status transition potentially depends on any inputs so we need to
            // propagate notification to output
            notifyOutp();
	} else {
	    LOGN(EInfo, "setActivated, observer not found");
        }
#endif
    }
}

void ASdc::setUpdated()
{
    if (!mUpdNotified) { // Notify owner
	MUnit* ownu = Owner()->lIf(ownu);
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	}
    }
}

void ASdc::onActivated(MDesSyncable* aComp)
{
    if (!mActNotified) { // Notify owner
	MDesObserver* obs = Owner() ? Owner()->lIf(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
    }
}

void ASdc::onUpdated(MDesSyncable* aComp)
{
    // Should not be called
    assert(false);
}

void ASdc::notifyOutp()
{
    mOapOut.NotifyInpsUpdated();
    mOutCInv = true;
}

void ASdc::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
}

void ASdc::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
}

void ASdc::onObsContentChanged(MObservable* aObl, const MContent* aCont)
{
}

void ASdc::onObsChanged(MObservable* aObl)
{
    UpdateMag();
}

MIface* ASdc::MObserver_getLif(const char *aType)
{
    MIface* res = nullptr;
    return res;
}

void ASdc::getOut(Sdata<bool>& aData)
{
    if (mOutCInv) {
	// Form status taking into acc control completion, ref ds_dcs_sdc_dsgn_cc
	aData.mData = getState();
	aData.mValid = true;
	LOGN(EDbg, string("Outp: ") + (aData.mData ? "true" : "false"));
	mOutCInv = false;
    }
}

void ASdc::onOwnerAttached()
{
    bool res = false;
    MObservable* obl = Owner()->lIf(obl);
    if (obl) {
	res = obl->addObserver(&mObrCp);
    }
    if (!res || !obl) {
	Logger()->Write(EErr, this, "Cannot attach to observer");
    } else {
	// Getting controllable
	UpdateMag();
    }
}

void ASdc::UpdateMag()
{
    // Attempt MDesAdapter local iface first
    MDesAdapter* desa = Owner()->lIf(desa);
    if (!desa) {
	// Then try IFR
	MUnit* ownru = Owner()->lIf(ownru);
	if (ownru) {
	    desa = ownru->getSif(desa);
	    LOGN(EDbg, "desa");
	}
    }
    MNode* mag = nullptr;
    if (desa) {
	mag = desa->getMag();
    } 
    if (!mag) {
	// Then explorable of owner
	mag = Owner()->lIf(mag);
    }
    if (!mag) {
	//LOGN(EErr, "Cannot get owners explorable");
    } else {
	// Set explored system
	if (mag != mMag) {
	    mMag = mag;
	    notifyOutp();
	    LOGN(EDbg, "Controllable is attached [" + mMag->Uid() + "]");
	}
    }
}


template<typename T> bool ASdc::GetInpSdata(const string aInpUri, T& aRes)
{
    bool res = false;
    MNode* inp = getNode(aInpUri);
    if (inp) {
	res =  GetSData(inp, aRes);
    } else {
	LOGN(EDbg, "Cannot get input [" + aInpUri + "]");
    }
    return res;
}

template<typename T> bool ASdc::GetInpData(const string aInpUri, T& aRes)
{
    bool res = false;
    MNode* inp = getNode(aInpUri);
    if (inp) {
	/*
	   T data;
	   res =  GetGData(inp, data);
	   if (res) {
	   aRes = data;
	   }
	   */
	GetGData(inp, aRes);
    } else {
	LOGN(EDbg, "Cannot get input [" + aInpUri + "]");
    }
    return res;
}

bool ASdc::registerIap(SdcIapb* aIap)
{
    addInput(aIap->getInpUri());
    mIaps.push_back(aIap);
    return true;
}

bool ASdc::registerPap(SdcPapb* aPap)
{
    addOutput(aPap->getCpUri());
    mPaps.push_back(aPap);
    return true;
}


void ASdc::SdcPapb::NotifyInpsUpdated()
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









/* SDC agent "Mutation" */

const string K_CpUri_Targ = "Target";
const string K_CpUri_Mut = "Mut";

ASdcMut::ASdcMut(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapTarg("Targ", this, K_CpUri_Targ), mIapMut("Mut", this, K_CpUri_Mut), mMutApplied(false)
{
}

bool ASdcMut::getState(bool aConf)
{
    return mMutApplied;
}

bool ASdcMut::doCtl()
{
    bool res = false;
    MNode* targn = mMag->getNode(mIapTarg.mCdt.mData);
    if (!targn) {
	LOGN(EInfo, "Cannot find target  [" + mIapTarg.mCdt.mData + "]");
    } else {
	Chromo2& chromo = mIapMut.mCdt.mData;
	TNs ns; MutCtx mutctx(NULL, ns);
	//chromo.Root().Dump(); // Debug
	targn->mutate(chromo.Root(), false, mutctx, true);
	string muts;
	chromo.Root().ToString(muts);
	LOGN(EInfo, "Managed agent is mutated  [" + muts + "]");
	res = true;
    }
    return res;
}


/* SDC agent "Adding Component" */

const string K_CpUri_Name = "Name";
const string K_CpUri_Parent = "Parent";
const string K_CpUri_OutpName = "OutpName";

ASdcComp::ASdcComp(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Name), mIapParent("Parent", this, K_CpUri_Parent)
{ }

bool ASdcComp::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	Sdata<string>& name = mIapName.data(aConf);
	if (name.IsValid()) {
	    res = mMag->getComp(name.mData);
	}
    } else {
	LOGN(EDbg, "Managed agent is not set");
    }
    return res;
}

bool ASdcComp::doCtl()
{
    bool res = false;
 //   PFL_DUR_STAT_START(PEvents::EDurStat_ASdcCtlCmp);
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    TMut mut(ENt_Node, ENa_Id, mIapName.mCdt.mData, ENa_Parent, mIapParent.mCdt.mData);
    chr->Root().AddChild(mut);
    PFL_DUR_STAT_START(PEvents::EDurStat_ASdcCtlCmp);
    mMag->mutate(chr->Root(), false, mutctx, true);
    PFL_DUR_STAT_REC(PEvents::EDurStat_ASdcCtlCmp);
    delete chr;
    string muts = mut.ToString();
    LOGN(EInfo, "Managed agent is mutated [" + muts + "]");
    res = true;
 //   PFL_DUR_STAT_REC(PEvents::EDurStat_ASdcCtlCmp);
    return res;
}

void ASdcComp::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    mOapOut.NotifyInpsUpdated();
}

void ASdcComp::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    mOapOut.NotifyInpsUpdated();
}

/* SDC agent "Adding Component into target" */

const string K_CpUri_Target = "Target";

ASdcCompT::ASdcCompT(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Name), mIapParent("Parent", this, K_CpUri_Parent),
    mIapTarg("Target", this, K_CpUri_Target)
{ }

bool ASdcCompT::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
        Sdata<string>& name = mIapName.data(aConf);
        if (name.IsValid()) {
            DGuri& targu = mIapTarg.data(aConf);
            if (targu.IsValid()) {
                MNode* targn = mMag->getNode(targu.mData);
                if (targn == nullptr) {
                    LOGN(EDbg, "Cannot find target [" + targu.mData.toString() + "]");
                } else {
                    res = targn->getComp(name.mData);
                }
            }
        }
    } else {
        LOGN(EDbg, "Managed agent is not set");
    }
    return res;
}

bool ASdcCompT::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    TMut mut(ENt_Node, ENa_Targ, mIapTarg.mCdt.mData, ENa_Id, mIapName.mCdt.mData, ENa_Parent, mIapParent.mCdt.mData);
    chr->Root().AddChild(mut);
    mMag->mutate(chr->Root(), false, mutctx, true);
    delete chr;
    string muts = mut.ToString();
    LOGN(EInfo, "Managed agent is mutated  [" + muts + "]");
    res = true;
    return res;
}

void ASdcCompT::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    mOapOut.NotifyInpsUpdated();
}

void ASdcCompT::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    mOapOut.NotifyInpsUpdated();
}



/* SDC agent "Rm Component" */

const string K_CpUri_Rm_Name = "Name";
const string K_CpUri_Rm_OutpName = "OutpName";

ASdcRm::ASdcRm(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Name)
{ }

bool ASdcRm::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	Sdata<string>& name = mIapName.data(aConf);
	if (name.IsValid()) {
	    res = !mMag->getComp(name.mData);
	}
    } else {
	LOGN(EDbg, "Managed agent is not set");
    }
    return res;
}

bool ASdcRm::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    TMut mut(ENt_Rm, ENa_Id, mIapName.mCdt.mData);
    chr->Root().AddChild(mut);
    mMag->mutate(chr->Root(), false, mutctx, true);
    delete chr;
    string muts = mut.ToString();
    LOGN(EInfo, "Managed agent is mutated  [" + muts + "]");
    res = true;
    return res;
}

void ASdcRm::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    mOapOut.NotifyInpsUpdated();
}

void ASdcRm::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    mOapOut.NotifyInpsUpdated();
}




/* SDC agent "Connect" */

const string K_CpUri_V1 = "V1";
const string K_CpUri_V2 = "V2";

ASdcConn::ASdcConn(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapV1("V1", this, K_CpUri_V1), mIapV2("V2", this, K_CpUri_V2), mNco1(this), mNco2(this)
{ }

bool ASdcConn::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	Sdata<string>& v1s = mIapV1.data(aConf);
	Sdata<string>& v2s = mIapV2.data(aConf);
	if (v1s.IsValid() && v2s.IsValid()) {
	    MNode* v1n = mMag->getNode(v1s.mData);
	    MNode* v2n = mMag->getNode(v2s.mData);
	    if (v1n && v2n) {
		MVert* v1v = v1n->lIf(v1v);
		MVert* v2v = v2n->lIf(v2v);
		if (v1v && v2v) {
		    res = v1v->isConnected(v2v);
		} else {
		    LOGN(EDbg, "CP is not connectable [" + (v1v ? v2s.mData : v1s.mData) + "]");
		}
	    } else {
		LOGN(EDbg, "Cannot find CP [" + (v1n ? v2s.mData : v1s.mData) + "]");
                if (!v1n) {
                    mNco1.startObserving(v1s.mData);
                }
                if (!v2n) {
                    mNco2.startObserving(v2s.mData);
                }
	    }
	} else {
	    LOGN(EDbg, "Vertexes URIs are not valid: " + string(v1s.IsValid() ? "v2" : "v1"));
	}
    } else {
        LOGN(EDbg, "Managed agent is not set");
    }
    return res;
}

bool ASdcConn::doCtl()
{
    bool res = false;
    MNode* v1n = mMag->getNode(mIapV1.mCdt.mData);
    MNode* v2n = mMag->getNode(mIapV2.mCdt.mData);
    if (v1n && v2n) {
	MVert* v1v = v1n->lIf(v1v);
	MVert* v2v = v2n->lIf(v2v);
	if (v1v && v2v) {
	    if (!v1v->isConnected(v2v)) {
		TNs ns; MutCtx mutctx(NULL, ns);
		MChromo* chr = mEnv->provider()->createChromo();
		TMut mut(ENt_Conn, ENa_P, mIapV1.mCdt.mData, ENa_Q, mIapV2.mCdt.mData);
		chr->Root().AddChild(mut);
		mMag->mutate(chr->Root(), false, mutctx, true);
		delete chr;
		string muts = mut.ToString();
		LOGN(EInfo, "Managed agent is mutated  [" + muts + "]");
		res = true;
	    } else {
		LOGN(EInfo, "CPs are already connected: [" + mIapV1.mCdt.mData + "] and [" + mIapV2.mCdt.mData + "]");
	    }
	    } else {
		LOGN(EInfo, "CP isn't vertex: [" + (v2v ? mIapV1.mCdt.mData : mIapV2.mCdt.mData) + "]");
	    }
	} else {
	    LOGN(EInfo, "CP doesn't exist: [" + (v2n ? mIapV1.mCdt.mData : mIapV2.mCdt.mData) + "]");
	}
    return res;
}

/* SDC agent "Connect targeted" */


ASdcConnT::ASdcConnT(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapTarg("Target", this, K_CpUri_Target), mIapV1("V1", this, K_CpUri_V1),
    mIapV2("V2", this, K_CpUri_V2), mNco1(this), mNco2(this)
{ }

bool ASdcConnT::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	DGuri& targu = mIapTarg.data(aConf);
        DGuri& v1s = mIapV1.data(aConf);
        DGuri& v2s = mIapV2.data(aConf);
        if (targu.IsValid()) {
            if (v1s.IsValid() && v2s.IsValid()) {
                MNode* targ = mMag->getNode(targu.mData);
                MNode* v1n = targ->getNode(v1s.mData);
                MNode* v2n = targ->getNode(v2s.mData);
                if (v1n && v2n) {
                    MVert* v1v = v1n->lIf(v1v);
                    MVert* v2v = v2n->lIf(v2v);
                    if (v1v && v2v) {
                        res = v1v->isConnected(v2v);
                    } else {
                        LOGN(EDbg, "CP is not connectable [" + (v1v ? v2s.mData.toString() : v1s.mData.toString()) + "]");
                    }
                } else {
                    LOGN(EDbg, "Cannot find CP [" + (v1n ? v2s.mData.toString() : v1s.mData.toString()) + "]");
                    if (!v1n) {
                        mNco1.startObserving(v1s.mData);
                    }
                    if (!v2n) {
                        mNco2.startObserving(v2s.mData);
                    }
                }
            } else {
                LOGN(EDbg, "Vertexes URIs are not valid: " + string(v1s.IsValid() ? "v2" : "v1"));
            }
        } else {
            LOGN(EDbg, "Target URI is not valid: " + targu.mData.toString());
        }
    } else {
        LOGN(EDbg, "Managed agent is not set");
    }
    return res;
}

bool ASdcConnT::doCtl()
{
    bool res = false;
    MNode* targ = mMag->getNode(mIapTarg.mCdt.mData);
    if (targ != nullptr) {
	MNode* v1n = targ->getNode(mIapV1.mCdt.mData);
	MNode* v2n = targ->getNode(mIapV2.mCdt.mData);
	if (v1n && v2n) {
	    MVert* v1v = v1n->lIf(v1v);
	    MVert* v2v = v2n->lIf(v2v);
	    if (v1v && v2v) {
		if (!v1v->isConnected(v2v)) {
		    TNs ns; MutCtx mutctx(NULL, ns);
		    MChromo* chr = mEnv->provider()->createChromo();
		    TMut mut(ENt_Conn, ENa_Targ, mIapTarg.mCdt.mData, ENa_P, mIapV1.mCdt.mData, ENa_Q, mIapV2.mCdt.mData);
		    chr->Root().AddChild(mut);
		    mMag->mutate(chr->Root(), false, mutctx, true);
		    delete chr;
		    string muts = mut.ToString();
		    LOGN(EInfo, "Managed agent is mutated  [" + muts + "]");
		    res = true;
		} else {
		    LOGN(EInfo, "CPs are already connected: [" + mIapV1.mCdt.mData.toString() + "] and [" + mIapV2.mCdt.mData.toString() + "]");
		}
	    } else {
		LOGN(EInfo, "CP isn't vertex: [" + (v2v ? mIapV1.mCdt.mData.toString() : mIapV2.mCdt.mData.toString()) + "]");
	    }
	} else {
	    LOGN(EInfo, "CP doesn't exist: [" + (v2n ? mIapV1.mCdt.mData.toString() : mIapV2.mCdt.mData.toString()) + "]");
	}
    } else {
	LOGN(EInfo, "Target doesn't exist: [" + mIapTarg.mCdt.mData.toString() + "]");
    }
    return res;
}



/* SDC agent "Disonnect" */

ASdcDisconn::ASdcDisconn(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapV1("V1", this, K_CpUri_V1), mIapV2("V2", this, K_CpUri_V2)
{ }

bool ASdcDisconn::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	Sdata<string>& v1s = mIapV1.data(aConf);
	Sdata<string>& v2s = mIapV2.data(aConf);
	if (v1s.IsValid() && v2s.IsValid()) {
	    MNode* v1n = mMag->getNode(v1s.mData);
	    MNode* v2n = mMag->getNode(v2s.mData);
	    if (v1n && v2n) {
		MVert* v1v = v1n->lIf(v1v);
		MVert* v2v = v2n->lIf(v2v);
		res = !v1v->isConnected(v2v);
	    }
	}
    } else {
	LOGN(EDbg, "Managed agent is not set");
    }
    return res;
}

bool ASdcDisconn::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    TMut mut(ENt_Disconn, ENa_P, mIapV1.mCdt.mData, ENa_Q, mIapV2.mCdt.mData);
    chr->Root().AddChild(mut);
    mMag->mutate(chr->Root(), false, mutctx, true);
    delete chr;
    string muts = mut.ToString();
    LOGN(EInfo, "Managed agent is mutated  [" + muts + "]");
    res = true;
    return res;
}


// SDC agent "Insert node into the list, ver. 2"

const string K_CpUri_Insr2_Name = "Name";
const string K_CpUri_Insr2_Prev = "Prev";
const string K_CpUri_Insr2_Next = "Next";
const string K_CpUri_Insr2_Pname = "Pname"; // Name of node before which given node is to be inserted

ASdcInsert2::ASdcInsert2(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Insr2_Name), mIapPrev("Prev", this, K_CpUri_Insr2_Prev), mIapNext("Next", this, K_CpUri_Insr2_Next),
    mIapPname("Pname", this, K_CpUri_Insr2_Pname),
    mDobsNprev(this, MagDobs::EO_CHG)
{ }

bool ASdcInsert2::getState(bool aConf)
{
    bool res = false;
    do {
	if (!mMag) break;
	if (!mIapName.data(aConf).IsValid() || !mIapPrev.data(aConf).IsValid() || !mIapNext.data(aConf).IsValid()) {
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data(aConf).mData);
	if (!comp) {
	    LOGN(EDbg, "State: Cannot find comp [" + mIapName.data(aConf).mData + "]");
	    break;
	}
	GUri uri_p = GUri(mIapPrev.data(aConf).mData);
	GUri uri_n = GUri(mIapNext.data(aConf).mData);
	GUri prev_uri(mIapName.data(aConf).mData); prev_uri += uri_p;
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    LOGN(EDbg, "State: Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += uri_n;
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    LOGN(EDbg, "State: Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    LOGN(EWarn, "State: Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    LOGN(EDbg, "State: Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data(aConf).mData);
	if (!pnode) {
	    LOGN(EDbg, "State: Cannot find position node [" + mIapPname.data(aConf).mData + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data(aConf).mData); pnode_next_uri += uri_n;
	MNode* pnode_next = mMag->getNode(pnode_next_uri);
	if (!pnode_next) {
	    LOGN(EDbg, "State: Cannot find position node next cp [" + pnode_next_uri.toString() + "]");
	    break;
	}
	MVert* pnode_nextv = pnode_next->lIf(pnode_nextv);
	if (pnode_nextv->pairsCount() != 1) {
	    LOGN(EDbg, "State: Position node next Cp pairs count != 1");
	    break;
	}
	// Checking if prev is in the connections chain to the given point Pname
	res = isBindedToEnd(prevv, pnode_nextv, uri_p, uri_n);
	//res = (prevv == pnode_nextv->getPair(0));
    } while (0);
    return res;
}

bool ASdcInsert2::isBindedToEnd(MVert* aCurPrevv, const MVert* aEndNextv, const GUri& aUriP, const GUri& aUriN)
{
    bool res = false;
    auto* nextLinkNextv = aCurPrevv->getPair(0);
    while (nextLinkNextv && nextLinkNextv != aEndNextv) {
	MUnit* nextLinkNextu = nextLinkNextv->lIf(nextLinkNextu); 
	MNode* nextLinkNextn = nextLinkNextu ? nextLinkNextu->lIf(nextLinkNextn ) : nullptr; // TODO Access via munit - hack
	if (nextLinkNextn) {
	    GUri nextLinkNextUri; nextLinkNextn->getUri(nextLinkNextUri, mMag);
	    GUri nextLinkUri;
	    if (nextLinkNextUri.getHead(aUriN, nextLinkUri)) {
		MNode* nextLinkPrevn = mMag->getNode(nextLinkUri + aUriP);
		MVert* nextLinkPrevv = nextLinkPrevn ? nextLinkPrevn->lIf(nextLinkPrevv) : nullptr ;
		nextLinkNextv = nextLinkPrevv ? nextLinkPrevv->getPair(0) : nullptr;
	    }
	}
    }
    res = nextLinkNextv == aEndNextv;
    return res;
}

bool ASdcInsert2::doCtl()
{
    bool res = false;
    do {
	MNode* comp = mMag->getNode(mIapName.data().mData);
	if (!comp) {
	    LOGN(EErr, "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data().mData); prev_uri += GUri(mIapPrev.data().mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    LOGN(EErr, "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	mDobsNprev.updateNuo(prev);
	GUri next_uri(mIapName.data().mData); next_uri += GUri(mIapNext.data().mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    LOGN(EErr, "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    LOGN(EErr, "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	// TODO this workaround fixes DS_ISS_013 but needs persistent solution (checking status, ref SdcInsert3)
	if (prevv->pairsCount()) {
	    LOGN(EErr, "Prev [" + prev_uri.toString() + "] is already connected to [" + prevv->getPair(0)->Uid() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    LOGN(EErr, "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	if (nextv->pairsCount()) {
	    LOGN(EErr, "Next is already connected [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data().mData);
	if (!pnode) {
	    LOGN(EErr, "Cannot find position node [" + mIapPname.data().mData + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data().mData); pnode_next_uri += GUri(mIapNext.data().mData);
	MNode* pnode_next = mMag->getNode(pnode_next_uri);
	if (!pnode_next) {
	    LOGN(EErr, "Cannot find position node next cp [" + pnode_next_uri.toString() + "]");
	    break;
	}
	MVert* pnode_nextv = pnode_next->lIf(pnode_nextv);
	if (pnode_nextv->pairsCount() != 1) {
	    LOGN(EErr, "Position node next Cp pairs count != 1");
	    break;
	}
	mCpPair = pnode_nextv->getPair(0);
	// Disconnect Cp
	bool cres = MVert::disconnect(pnode_nextv, mCpPair);
	if (!cres) {
	    LOGN(EErr, "Failed disconnecting [" + pnode_next->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	// Connect
	cres = MVert::connect(prevv, pnode_nextv);
	if (!cres) {
	    LOGN(EErr, "Failed connecting [" + prevv->Uid() + "] - [" + pnode_next->Uid() + "]");
	    break;
	}
	cres = MVert::connect(nextv, mCpPair);
	if (!cres) {
	    LOGN(EErr, "Failed connecting [" + nextv->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	LOGN(EInfo, "Managed agent is updated, inserted [" + comp->Uid() + "] before [" + pnode->Uid() + "]");
	res = true;
    } while (0);
    return res;
}

void ASdcInsert2::onObsChanged(MObservable* aObl)
{
    ASdc::onObsChanged(aObl);
    mOapOut.NotifyInpsUpdated();
}

#if 0

// SDC agent "Insert node into list, ver. 3. DS_ISS_013 fixed.
//
static const string K_CpUri_Insr3_Start = "Start";
static const string K_CpUri_Insr3_End = "End";

ASdcInsert3::ASdcInsert3(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapStartUri("Start", this, K_CpUri_Insr3_Start), mIapStartUri("End", this, K_CpUri_Insr3_Start),
    mIapName("Name", this, K_CpUri_Insr2_Name), mIapPrev("Prev", this, K_CpUri_Insr2_Prev), mIapNext("Next", this, K_CpUri_Insr2_Next),
    mIapPname("Pname", this, K_CpUri_Insr2_Pname),
    mDobsNprev(this, MagDobs::EO_CHG)
{ }

bool ASdcInsert3::getState(bool aConf)
{
    bool res = false;
    do {
	if (!mMag) break;
	if (!mIapStartUri.data(aConf).IsValid() || !mIapEndUri.data(aConf).IsValid() || !mIapName.data(aConf).IsValid() ||
		!mIapPrev.data(aConf).IsValid() || !mIapNext.data(aConf).IsValid()) {
	    break;
	}
	MNode* start = mMag->getNode(mIapStartUri.data(aConf).mData);
	if (!start) {
	    LOGN(EDbg, "Getting state: Cannot find list start [" + mIapStartUri.data(aConf).mData + "]");
	    break;
	}
	MNode* end = mMag->getNode(mIapEndUri.data(aConf).mData);
	if (!end) {
	    LOGN(EDbg, "Getting state: Cannot find list end [" + mIapEndUri.data(aConf).mData + "]");
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data(aConf).mData);
	if (!comp) {
	    LOGN(EDbg, "Getting state: Cannot find comp [" + mIapName.data(aConf).mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data(aConf).mData);
	prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    LOGN(EDbg, "Getting state: Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    LOGN(EDbg, "Getting state: Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    LOGN(EErr, "Getting state: Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    LOGN(EErr, "Getting state: Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data(aConf).mData);
	if (!pnode) {
	    LOGN(EErr, "Getting state: Cannot find position node [" + mIapPname.data(aConf).mData + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data(aConf).mData);
	pnode_next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* pnode_next = mMag->getNode(pnode_next_uri);
	if (!pnode_next) {
	    LOGN(EErr, "Getting state: Cannot find position node next cp [" + pnode_next_uri.toString() + "]");
	    break;
	}
	MVert* pnode_nextv = pnode_next->lIf(pnode_nextv);
	if (pnode_nextv->pairsCount() != 1) {
	    LOGN(EErr, "Getting state: Position node next Cp pairs count != 1");
	    break;
	}
	// Traverse to list end from link Prev
    } while (0);
    return res;
}

#endif




/// SDC agent "Insert node into list AFTER a the chain given node"

ASdcInsertN::ASdcInsertN(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Insr2_Name), mIapPrev("Prev", this, K_CpUri_Insr2_Prev), mIapNext("Next", this, K_CpUri_Insr2_Next),
    mIapPname("Pname", this, K_CpUri_Insr2_Pname),
    mDobsNprev(this, MagDobs::EO_CHG)
{ }

bool ASdcInsertN::getState(bool aConf)
{
    bool res = false;
    do {
	if (!mMag) break;
	if (!mIapName.data(aConf).IsValid() || !mIapPrev.data(aConf).IsValid() || !mIapNext.data(aConf).IsValid()) {
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data(aConf).mData);
	if (!comp) {
	    LOGN(EDbg, "Cannot find comp [" + mIapName.data(aConf).mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data(aConf).mData);
	prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    LOGN(EDbg, "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    LOGN(EDbg, "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    LOGN(EErr, "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    LOGN(EErr, "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data(aConf).mData);
	if (!pnode) {
	    LOGN(EErr, "Cannot find position node [" + mIapPname.data(aConf).mData + "]");
	    break;
	}
	GUri pnode_prev_uri(mIapPname.data(aConf).mData);
	pnode_prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* pnode_prev = mMag->getNode(pnode_prev_uri);
	if (!pnode_prev) {
	    LOGN(EErr, "Cannot find position node prev cp [" + pnode_prev_uri.toString() + "]");
	    break;
	}
	MVert* pnode_prevv = pnode_prev->lIf(pnode_prevv);
	if (pnode_prevv->pairsCount() != 1) {
	    LOGN(EErr, "Position node prev Cp pairs count != 1");
	    break;
	}
	res = (nextv == pnode_prevv->getPair(0));
    } while (0);
    return res;
}

bool ASdcInsertN::doCtl()
{
    bool res = false;
    do {
	MNode* comp = mMag->getNode(mIapName.data().mData);
	if (!comp) {
	    LOGN(EErr, "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data().mData); prev_uri += GUri(mIapPrev.data().mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    LOGN(EErr, "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	mDobsNprev.updateNuo(prev);
	GUri next_uri(mIapName.data().mData); next_uri += GUri(mIapNext.data().mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    LOGN(EErr, "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    LOGN(EErr, "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    LOGN(EErr, "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data().mData);
	if (!pnode) {
	    LOGN(EErr, "Cannot find position node [" + mIapPname.data().mData + "]");
	    break;
	}
	GUri pnode_prev_uri(mIapPname.data().mData); pnode_prev_uri += GUri(mIapPrev.data().mData);
	MNode* pnode_prev = mMag->getNode(pnode_prev_uri);
	if (!pnode_prev) {
	    LOGN(EErr, "Cannot find position node prev cp [" + pnode_prev_uri.toString() + "]");
	    break;
	}
	MVert* pnode_prevv = pnode_prev->lIf(pnode_prevv);
	if (pnode_prevv->pairsCount() != 1) {
	    LOGN(EErr, "Position node prev cp [" + pnode_prev_uri.toString() + "] pairs count != 1");
	    break;
	}
	mCpPair = pnode_prevv->getPair(0);
	// Disconnect Cp
	bool cres = MVert::disconnect(pnode_prevv, mCpPair);
	if (!cres) {
	    LOGN(EErr, "Failed disconnecting [" + pnode_prev->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	// Connect
	cres = MVert::connect(nextv, pnode_prevv);
	if (!cres) {
	    LOGN(EErr, "Failed connecting [" + nextv->Uid() + "] - [" + pnode_prev->Uid() + "]");
	    break;
	}
	cres = MVert::connect(prevv, mCpPair);
	if (!cres) {
	    LOGN(EErr, "Failed connecting [" + prevv->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	LOGN(EInfo, "Managed agent is updated, inserted [" + comp->Uid() + "] after [" + pnode->Uid() + "]");
	res = true;
    } while (0);
    return res;
}

void ASdcInsertN::onObsChanged(MObservable* aObl)
{
    ASdc::onObsChanged(aObl);
    mOapOut.NotifyInpsUpdated();
}




// SDC agent "Extract link from chain"

const string K_CpUri_Extr_Name = "Name";
const string K_CpUri_Extr_Prev = "Prev";
const string K_CpUri_Extr_Next = "Next";

ASdcExtract::ASdcExtract(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Extr_Name), mIapPrev("Prev", this, K_CpUri_Extr_Prev), mIapNext("Next", this, K_CpUri_Extr_Next)
{ }

bool ASdcExtract::getState(bool aConf)
{
    bool res = false;
    do {
	if (!mMag) break;
	if (!mIapName.data(aConf).IsValid() || !mIapPrev.data(aConf).IsValid() || !mIapNext.data(aConf).IsValid()) {
	    break;
	}
	res = true; // Note, errors are interpreted in favor of "extracted" state
	MNode* comp = mMag->getNode(mIapName.data(aConf).mData);
	if (!comp) {
	    LOGN(EDbg, "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data(aConf).mData); prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    LOGN(EDbg, "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    LOGN(EDbg, "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    LOGN(EDbg, "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    LOGN(EDbg, "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	res = (prevv->pairsCount() == 0) && (nextv->pairsCount() == 0);
    } while (0);

    return res;
}

bool ASdcExtract::doCtl()
{
    bool res = false;
    do {
	MNode* comp = mMag->getNode(mIapName.data().mData);
	if (!comp) {
	    LOGN(EErr, "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data().mData); prev_uri += GUri(mIapPrev.data().mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    LOGN(EErr, "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data().mData); next_uri += GUri(mIapNext.data().mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    LOGN(EErr, "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    LOGN(EErr, "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    LOGN(EErr, "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	if (prevv->pairsCount() != 1) {
	    LOGN(EErr, "Node prev Cp pairs count != 1");
	    break;
	}
	if (nextv->pairsCount() != 1) {
	    LOGN(EErr, "Node next Cp pairs count != 1");
	    break;
	}
	MVert* nextPair = nextv->getPair(0);
	bool cres = MVert::disconnect(nextv, nextPair);
	if (!cres) {
	    LOGN(EInfo, "Failed disconnecting [" + nextv->Uid() + "] - [" + nextPair->Uid() + "]");
	    break;
	}
	MVert* prevPair = prevv->getPair(0);
	cres = MVert::disconnect(prevv, prevPair);
	if (!cres) {
	    LOGN(EInfo, "Failed disconnecting [" + prevv->Uid() + "] - [" + prevPair->Uid() + "]");
	    break;
	}
	cres = MVert::connect(nextPair, prevPair);
	if (!cres) {
	    LOGN(EInfo, "Failed connecting [" + nextPair->Uid() + "] - [" + prevPair->Uid() + "]");
	    break;
	}
	LOGN(EInfo, "Managed agent is updated, extracted node [" + comp->Uid() + "] before [" + prevPair->Uid() + "]");
	res = true;
    } while (0);
    return res;
}


/* SDC agent "Pause manageable" */

ASdcPause::ASdcPause(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv)
{ }

MDesManageable* ASdcPause::getDesManageable()
{
    MDesManageable* res = nullptr;
    MUnit* magu = mMag->lIf(magu);
    res = magu ? magu->getSif(res) : nullptr;
    return res;
}

bool ASdcPause::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	MDesManageable* magm = getDesManageable();
	if (!magm) {
	    LOGN(EErr, "Managed agent isn't DES manageable");
	} else {
	    res = magm->isPaused();
	}
    }
    return res;
}

bool ASdcPause::doCtl()
{
    bool res = false;
    MDesManageable* magm = getDesManageable();
    if (!magm) {
	LOGN(EErr, "Managed agent isn't DES manageable");
    } else {
	magm->pauseDes();
	LOGN(EDbg, "Paused [" + magm->Uid() + "]");
	res = true;
    }
    return res;
}


/* SDC agent "Resume manageable" */

ASdcResume::ASdcResume(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv)
{ }

MDesManageable* ASdcResume::getDesManageable()
{
    MDesManageable* res = nullptr;
    MUnit* magu = mMag->lIf(magu);
    res = magu ? magu->getSif(res) : nullptr;
    return res;
}

bool ASdcResume::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	MDesManageable* magm = getDesManageable();
	if (!magm) {
	    LOGN(EErr, "Managed agent isn't DES manageable");
	} else {
	    res = !magm->isPaused();
	}
    }
    return res;
}

bool ASdcResume::doCtl()
{
    bool res = false;
    MDesManageable* magm = getDesManageable();
    if (!magm) {
	LOGN(EErr, "Managed agent isn't DES manageable");
    } else {
	magm->resumeDes();
	LOGN(EDbg, "Resumed [" + magm->Uid() + "]");
	res = true;
    }
    return res;
}


