
#include "des.h"
#include "dessdc.h"

/* SDC base agent */

const string K_CpUri_Enable = "Enable";
const string K_CpUri_Outp = "Outp";

ASdc::SdcIapb::SdcIapb(const string& aName, ASdc* aHost, const string& aInpUri):
    mName(aName), mHost(aHost), mInpUri(aInpUri), mUpdated(false), mActivated(false), mChanged(false)
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
    bool res = mHost->GetInpSdata<T>(mInpUri, mCdt);
    if (mHost->isLogLevel(EDbg)/* && mUdt != mCdt*/) {
	mHost->Log(TLog(EDbg, mHost) + "[" + mName + "] Updated: [" + toStr(mCdt) + "] -> [" + toStr(mUdt) + "]");
    }
    mActivated = false;
    //setUpdated();
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
    mIaps(), mMag(NULL), mUpdNotified(false), mActNotified(false), mObrCp(this), mAgtCp(this), mIapEnb("Enb", this, K_CpUri_Enable),
    mOapOut("Outp", this, K_CpUri_Outp, [this](Sdata<bool>& aData) {getOut(aData);})/*, mMapCcd("Ccd", this, [this](bool& aData) {getCcd(aData);})*/,
    mMagObs(this)
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
    else if (res = checkLif<MAgent>(aType));
    else if (res = checkLif<MDesObserver>(aType));
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

void ASdc::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	for (auto iap : mIaps) {
	    rifDesIobs(*iap, aReq);
	}
    } else if (aName == MDesObserver::Type()) {
	if (!isRequestor(aReq, ahostNode())) {
	    MIface* iface = MNode_getLif(MDesObserver::Type());
	    addIfpLeaf(iface, aReq);
	}
    } else if (aName == MDVarGet::Type()) {
	MNode* cp = getNode(K_CpUri_Outp);
	if (isRequestor(aReq, cp)) {
	    MIface* iface = dynamic_cast<MDVarGet*>(&mOapOut);
	    addIfpLeaf(iface, aReq);
	}
    } else {
	Unit::resolveIfc(aName, aReq);
    }
}

MIface* ASdc::MAgent_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MUnit>(aType)); // To allow client to request IFR
    else if (res = checkLif<MDesObserver>(aType));
    return res;
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
    /*
    if (mIapEnb.mActivated) {
	mIapEnb.update();
    }
    */
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
    /*
    for (auto iap : mIaps) {
	if (iap->mUpdated) {
	    iap->confirm();
	    changed |= iap->mChanged;
	}
    }
    */
    /*
    for (auto map : mMaps) {
	if (map->mUpdated) {
	    map->confirm();
	    changed |= map->mChanged;
	}
    }
    */
    if (true) {
	bool res = doCtl();
	if (!res) {
	    Log(TLog(EErr, this) + "Failed controlling managed agent");
	} else {
	    mOapOut.NotifyInpsUpdated();
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

void ASdc::onInpUpdated()
{
    setActivated();
    // Notify Outp
    mOapOut.NotifyInpsUpdated();
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
}

MIface* ASdc::MObserver_getLif(const char *aType)
{
    MIface* res = nullptr;
    return res;
}

void ASdc::getOut(Sdata<bool>& aData)
{
    aData.mData = getState();
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
    if (!res) {
	Logger()->Write(EErr, this, "Cannot attach to observer");
    }
    // Registering in agent host
    MActr* ac = Owner()->lIf(ac);
    res = ac->attachAgent(&mAgtCp);
    if (!res) {
	Logger()->Write(EErr, this, "Cannot attach to host");
    }
    // Set managed agent
    mMag = ahostNode();
    /* MAG access point is disabled ATM
    MObservable* magob = mMag->lIf(magob);
    magob->addObserver(&mMagObs.mOcp);
    */
}

MNode* ASdc::ahostNode()
{
    auto pair = mAgtCp.firstPair();
    MAhost* ahost = pair ? pair->provided() : nullptr;
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    return hostn;
}

template<typename T> bool ASdc::GetInpSdata(const string aInpUri, T& aRes)
{
    bool res = false;
    MNode* inp = getNode(aInpUri);
    if (inp) {
	res =  GetSData(inp, aRes);
    } else {
	Log(TLog(EErr, this) + "Cannot get input [" + aInpUri + "]");
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
	Log(TLog(EErr, this) + "Cannot get input [" + aInpUri + "]");
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
    /* TODO The ifaces aren't collected. To debug
    auto ifaces = cpu->getIfs<MDesInpObserver>();
    if (ifaces) for (auto ifc : *ifaces) {
	MDesInpObserver* ifco = dynamic_cast<MDesInpObserver*>(ifc);
	if (ifco) {
	    ifco->onInpUpdated();
	}
    }
    */
    MIfProv* ifp = cpu->defaultIfProv(MDesInpObserver::Type());
    MIfProv* prov = ifp->first();
    while (prov) {
	MDesInpObserver* obs = dynamic_cast<MDesInpObserver*>(prov->iface());
	obs->onInpUpdated();
	prov = prov->next();
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
    bool res = true;
    if (mMag && mIapEnb.mCdt) {
	res = false;
	MNode* targn = mMag->getNode(mIapTarg.mCdt);
	if (!targn) {
	    Log(TLog(EInfo, this) + "Cannot find target  [" + mIapTarg.mCdt + "]");
	} else {
	    Chromo2& chromo = mIapMut.mCdt.mData;
	    TNs ns; MutCtx mutctx(NULL, ns);
	    chromo.Root().Dump();
	    targn->mutate(chromo.Root(), false, mutctx, true);
	    string muts;
	    chromo.Root().ToString(muts);
	    Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	    res = true;
	}
    }
    return res;
}


/* SDC agent "Component" */

const string K_CpUri_Name = "Name";
const string K_CpUri_Parent = "Parent";
const string K_CpUri_OutpName = "OutpName";

ASdcComp::ASdcComp(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName("Name", this, K_CpUri_Name), mIapParent("Parent", this, K_CpUri_Parent)
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
    bool res = true;
    if (mMag && mIapEnb.mCdt) {
	res = false;
	TNs ns; MutCtx mutctx(NULL, ns);
	MChromo* chr = mEnv->provider()->createChromo();
	TMut mut(ENt_Node, ENa_Id, mIapName.mCdt, ENa_Parent, mIapParent.mCdt);
	chr->Root().AddChild(mut);
	mMag->mutate(chr->Root(), false, mutctx, true);
	delete chr;
	string muts = mut.ToString();
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	res = true;
    }
    return res;
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
    bool res = true;
    if (mMag && /*mMapCcd.mCdt &&*/ mIapEnb.mCdt) {
	res = false;
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
    bool res = true;
    if (mMag && mIapEnb.mCdt) {
	res = false;
	TNs ns; MutCtx mutctx(NULL, ns);
	MChromo* chr = mEnv->provider()->createChromo();
	TMut mut(ENt_Disconn, ENa_P, mIapV1.mCdt, ENa_Q, mIapV2.mCdt);
	chr->Root().AddChild(mut);
	mMag->mutate(chr->Root(), false, mutctx, true);
	delete chr;
	string muts = mut.ToString();
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	res = true;
    }
    return res;
}


// SDC agent "Insert"

const string K_CpUri_Cp = "TCp";
const string K_CpUri_Icp = "ICp";
const string K_CpUri_Icpp = "ICpp";

ASdcInsert::ASdcInsert(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapCp("Cp", this, K_CpUri_Cp), mIapIcp("Icp", this, K_CpUri_Icp), mIapIcpp("Icpp", this, K_CpUri_Icpp)
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
    bool res = true;
    if (mMag && /*mMapCcd.mCdt &&*/ mIapEnb.mCdt) {
	res = false;
	// Verify conditions
	MNode* cp = mMag->getNode(mIapCp.mCdt);
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
		    MNode* icpn = mMag->getNode(mIapIcp.mCdt);
		    MNode* icppn = mMag->getNode(mIapIcpp.mCdt);
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
				Log(TLog(EInfo, this) + "Managed agent is updated, inserted [" + icpv->Uid() + "] into [" + cpv->Uid() + "]");
				res = true;
			    }
			}
		    }
		}
		// Do mutation
		/*
		   TNs ns; MutCtx mutctx(NULL, ns);
		   MChromo* chr = mEnv->provider()->createChromo();
		   chr->Init(ENt_Node);
		   string cpPairUri = mCpPair->getUriS(mMag);
		// TODO to implement one-side disconnecting
		TMut mut(ENt_Disconn, ENa_P, mIapCp.mCdt, ENa_Q, cpPairUri);
		chr->Root().AddChild(mut);
		chr->Root().AddChild(TMut(ENt_Cconn, ENa_P, mIapIcp.mCdt, ENa_Q, mIapCp.mCdt));
		chr->Root().AddChild(TMut(ENt_Cconn, ENa_P, mIapIcpp.mCdt, ENa_Q, cpPairUri));
		mMag->mutate(chr->Root(), false, mutctx, true);
		delete chr;
		string muts = mut.ToString();
		Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
		*/
	    }
	}
    }
    return res;
}
