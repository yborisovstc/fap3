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
    return false;
}

MIfProv* IfrNode::next(MIfProv::TCp* aProvCp) const
{
    return nullptr;
}

void IfrNode::MIfProv_dump(int aIdt) const
{
    cout << string(aIdt, ' ') << "NODE. Name: " << name() << endl;
    auto self = const_cast<IfrNode*>(this);
    auto pair = self->binded()->firstPair();
    aIdt++;
    while (pair) {
	pair->provided()->dump(aIdt);
	pair = self->binded()->nextPair(pair);
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
    cout << string(aIdt,' ') << "LEAF. Name: " << name() << ", Iface: " << (mIface ? mIface->Uid() : "null") << endl;
}

