
#include "des.h"
#include "dessdc.h"

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
    mName(aName), mHost(aHost), mInpUri(aInpUri), mUpdated(false), mActivated(true), mChanged(false)
{
    mHost->registerIap(this);
}

ASdc::SdcPapb::SdcPapb(const string& aName, ASdc* aHost, const string& aCpUri):
    mName(aName), mHost(aHost), mCpUri(aCpUri)
{
    mHost->registerPap(this);
}

ASdc::SdcMapb::SdcMapb(const string& aName, ASdc* aHost):
    mName(aName), mHost(aHost), mUpdated(false), mActivated(false), mChanged(false)
{
    mHost->registerMap(this);
}

void ASdc::SdcMapb::onMagUpdated()
{
    setActivated();
}


template <typename T> string toStr(const T& aData) { return to_string(aData); }

string toStr(const string& aData) { return aData;}


template <class T>
void ASdc::SdcIap<T>::update()
{
    T old_data = mUdt;
    bool res = mHost->GetInpSdata<T>(mInpUri, mUdt);
    if (mHost->isLogLevel(EDbg)/* && mUdt != mCdt*/) {
	mHost->Log(TLog(EDbg, mHost) + "[" + mName + "] Updated: [" + toStr(old_data) + "] -> [" + toStr(mUdt) + "]");
    }
    mActivated = false;
    setUpdated();
}

template <class T>
void ASdc::SdcIap<T>::confirm()
{
    if (mUdt != mCdt) {
	mCdt = mUdt;
	mChanged = true;
    } else {
	mChanged = false;
    }
    mUpdated = false;
}

void ASdc::SdcIapEnb::update()
{
    bool old_data = mUdt;
    MNode* inp = mHost->getNode(mInpUri);
    MUnit* vgetu = inp->lIf(vgetu);
    auto ifaces = vgetu->getIfs<MDVarGet>();
    bool first = true;
    if (ifaces) for (auto ifc : *ifaces) {
	MDVarGet* vget = dynamic_cast<MDVarGet*>(ifc);
	MDtGet<Sdata<bool>>* gsd = vget->GetDObj(gsd);
	if (gsd) {
	    Sdata<bool> st;
	    gsd->DtGet(st);
	    if (first) mUdt = st.mData; else mUdt &= st.mData;
	    first = false;
	}
    }
    if (mHost->isLogLevel(EDbg)) {
	mHost->Log(TLog(EDbg, mHost) + "[" + mName + "] Updated: [" + toStr(old_data) + "] -> [" + toStr(mUdt) + "]");
    }
    mActivated = false;
    setUpdated();
}

    template <class T>
void ASdc::SdcIapg<T>::update()
{
    bool res = mHost->GetInpData<T>(mInpUri, mCdt);
    mActivated = false;
    //setUpdated();
}

template <class T>
void ASdc::SdcIapg<T>::confirm()
{
    if (mUdt != mCdt) {
	mCdt = mUdt;
	mChanged = true;
    } else {
	mChanged = false;
    }
    mUpdated = false;
}

template <class T>
void ASdc::SdcMap<T>::update()
{
    mGetData(mUdt);
    if (mHost->isLogLevel(EDbg) && mUdt != mCdt) {
	mHost->Log(TLog(EDbg, mHost) + "[" + mName + "] Updated: [" + toStr(mCdt) + "] -> [" + toStr(mUdt) + "]");
    }
    mActivated = false;
    setUpdated();
}

template <class T>
void ASdc::SdcMap<T>::confirm()
{
    if (mUdt != mCdt) {
	mCdt = mUdt;
	mChanged = true;
    } else {
	mChanged = false;
    }
    mUpdated = false;
}



ASdc::ASdc(const string &aType, const string& aName, MEnv* aEnv): Unit(aType, aName, aEnv),
    mIaps(), mMag(NULL), mUpdNotified(false), mActNotified(false), mObrCp(this), mIapEnb("Enb", this, K_CpUri_Enable),
    mOapOut("Outp", this, K_CpUri_Outp, [this](Sdata<bool>& aData) {getOut(aData);})/*, mMapCcd("Ccd", this, [this](bool& aData) {getCcd(aData);})*/,
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
    } else if (aName == MDesObserver::Type()) {
	MIface* iface = MNode_getLif(MDesObserver::Type());
	addIfpLeaf(iface, aReq);
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
	if (iap->mActivated) {
	    iap->update();
	}
    }
    /*
    for (auto map : mMaps) {
	if (map->mActivated) {
	    map->update();
	}
    }
    */
    mActNotified = false;
    setUpdated();
}

void ASdc::confirm()
{
    /*
    if (mIapEnb.mUpdated) {
	mIapEnb.confirm();
    }
    */
    bool changed = false;
    for (auto iap : mIaps) {
	if (iap->mUpdated) {
	    iap->confirm();
	    changed |= iap->mChanged;
	}
    }
    /*
    for (auto map : mMaps) {
	if (map->mUpdated) {
	    map->confirm();
	    changed |= map->mChanged;
	}
    }
    */
    if (/*changed &&*/ mMag && mIapEnb.data()) { // Ref ds_dcs_sdc_dsgn_oin Solution#1
	if (!getState()) { // Ref ds_dcs_sdc_dsgn_cc Solution#2
	    bool res = doCtl();
	    if (!res) {
		Log(TLog(EErr, this) + "Failed controlling managed agent");
	    } else {
		mCdone = true;
		mOapOut.NotifyInpsUpdated();
	    }
	}
    }
    mUpdNotified = false;
}

void ASdc::setActivated()
{
    if (!mActNotified) {
	MUnit* ownu = Owner()->lIf(ownu);
	MDesObserver* obs = ownu ? ownu->getSif(obs) : nullptr;
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
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

void ASdc::notifyMaps()
{
    for (auto map : mMaps) {
	map->onMagUpdated();
    }
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
    // Form status taking into acc control completion, ref ds_dcs_sdc_dsgn_cc
    aData.mData = getState()/* && mCdone*/;
    aData.mValid = true;
    Log(TLog(EDbg, this) + "Outp: " + (aData.mData ? "true" : "false"));
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
    // Attempt MDesAdapter iface first
    MDesAdapter* desa = Owner()->lIf(desa);
    MNode* mag = nullptr;
    if (desa) {
	mag = desa->getMag();
    } else {
	// Then explorable of owner
	mag = Owner()->lIf(mag);
    }
    if (!mag) {
	//Log(TLog(EErr, this) + "Cannot get owners explorable");
    } else {
	// Set explored system
	if (mag != mMag) {
	    mMag = mag;
	    notifyOutp();
	    Log(TLog(EDbg, this) + "Controllable is attached [" + mMag->Uid() + "]");
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
	Log(TLog(EDbg, this) + "Cannot get input [" + aInpUri + "]");
    }
    return res;
}

template<typename T> bool ASdc::GetInpData(const string aInpUri, T& aRes)
{
    bool res = false;
    MNode* inp = getNode(aInpUri);
    if (inp) {
	res =  GetGData(inp, aRes);
    } else {
	Log(TLog(EDbg, this) + "Cannot get input [" + aInpUri + "]");
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

bool ASdc::registerMap(SdcMapb* aMap)
{
    mMaps.push_back(aMap);
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
    mIapTarg("Targ", this, K_CpUri_Targ), mIapMut("Mut", this, K_CpUri_Mut)
{
}

bool ASdcMut::getState()
{
    return mMutApplied;
}

bool ASdcMut::doCtl()
{
    bool res = false;
    MNode* targn = mMag->getNode(mIapTarg.mCdt);
    if (!targn) {
	Log(TLog(EInfo, this) + "Cannot find target  [" + mIapTarg.mCdt + "]");
    } else {
	Chromo2& chromo = mIapMut.mCdt.mData;
	TNs ns; MutCtx mutctx(NULL, ns);
	//chromo.Root().Dump(); // Debug
	targn->mutate(chromo.Root(), false, mutctx, true);
	string muts;
	chromo.Root().ToString(muts);
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	res = true;
    }
    return res;
}


/* SDC agent "Adding Component" */

const string K_CpUri_Name = "Name";
const string K_CpUri_Parent = "Parent";
const string K_CpUri_OutpName = "OutpName";

ASdcComp::ASdcComp(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Name), mIapParent("Parent", this, K_CpUri_Parent),
    mOapName("OutName", this, K_CpUri_OutpName)
{ }

bool ASdcComp::getState()
{
    bool res = false;
    string name;
    bool rr = GetInpSdata(K_CpUri_Name, name);
    if (rr && mMag) {
	res = mMag->getComp(name);
    }
    return res;
}

bool ASdcComp::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    TMut mut(ENt_Node, ENa_Id, mIapName.mCdt, ENa_Parent, mIapParent.mCdt);
    chr->Root().AddChild(mut);
    mMag->mutate(chr->Root(), false, mutctx, true);
    delete chr;
    string muts = mut.ToString();
    Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
    mOapName.updateData(mIapName.mCdt);
    res = true;
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


/* SDC agent "Rm Component" */

const string K_CpUri_Rm_Name = "Name";
const string K_CpUri_Rm_OutpName = "OutpName";

ASdcRm::ASdcRm(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Name), mOapName("OutName", this, K_CpUri_OutpName)
{ }

bool ASdcRm::getState()
{
    bool res = false;
    string name;
    bool rr = GetInpSdata(K_CpUri_Rm_Name, name);
    if (rr && mMag) {
	res = !mMag->getComp(name);
    }
    return res;
}

bool ASdcRm::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    TMut mut(ENt_Rm, ENa_Id, mIapName.mCdt);
    chr->Root().AddChild(mut);
    mMag->mutate(chr->Root(), false, mutctx, true);
    delete chr;
    string muts = mut.ToString();
    Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
    mOapName.updateData(mIapName.mCdt);
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
    mIapV1("V1", this, K_CpUri_V1), mIapV2("V2", this, K_CpUri_V2)
{ }

void ASdcConn::getCcd(bool& aData)
{
    bool res = false;
    MNode* v1n = mMag->getNode(mIapV1.mCdt);
    MNode* v2n = mMag->getNode(mIapV2.mCdt);
    if (v1n && v2n) {
	MVert* v1v = v1n->lIf(v1v);
	MVert* v2v = v2n->lIf(v2v);
	if (v1v && v2v) {
	    res = !v1v->isConnected(v2v);
	}
    }
    aData = res;
}

bool ASdcConn::getState()
{
    bool res = false;
    string v1s, v2s;
    bool rr = GetInpSdata(K_CpUri_V1, v1s);
    rr = rr && GetInpSdata(K_CpUri_V2, v2s);
    if (rr && mMag) {
	MNode* v1n = mMag->getNode(v1s);
	MNode* v2n = mMag->getNode(v2s);
	if (v1n && v2n) {
	    MVert* v1v = v1n->lIf(v1v);
	    MVert* v2v = v2n->lIf(v2v);
	    if (v1v && v2v) {
		res = v1v->isConnected(v2v);
	    }
	}
    }
    return res;
}

bool ASdcConn::doCtl()
{
    bool res = false;
    // Checking the control condition met
    MNode* v1n = mMag->getNode(mIapV1.mCdt);
    MNode* v2n = mMag->getNode(mIapV2.mCdt);
    if (v1n && v2n) {
	MVert* v1v = v1n->lIf(v1v);
	MVert* v2v = v2n->lIf(v2v);
	if (v1v && v2v) {
	    if (!v1v->isConnected(v2v)) {
		TNs ns; MutCtx mutctx(NULL, ns);
		MChromo* chr = mEnv->provider()->createChromo();
		TMut mut(ENt_Conn, ENa_P, mIapV1.mCdt, ENa_Q, mIapV2.mCdt);
		chr->Root().AddChild(mut);
		mMag->mutate(chr->Root(), false, mutctx, true);
		delete chr;
		string muts = mut.ToString();
		Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
		res = true;
	    } else {
		Log(TLog(EInfo, this) + "CPs are already connected: [" + mIapV1.mCdt + "] and [" + mIapV2.mCdt + "]");
	    }
	} else {
	    Log(TLog(EInfo, this) + "CPs are not vertexes: [" + mIapV1.mCdt + "] or [" + mIapV2.mCdt + "]");
	}
    } else {
	Log(TLog(EInfo, this) + "CPs are not exist: [" + mIapV1.mCdt + "] or [" + mIapV2.mCdt + "]");
    }
    return res;
}


/* SDC agent "Disonnect" */

ASdcDisconn::ASdcDisconn(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapV1("V1", this, K_CpUri_V1), mIapV2("V2", this, K_CpUri_V2)
{ }

bool ASdcDisconn::getState()
{
    bool res = false;
    string v1s, v2s;
    bool rr = GetInpSdata(K_CpUri_V1, v1s);
    rr = rr && GetInpSdata(K_CpUri_V2, v2s);
    if (rr && mMag) {
	MNode* v1n = mMag->getNode(v1s);
	MNode* v2n = mMag->getNode(v2s);
	if (v1n && v2n) {
	    MVert* v1v = v1n->lIf(v1v);
	    MVert* v2v = v2n->lIf(v2v);
	    res = !v1v->isConnected(v2v);
	}
    }
    return res;
}

bool ASdcDisconn::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    TMut mut(ENt_Disconn, ENa_P, mIapV1.mCdt, ENa_Q, mIapV2.mCdt);
    chr->Root().AddChild(mut);
    mMag->mutate(chr->Root(), false, mutctx, true);
    delete chr;
    string muts = mut.ToString();
    Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
    res = true;
    return res;
}


// SDC agent "Insert"

const string K_CpUri_Cp = "TCp";
const string K_CpUri_Icp = "ICp";
const string K_CpUri_Icpp = "ICpp";
const string K_CpUri_Ins_OutpName = "OutpName";

ASdcInsert::ASdcInsert(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapCp("Cp", this, K_CpUri_Cp), mIapIcp("Icp", this, K_CpUri_Icp), mIapIcpp("Icpp", this, K_CpUri_Icpp),
    mOapName("OutName", this, K_CpUri_Ins_OutpName),
    mDobsIcp(this, MagDobs::EO_CHG | MagDobs::EO_DTCH)
{ }

void ASdcInsert::getCcd(bool& aData)
{
    bool res = false;
    string cp, icp, icpp;
    bool rr = GetInpSdata(K_CpUri_Cp, cp);
    rr = rr && GetInpSdata(K_CpUri_Icp, icp);
    rr = rr && GetInpSdata(K_CpUri_Icpp, icpp);
    if (rr && mMag) {
	MNode* cpn = mMag->getNode(cp);
	MNode* icpn = mMag->getNode(icp);
	MNode* icppn = mMag->getNode(icpp);
	if (cpn && icpn && icppn) {
	    MVert* cpv = cpn->lIf(cpv);
	    MVert* icpv = icpn->lIf(icpv);
	    MVert* icppv = icppn->lIf(icppv);
	    if (cpv && icpv && icppv) {
		res = (cpv->pairsCount() == 1) && (icpv->pairsCount() == 0) && (icppv->pairsCount() == 0);
	    }
	}
    }
    aData = res;;

}

bool ASdcInsert::getState()
{
    bool res = false;
    string cp, icp, icpp;
    bool rr = GetInpSdata(K_CpUri_Cp, cp);
    rr = rr && GetInpSdata(K_CpUri_Icp, icp);
    rr = rr && GetInpSdata(K_CpUri_Icpp, icpp);
    if (rr && mMag && mCpPair) {
	MNode* cpn = mMag->getNode(cp);
	MNode* icpn = mMag->getNode(icp);
	MNode* icppn = mMag->getNode(icpp);
	if (cpn && icpn && icppn) {
	    MVert* cpv = cpn->lIf(cpv);
	    MVert* icpv = icpn->lIf(icpv);
	    MVert* icppv = icppn->lIf(icppv);
	    res = cpv->isConnected(icpv) && mCpPair->isConnected(icppv);
	}
    }
    return res;
}

bool ASdcInsert::doCtl()
{
    bool res = false;
    // Verify conditions
    MNode* cp = mMag->getNode(mIapCp.data());
    MVert* cpv = cp ? cp->lIf(cpv) : nullptr;
    if (!cpv) {
	Log(TLog(EErr, this) + "Cannot find Cp or Cp isn't vert [" + mIapCp.mCdt + "]");
    } else {
	if (cpv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Cp pairs count != 1");
	} else {
	    mCpPair = cpv->getPair(0);	
	    // Disconnect Cp
	    bool cres = MVert::disconnect(cpv, mCpPair);
	    if (!cres) {
		Log(TLog(EInfo, this) + "Failed disconnecting [" + cpv->Uid() + "] - [" + mCpPair->Uid() + "]");
	    } else {
		// Connect
		MNode* icpn = mMag->getNode(mIapIcp.data());
		MNode* icppn = mMag->getNode(mIapIcpp.data());
		if (icpn && icppn) {
		    MVert* icpv = icpn->lIf(icpv);
		    MVert* icppv = icppn->lIf(icppv);
		    cres = MVert::connect(icpv, cpv);
		    if (!cres) {
			Log(TLog(EInfo, this) + "Failed connecting [" + icpv->Uid() + "] - [" + cpv->Uid() + "]");
		    } else {
			cres = MVert::connect(icppv, mCpPair);
			if (!cres) {
			    Log(TLog(EInfo, this) + "Failed connecting [" + icppv->Uid() + "] - [" + mCpPair->Uid() + "]");
			} else {
			    mDobsIcp.updateNuo(icpn);
			    Log(TLog(EInfo, this) + "Managed agent is updated, inserted [" + icpv->Uid() + "] into [" + cpv->Uid() + "]");
			    // TODO Replace this workaround with final code
			    GUri icpuri(mIapIcp.data());
			    mOapName.updateData(icpuri.at(0));
			    res = true;
			}
		    }
		}
	    }
	}
    }
    return res;
}

void ASdcInsert::onObsChanged(MObservable* aObl)
{
    mOapOut.NotifyInpsUpdated();
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

bool ASdcInsert2::getState()
{
    bool res = false;
    do {
	if (!mMag) break;
	MNode* comp = mMag->getNode(mIapName.mCdt);
	if (!comp) {
	    Log(TLog(EErr, this) + "Cannot find comp [" + mIapName.mCdt + "]");
	    break;
	}
	GUri prev_uri(mIapName.data()); prev_uri += GUri(mIapPrev.data());
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data()); next_uri += GUri(mIapNext.data());
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EErr, this) + "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    Log(TLog(EErr, this) + "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EErr, this) + "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data());
	if (!pnode) {
	    Log(TLog(EErr, this) + "Cannot find position node [" + mIapPname.data() + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data()); pnode_next_uri += GUri(mIapNext.data());
	MNode* pnode_next = mMag->getNode(pnode_next_uri);
	if (!pnode_next) {
	    Log(TLog(EErr, this) + "Cannot find position node next cp [" + pnode_next_uri.toString() + "]");
	    break;
	}
	MVert* pnode_nextv = pnode_next->lIf(pnode_nextv);
	if (pnode_nextv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Position node next Cp pairs count != 1");
	    break;
	}
	res = (prevv == pnode_nextv->getPair(0));	
    } while (0);
    return res;
}

bool ASdcInsert2::doCtl()
{
    bool res = false;
    // Verify conditions
    do {
	MNode* comp = mMag->getNode(mIapName.data());
	if (!comp) {
	    Log(TLog(EErr, this) + "Cannot find comp [" + mIapName.mCdt + "]");
	    break;
	}
	GUri prev_uri(mIapName.data()); prev_uri += GUri(mIapPrev.data());
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	mDobsNprev.updateNuo(prev);
	GUri next_uri(mIapName.data()); next_uri += GUri(mIapNext.data());
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EErr, this) + "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    Log(TLog(EErr, this) + "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EErr, this) + "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data());
	if (!pnode) {
	    Log(TLog(EErr, this) + "Cannot find position node [" + mIapPname.data() + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data()); pnode_next_uri += GUri(mIapNext.data());
	MNode* pnode_next = mMag->getNode(pnode_next_uri);
	if (!pnode_next) {
	    Log(TLog(EErr, this) + "Cannot find position node next cp [" + pnode_next_uri.toString() + "]");
	    break;
	}
	MVert* pnode_nextv = pnode_next->lIf(pnode_nextv);
	if (pnode_nextv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Position node next Cp pairs count != 1");
	    break;
	}
	mCpPair = pnode_nextv->getPair(0);	
	// Disconnect Cp
	bool cres = MVert::disconnect(pnode_nextv, mCpPair);
	if (!cres) {
	    Log(TLog(EInfo, this) + "Failed disconnecting [" + pnode_next->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	// Connect
	cres = MVert::connect(prevv, pnode_nextv);
	if (!cres) {
	    Log(TLog(EInfo, this) + "Failed connecting [" + pnode_next->Uid() + "] - [" + next->Uid() + "]");
	    break;
	}
	cres = MVert::connect(nextv, mCpPair);
	if (!cres) {
	    Log(TLog(EInfo, this) + "Failed connecting [" + nextv->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	Log(TLog(EInfo, this) + "Managed agent is updated, inserted [" + comp->Uid() + "] before [" + pnode->Uid() + "]");
	res = true;
    } while (0);
    return res;
}

void ASdcInsert2::onObsChanged(MObservable* aObl)
{
    mOapOut.NotifyInpsUpdated();
}



// SDC agent "Extract link from chain"

const string K_CpUri_Extr_Name = "Name";
const string K_CpUri_Extr_Prev = "Prev";
const string K_CpUri_Extr_Next = "Next";

ASdcExtract::ASdcExtract(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Extr_Name), mIapPrev("Prev", this, K_CpUri_Extr_Prev), mIapNext("Next", this, K_CpUri_Extr_Next)
{ }

bool ASdcExtract::getState()
{
    bool res = false;
    do {
	if (!mMag) break;
	MNode* comp = mMag->getNode(mIapName.data());
	if (!comp) {
	    Log(TLog(EErr, this) + "Cannot find comp [" + mIapName.mCdt + "]");
	    break;
	}
	GUri prev_uri(mIapName.data()); prev_uri += GUri(mIapPrev.data());
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data()); next_uri += GUri(mIapNext.data());
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EErr, this) + "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    Log(TLog(EErr, this) + "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EErr, this) + "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	res = (prevv->pairsCount() == 0) && (nextv->pairsCount() == 0);
    } while (0);

    return res;
}

bool ASdcExtract::doCtl()
{
    bool res = false;
    // Verify conditions
    do {
	MNode* comp = mMag->getNode(mIapName.data());
	if (!comp) {
	    Log(TLog(EErr, this) + "Cannot find comp [" + mIapName.mCdt + "]");
	    break;
	}
	GUri prev_uri(mIapName.data()); prev_uri += GUri(mIapPrev.data());
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data()); next_uri += GUri(mIapNext.data());
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EErr, this) + "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    Log(TLog(EErr, this) + "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EErr, this) + "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	if (prevv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Node prev Cp pairs count != 1");
	    break;
	}
	if (nextv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Node next Cp pairs count != 1");
	    break;
	}
	MVert* nextPair = nextv->getPair(0);
	bool cres = MVert::disconnect(nextv, nextPair);
	if (!cres) {
	    Log(TLog(EInfo, this) + "Failed disconnecting [" + nextv->Uid() + "] - [" + nextPair->Uid() + "]");
	    break;
	}
	MVert* prevPair = prevv->getPair(0);
	cres = MVert::disconnect(prevv, prevPair);
	if (!cres) {
	    Log(TLog(EInfo, this) + "Failed disconnecting [" + prevv->Uid() + "] - [" + prevPair->Uid() + "]");
	    break;
	}
	cres = MVert::connect(nextPair, prevPair);
	if (!cres) {
	    Log(TLog(EInfo, this) + "Failed connecting [" + nextPair->Uid() + "] - [" + prevPair->Uid() + "]");
	    break;
	}
	Log(TLog(EInfo, this) + "Managed agent is updated, extracted node [" + comp->Uid() + "] before [" + prevPair->Uid() + "]");
	res = true;
    } while (0);
    return res;
}
