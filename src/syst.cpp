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



ConnPointu::ConnPointu(const string &aName, MEnv* aEnv): Vertu(aName, aEnv)
{
}

MIface* ConnPointu::MUnit_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else 
	res = Vertu::MUnit_getLif(aType);
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
    bool pres = getContent(res, KContProvided);
    return res;
}

string ConnPointu::reqName() const
{
    string res;
    bool pres = getContent(res, KContRequired);
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
