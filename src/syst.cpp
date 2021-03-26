#include "syst.h"


const string KContProvided = "Provided";
const string KContRequired = "Required";

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
    MIface* ifr = host->MUnit_getLif(aName.c_str());
    if (ifr && !findIface(ifr)) {
	IfrLeaf* lf = new IfrLeaf(mOwner, ifr);
	mCnode.connect(lf);
    }
    if (aName == mHost->provName()) {
	// Requested provided iface - cannot be obtain via pairs - redirect to host
	auto owner = mHost->Owner();
	if (owner) {
	    const MIfProvOwner* powno = owner->lIf(powno);
	    const MUnit* pownu = powno ? powno->lIf(pownu): nullptr;
	    if (powno && !findOwner(powno)) {
		res = const_cast<MUnit*>(pownu)->resolveIface(aName, this->binded());
	    }
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

ConnPointu::Cnt::~Cnt() {}

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

bool ConnPointu::isCompatible(MCIface* aPair) const
{
    MConnPoint* cp = aPair->lIf(cp);
    return cp && cp->provName() == reqName() && cp->reqName() == provName();
}

IfrNode* ConnPointu::createIfProv(const string& aName, TIfReqCp* aReq) const
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
    else res = false;
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
    else res = false;
    return res;
}

void ConnPointu::Cnt::MContent_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    offset(aIdt, aOs);
    cout << mName << ": " << mData << endl;
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
    else if (res = checkLif<MIfProvOwner>(aType));
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


