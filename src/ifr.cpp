#include <stdio.h>
#include <iostream> 

#include "ifr.h"
#include "prof_ids.h"
#include "prof.h"

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
    string nm = name();
    IfrNode* self = const_cast<IfrNode*>(this);
    if (!mValid) {
	self->resolve(nm);
    }
    if (mValid) {
	auto fl = self->firstLeafB();
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

/* Default implementation just redirects the request to the owner
 * Another solutionis is to use specific node that does resolution in 
 * behalf of the owner.
 * */
void IfrNode::resolve(const string& aName)
{
    mOwner->resolveIfc(aName, binded());
    setValid(true);
}

MIfProv* IfrNode::next(MIfProv::TCp* aProvCp) const
{
    return nullptr;
}

MIfReq* IfrNode::prev()
{
    MIfReq* res = nullptr;
    auto prevCp = firstPair();
    res = prevCp ? prevCp->provided() : nullptr;
    return res;
}

MIfReq* IfrNode::tail()
{
    MIfReq* res = nullptr;
    MIfReq* pr = prev();
    while (pr) {
	res = pr;
	pr = pr->prev();
    }
    return res;
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

void IfrNode::MIfReq_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    Ifu::offset(aIdt, aOs);
    aOs  << "[" << name() << "], " << mOwner->Uid() << ", Valid: " << mValid << endl;
    auto self = const_cast<IfrNode*>(this);
    auto prev = self->binded()->provided()->prev();
    if (prev) {
	aIdt += 2;
	prev->doDump(aLevel, aIdt, aOs);
    }
}

void IfrNode::setValid(bool aValid)
{
    assert(mValid != aValid);
    mValid = aValid;
    if (!aValid) {
	// Disconnect owneds
	erase();
	// Notify provider owner
	if (mOwner) {
	    mOwner->onIfpInvalidated(this);
	}
	// Propagate to requestors
	auto owr = firstPair();
	MIfReq* owrr = owr ? owr->provided() : nullptr;
	if (owrr) {
	    owrr->onProvInvalidated();
	}
    }
}

bool IfrNode::detach(TPair* aPair)
{
    bool res = TBase::detach(aPair);
    mOwner->onIfpDisconnected(this);
    return res;
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

void IfrNode::erase()
{
    // IfrNode doesn't own sub-nodes, so no deletion here, just disconnect
    auto pair = binded()->firstPair();
    while (pair) {
	auto prov = pair->provided();
	binded()->disconnect(pair);
	pair = binded()->firstPair(); // pairs container in binded is updated, starts from the firs
    }
}

bool IfrNode::isRequestor(MIfProvOwner* aOwner, int aPos) const
{
    bool res = aOwner == mOwner;
    if (aPos-- && !res && mPair) {
	res = mPair->provided()->isRequestor(aOwner, aPos);
    }
    return res;
}


// Override firstLeaf, nextLeaf to resolve invalidated node
IfrNode::TSelf* IfrNode::firstLeafB()
{
    TSelf* res = nullptr;
    if (!mValid) {
	string nm = name();
	resolve(nm);
    }
    if (mValid) {
	res = TBase::firstLeafB();
    }
    return res;
}

IfrNode::TPair* IfrNode::nextLeaf(TPair* aLeaf)
{
    TPair* res = nullptr;
    if (!mValid) {
	string nm = name();
	resolve(nm);
    }
    if (mValid) {
	TBase::nextLeaf(aLeaf);
    }
    return res;
}

bool IfrNode::isResolved()
{
    return TBase::firstLeafB();
}

void IfrNode::onProvInvalidated()
{
    //mValid = false;
    // Notify prov owner
    if (mOwner) {
	mOwner->onIfpInvalidated(this);
    }
    // Propagate upward
    auto owr = firstPair();
    MIfReq* owrr = owr ? owr->provided() : nullptr;
    if (owrr) {
	owrr->onProvInvalidated();
    }
}


//// IfrLeaf

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
    if (!aValid) {
	// Propagate upward
	auto owr = firstPair();
	MIfReq* owrr = owr ? owr->provided() : nullptr;
	if (owrr) {
	    owrr->onProvInvalidated();
	}
    }
}


///// IfrNodeRoot

MIfProv::TIfaces* IfrNodeRoot::ifaces()
{
    TIfaces* res = nullptr;
    if (!mValid) {
	PFLC_DUR_STAT(START, PEvents::EDurStat_IFR_IFaces);
	string nm = name();
	auto self = const_cast<IfrNodeRoot*>(this);
	self->resolve(nm);
	if (mValid) {
	    res = &mIcache;
	}
	PFLC_DUR_STAT(REC, PEvents::EDurStat_IFR_IFaces);
    } else if (!mIcacheValid) {
	updateIcache();
	res = &mIcache;
    } else {
	res = &mIcache;
    }
    return res;
}

void IfrNodeRoot::setValid(bool aValid)
{
    IfrNode::setValid(aValid);
    if (aValid) {
	// Update iface cache
	updateIcache();
    } else {
	cleanIcache();
    }
}

void IfrNodeRoot::cleanIcache()
{
    mIcache.clear();
    mIcacheValid = false;
}

void IfrNodeRoot::updateIcache()
{
    mIcache.clear();
    MIfProv* maprov = first();
    while (maprov) {
	auto res = maprov->iface();
	mIcache.push_back(res);
	maprov = maprov->next();
    }
    mIcacheValid = true;
}

void IfrNodeRoot::onProvInvalidated()
{
    mIcacheValid = false;
    IfrNode::onProvInvalidated();
}
