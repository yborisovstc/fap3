#include "syst.h"


const string KContProvided = "Provided";
const string KContRequired = "Required";

class CpIfrNodeRoot: public IfrNodeRoot
{
    public:
	CpIfrNodeRoot(const string& aIfName, const ConnPointu* aHost): IfrNodeRoot(aIfName), mHost(aHost) {};
	// From MIfProv
	virtual bool resolve(const string& aName) override;
    private:
	const ConnPointu* mHost;
};

bool CpIfrNodeRoot::resolve(const string& aName)
{
    bool res = false;
    ConnPointu* host = const_cast<ConnPointu*>(mHost);
    MIface* ifr = host->getLif(aName.c_str());
    if (ifr) {
	IfrLeaf* lf = new IfrLeaf(ifr);
	mCnode.connect(lf);
	mValid = true;
	res = true;
    }
    return res;
}




ConnPointu::ConnPointu(const string &aName, MEnv* aEnv): Vertu(aName, aEnv)
{
}

MIface* ConnPointu::getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else 
	res = Vertu::getLif(aType);
    return res;
}


string ConnPointu::provided() const
{
    string res;
    bool pres = getContent(res, KContProvided);
    return res;
}

string ConnPointu::required() const
{
    string res;
    bool pres = getContent(res, KContRequired);
    return res;
}

bool ConnPointu::isCompatible(MCIface* aPair) const
{
    MConnPoint* cp = aPair->lIf(cp);
    return cp && cp->provided() == required() && cp->required() == provided();
}

IfrNode* ConnPointu::createDefaultIfProv(const string& aName) const
{
    return new CpIfrNodeRoot(aName, this);
}

