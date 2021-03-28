
#include <assert.h>

#include "unit.h"
#include "ifr.h"


Unit::Unit(const string &aName, MEnv* aEnv): Node(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

Unit::~Unit()
{
    deleteOwned();
    for (auto item : mIrns) {
	delete item;
    }
}

MIface* Unit::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    else res = checkLif<MNode>(aType);
    return res;
}

MIface* Unit::MUnit_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    else if (res = checkLif<MIfProvOwner>(aType));
    else res = MNode_getLif(aType); //YB??
    return res;
}

MIface* Unit::MIfProvOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    return res;
}

bool Unit::resolveIface(const string& aName, MIfReq::TIfReqCp* aReq)
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

IfrNode* Unit::createIfProv(const string& aName, MIfReq::TIfReqCp* aReq) const
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


void Unit::addIfpLeaf(MIface* aIfc, MIfReq::TIfReqCp* aReq)
{
	if (aIfc) {
	IfrLeaf* lf = new IfrLeaf(this, aIfc);
	aReq->connect(lf);
    }
}
