
#include "des.h"
#include "dessdc.h"

/* SDC base agent */

const string K_CpUri_Enable = "Enable";
const string K_CpUri_Outp = "Outp";


template <class T>
void ASdc::SdcIap<T>::update()
{
    bool res = mHost->GetInpSdata<T>(mInpUri, mUdt);
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

ASdc::ASdc(const string &aType, const string& aName, MEnv* aEnv): Unit(aType, aName, aEnv),
   mMag(NULL), mUpdNotified(false), mActNotified(false), mObrCp(this), mAgtCp(this), mIapEnb(this, K_CpUri_Enable),
    mOapOut(this, K_CpUri_Outp)
{
    addInput(K_CpUri_Enable);
    addOutput(K_CpUri_Outp);
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
	rifDesIobs(mIapEnb, aReq);
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
    if (mIapEnb.mActivated) {
	mIapEnb.update();
    }
    mActNotified = false;
    setUpdated();
}

void ASdc::confirm()
{
    if (mIapEnb.mUpdated) {
	mIapEnb.confirm();
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
    if (!mUpdNotified) { // Notify owner
	MDesObserver* obs = Owner() ? Owner()->lIf(obs) : nullptr;
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	}
    }
}

void ASdc::onInpUpdated()
{
    setActivated();
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












/* SDC agent "Component" */

const string K_CpUri_Name = "Name";
const string K_CpUri_Parent = "Parent";

ASdcComp::ASdcComp(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapName(this, K_CpUri_Name), mIapParent(this, K_CpUri_Parent)
{
    addInput(K_CpUri_Name);
    addInput(K_CpUri_Parent);
}

ASdcComp::~ASdcComp()
{
}

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

void ASdcComp::update()
{
    if (mIapName.mActivated) {
	mIapName.update();
    }
    if (mIapParent.mActivated) {
	mIapParent.update();
    }
    ASdc::update();
}

void ASdcComp::confirm()
{
    ASdc::confirm();
    if (mIapName.mUpdated) {
	mIapName.confirm();
    }
    if (mIapParent.mUpdated) {
	mIapParent.confirm();
    }
    if (mIapName.mChanged || mIapParent.mChanged) {
	addComp();
	mOapOut.NotifyInpsUpdated();
    }
}

bool ASdcComp::addComp()
{
    /*
    bool res = false;
    if (mMag) {
	string name, parent;
	bool enable;
	res = GetInpSdata<string>(K_CpUri_Name, name);
	res &= GetInpSdata<string>(K_CpUri_Parent, parent);
	res &= GetInpSdata<bool>(K_CpUri_Enable, enable);
	if (res && enable) {
	    TNs ns; MutCtx mutctx(NULL, ns);
	    MChromo* chr = mEnv->provider()->createChromo();
	    chr->Init(ENt_Node);
	    TMut mut(ENt_Node, ENa_Id, name, ENa_Parent, parent);
	    chr->Root().AddChild(mut);
	    mMag->mutate(chr->Root(), false, mutctx, true);
	    delete chr;
	    string muts = mut.ToString();
	    Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
	}
    }
    return res;
    */
    bool res = false;
    if (mMag && mIapEnb.mCdt) {
	TNs ns; MutCtx mutctx(NULL, ns);
	MChromo* chr = mEnv->provider()->createChromo();
	chr->Init(ENt_Node);
	TMut mut(ENt_Node, ENa_Id, mIapName.mCdt, ENa_Parent, mIapParent.mCdt);
	chr->Root().AddChild(mut);
	mMag->mutate(chr->Root(), false, mutctx, true);
	delete chr;
	string muts = mut.ToString();
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
    }
    return res;

}

void ASdcComp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	rifDesIobs(mIapName, aReq);
	rifDesIobs(mIapParent, aReq);
    }
    ASdc::resolveIfc(aName, aReq);
}



/* SDC agent "Connect" */

const string K_CpUri_V1 = "V1";
const string K_CpUri_V2 = "V2";

ASdcConn::ASdcConn(const string &aType, const string& aName, MEnv* aEnv): ASdc(aType, aName, aEnv),
    mIapV1(this, K_CpUri_V1), mIapV2(this, K_CpUri_V2)
{
    addInput(K_CpUri_V1);
    addInput(K_CpUri_V2);
}

ASdcConn::~ASdcConn()
{
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
	    res = v1v->isConnected(v2v);
	}
    }
    return res;
}

void ASdcConn::update()
{
    if (mIapV1.mActivated) {
	mIapV1.update();
    }
    if (mIapV2.mActivated) {
	mIapV2.update();
    }
    ASdc::update();
}

void ASdcConn::confirm()
{
    ASdc::confirm();
    if (mIapV1.mUpdated) {
	mIapV1.confirm();
    }
    if (mIapV2.mUpdated) {
	mIapV2.confirm();
    }
    if (mIapV1.mChanged || mIapV2.mChanged) {
	doCtl();
	mOapOut.NotifyInpsUpdated();
    }
}

bool ASdcConn::doCtl()
{
    bool res = false;
    if (mMag && mIapEnb.mCdt) {
	TNs ns; MutCtx mutctx(NULL, ns);
	MChromo* chr = mEnv->provider()->createChromo();
	chr->Init(ENt_Node);
	TMut mut(ENt_Conn, ENa_P, mIapV1.mCdt, ENa_Q, mIapV2.mCdt);
	chr->Root().AddChild(mut);
	mMag->mutate(chr->Root(), false, mutctx, true);
	delete chr;
	string muts = mut.ToString();
	Log(TLog(EInfo, this) + "Managed agent is mutated  [" + muts + "]");
    }
    return res;
}

void ASdcConn::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	rifDesIobs(mIapV1, aReq);
	rifDesIobs(mIapV2, aReq);
    }
    ASdc::resolveIfc(aName, aReq);
}


