#include "syst.h"


const string KContProvided = "Provided";
const string KContRequired = "Required";

/** @brief CP specific IFR node
 * CP delegates IF resolution to this node. This is one option of IFR design
 * Another option is to request MIfProvOwner to resolve, ref MIfProvOwner::getIfaces()
 * */
class CpIfrNode: public IfrNode
{
    public:
	CpIfrNode(MIfProvOwner* aOwner, const ConnPointu* aHost): IfrNode(aOwner), mHost(aHost) {};
	// From MIfProv
	virtual bool resolve(const string& aName) override;
    private:
	const ConnPointu* mHost;
};

bool CpIfrNode::resolve(const string& aName)
{
    bool res = true;
    ConnPointu* host = const_cast<ConnPointu*>(mHost);
    MIface* ifr = host->MNode_getLif(aName.c_str());
    if (ifr && !findIface(ifr)) {
	IfrLeaf* lf = new IfrLeaf(mOwner, ifr);
	mCnode.connect(lf);
    }
    if (aName == mHost->provName()) {
	// Requested provided iface - cannot be obtain via pairs - redirect to host
	auto owner = mHost->Owner();
	const MUnit* ownu = owner ? owner->lIf(ownu): nullptr;
	if (ownu) {
	    res = const_cast<MUnit*>(ownu)->resolveIface(aName, this->binded());
	}
    } else if (aName == mHost->reqName()) {
	for (MVert* pair : mHost->mPairs) {
	    MUnit* pairu = pair->lIf(pairu);
	    MIfProvOwner* pairo = pairu->lIf(pairo);
	    if (!findOwner(pairo)) {
		res = pairu->resolveIface(aName, this->binded());
	    }
	}
    }
    eraseInvalid();
    mValid = res;

    return res;
}

class CpIfrNodeRoot : public CpIfrNode
{
    public:
	CpIfrNodeRoot(MIfProvOwner* aOwner, const string& aIfName, const ConnPointu* aHost): CpIfrNode(aOwner, aHost), mName(aIfName) {}
	virtual string name() const override { return mName;}
    private:
	string mName;
};


string ConnPointu::KReqName = "Required";
string ConnPointu::KProvName = "Provided";


ConnPointu::ConnPointu(const string &aName, MEnv* aEnv): Vertu(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

MIface* ConnPointu::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MNode_getLif(aType);
    return res;
}

MIface* ConnPointu::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MVert_getLif(aType);
    return res;
}


MIface* ConnPointu::MIfProvOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MIfProvOwner_getLif(aType);
    return res;
}

string ConnPointu::provName() const
{
    string res;
    bool pres = getContent(KContProvided, res);
    return res;
}

string ConnPointu::reqName() const
{
    string res;
    bool pres = getContent(KContRequired, res);
    return res;
}

IfrNode* ConnPointu::createIfProv(const string& aName, MIfReq::TIfReqCp* aReq) const
{
    IfrNode* res = nullptr;
    ConnPointu* self = const_cast<ConnPointu*>(this);
    if (!aReq) {
	res = new CpIfrNodeRoot(self, aName, this);
    } else {
	res = new CpIfrNode(self, this);
    }
    return res;
}

void ConnPointu::onConnected()
{
    invalidateIrm();
}

void ConnPointu::onDisconnected()
{
    invalidateIrm();
}

MContent* ConnPointu::getCont(int aIdx)
{
    MContent* res = nullptr;
    if (aIdx == 0) res = &mProv;
    else if (aIdx == 1) res = &mReq;
    return res;
}

const MContent* ConnPointu::getCont(int aIdx) const
{
    const MContent* res = nullptr;
    if (aIdx == 0) res = &mProv;
    else if (aIdx == 1) res = &mReq;
    return res;
}

bool ConnPointu::getContent(const GUri& aCuri, string& aRes) const
{
    bool res = true;
    string name = aCuri;
    if (name == KProvName)
	res = mProv.getData(aRes);
    else if (name == KReqName)
	res = mReq.getData(aRes);
    else res = Vertu::getContent(aCuri, aRes);
    return res;
}

bool ConnPointu::setContent(const GUri& aCuri, const string& aData)
{
    bool res = true;
    string name = aCuri;
    if (name == KProvName)
	res = mProv.setData(aData);
    else if (name == KReqName)
	res = mReq.setData(aData);
    else res = Vertu::setContent(aCuri, aData);
    return res;
}

bool ConnPointu::isCompatible(MVert* aPair, bool aExt)
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




// Extender agent, monolitic, multicontent, unit


const string KUriInt = "Int";  /*!< Internal connpoint */
const string KContDir = "Direction";
const string KContDir_Val_Regular = "Regular";
const string KContDir_Val_Inp = "Inp";
const string KContDir_Val_Out = "Out";



Extd::Extd(const string& aName, MEnv* aEnv): Vertu(aName, aEnv)
{
    if (aName.empty()) mName = Type();
    setContent(KContDir, KContDir_Val_Regular);
}

bool Extd::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
    MIface* ifc = MNode_getLif(aName.c_str());
    if (ifc) { // Local iface
	addIfpLeaf(ifc, aReq);
    } else {
	// Redirect to internal point or pair depending on the requiestor
	MVert* intcp = getExtd();
	MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
	MIfProvOwner* intcpo = intcpu ? intcpu->lIf(intcpo) : nullptr;
	if (intcpo && aReq->provided()->isRequestor(intcpo)) {
	    // Redirect to pairs
	    for (int i = 0; i < pairsCount(); i++) {
		MVert* pair = getPair(i);
		MUnit* pairu = pair ? pair->lIf(pairu) : nullptr;  
		MIfProvOwner* pairo = pairu ? pairu->lIf(pairo) : nullptr;
		if (pairo && aReq->provided()->isRequestor(pairo)) {
		    res = pairu->resolveIface(aName, aReq);
		}
	    }
	} else {
	    // Redirect to internal CP
	    MUnit* intcpu = intcp->lIf(intcpu);  
	    res = intcpu->resolveIface(aName, aReq);
	}
    }
    return res;
}

/*
void Extd::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TBool resg = EFalse;
    TIfRange rr;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    if (res == NULL) {
	// Redirect to internal point or pair depending on the requiestor
	MUnit* intcp = GetExtd();
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, intcp, rr);
	    resg = resg || (rr.first != rr.second);
	}
	else {
	    TInt count = PairsCount();
	    for (TInt ct = 0; ct < count; ct++) {
		MVert* pair = GetPair(ct);
		MUnit* ep = pair->GetObj(ep);
		if (ep != NULL && !ctx.IsInContext(ep)) {
		    rr = ep->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, ep, rr);
		    resg = resg || (rr.first != rr.second);
		}
	    }
	}
    }
    // Responsible pairs not found, redirect to upper layer
    if (rr.first == rr.second && iMan != NULL) {
	MUnit* hostmgr = GetMan();
	MUnit* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, mgr, rr);
	    resg = resg || (rr.first != rr.second);
	}
    }
}
*/

bool Extd::isCompatible(MVert* aPair, bool aExt)
{
    bool res = false;
    MVert* intcp = getExtd();
    if (intcp) {
	res = intcp->isCompatible(aPair, !aExt);
    }
    return res;
}

MVert* Extd::getExtd()
{
    MVert* res = nullptr;
    MNode* extn = getNode(KUriInt);
    res = extn->lIf(res);
    return res;
}

MVert::TDir Extd::getDir() const
{
    TDir res = ERegular;
    string cdir;
    getContent(KContDir, cdir);
    if (cdir == KContDir_Val_Inp) res = EInp;
    else if (cdir == KContDir_Val_Out) res = EOut;
    return res;
}



// System


class SystIfrNode: public IfrNode
{
    public:
	SystIfrNode(MIfProvOwner* aOwner, const Syst* aHost): IfrNode(aOwner), mHost(aHost) {};
	virtual bool resolve(const string& aName) override;
    private:
	const Syst* mHost;
};

class SystIfrNodeRoot : public SystIfrNode
{
    public:
	SystIfrNodeRoot(MIfProvOwner* aOwner, const string& aIfName, const Syst* aHost): SystIfrNode(aOwner, aHost), mName(aIfName) {}
	virtual string name() const override { return mName;}
    private:
	string mName;
};

bool SystIfrNode::resolve(const string& aName)
{
    bool res = true;
    Syst* host = const_cast<Syst*>(mHost);
    MIface* ifr = host->MNode_getLif(aName.c_str());
    if (ifr && !findIface(ifr)) {
	IfrLeaf* lf = new IfrLeaf(mOwner, ifr);
	mCnode.connect(lf);
    }
    if (aName == MAgent::Type()) {
	for (int i = 0; i < host->mCpOwner.pcount(); i++) {
	    MOwned* comp = host->mCpOwner.pairAt(i);
	    MNode* compn = comp->lIf(compn);
	    MAgent* compa = compn ? compn->lIf(compa) : nullptr;
	    if (compa) {
		if (compa && !findIface(compa)) {
		    IfrLeaf* lf = new IfrLeaf(mOwner, compa);
		    mCnode.connect(lf);
		}
	    }
	}
    } else {
	// Redirect to agents
	MIfProv* ifp = host->defaultIfProv(MAgent::Type());
	MIfProv* maprov = ifp->first();
	while (maprov) {
	    MUnit* agtu = maprov ? maprov->iface()->lIf(agtu) : nullptr;
	    if (agtu && !findIface(agtu)) {
		res = agtu->resolveIface(aName, this->binded());
	    }
	    maprov = maprov->next();
	}
    }
    mValid = res;
    return res;
}



Syst::Syst(const string &aName, MEnv* aEnv): Elem(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

Syst::~Syst()
{
}

MIface* Syst::doMOwnerGetLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    else if (res = checkLif<MUnit>(aType));
    return res;
}

void Syst::mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string sp = aMut.Attr(ENa_P);
    string sq = aMut.Attr(ENa_Q);
    MNode* pn = getNode(sp);
    MNode* qn = getNode(sq);
    if (pn && qn) {
	MVert* pv = pn->lIf(pv);
	MVert* qv = qn->lIf(pv);
	if (pv && qv) {
	    bool res = MVert::connect(pv, qv);
	    if (!res) {
		Log(TLog(EErr, this) + "Failed connecting [" + sp + "] to [" + sq + "]");
	    }
	} else {
	    Log(TLog(EErr, this) + "Connecting [" + sp + "] to [" + sq + "] -  [" + (pv ? sq : sp) + "] isn't connectable");
	}
    } else {
	Log(TLog(EErr, this) + "Connecting [" + sp + "] to [" + sq + "] - cannot find [" + (pn ? sq : sp) + "]");
    }
    if (!aUpdOnly) {
	mChromo->Root().AddChild(aMut, true, false);
	notifyNodeMutated(aMut, aCtx);
    }
}

IfrNode* Syst::createIfProv(const string& aName, MIfReq::TIfReqCp* aReq) const
{
    Syst* self = const_cast<Syst*>(this);
    return aReq ? new SystIfrNode(self, this) : new SystIfrNodeRoot(self, aName, this);
}

