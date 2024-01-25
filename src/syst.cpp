#include "syst.h"
#include "mlink.h"


const string KContProvided = "Provided";
const string KContRequired = "Required";

string ConnPointu::KReqName = "Required";
string ConnPointu::KProvName = "Provided";


ConnPointu::ConnPointu(const string &aType, const string &aName, MEnv* aEnv): Vertu(aType, aName, aEnv)
{
}

MIface* ConnPointu::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else if (res = checkLif<MIfProvOwner>(aType));
    else res = Vertu::MNode_getLif(aType);
    return res;
}

MIface* ConnPointu::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MVert_getLif(aType);
    return res;
}


MIface* ConnPointu::MIfProvOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MIfProvOwner_getLif(aType);
    return res;
}

string ConnPointu::provName() const
{
    string res;
    bool pres = getContent(KContProvided, res);
    return res;
}

string ConnPointu::reqName() const
{
    string res;
    bool pres = getContent(KContRequired, res);
    return res;
}

MContent* ConnPointu::getCont(int aIdx)
{
    MContent* res = nullptr;
    if (aIdx == 0) res = &mProv;
    else if (aIdx == 1) res = &mReq;
    return res;
}

const MContent* ConnPointu::getCont(int aIdx) const
{
    const MContent* res = nullptr;
    if (aIdx == 0) res = &mProv;
    else if (aIdx == 1) res = &mReq;
    return res;
}

bool ConnPointu::getContent(const GUri& aCuri, string& aRes) const
{
    bool res = true;
    string name = aCuri;
    if (name == KProvName)
	res = mProv.getData(aRes);
    else if (name == KReqName)
	res = mReq.getData(aRes);
    else res = Vertu::getContent(aCuri, aRes);
    return res;
}

bool ConnPointu::setContent(const GUri& aCuri, const string& aData)
{
    bool res = true;
    string name = aCuri;
    if (name == KProvName)
	res = mProv.setData(aData);
    else if (name == KReqName)
	res = mReq.setData(aData);
    else res = Vertu::setContent(aCuri, aData);
    return res;
}

bool ConnPointu::isCompatible(MVert* aPair, bool aExt)
{
    bool res = false;
    bool ext = aExt;
    MVert* cp = aPair;
    // Checking if the pair is Extender
    if (aPair != this) {
	MVert* ecp = cp->getExtd(); 
	if (ecp) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp) {
	    // Check roles conformance
	    string prov = provName();
	    string req = reqName();
	    MConnPoint* mcp = cp->lIf(mcp);
	    if (mcp) {
		string pprov = mcp->provName();
		string preq = mcp->reqName();
		if (ext) {
		    res = prov == pprov && req == preq;
		} else {
		    res = prov == preq && req == pprov;
		}
	    }
	}
    } else {
	res = aExt;
    }
    return res;
}

void ConnPointu::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aName.c_str()); // Local
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    }
    if (!ifr) { // TODO this disables both local and remote ifaces. To fix
	if (aName == provName()) {
	    // Requested provided iface - cannot be obtain via pairs - redirect to owner
	    auto owner = Owner();
	    MUnit* ownu = owner ? const_cast<MOwner*>(owner)->lIf(ownu): nullptr;
	    if (ownu) {
		MIfProvOwner* ownupo = ownu ? ownu->lIf(ownupo) : nullptr;
		// Disable routing to direct requestor only. Redirecting to non-direct
		// requestor is enabled to support loop-back IRM path
		bool isReq = aReq->provided()->isRequestor(ownupo, 1);
		if (!isReq) {
		    ownu->resolveIface(aName, aReq);
		}
	    }
	} else if (aName == reqName()) {
	    // Requested required iface - redirect to pairs
	    for (MVert* pair : mPairs) {
		MUnit* pairu = pair->lIf(pairu);
		pairu->resolveIface(aName, aReq);
	    }
	}
    }
}



// Extender agent, monolitic, multicontent, unit


const string Extd::KUriInt = "Int";  /*!< Internal connpoint */
const string KContDir = "Direction";
const string KContDir_Val_Regular = "Regular";
const string KContDir_Val_Inp = "Inp";
const string KContDir_Val_Out = "Out";


// Extender, monolitic, multicontent, unit. Redirects request for iface to internal CP of extention.


Extd::Extd(const string &aType, const string& aName, MEnv* aEnv): Vertu(aType, aName, aEnv)
{
    setContent(KContDir, KContDir_Val_Regular);
}

void Extd::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifc = MNode_getLif(aName.c_str());
    if (ifc) { // Local iface
	addIfpLeaf(ifc, aReq);
    } else {
	// Redirect to internal point or pair depending on the requiestor
	MVert* intcp = getExtd();
	MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
	MIfProvOwner* intcpo = intcpu ? intcpu->lIf(intcpo) : nullptr;
	if (intcpo && aReq->provided()->isRequestor(intcpo)) {
	    // Redirect to pairs
	    for (int i = 0; i < pairsCount(); i++) {
		MVert* pair = getPair(i);
		MUnit* pairu = pair ? pair->lIf(pairu) : nullptr;  
		MIfProvOwner* pairo = pairu ? pairu->lIf(pairo) : nullptr;
		if (pairo && !aReq->provided()->isRequestor(pairo)) {
		    pairu->resolveIface(aName, aReq);
		}
	    }
	} else {
	    // Redirect to internal CP
	    if (intcpu) intcpu->resolveIface(aName, aReq);
	}
    }
}

bool Extd::isCompatible(MVert* aPair, bool aExt)
{
    bool res = false;
    MVert* intcp = getExtd();
    if (intcp) {
	res = intcp->isCompatible(aPair, !aExt);
    }
    return res;
}

MVert* Extd::getExtd()
{
    MVert* res = nullptr;
    MNode* extn = getNode(KUriInt);
    res = extn ? extn->lIf(res) : nullptr;
    return res;
}

MVert::TDir Extd::getDir() const
{
    TDir res = ERegular;
    string cdir;
    getContent(KContDir, cdir);
    if (cdir == KContDir_Val_Inp) res = EInp;
    else if (cdir == KContDir_Val_Out) res = EOut;
    return res;
}


// Extender, chromoable, monolitic, multicontent, unit. Redirects request for iface to internal CP of extention.

const string Extde::KUriInt = "Int";  /*!< Internal connpoint */

Extde::Extde(const string &aType, const string& aName, MEnv* aEnv): Vert(aType, aName, aEnv)
{
    setContent(KContDir, KContDir_Val_Regular);
}

void Extde::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifc = MNode_getLif(aName.c_str());
    if (ifc) { // Local iface
	addIfpLeaf(ifc, aReq);
    } else {
	// Redirect to internal point or pair depending on the requiestor
	MVert* intcp = getExtd();
	MUnit* intcpu = intcp ? intcp->lIf(intcpu) : nullptr;
	MIfProvOwner* intcpo = intcpu ? intcpu->lIf(intcpo) : nullptr;
	if (intcpo && aReq->provided()->isRequestor(intcpo)) {
	    // Redirect to pairs
	    for (int i = 0; i < pairsCount(); i++) {
		MVert* pair = getPair(i);
		MUnit* pairu = pair ? pair->lIf(pairu) : nullptr;  
		MIfProvOwner* pairo = pairu ? pairu->lIf(pairo) : nullptr;
		if (pairo && !aReq->provided()->isRequestor(pairo)) {
		    pairu->resolveIface(aName, aReq);
		}
	    }
	} else {
	    // Redirect to internal CP
	    if (intcpu) intcpu->resolveIface(aName, aReq);
	}
    }
}

bool Extde::isCompatible(MVert* aPair, bool aExt)
{
    bool res = false;
    MVert* intcp = getExtd();
    if (intcp) {
	res = intcp->isCompatible(aPair, !aExt);
    }
    return res;
}

MVert* Extde::getExtd()
{
    MVert* res = nullptr;
    MNode* extn = getNode(KUriInt);
    res = extn ? extn->lIf(res) : nullptr;
    return res;
}

MVert::TDir Extde::getDir() const
{
    TDir res = ERegular;
    string cdir;
    getContent(KContDir, cdir);
    if (cdir == KContDir_Val_Inp) res = EInp;
    else if (cdir == KContDir_Val_Out) res = EOut;
    return res;
}



//// Socket

Socket::Socket(const string &aType, const string& aName, MEnv* aEnv): Vert(aType, aName, aEnv)
{
//    if (aName.empty()) mName = Type();
    setContent(KContDir, KContDir_Val_Regular);
}

MIface* Socket::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MSocket>(aType));
    else res = Vert::MNode_getLif(aType);
    return res;
}

void Socket::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifc = MNode_getLif(aName.c_str());
    if (ifc) { // Local iface
	addIfpLeaf(ifc, aReq);
    } else {
	MIfReq::TIfReqCp* req = aReq->binded()->firstPair();
	if (req) {
	    // There is the context
	    const MIfProvOwner* reqo = req ? req->provided()->rqOwner() : nullptr;
	    MNode* reqn = const_cast<MNode*>(reqo ? reqo->lIf(reqn) : nullptr); // Current requestor as node
	    if (isNodeOwned(reqn)) {
		// Request from internal CP.
		if (!mPairs.empty()) {
		    // There are pairs. Redirect to piars. 
		    for (auto it = mPairs.begin(); it != mPairs.end(); it++) {
			MVert* pair = *it;
			MUnit* pairu = pair->lIf(pairu);
			if (pairu) {
			    pairu->resolveIface(aName, aReq);
			}
		    }
		} else {
		    // No pairs. Redirect to owner.
		    MUnit* owu = Owner()->lIf(owu);
		    if (owu) {
			owu->resolveIface(aName, aReq);
		    }
		}
	    } else {
		// Request from outside
		// First check the pairs, ref FAP2 DSI_SRST
		for (MVert* pair : mPairs) {
		    MUnit* pairu = pair->lIf(pairu);
		    MIfProvOwner* pairo = pairu ? pairu->lIf(pairo) : nullptr;
		    if (pairo) {
			bool isReq = aReq->provided()->isRequestor(pairo);
			bool isFirstReq = req ? (pairo == req->provided()->rqOwner()) : false;
			if (!isReq || isReq && !isFirstReq) {
			    pairu->resolveIface(aName, aReq);
			}
		    }
		}
		if (!aReq->provided()->isResolved()) {
		    // Continue downwards if iface not provided by pairs, ref DSI_SRST_S1_EDC
		    MNode* apair = nullptr; // Assosiated pair in requestors
		    MNode* pcomp = nullptr;
		    bool isextd = false;
		    while(reqn && !pcomp) {
			MVert* cp = reqn->lIf(cp); // Current requestor as connpoint
			// Update extention option if met extention in context.
			if (cp) {
			    apair = nullptr;
			    if (cp->isCompatible(this, isextd)) {
				isextd ^= true;
				MVert* extd = cp->getExtd();
				if (extd != this) {
				    MNode* extdn = extd ? extd->lIf(extdn) : nullptr;
				    apair = reqn;
				    if (extdn) {
					// DS_ISS_012 Set assosiatad pair to extender only in case if extender is next requestor
					auto* req_tmp = req->binded()->firstPair();
					auto* req_tmpo = req_tmp ? req_tmp->provided()->rqOwner() : nullptr;
					MNode* req_tmpn = const_cast<MNode*>(req_tmpo ? req_tmpo->lIf(req_tmpn) : nullptr);
					if (extdn == req_tmpn) {
					    apair = extdn;
					    req = req_tmp;
					    isextd ^= true;
					}
				    }
				}
			    }
			}
			if (apair) {
			    // Find associated pairs pin within the context
			    MSocket* apairs = apair->lIf(apairs);
			    if (apairs != NULL) {
				MNode* pereq = apairs->GetPin(req);
				if (pereq) {
				    pcomp = getNode(pereq->name());
				}
			    }
			}
			req = req->binded()->firstPair();
			reqo = req ? req->provided()->rqOwner() : nullptr;
			reqn = const_cast<MNode*>(reqo ? reqo->lIf(reqn) : nullptr);
		    }
		    if (pcomp) {
			MUnit* pcompu = pcomp->lIf(pcompu);
			if (pcompu) {
			    pcompu->resolveIface(aName, aReq);
			}
		    }
		}
		// Using priority logic: if resolved then no redirection to owner
		if (!aReq->provided()->isResolved()) {
		    // Redirect to upper layer socket
		    MUnit* owu = Owner()->lIf(owu);
		    MIfProvOwner* owo = owu ? owu->lIf(owo) : nullptr;
		    if (owo) {
			// Disable routing to direct requestor only. Redirecting to non-direct
			// requestor is enabled to support loop-back IRM path. !! Att, isn't tested deep
			bool isReq = aReq->provided()->isRequestor(owo,1);
			if (!isReq) {
			    owu->resolveIface(aName, aReq);
			}
		    }
		}
	    }
	} else {
	    // There is no the context - the request is "anonymous"
	    // Redirect to pins first.
	    auto compcp = owner()->firstPair();
	    while (compcp) {
		MOwned* compo = compcp ? compcp->provided() : nullptr;
		MUnit* compu = compo->lIf(compu);
		if (compu) {
		    compu->resolveIface(aName, aReq);
		}
		compcp = owner()->nextPair(compcp);
	    }
#if 0 // ds_iss_010. Disabled redirection to pair to avoid resolving iface not provided by pins
	    // Using priority logic: if pins resolve then stop
	    if (!aReq->provided()->isResolved()) {
		// Then redirect to pair
		for (MVert* pair : mPairs) {
		    MUnit* pairu = pair->lIf(pairu);
		    if (pairu) {
			pairu->resolveIface(aName, aReq);
		    }
		}
	    }
#endif
	}
    }
}

bool Socket::isCompatible(MVert* aPair, bool aExt)
{
    bool res = true;
    // Going thru connectable components and check their compatibility
    bool ext = aExt;
    MVert *cp = aPair;
    MVert* ecp = aPair->getExtd(); 
    // Checking if the pair is Extender
    if (ecp) {
	ext = !ext;
	cp = ecp;
    }
    if (cp) {
	// TODO MNode shall not be resolved thru MVert. This is MVert resolution issue
	MNode* cpn = cp->lIf(cpn);
	for (int ci = 0; ci < owner()->pcount() && res; ci++) {
	    auto comp = owner()->pairAt(ci)->provided();
	    MNode* compn = comp->lIf(compn);
	    MVert* compv = compn ? compn->lIf(compv) : nullptr;
	    if (compv) {
		MNode* compn = comp->lIf(compn);
		MNode* pcomp = cpn->getNode(compn->name());
		if (pcomp) {
		    MVert* pcompv = pcomp->lIf(pcompv);
		    res = compv->isCompatible(pcompv, ext);
		} else {
		    res = false;
		}
	    }
	}
    }
    return res;
}

MVert* Socket::getExtd()
{
    MVert* res = nullptr;
    return res;
}

MVert::TDir Socket::getDir() const
{
    TDir res = ERegular;
    string cdir;
    getContent(KContDir, cdir);
    if (cdir == KContDir_Val_Inp) res = EInp;
    else if (cdir == KContDir_Val_Out) res = EOut;
    return res;
}

int Socket::PinsCount() const
{
    int res = 0;
    for (int i = 0; i < owner()->pcount(); i++) {
	const MOwned* comp = owner()->pairAt(i)->provided();
	const MVert* compv = comp->lIf(compv);
	if (compv) res++;
    }
    return res;
}

MNode* Socket::GetPin(int aInd)
{
    MNode* res = nullptr;
    return res;
}

MNode* Socket::GetPin(MIfReq::TIfReqCp* aReq)
{
    MNode* res = nullptr;
    MIfReq::TIfReqCp* req = aReq->binded()->firstPair();
    const MIfProvOwner* reqo = req ? req->provided()->rqOwner() : nullptr;
    const MNode* reqn = reqo ? reqo->lIf(reqn) : nullptr; // Current requestor as node
    if (isNodeOwned(reqn)) {
	res = const_cast<MNode*>(reqn);
    }
    return res;
}



// System


Syst::Syst(const string &aType, const string &aName, MEnv* aEnv): Elem(aType, aName, aEnv), mAgtCp(this)
{
}

Syst::~Syst()
{
}

MIface* Syst::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MSyst>(aType));
    else res = Elem::MNode_getLif(aType);
    return res;
}

/** TODO Vulnerability: access to many ifaces. This is required for embedded agents.
 *  Maybe it makes sense to have owner API getting owned as arg, so owner can analyze
 *  if the owned if agent. We also can have system propery with the policy of getting
 *  access to ifaces
 * */
MIface* Syst::MOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    else if (res = checkLif<MContentOwner>(aType));
    else if (res = checkLif<MActr>(aType));
    else res = Unit::MOwner_getLif(aType);
    return res;
}

void Syst::mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    bool res = false;
    string sp = aMut.Attr(ENa_P);
    string sq = aMut.Attr(ENa_Q);
    MNode* pn = getNode(sp, aCtx.mNs);
    MNode* qn = nullptr;
    if (sq.empty()) {
	if (aMut.Count() == 1) {
	    // Q-dependency
	    ChromoNode qdep = *aMut.Begin();
	    mutate(qdep, aUpdOnly, aCtx);
	    if (qdep.Type() == ENt_Node) {
		sq = qdep.Attr(ENa_Id);
	    } else if (qdep.Type() == ENt_Seg) {
		if (qdep.AttrExists(ENa_Targ)) {
		    sq = qdep.Attr(ENa_Targ);
		} else if (qdep.AttrExists(ENa_NS)) {
		    sq = qdep.Attr(ENa_NS);
		}
	    }
	    qn = getNode(sq, aCtx.mNs);
	}
    } else {
	qn = getNode(sq, aCtx.mNs);
    }
    if (pn && qn) {
	MVert* pv = pn->lIf(pv);
	MVert* qv = qn->lIf(pv);
	if (pv && qv) {
	    // Vertex to Vertex
	    if (!pv->isPair(qv) && !qv->isPair(pv)) {
		res = MVert::connect(pv, qv);
		if (!res) {
		    LOGN(EErr, "Failed connecting [" + sp + "] to [" + sq + "]");
		    res = MVert::connect(pv, qv);
		} else {
		    mEdges.push_back(TEdge(pv, qv));
		}
	    } else {
		LOGN(EErr, "Connecting [" + sp + "] to [" + sq + "] - already connected");
	    }
	} else {
	    MLink* pl = pn->lIf(pl);
	    if (pl) {
		// Link, one-way
		res = pl->connect(qn);	
		if (!res) {
		    LOGN(EErr, "Failed link [" + sq + "] to [" + sp + "]");
		}
	    } else {
		LOGN(EErr, "Connecting [" + sp + "] to [" + sq + "] -  [" + (pv ? sq : sp) + "] isn't connectable");
	    }
	}
    } else {
	LOGN(EErr, "Connecting [" + sp + "] to [" + sq + "] - cannot find [" + (pn ? sq : sp) + "]");
    }
    if (!aUpdOnly) {
	//notifyNodeMutated(aMut, aCtx);
    }
}

void Syst::mutDisconnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    bool res = false;
    string sp = aMut.Attr(ENa_P);
    string sq = aMut.Attr(ENa_Q);
    MNode* pn = getNode(sp, aCtx.mNs);
    MNode* qn = getNode(sq, aCtx.mNs);
    if (pn && qn) {
	MVert* pv = pn->lIf(pv);
	MVert* qv = qn->lIf(pv);
	if (pv && qv) {
	    if (pv->isConnected(qv)) {
		// Vertex to Vertex
		res = MVert::disconnect(pv, qv);
		if (!res) {
		    LOGN(EErr, "Failed disconnecting [" + sp + "] from [" + sq + "]");
		}
	    } else {
		LOGN(EWarn, "Disconnecting [" + sp + "] and [" + sq + "]: aren't connected");
	    }
	} else {
	    MLink* pl = pn->lIf(pl);
	    if (pl) {
		// Link, one-way
		res = pl->disconnect(qn);	
		if (!res) {
		    LOGN(EErr, "Failed disconnecting [" + sq + "] to [" + sp + "]");
		}
	    } else {
		LOGN(EErr, "Disconnecting [" + sp + "] from [" + sq + "] -  [" + (pv ? sq : sp) + "] isn't connectable");
	    }
	}
    } else {
	LOGN(EErr, "Disconnecting [" + sp + "] from [" + sq + "] - cannot find [" + (pn ? sq : sp) + "]");
    }
    if (!aUpdOnly) {
	//notifyNodeMutated(aMut, aCtx);
    }
}

void Syst::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aName.c_str());
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    } else {
	// Stop resolving if local iface matches
	if (aName == MAgent::Type()) {
	    for (int i = 0; i < owner()->pcount(); i++) {
		MOwned* comp = owner()->pairAt(i)->provided();
		MNode* compn = comp->lIf(compn);
		MAgent* compa = compn ? compn->lIf(compa) : nullptr;
		if (compa) {
		    addIfpLeaf(compa, aReq);
		}
	    }
	} else {
	    // Redirect to agents
	    auto* ifcs = getTIfs<MAgent>();
	    for (auto* agt : *ifcs) {
		MUnit* agtu = agt->lIf(agtu);
		if (agtu) {
		    agtu->resolveIface(aName, aReq);
		}
	    }
	}
    }
}

bool Syst::attachAgent(MAgent::TCp* aAgt)
{
    bool res = false;
    res = mAgtCp.connect(aAgt);
    invalidateIrm();
    return res;
}

bool Syst::detachAgent(MAgent::TCp* aAgt)
{
    bool res = false;
    res = mAgtCp.disconnect(aAgt);
    invalidateIrm();
    return res;
}

MIface* Syst::MAhost_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MNode>(aType));
    else if (res = checkLif<MContentOwner>(aType)); // To get agent an access to content
    return res;
}



// Connection point - access to MNode, input

CpMnodeInp::CpMnodeInp(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
    bool res = setContent("Provided", "");
    res &= setContent("Required", "MLink");
    assert(res);
}


// Connection point - access to MNode, outpur

CpMnodeOutp::CpMnodeOutp(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MLink");
    res &= setContent("Required", "");
    assert(res);
}
