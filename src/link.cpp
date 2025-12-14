

#include "link.h"


Link::Link(const string &aType, const string &aName, MEnv* aEnv): Unit(aType, aName, aEnv), mPair(nullptr), mOcp(this)
{
}

Link::~Link()
{
}

MIface* Link::MLink_getLif(TIdHash aTid)
{
    return nullptr;
}

void Link::MLink_doDump(int aLevel, int aIdt, ostream& aOs) const
{
}

MIface* Link::MNode_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMLinkPtr));
    else res = Unit::MNode_getLif(aTid);
    return res;
}


bool Link::connect(MNode* aPair)
{
    bool res = false;
    if (!mPair) {
	MObservable* pairo = aPair->lIf(pairo);
	if (pairo) {
	    mPair = aPair;
	    res = pairo->addObserver(&mOcp);
	}
	notifyChanged();
    }
    return res;
}

bool Link::disconnect(MNode* aPair)
{
    bool res = false;
    if (mPair && mPair == aPair) {
	MObservable* pairo = aPair->lIf(pairo);
	if (pairo) {
	    mPair = nullptr;
	    res = pairo->rmObserver(&mOcp);
	}
	notifyChanged();
    }
    return res;
}


MNode* Link::pair()
{
    return mPair;
}
