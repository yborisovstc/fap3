
#include <assert.h>

#include "unit.h"
#include "ifr.h"


Unit::Unit(const string &aName, MEnv* aEnv): mName(aName), mEnv(aEnv)
{
}

Unit::~Unit()
{
    deleteOwned();
    for (auto item : mIrns) {
	delete item;
    }
}

MIface* Unit::MUnit_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    if (res = checkLif<MIfProvOwner>(aType));
    return res;
}

MIface* Unit::MIfProvOwner_getLif(const char *aType)
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
	mIrns.push_back(node);
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
	mIrns.push_back(node);
	mLocalIrn[aName] = node;
	res = node;
    }
    return res;
}

IfrNode* Unit::createIfProv(const string& aName, TIfReqCp* aReq) const
{
    IfrNode* res = nullptr;
    if (aReq) {
	Unit* self = const_cast<Unit*>(this);
	res = new IfrNodeRoot(self, aName);
    }
    return res;
}

void Unit::invalidateIrm()
{
    for (auto node : mIrns) {
	node->setValid(false);
    }
    for (auto node : mLocalIrn) {
	node.second->setValid(false);
    }
}

void Unit::onIfpDisconnected(MIfProv* aProv)
{
    for (auto it = mIrns.begin(); it != mIrns.end(); it++) {
	if ((*it)->provided() == aProv) {
	    mIrns.erase(it); break;
	}
    }
    delete aProv;
}

MUnit* Unit::getComp(const string& aId)
{
    MOwned* ores = mCpOwner.at(aId);
    MUnit* res = ores ? ores->lIf(res) : nullptr;
    return res;
}

MUnit* Unit::getNode(const GUri& aUri)
{
    MUnit* res = getComp(aUri.at(0));
    for (int i = 1; i < aUri.size() && res; i++) {
	res = res->getComp(aUri.at(i));
    }
    return res;
}
