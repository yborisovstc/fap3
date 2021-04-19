#include "syst.h"


const string KContProvided = "Provided";
const string KContRequired = "Required";

string ConnPointu::KReqName = "Required";
string ConnPointu::KProvName = "Provided";


ConnPointu::ConnPointu(const string &aName, MEnv* aEnv): Vertu(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

MIface* ConnPointu::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
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


bool ConnPointu::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = true;
    MIface* ifr = MNode_getLif(aName.c_str()); // Local
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    }
    if (aName == provName()) {
	// Requested provided iface - cannot be obtain via pairs - redirect to host
	auto owner = Owner();
	MUnit* ownu = owner ? const_cast<MOwner*>(owner)->lIf(ownu): nullptr;
	res = const_cast<MUnit*>(ownu)->resolveIface(aName, aReq);
    } else if (aName == reqName()) {
	// Requested required iface - redirect to pairs
	for (MVert* pair : mPairs) {
	    MUnit* pairu = pair->lIf(pairu);
	    res = pairu->resolveIface(aName, aReq);
	}
    }
    return res;
}



// Extender agent, monolitic, multicontent, unit


const string KUriInt = "Int";  /*!< Internal connpoint */
const string KContDir = "Direction";
const string KContDir_Val_Regular = "Regular";
const string KContDir_Val_Inp = "Inp";
const string KContDir_Val_Out = "Out";



Extd::Extd(const string& aName, MEnv* aEnv): Vertu(aName, aEnv)
{
    if (aName.empty()) mName = Type();
    setContent(KContDir, KContDir_Val_Regular);
}

bool Extd::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = false;
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
		if (pairo && aReq->provided()->isRequestor(pairo)) {
		    res = pairu->resolveIface(aName, aReq);
		}
	    }
	} else {
	    // Redirect to internal CP
	    MUnit* intcpu = intcp->lIf(intcpu);  
	    res = intcpu->resolveIface(aName, aReq);
	}
    }
    return res;
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
    res = extn->lIf(res);
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



// System



Syst::Syst(const string &aName, MEnv* aEnv): Elem(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

Syst::~Syst()
{
}

MIface* Syst::doMOwnerGetLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MUnit>(aType));
    else if (res = checkLif<MUnit>(aType));
    return res;
}

void Syst::mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string sp = aMut.Attr(ENa_P);
    string sq = aMut.Attr(ENa_Q);
    MNode* pn = getNode(sp, aCtx.mNs);
    MNode* qn = getNode(sq, aCtx.mNs);
    if (pn && qn) {
	MVert* pv = pn->lIf(pv);
	MVert* qv = qn->lIf(pv);
	if (pv && qv) {
	    bool res = MVert::connect(pv, qv);
	    if (!res) {
		Log(TLog(EErr, this) + "Failed connecting [" + sp + "] to [" + sq + "]");
	    }
	} else {
	    Log(TLog(EErr, this) + "Connecting [" + sp + "] to [" + sq + "] -  [" + (pv ? sq : sp) + "] isn't connectable");
	}
    } else {
	Log(TLog(EErr, this) + "Connecting [" + sp + "] to [" + sq + "] - cannot find [" + (pn ? sq : sp) + "]");
    }
    if (!aUpdOnly) {
	notifyNodeMutated(aMut, aCtx);
    }
}

bool Syst::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = true;
    MIface* ifr = MNode_getLif(aName.c_str());
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    }
    if (aName == MAgent::Type()) {
	for (int i = 0; i < mCpOwner.pcount(); i++) {
	    MOwned* comp = mCpOwner.pairAt(i);
	    MNode* compn = comp->lIf(compn);
	    MAgent* compa = compn ? compn->lIf(compa) : nullptr;
	    if (compa) {
		addIfpLeaf(ifr, aReq);
	    }
	}
    } else {
	// Redirect to agents
	MIfProv* ifp = defaultIfProv(MAgent::Type());
	MIfProv* maprov = ifp->first();
	while (maprov) {
	    MUnit* agtu = maprov ? maprov->iface()->lIf(agtu) : nullptr;
	    if (agtu) {
		res = agtu->resolveIface(aName, aReq);
	    }
	    maprov = maprov->next();
	}
    }
    return res;
}



