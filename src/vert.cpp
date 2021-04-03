
#include <cstring>

#include "vert.h"


Vertu::Vertu(const string &aName, MEnv* aEnv): Unit(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

Vertu::~Vertu()
{
}

MIface* Vertu::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MVert>(aType));
    else res = Unit::MNode_getLif(aType);
    return res;
}

MIface* Vertu::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MVert>(aType));
    else if (res = checkLif<MUnit>(aType));
    else res = MNode_getLif(aType);
    return res;
}

bool Vertu::connect(MCIface* aPair)
{
    assert(aPair && !isConnected(aPair));
    bool res = isCompatible(aPair);
    if (res) {
	MVert* vp = dynamic_cast<MVert*>(aPair);
	if (vp) {
	    mPairs.insert(vp);
	    onConnected();
	    res = true;
	}
    }
    return res;
}

bool Vertu::disconnect(MCIface* aPair)
{
    assert(aPair && isConnected(aPair));
    bool res = false;
    MVert* vp = dynamic_cast<MVert*>(aPair);
    if (vp) {
	mPairs.erase(vp);
	onDisconnected();
	res = true;
    }
    return res;
}

bool Vertu::isConnected(MCIface* aPair) const
{
    MVert* vp = dynamic_cast<MVert*>(aPair);
    return vp && mPairs.count(vp) == 1;
}

/*
bool Vertu::isCompatible(MCIface* aPair) const
{
    return dynamic_cast<MVert*>(aPair);
}
*/

bool Vertu::isCompatible(MCIface* aPair) const
{
    bool res = false;
    MVert* cp = aPair->lIf(cp);
    if (cp) {
	Vertu* self = const_cast<Vertu*>(this);
	res = self->isCompatible(cp, false);
    }
    return res;
}

int Vertu::pairsCount() const
{
    return mPairs.size();
}

MVert* Vertu::getPair(int aInd) const
{
    return nullptr;
}

bool Vertu::isPair(const MVert* aPair) const
{
    MVert* v = const_cast<MVert*>(aPair);
    return mPairs.count(v) > 0;
}

bool Vertu::isLinked(const MVert* aPair, bool aDirect) const
{
    return false;
}

void Vertu::onConnected()
{
    invalidateIrm();
}

void Vertu::onDisconnected()
{
    invalidateIrm();
}


