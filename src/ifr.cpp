#include "ifr.h"

string IfrNode::name() const
{
    if (mPair) {
	return mPair->provided()->owner()->name();
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
	    res =  mCnode.first()->provided();
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
    auto next =  mCnode.next(aProvCp)->provided();
    return next;
}





string IfrLeaf::name() const
{
    if (mPair) {
	return mPair->provided()->owner()->name();
    } else {
	return string();
    }
}


