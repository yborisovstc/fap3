
#include "dessp.h"


// DES Service terminal Pont, ref ds_ssp

DesSpt::DesSpt(const string &aType, const string& aName, MEnv* aEnv): Socket(aType, aName, aEnv)
{
}

void DesSpt::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifc = MNode_getLif(aName.c_str());
    if (ifc) { // Local iface
	addIfpLeaf(ifc, aReq);
    } else if (aName == MDesSpc::Type()) {
	// Redirect to owner
	MUnit* owu = Owner()->lIf(owu);
	MIfProvOwner* owo = owu ? owu->lIf(owo) : nullptr;
	if (owo) {
	    owu->resolveIface(aName, aReq);
	}
    } else {
	Socket::resolveIfc(aName, aReq);
    }
}



// DES Service Pont (extender), ref ds_ssp

DesSpe::DesSpe(const string &aType, const string& aName, MEnv* aEnv): Extd(aType, aName, aEnv)
{
}

void DesSpe::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifc = MNode_getLif(aName.c_str());
    if (ifc) { // Local iface
	addIfpLeaf(ifc, aReq);
    } else {
	// Get the initial requestor
	//auto* reqRoot = aReq->provided()->tail();
	auto* req = aReq->binded()->firstPair();
	const MIfProvOwner* reqo  = req ? req->provided()->rqOwner() : nullptr;
	if (reqo) {
	    // Separate request from Int atm
	    MNode* extn = getNode(KUriInt);
	    MUnit* extu = extn->lIf(extu);
	    MIfProvOwner* exto = extu->lIf(exto);
	    if (reqo == exto) {
		// Request from Int
		// Obtain MDesSpc from next requestor to get the client identification
		auto* req2 = req->binded()->firstPair();
		const MIfProvOwner* req2o  = req2 ? req2->provided()->rqOwner() : nullptr;
		const MUnit* req2uc = req2o->lIf(req2uc); 
		MUnit* req2u = const_cast<MUnit*>(req2uc);
		MDesSpc* spc = req2u->getSif(spc);
		if (spc) {
		    auto sid = spc->getId();
		    if (!sid.empty()) {
			// Iterate thru pairs and find the service subs with same ID
			MUnit* cltu = nullptr;
			for (int i = 0; i < pairsCount(); i++) {
			    auto* pair = getPair(i);
			    MUnit* pairu = pair->lIf(pairu); 
			    MDesSpc* pairspc = pairu->getSif(pairspc);
			    if (pairspc) {
				auto cid = pairspc->getId();
				if (sid == cid) {
				    Log(EDbg, TLog(this) + "Client found for service [" + cid + "]");
				    cltu = pairu;
				    break;
				}
			    } else {
				Log(EErr, TLog(this) + "Cannot obtain client MDesSpc interface");
			    }
			}
			if (cltu) {
			    // Client for the servicing subs is found. Redirect IFR to it. 
			    cltu->resolveIface(aName, aReq);
			}
		    }
		} else {
		    Log(EErr, TLog(this) + "Cannot obtain service  MDesSpc interface");
		}
	    } else {
		// Request from outside. Redirect to Int
		MVert* intcp = getExtd();
		MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
		if (intcpu) intcpu->resolveIface(aName, aReq);
	    }
	}
    }
}


// DES Service Pont (extended), ref ds_ssp

DesSp::DesSp(const string &aType, const string& aName, MEnv* aEnv): Socket(aType, aName, aEnv)
{
}

void DesSp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifc = MNode_getLif(aName.c_str());
    if (ifc) { // Local iface
	addIfpLeaf(ifc, aReq);
    } else {
	// Get the direct requestor
	auto* req = aReq->binded()->firstPair();
	const MIfProvOwner* reqo  = req ? req->provided()->rqOwner() : nullptr;
	if (reqo) {
	    const MUnit* requc = reqo->lIf(requc); 
	    const MUnit* ownu = Owner()->lIf(ownu);
	    if (requc == ownu) {
		// Direct requestor is the owner
		// Obtain MDesSpc from next requestor to get the client identification
		auto* req2 = req->binded()->firstPair();
		const MIfProvOwner* req2o  = req2 ? req2->provided()->rqOwner() : nullptr;
		const MUnit* req2uc = req2o->lIf(req2uc); 
		MUnit* req2u = const_cast<MUnit*>(req2uc);
		MDesSpc* spc = req2u->getSif(spc);
		if (spc) {
		    auto cid = spc->getId();
		    if (!cid.empty()) {
			// Iterate thru pairs and find the service subs with same ID
			MUnit* ssubsu = nullptr;
			for (int i = 0; i < pairsCount(); i++) {
			    auto* pair = getPair(i);
			    MUnit* pairu = pair->lIf(pairu); 
			    MDesSpc* pairspc = pairu->getSif(pairspc);
			    if (pairspc) {
				auto sid = pairspc->getId();
				if (sid == cid) {
				    Log(EDbg, TLog(this) + "Servicing subs found for client [" + cid + "]");
				    ssubsu = pairu;
				    break;
				}
			    } else {
				Log(EErr, TLog(this) + "Cannot obtain service MDesSpc interface");
			    }
			}
			if (ssubsu) {
			    // Servicing subs for the client found. Redirect IFR to it. 
			    ssubsu->resolveIface(aName, aReq);
			}
		    }
		} else {
		    Log(EErr, TLog(this) + "Cannot obtain client MDesSpc interface");
		}
	    } else {
		// Direct requestor is the pair. Redirect to owner.
		MUnit* ownu = Owner()->lIf(ownu);
		if (ownu) ownu->resolveIface(aName, aReq);
	    }
	}
    }
}


// DES service point client agent

ADesSpc::ADesSpc(const string &aType, const string& aName, MEnv* aEnv): Unit(aType, aName, aEnv), mAgtCp(this)
{
}

MIface* ADesSpc::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSpc>(aType));
    else if (res = checkLif<MAgent>(aType));
    else res = Unit::MNode_getLif(aType);
    return res;
}

MIface* ADesSpc::MAgent_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSpc>(aType));
    else if (res = checkLif<MUnit>(aType)); // To allow client to request IFR
    return res;
}

void ADesSpc::onOwnerAttached()
{
    bool res = false;
    // Registering in agent host
    MActr* ac = Owner()->lIf(ac);
    res = ac->attachAgent(&mAgtCp);
    if (!res) {
	Logger()->Write(EErr, this, "Cannot attach to host");
    }
}

string ADesSpc::getId() const
{
    auto pair = const_cast<ADesSpc*>(this)->mAgtCp.firstPair();
    MAhost* ahost = pair ? pair->provided() : nullptr;
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    return hostn ? hostn->name() : string();
}
