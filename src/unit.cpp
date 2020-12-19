
#include <assert.h>

#include "unit.h"
#include "ifr.h"

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

MIface* Unit::MUnit_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    return res;
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
    //MCont2* cont = mContent.provided()->node()->getContent(aName);
    MCont2* cont = mContent.getContent(aName);
    if (cont) {
	res = cont->getData(aData);
    }
    return res;
}

bool Unit::setContent(const string& aData, const string& aName)
{
    bool res = false;
    MCont2* cont = nullptr;
    if (aName.empty() && Contu::isComplexContent(aData)) {
	cont = &mContent;
	res = true;
    } else {
	res = addContent(aName, !Contu::isComplexContent(aData));
	if (res) {
	    cont = mContent.getContent(aName);
	}
    }
    if (res && cont) {
	res = cont->setData(aData);
    }
    return res;
}

bool Unit::addContent(const string& aName, bool aLeaf)
{
    bool res = false;
    CUri uri(aName);
    MCont2* cont = &mContent;
    int i = 0;
    for (; i < uri.size(); i++) {
	string name = uri.at(i);
	MContNode2 *node = cont->node();
	MCont2* nextCont = node ? node->at(name) : nullptr;
	if (!nextCont) break;
	cont = nextCont;
    }
    if (i < uri.size()) {
	MContNode2* node = cont->node();
	if (node) {
	    CUri tail = uri.tail(i);
	    res = node->addCont(tail, aLeaf);
	}
    } else { // Already exists
	res = true;
    }
    return res;
}


bool Unit::resolveIface(const string& aName, TIfReqCp* aReq)
{
    bool res = false;
    // Check if the requestor was already registered
    bool connected = false;
    for (auto item : mIrns) {
	if (item->isConnected(aReq)) { connected = true; break;}
    }
    if (!connected) {
	IfrNode* node = createIfProv(aName, aReq);
	res = node->connect(aReq);
	if (res) {
	    res = node->resolve(aName);
	}
    }
    return res;
}

MIfProv* Unit::defaultIfProv(const string& aName)
{
    MIfProv* res = nullptr;
    if (mLocalIrn.count(aName) > 0) {
	res = mLocalIrn.at(aName);
    } else {
	IfrNode* node = createIfProv(aName, nullptr);
	mLocalIrn[aName] = node;
	res = node;
    }
    return res;
}

IfrNode* Unit::createIfProv(const string& aName, TIfReqCp* aReq) const
{
    IfrNode* res = nullptr;
    if (aReq) {
	res = new IfrNodeRoot(aName);
    }
    return res;
}

