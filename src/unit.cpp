
#include <assert.h>

#include "unit.h"
#include "ifr.h"
#include "prof_ids.h"

// Enable debugging IFR tree quantity when invalidating
//#define DBG_INV_IFR_QNT
#define DBG_INV_IFR_QNT_LIM (200)

Unit::Unit(const string &aType, const string &aName, MEnv* aEnv): Node(aType, aName, aEnv)
{
}

Unit::~Unit()
{
    while (!mIrns.empty()) {
	auto item = mIrns.back();
	mIrns.pop_back();
	item-> disconnectAll();
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
    else res = MNode_getLif(aType); //YB?? vulnerability
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
	/*
	Ifu::offset(aIdt, aOs); aOs << "== Root IRNs ==: " << endl;
	for (auto itr : mLocalIrn) {
	    Ifu::offset(aIdt, aOs); aOs << "Iface: " << itr.first << endl;
	    itr.second->MIfProv_doDump(aLevel, aIdt + 1, aOs);
	}
	*/
	Ifu::offset(aIdt, aOs); aOs << "== IRNs: ==" << endl;
	for (auto item : mIrns) {
	    Ifu::offset(aIdt, aOs); aOs << "Iface: " << item->name() << endl;
	    item->MIfProv_doDump(aLevel, aIdt + 1, aOs);
	    //auto req = item->firstPair() ? item->firstPair()->provided() : nullptr;
	    auto* fpair = *item->pairsBegin();
	    auto req = fpair ? fpair->provided() : nullptr;
	    if (req) {
		Ifu::offset(aIdt, aOs); aOs << "Req chain: " << endl;
		req->doDump(aLevel, aIdt + 1, aOs);
	    }
	}
    }
}

void Unit::resolveIface(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
    // Check if the requestor was already registered
    MIfProv* prov = nullptr;
    for (auto item : mIrns) {
	if (item->isConnected(aReq)) { prov = item; break;}
    }
    if (prov) {
	if (!prov->isValid()) {
	    prov->resolve(aName);
	}
    } else {
	IfrNode* node = createIfProv(aName, aReq);
	mIrns.push_back(node);
	res = node->connect(aReq);
	if (res) {
	    node->resolve(aName);
	}
    }
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
    if (!mIrns.empty()) {
	PFL_DUR_STAT_START(PEvents::EDurStat_UInvldIrm);
	// Debugging. Use pcount to set breakpoint with the given limit.
#ifdef DBG_INV_IFR_QNT
	int pcount = 0;
	for (auto node : mIrns) {
	    pcount += node->pcount(true);
	}
	if (pcount > DBG_INV_IFR_QNT_LIM) {
	    LOGN(EErr, "Large IFR tree invalidated: " + to_string(pcount));
	}
#endif
	// We need to invalidate all first and then to update.
	// This is because there can be deps one iface from another
	// example is dep of some iface on MAgent
	for (auto node : mIrns) {
	    if (node->isValid()) {
		node->setValid(false);
	    }
	}
	// Keep ifaces actual, ref ds_desopt_uic
	for (auto node : mIrns) {
	    if (!node->isValid()) {
		node->ifaces();
	    }
	}
	PFL_DUR_STAT_REC(PEvents::EDurStat_UInvldIrm);
    }
}

void Unit::invalidateIrm(const string& aIfcName)
{
    PFL_DUR_STAT_START(PEvents::EDurStat_UInvldIrm);
    for (auto node : mIrns) {
	if (node->isValid() && node->name() == aIfcName) {
	    node->setValid(false);
	    node->ifaces();
	}
    }
    /*
    for (auto node : mIrns) {
	if (node->name() == aIfcName && !node->isValid()) {
	    node->ifaces();
	}
    }
    */
    PFL_DUR_STAT_REC(PEvents::EDurStat_UInvldIrm);
}

void Unit::onIfpDisconnected(MIfProv* aProv)
{
    for (auto it = mIrns.begin(); it != mIrns.end(); it++) {
	if ((*it)->provided() == aProv) {
	    auto cmp = *it;
	    mIrns.erase(it);
	    delete cmp;
	    break;
	}
    }
    //delete aProv;
}

void Unit::onIfpInvalidated(MIfProv* aProv)
{
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

void Unit::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aName.c_str());
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    }
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
    // TODO !! Attempt of optimization. To check all use-cases
    //!! invalidateIrm();
}

bool Unit::isRequestor(MIfReq::TIfReqCp* aReq, MNode* aOwner) const
{
    MUnit* ou = aOwner ? aOwner->lIf(ou) : nullptr;
    MIfProvOwner* oo = ou ? ou->lIf(oo) : nullptr;
    return aReq->provided()->isRequestor(oo);
}
