
#include "vert.h"


Vertu::~Vertu()
{
}

bool Vertu::connect(MCIface* aPair)
{
    assert(aPair && !aPair->isConnected(dynamic_cast<MVert*>(this)) && !isConnected(aPair));
    bool res = false;
    MVert* vp = dynamic_cast<MVert*>(aPair);
    if (vp) {
	mPairs.insert(vp);
	res = true;
    }
    return res;
}

bool Vertu::disconnect(MCIface* aPair)
{
    return false;
}

bool Vertu::isConnected(MCIface* aPair) const
{
    MVert* vp = dynamic_cast<MVert*>(aPair);
    return vp && mPairs.count(vp) == 1;
}

bool Vertu::isCompatible(MCIface* aPair) const
{
    return dynamic_cast<MVert*>(aPair);
}

