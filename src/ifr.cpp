#include <stdio.h>
#include <iostream> 

#include "ifr.h"

string IfrNode::name() const
{
    if (mPair) {
	//return mPair->provided()->owner()->name();
	return mPair->binded()->provided()->name();
    } else {
	return string();
    }
}

MIfProv* IfrNode::first() const
{
    MIfProv* res = nullptr;
    bool pres = true;
    string nm = name();
    IfrNode* self = const_cast<IfrNode*>(this);
    if (!mValid) {
	pres = self->resolve(nm);
    }
    if (pres && mValid) {
	auto fl = self->mCnode.firstLeaf();
	res =  fl ? fl->provided() : nullptr;
    }
    return res;
}

MIfProv* IfrNode::next() const
{
    MIfProv* res = nullptr;
    auto self = const_cast<IfrNode*>(this);
    res = mPair->provided()->next(self);
    if (!res->iface()) {
	res = res->next();
    }
    return res;
}

/* Default implementation just redirect the request to the owner
 * Another solutionis is to use specific node that does resolution in 
 * behalf of the owner.
 * */
bool IfrNode::resolve(const string& aName)
{
    mValid = mOwner->resolveIfc(aName, binded());
    // Cleanup the node - remove all invalid pairs
    eraseInvalid();
    return mValid;
}

MIfProv* IfrNode::next(MIfProv::TCp* aProvCp) const
{
    return nullptr;
}

void IfrNode::MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    Ifu::offset(aIdt, aOs);
    aOs  << "[" << name() << "], " << mOwner->Uid() << ", Valid: " << mValid << endl;
    auto self = const_cast<IfrNode*>(this);
    auto pair = self->binded()->firstPair();
    aIdt += 2;
    while (pair) {
	pair->provided()->doDump(aLevel, aIdt, aOs);
	pair = self->binded()->nextPair(pair);
    }
}

void IfrNode::setValid(bool aValid)
{
    mValid = aValid;
    if (mPair && !aValid) {
	auto down = mPair->binded();
	if (down) {
	    down->provided()->setValid(aValid);
	}
    }
}

MIfProv* IfrNode::findIface(const MIface* aIface)
{
    MIfProv* res = nullptr;
    auto pair = binded()->firstPair();
    while (pair) {
	auto prov = pair->provided();
	if (prov->iface() == aIface) {
	    res = prov; break;
	}
	pair = binded()->nextPair(pair);
    }
    return res;
}

MIfProv* IfrNode::findOwner(const MIfProvOwner* aOwner)
{
    MIfProv* res = nullptr;
    auto pair = binded()->firstPair();
    while (pair) {
	auto prov = pair->provided();
	if (prov->owner() == aOwner) {
	    res = prov; break;
	}
	pair = binded()->nextPair(pair);
    }
    return res;
}

void IfrNode::eraseInvalid()
{
    auto pair = binded()->firstPair();
    while (pair) {
	auto prov = pair->provided();
	if (!prov->isValid()) {
	    binded()->disconnect(pair);
	    // pairs container in binded is updated, starts from the firs
	    pair = binded()->firstPair();
	} else {
	    pair = binded()->nextPair(pair);
	}
    }
}

bool IfrNode::isRequestor(MIfProvOwner* aOwner) const
{
    bool res = aOwner == mOwner;
    if (!res && mPair) {
	res = mPair->provided()->isRequestor(aOwner);
    }
    return res;
}



string IfrLeaf::name() const
{
    if (mPair) {
	//return mPair->provided()->owner()->name();
	return mPair->binded()->provided()->name();
    } else {
	return string();
    }
}

MIfProv* IfrLeaf::next() const
{
    IfrLeaf* self = const_cast<IfrLeaf*>(this);
    auto next = self->nextLeaf();
    return next ? next->provided() : nullptr;
}

void IfrLeaf::MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    Ifu::offset(aIdt, aOs);
    aOs  << "<" << name() << ">, " << mOwner->Uid() << ", Valid: " << mValid << ", Iface: " << (mIface ? mIface->Uid() : "null") << endl;
}

void IfrLeaf::setValid(bool aValid)
{
    mValid = aValid;
    if (mPair && !aValid) {
	auto down = mPair->binded();
	if (down) {
	    down->provided()->setValid(aValid);
	}
    }
}


