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
    if (!mValid) {
	string nm = name();
	IfrNode* self = const_cast<IfrNode*>(this);
	bool pres = self->resolve(nm);
	if (pres && mValid) {
	    res =  self->mCnode.firstLeaf()->provided();
	}
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

bool IfrNode::resolve(const string& aName)
{
    // Cleanup the node - remove all invalid pairs
    auto up = binded()->firstPair();
    while (up) {
	if (!up->provided()->isValid()) {
	    binded()->disconnect(up);
	} else {
	    up = binded()->nextPair(up);
	}
    }
    return false;
}

MIfProv* IfrNode::next(MIfProv::TCp* aProvCp) const
{
    return nullptr;
}

void IfrNode::MIfProv_dump(int aIdt) const
{
    cout << string(aIdt, ' ') << "NODE [" << name() << "], Owner: " << mOwner->Uid() << ", Valid: " << mValid << endl;
    auto self = const_cast<IfrNode*>(this);
    auto pair = self->binded()->firstPair();
    aIdt++;
    while (pair) {
	pair->provided()->dump(aIdt);
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
	}
	pair = binded()->nextPair(pair);
    }

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

void IfrLeaf::MIfProv_dump(int aIdt) const
{
    cout << string(aIdt,' ') << "LEAF [" << name() << "], Owner: " << mOwner->Uid() << ", Valid: " << mValid << ", Iface: " << (mIface ? mIface->Uid() : "null") << endl;
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


