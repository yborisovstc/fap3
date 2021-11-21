
#include <assert.h>

#include "unit.h"
#include "ifr.h"


Unit::Unit(const string &aName, MEnv* aEnv): Node(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

Unit::~Unit()
{
    for (auto item : mIrns) {
	item->disconnectAll();
	delete item;
    }
}

MIface* Unit::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    else res = Node::MNode_getLif(aType);
    return res;
}

MIface* Unit::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType)); // Enagle IRM from owner
    else res = Node::MOwned_getLif(aType);
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

/**
 * MNode - to support of owner-owned deps when IFR
 * */
MIface* Unit::MIfProvOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    else if (res = checkLif<MNode>(aType));
    return res;
}


void Unit::MUnit_doDump(int aLevel, int aIdt, std::ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, aOs); aOs << "UID: " << MUnit_Uid() << endl;
	Ifu::offset(aIdt, aOs); aOs << "Root IRNs: " << endl;
	for (auto itr : mLocalIrn) {
	    Ifu::offset(aIdt, aOs); aOs << "Iface: " << itr.first << endl;
	    itr.second->MIfProv_doDump(aLevel, aIdt + 1, aOs);
	}
    }
}

bool Unit::resolveIface(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
    // Check if the requestor was already registered
    MIfProv* prov = nullptr;
    for (auto item : mIrns) {
	if (item->isConnected(aReq)) { prov = item; break;}
    }
    if (prov) {
	prov->resolve(aName);
    } else {
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
    Unit* self = const_cast<Unit*>(this);
    if (!aReq) {  // No requestor, create root node
	res = new IfrNodeRoot(self, aName);
    } else {  // There is a requestor, created regular node
	res = new IfrNode(self); 
    }
    return res;
}

void Unit::invalidateIrm()
{
    for (auto node : mIrns) {
	if (node->isValid()) {
	    node->setValid(false);
	}
    }
    for (auto node : mLocalIrn) {
	if (node.second->isValid()) {
	    node.second->setValid(false);
	}
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

void Unit::addIfpLeafs(MIfProv::TIfaces* aIfcs, MIfReq::TIfReqCp* aReq)
{
    for (auto ifc : *aIfcs) {
	addIfpLeaf(ifc, aReq);
    }
}

bool Unit::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
    MIface* ifr = MNode_getLif(aName.c_str());
    if (ifr /* && !findIface(ifr)YB??*/) {
	IfrLeaf* lf = new IfrLeaf(this, ifr);
	aReq->connect(lf);
	res = true;
    }
    return res;
}

MIface* Unit::MOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType)); // To enable ifr request to owner
    else res = Node::MOwner_getLif(aType);
    return res;
}

void Unit::onOwnedAttached(MOwned* aOwned)
{
    Node::onOwnedAttached(aOwned);
    // Invalidate IRM
    invalidateIrm();
}

bool Unit::isRequestor(MIfReq::TIfReqCp* aReq, MNode* aOwner) const
{
    MUnit* ou = aOwner ? aOwner->lIf(ou) : nullptr;
    MIfProvOwner* oo = ou ? ou->lIf(oo) : nullptr;
    return aReq->provided()->isRequestor(oo);
}


