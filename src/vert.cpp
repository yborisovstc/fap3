
#include <cstring>

#include "vert.h"


//// Vertu

Vertu::Vertu(const string &aType, const string &aName, MEnv* aEnv): Unit(aType, aName, aEnv)
{
}

Vertu::~Vertu()
{
    disconnect();
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
    return res;
}

void Vertu::MVert_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, aOs); aOs << "UID: " << MVert_Uid() << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	Ifu::offset(aIdt, aOs); aOs << "Pairs: " << endl;
	for (int i = 0; i < pairsCount(); i++) {
	    const MVert* pair = getPair(i);
	    Ifu::offset(aIdt, aOs); aOs << "- "  << pair->Uid() << endl;
	}
    }
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

void Vertu::disconnect()
{
    while (pairsCount()) {
	MVert::disconnect(this, getPair(0));
    }
}

bool Vertu::isConnected(MCIface* aPair) const
{
    MVert* vp = dynamic_cast<MVert*>(aPair);
    return vp && mPairs.count(vp) == 1;
}

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
    for (auto it = mPairs.begin(); it != mPairs.end(); it++)
	if (aInd-- == 0) return *it;
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
    notifyChanged();
}

void Vertu::onDisconnected()
{
    invalidateIrm();
    notifyChanged();
}


//// Vert

Vert::Vert(const string &aType, const string &aName, MEnv* aEnv): Elem(aType, aName, aEnv)
{
}

Vert::~Vert()
{
    disconnect();
}

MIface* Vert::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MVert>(aType));
    else res = Elem::MNode_getLif(aType);
    return res;
}

MIface* Vert::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MVert>(aType));
    else if (res = checkLif<MUnit>(aType));
    else res = MNode_getLif(aType);
    return res;
}

void Vert::MVert_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, aOs); aOs << "UID: " << MVert_Uid() << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	for (int i = 0; i < pairsCount(); i++) {
	    const MVert* pair = getPair(i);
	    Ifu::offset(aIdt, aOs); aOs << "- "  << pair->Uid() << endl;
	}
    }
}

bool Vert::connect(MCIface* aPair)
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

bool Vert::disconnect(MCIface* aPair)
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

void Vert::disconnect()
{
    while (pairsCount()) {
	MVert::disconnect(this, getPair(0));
    }
}


bool Vert::isConnected(MCIface* aPair) const
{
    MVert* vp = dynamic_cast<MVert*>(aPair);
    return vp && mPairs.count(vp) == 1;
}

bool Vert::isCompatible(MCIface* aPair) const
{
    bool res = false;
    MVert* cp = aPair->lIf(cp);
    if (cp) {
	Vert* self = const_cast<Vert*>(this);
	res = self->isCompatible(cp, false);
    }
    return res;
}

int Vert::pairsCount() const
{
    return mPairs.size();
}

MVert* Vert::getPair(int aInd) const
{
    for (auto it = mPairs.begin(); it != mPairs.end(); it++)
	if (aInd-- == 0) return *it;
    return nullptr;
}

bool Vert::isPair(const MVert* aPair) const
{
    MVert* v = const_cast<MVert*>(aPair);
    return mPairs.count(v) > 0;
}

bool Vert::isLinked(const MVert* aPair, bool aDirect) const
{
    return false;
}

void Vert::onConnected()
{
    invalidateIrm();
}

void Vert::onDisconnected()
{
    invalidateIrm();
}


