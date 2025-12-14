
#include <assert.h>

#include "unit.h"
#include "ifr.h"
#include "prof_ids.h"

// Enable debugging IFR tree quantity when invalidating
//#define DBG_INV_IFR_QNT
#define DBG_INV_IFR_QNT_LIM (200)

vector<GUri> Unit::getParentsUri()
{
    auto p = Node::getParentsUri();
    p.insert(p.begin(), string(idStr()));
    return p;
}

Unit::Unit(const string &aType, const string &aName, MEnv* aEnv): Node(aType, aName, aEnv)
{
}

Unit::~Unit()
{
    // We need quite complicated process of removing IFR nodes
    // The cause is that removing some node can initiate handling by node owner that can in turn
    // cause other iface resolution. Example: MDesInpObserver causes MDesObserver resolution
    // TODO: consider avoiding such ifaces deps
    vector<IfrNode::TPair*> owrs;
    for (auto item : mIrns) {
	auto* owr = *(item->pairsBegin());
        if (owr) {
            owrs.push_back(owr);
            // Regular node, set valid and disconnect from owned to keep make node as stub
            // This will prevent propagating ifaces resolution ahead the node
            item->binded()->disconnectAll();
            if (!item->isValid()) {
                item->setValid(true);
            }
        }
    }
    // Now invlidate the requestors to clean ifaces caches
    for (auto owr : owrs) {
	MIfReq* owrr = owr ? owr->provided() : nullptr;
	if (owrr) {
	    owrr->onProvInvalidated();
	}
    }
    // And finally we can remove all IFR nodes
    while (!mIrns.empty()) {
	auto item = mIrns.back();
        mIrns.pop_back();
        delete item;
    }
}

MIface* Unit::MNode_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMUnitPtr));
    else res = Node::MNode_getLif(aTid);
    return res;
}

MIface* Unit::MOwned_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMUnitPtr)); // Enable IRM from owner
    else res = Node::MOwned_getLif(aTid);
    return res;
}

MIface* Unit::MUnit_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMUnitPtr));
    else if (res = checkLif2(aTid, mMIfProvOwnerPtr));
    else res = MNode_getLif(aTid); //YB?? vulnerability
    return res;
}

/**
 * MNode - to support of owner-owned deps when IFR
 * */
MIface* Unit::MIfProvOwner_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMUnitPtr));
    else if (res = checkLif2(aTid, mMNodePtr));
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
	    Ifu::offset(aIdt, aOs); aOs << "Iface: " << item->ifId() << endl;
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

void Unit::resolveIface(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
    // Check if the requestor was already registered
    MIfProv* prov = nullptr;
    for (auto item : mIrns) {
	if (item->isConnected(aReq)) { prov = item; break;}
    }
    if (prov) {
	if (!prov->isValid()) {
	    prov->resolve(aTid);
	}
    } else {
	IfrNode* node = createIfProv(aTid, aReq);
	mIrns.push_back(node);
	res = node->connect(aReq);
	if (res) {
	    node->resolve(aTid);
	}
    }
}

MIfProv* Unit::defaultIfProv(TIdHash aTid)
{
    MIfProv* res = nullptr;
    if (mLocalIrn.count(aTid) > 0) {
	res = mLocalIrn.at(aTid);
    } else {
	IfrNode* node = createIfProv(aTid, nullptr);
	mIrns.push_back(node);
	mLocalIrn[aTid] = node;
	res = node;
    }
    return res;
}

IfrNode* Unit::createIfProv(TIdHash aTid, MIfReq::TIfReqCp* aReq) const
{
    IfrNode* res = nullptr;
    Unit* self = const_cast<Unit*>(this);
    if (!aReq) {  // No requestor, create root node
	res = new IfrNodeRoot(self, aTid);
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
        // Assuming that ifaces are independent! All deps need to be handled in heir
	for (auto node : mIrns) {
	    if (node->isValid()) {
		node->setValid(false);
	    }
	}
	PFL_DUR_STAT_REC(PEvents::EDurStat_UInvldIrm);
    }
}

void Unit::invalidateIrm(TIdHash aIfid)
{
    PFL_DUR_STAT_START(PEvents::EDurStat_UInvldIrm);
    for (auto node : mIrns) {
	if (node->isValid() && node->ifId() == aIfid) {
	    node->setValid(false);
	}
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_UInvldIrm);
}

void Unit::onIfpDisconnected(MIfProv* aProv)
{
    bool found = false;
    for (auto it = mIrns.begin(); it != mIrns.end(); it++) {
	if ((*it)->provided() == aProv) {
	    auto cmp = *it;
	    mIrns.erase(it);
            found = true;
	    delete cmp;
	    break;
	}
    }
    assert(found);
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

void Unit::resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aTid);
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    }
}

MIface* Unit::MOwner_getLif(TIdHash aTid)
{
    MIface* res = nullptr;
    if (res = checkLif2(aTid, mMUnitPtr)); // To enable ifr request to owner
    else res = Node::MOwner_getLif(aTid);
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
