
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
    return mHost->GetInpData<T>(mInpUri, mUdt);
}

template <class T>
void ASdc::SdcIap<T>::update()
{
    T old_data = mUdt;
    bool res = updateData();
    if (mHost->isLogLevel(EDbg)/* && mUdt != mCdt*/) {
	mHost->Log(TLog(EDbg, mHost) + "[" + mName + "] Updated: [" + toStr(old_data.mData) + "] -> [" + toStr(mUdt.mData) + "]");
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

bool ASdc::SdcIapEnb::updateData()
{
    bool res = false;
    MNode* inp = mHost->getNode(mInpUri);
    MVert* inpv = inp ? inp->lIf(inpv) : nullptr;
    MUnit* vgetu = inp->lIf(vgetu);
    auto ifaces = vgetu->getIfs<MDVarGet>();
    // We need to interpret not-connecting "enable" in favor of "disable"
    // So using workaround here
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
    return res;
}

template <class T>
bool ASdc::SdcIapg<T>::updateData()
{
    return mHost->GetInpData<T>(mInpUri, mUdt);
}

template <class T>
void ASdc::SdcIapg<T>::update()
{
    bool res = updateData();
    mActivated = false;
    setUpdated();
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
    if (/*mIapEnb.data(false)*/ true) {
	for (auto iap : mIaps) {
	    if (iap->mActivated) {
		iap->update();
	    }
	}
	mActNotified = false;
	setUpdated();
    }
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
    if (/*changed &&*/ mMag && mIapEnb.data(true).IsValid() && mIapEnb.data(true).mData) { // Ref ds_dcs_sdc_dsgn_oin Solution#1
	if (!getState(true)) { // Ref ds_dcs_sdc_dsgn_cc Solution#2
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
	// System is activated, this means some inputs is notified of change
	// Status transition potentially depends on any inputs so we need to
	// propagate notification to output
	notifyOutp();
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
	/*
	T data;
	res =  GetGData(inp, data);
	if (res) {
	    aRes = data;
	}
	*/
	GetGData(inp, aRes);
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

bool ASdcMut::getState(bool aConf)
{
    return mMutApplied;
}

bool ASdcMut::doCtl()
{
    bool res = false;
    if (mIapTarg.mCdt.IsValid()) {
	MNode* targn = mMag->getNode(mIapTarg.mCdt.mData);
	if (!targn) {
	    Log(TLog(EInfo, this) + "Cannot find target  [" + mIapTarg.mCdt.mData + "]");
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

bool ASdcComp::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	Sdata<string> name = mIapName.data(aConf);
	if (name.IsValid()) {
	    res = mMag->getComp(name.mData);
	}
    } else {
	Log(TLog(EDbg, this) + "Managed agent is not set");
    }
    return res;
}

bool ASdcComp::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    if (mIapName.mCdt.IsValid() && mIapParent.mCdt.IsValid()) {
	TMut mut(ENt_Node, ENa_Id, mIapName.mCdt.mData, ENa_Parent, mIapParent.mCdt.mData);
	chr->Root().AddChild(mut);
	mMag->mutate(chr->Root(), false, mutctx, true);
	delete chr;
	string muts = mut.ToString();
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	mOapName.updateData(mIapName.mCdt);
	res = true;
    }
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

bool ASdcRm::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	Sdata<string> name = mIapName.data(aConf);
	if (name.IsValid()) {
	    res = !mMag->getComp(name.mData);
	}
    } else {
	Log(TLog(EDbg, this) + "Managed agent is not set");
    }
    return res;
}

bool ASdcRm::doCtl()
{
    bool res = false;
    TNs ns; MutCtx mutctx(NULL, ns);
    MChromo* chr = mEnv->provider()->createChromo();
    if (mIapName.mCdt.IsValid()) {
	TMut mut(ENt_Rm, ENa_Id, mIapName.mCdt.mData);
	chr->Root().AddChild(mut);
	mMag->mutate(chr->Root(), false, mutctx, true);
	delete chr;
	string muts = mut.ToString();
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	mOapName.updateData(mIapName.mCdt);
	res = true;
    }
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

bool ASdcConn::getState(bool aConf)
{
    bool res = false;
    if (mMag) {
	Sdata<string> v1s = mIapV1.data(aConf);
	Sdata<string> v2s = mIapV2.data(aConf);
	if (v1s.IsValid() && v2s.IsValid()) {
	    MNode* v1n = mMag->getNode(v1s.mData);
	    MNode* v2n = mMag->getNode(v2s.mData);
	    if (v1n && v2n) {
		MVert* v1v = v1n->lIf(v1v);
		MVert* v2v = v2n->lIf(v2v);
		if (v1v && v2v) {
		    res = v1v->isConnected(v2v);
		} else {
		    Log(TLog(EDbg, this) + "CP is not connectable [" + (v1v ? v2s.mData : v1s.mData) + "]");
		}
	    } else {
		Log(TLog(EDbg, this) + "Cannot find CP [" + (v1n ? v2s.mData : v1s.mData) + "]");
	    }
	} else {
	    Log(TLog(EDbg, this) + "Managed agent is not set");
	}
    }
    return res;
}

bool ASdcConn::doCtl()
{
    bool res = false;
    // Checking the control condition met
    if (mIapV1.mCdt.IsValid() && mIapV2.mCdt.IsValid()) {
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
		    Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
		    res = true;
		} else {
		    Log(TLog(EInfo, this) + "CPs are already connected: [" + mIapV1.mCdt.mData + "] and [" + mIapV2.mCdt.mData + "]");
		}
	    } else {
		Log(TLog(EInfo, this) + "CP isn't vertex: [" + (v2v ? mIapV1.mCdt.mData : mIapV2.mCdt.mData) + "]");
	    }
	} else {
	    Log(TLog(EInfo, this) + "CP doesn't exist: [" + (v2n ? mIapV1.mCdt.mData : mIapV2.mCdt.mData) + "]");
	}
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
	Sdata<string> v1s = mIapV1.data(aConf);
	Sdata<string> v2s = mIapV2.data(aConf);
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
	Log(TLog(EDbg, this) + "Managed agent is not set");
    }
    return res;
}

bool ASdcDisconn::doCtl()
{
    bool res = false;
    if (mIapV1.mCdt.IsValid() && mIapV2.mCdt.IsValid()) {
	TNs ns; MutCtx mutctx(NULL, ns);
	MChromo* chr = mEnv->provider()->createChromo();
	TMut mut(ENt_Disconn, ENa_P, mIapV1.mCdt.mData, ENa_Q, mIapV2.mCdt.mData);
	chr->Root().AddChild(mut);
	mMag->mutate(chr->Root(), false, mutctx, true);
	delete chr;
	string muts = mut.ToString();
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	res = true;
    }
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
	    Log(TLog(EErr, this) + "State: Cannot find comp [" + mIapName.data(aConf).mData + "]");
	    break;
	}
	GUri uri_p = GUri(mIapPrev.data(aConf).mData);
	GUri uri_n = GUri(mIapNext.data(aConf).mData);
	GUri prev_uri(mIapName.data(aConf).mData); prev_uri += uri_p;
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "State: Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += uri_n;
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EErr, this) + "State: Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    Log(TLog(EErr, this) + "State: Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EErr, this) + "State: Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data(aConf).mData);
	if (!pnode) {
	    Log(TLog(EErr, this) + "State: Cannot find position node [" + mIapPname.data(aConf).mData + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data(aConf).mData); pnode_next_uri += uri_n;
	MNode* pnode_next = mMag->getNode(pnode_next_uri);
	if (!pnode_next) {
	    Log(TLog(EErr, this) + "State: Cannot find position node next cp [" + pnode_next_uri.toString() + "]");
	    break;
	}
	MVert* pnode_nextv = pnode_next->lIf(pnode_nextv);
	if (pnode_nextv->pairsCount() != 1) {
	    Log(TLog(EDbg, this) + "State: Position node next Cp pairs count != 1");
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
    // Verify conditions
    do {
	if (!mIapName.mCdt.IsValid() || !mIapPrev.mCdt.IsValid() || !mIapNext.mCdt.IsValid()) {
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data().mData);
	if (!comp) {
	    Log(TLog(EErr, this) + "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data().mData); prev_uri += GUri(mIapPrev.data().mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	mDobsNprev.updateNuo(prev);
	GUri next_uri(mIapName.data().mData); next_uri += GUri(mIapNext.data().mData);
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
	// TODO this workaround fixes DS_ISS_013 but needs persistent solution (checking status, ref SdcInsert3)
	if (prevv->pairsCount()) {
	    Log(TLog(EErr, this) + "Prev [" + prev_uri.toString() + "] is already connected to [" + prevv->getPair(0)->Uid() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EErr, this) + "Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	if (nextv->pairsCount()) {
	    Log(TLog(EErr, this) + "Next is already connected [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data().mData);
	if (!pnode) {
	    Log(TLog(EErr, this) + "Cannot find position node [" + mIapPname.data().mData + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data().mData); pnode_next_uri += GUri(mIapNext.data().mData);
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
	    Log(TLog(EErr, this) + "Failed disconnecting [" + pnode_next->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	// Connect
	cres = MVert::connect(prevv, pnode_nextv);
	if (!cres) {
	    Log(TLog(EErr, this) + "Failed connecting [" + prevv->Uid() + "] - [" + pnode_next->Uid() + "]");
	    break;
	}
	cres = MVert::connect(nextv, mCpPair);
	if (!cres) {
	    Log(TLog(EErr, this) + "Failed connecting [" + nextv->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	Log(TLog(EInfo, this) + "Managed agent is updated, inserted [" + comp->Uid() + "] before [" + pnode->Uid() + "]");
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
	    Log(TLog(EDbg, this) + "Getting state: Cannot find list start [" + mIapStartUri.data(aConf).mData + "]");
	    break;
	}
	MNode* end = mMag->getNode(mIapEndUri.data(aConf).mData);
	if (!end) {
	    Log(TLog(EDbg, this) + "Getting state: Cannot find list end [" + mIapEndUri.data(aConf).mData + "]");
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data(aConf).mData);
	if (!comp) {
	    Log(TLog(EDbg, this) + "Getting state: Cannot find comp [" + mIapName.data(aConf).mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data(aConf).mData);
	prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EDbg, this) + "Getting state: Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EDbg, this) + "Getting state: Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    Log(TLog(EErr, this) + "Getting state: Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EErr, this) + "Getting state: Next is not connectable [" + next_uri.toString() + "]");
	    break;
	}
	MNode* pnode = mMag->getNode(mIapPname.data(aConf).mData);
	if (!pnode) {
	    Log(TLog(EErr, this) + "Getting state: Cannot find position node [" + mIapPname.data(aConf).mData + "]");
	    break;
	}
	GUri pnode_next_uri(mIapPname.data(aConf).mData);
	pnode_next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* pnode_next = mMag->getNode(pnode_next_uri);
	if (!pnode_next) {
	    Log(TLog(EErr, this) + "Getting state: Cannot find position node next cp [" + pnode_next_uri.toString() + "]");
	    break;
	}
	MVert* pnode_nextv = pnode_next->lIf(pnode_nextv);
	if (pnode_nextv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Getting state: Position node next Cp pairs count != 1");
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
	    Log(TLog(EDbg, this) + "Cannot find comp [" + mIapName.data(aConf).mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data(aConf).mData);
	prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EDbg, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EDbg, this) + "Cannot find Next Cp [" + next_uri.toString() + "]");
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
	MNode* pnode = mMag->getNode(mIapPname.data(aConf).mData);
	if (!pnode) {
	    Log(TLog(EErr, this) + "Cannot find position node [" + mIapPname.data(aConf).mData + "]");
	    break;
	}
	GUri pnode_prev_uri(mIapPname.data(aConf).mData);
	pnode_prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* pnode_prev = mMag->getNode(pnode_prev_uri);
	if (!pnode_prev) {
	    Log(TLog(EErr, this) + "Cannot find position node prev cp [" + pnode_prev_uri.toString() + "]");
	    break;
	}
	MVert* pnode_prevv = pnode_prev->lIf(pnode_prevv);
	if (pnode_prevv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Position node prev Cp pairs count != 1");
	    break;
	}
	res = (nextv == pnode_prevv->getPair(0));
    } while (0);
    return res;
}

bool ASdcInsertN::doCtl()
{
    bool res = false;
    // Verify conditions
    do {
	if (!mIapName.mCdt.IsValid() || !mIapPrev.mCdt.IsValid() || !mIapNext.mCdt.IsValid()) {
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data().mData);
	if (!comp) {
	    Log(TLog(EErr, this) + "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data().mData); prev_uri += GUri(mIapPrev.data().mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	mDobsNprev.updateNuo(prev);
	GUri next_uri(mIapName.data().mData); next_uri += GUri(mIapNext.data().mData);
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
	MNode* pnode = mMag->getNode(mIapPname.data().mData);
	if (!pnode) {
	    Log(TLog(EErr, this) + "Cannot find position node [" + mIapPname.data().mData + "]");
	    break;
	}
	GUri pnode_prev_uri(mIapPname.data().mData); pnode_prev_uri += GUri(mIapPrev.data().mData);
	MNode* pnode_prev = mMag->getNode(pnode_prev_uri);
	if (!pnode_prev) {
	    Log(TLog(EErr, this) + "Cannot find position node prev cp [" + pnode_prev_uri.toString() + "]");
	    break;
	}
	MVert* pnode_prevv = pnode_prev->lIf(pnode_prevv);
	if (pnode_prevv->pairsCount() != 1) {
	    Log(TLog(EErr, this) + "Position node prev cp [" + pnode_prev_uri.toString() + "] pairs count != 1");
	    break;
	}
	mCpPair = pnode_prevv->getPair(0);
	// Disconnect Cp
	bool cres = MVert::disconnect(pnode_prevv, mCpPair);
	if (!cres) {
	    Log(TLog(EErr, this) + "Failed disconnecting [" + pnode_prev->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	// Connect
	cres = MVert::connect(nextv, pnode_prevv);
	if (!cres) {
	    Log(TLog(EErr, this) + "Failed connecting [" + nextv->Uid() + "] - [" + pnode_prev->Uid() + "]");
	    break;
	}
	cres = MVert::connect(prevv, mCpPair);
	if (!cres) {
	    Log(TLog(EErr, this) + "Failed connecting [" + prevv->Uid() + "] - [" + mCpPair->Uid() + "]");
	    break;
	}
	Log(TLog(EInfo, this) + "Managed agent is updated, inserted [" + comp->Uid() + "] after [" + pnode->Uid() + "]");
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
    bool res = true; // Note, errors are interpreted in favor of "extracted" state
    do {
	if (!mMag) break;
	if (!mIapName.data(aConf).IsValid() || !mIapPrev.data(aConf).IsValid() || !mIapNext.data(aConf).IsValid()) {
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data(aConf).mData);
	if (!comp) {
	    Log(TLog(EDbg, this) + "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data(aConf).mData); prev_uri += GUri(mIapPrev.data(aConf).mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EDbg, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data(aConf).mData); next_uri += GUri(mIapNext.data(aConf).mData);
	MNode* next = mMag->getNode(next_uri);
	if (!next) {
	    Log(TLog(EDbg, this) + "Cannot find Next Cp [" + next_uri.toString() + "]");
	    break;
	}
	MVert* prevv = prev->lIf(prevv);
	if (!prevv) {
	    Log(TLog(EDbg, this) + "Prev is not connectable [" + prev_uri.toString() + "]");
	    break;
	}
	MVert* nextv = next->lIf(nextv);
	if (!nextv) {
	    Log(TLog(EDbg, this) + "Next is not connectable [" + next_uri.toString() + "]");
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
	if (!mIapName.mCdt.IsValid() || !mIapPrev.mCdt.IsValid() || !mIapNext.mCdt.IsValid()) {
	    break;
	}
	MNode* comp = mMag->getNode(mIapName.data().mData);
	if (!comp) {
	    Log(TLog(EErr, this) + "Cannot find comp [" + mIapName.mCdt.mData + "]");
	    break;
	}
	GUri prev_uri(mIapName.data().mData); prev_uri += GUri(mIapPrev.data().mData);
	MNode* prev = mMag->getNode(prev_uri);
	if (!prev) {
	    Log(TLog(EErr, this) + "Cannot find Prev Cp [" + prev_uri.toString() + "]");
	    break;
	}
	GUri next_uri(mIapName.data().mData); next_uri += GUri(mIapNext.data().mData);
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
