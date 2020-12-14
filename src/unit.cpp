
#include <assert.h>

#include "unit.h"

#if 0
bool Unit::NCpOwner::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this));
    assert(mPairs.count(aPair->ownedId()) == 0);
    bool res = aPair->connect(this);
    if (res) {
	mPairs.insert(TPairsElem(aPair->ownedId(), aPair));
    }
    return res;
}

bool Unit::NCpOwner::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this));
    assert(mPairs.count(aPair->ownedId()) == 1);
    bool res = aPair->disconnect(this);
    if (res) {
	mPairs.erase(aPair->ownedId());
    }
    return res;
}

bool Unit::NCpOwner::isConnected(TPair* aPair) const
{
    return mPairs.count(aPair->ownedId()) == 1 && mPairs.at(aPair->ownedId()) == aPair;
}

bool Unit::NCpOwned::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this));
    assert(!mPair);
    bool res = true;
    mPair = aPair;
    return res;
}

bool Unit::NCpOwned::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this));
    bool res = true;
    mPair = nullptr;
    return res;
}

void Unit::NCpOwned::deleteOwned()
{
    delete mHost;
}

#endif

Unit::Unit(const string &aName, MEnv* aEnv): mName(aName), mEnv(aEnv)
{
}

Unit::~Unit()
{
    deleteOwned();
}

void Unit::deleteOwned()
{
    for (auto item : mCpOwner.mPairs) {
	item.second->deleteOwned();
    }
}

bool Unit::getContent(string& aData, const string& aName) const
{
    bool res = false;
    MCont2* cont = mContent.ownerCp().provided()->getContent(aName);
    if (cont) {
	res = cont->getData(aData);
    }
    return res;
}

bool Unit::setContent(const string& aData, const string& aName)
{
    bool res = false;
    MCont2* cont = mContent.ownerCp().provided()->getContent(aName);
    if (cont) {
	res = cont->setData(aData);
    }
    return res;
}

bool Unit::addContent(const string& aName, bool aLeaf)
{
    bool res = false;
    return res;
}


