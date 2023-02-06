
#include "dest.h"
#include "syst.h"
#include "data.h"


TrBase::TrBase(const string &aType, const string& aName, MEnv* aEnv): CpStateOutp(aType, aName, aEnv)
{
}

#if 0
string TrBase::provName() const
{
    return MDVarGet::Type();
}

string TrBase::reqName() const
{
    return MDesInpObserver::Type();
}

bool TrBase::isCompatible(MVert* aPair, bool aExt)
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
#endif

MIface* TrBase::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = CpStateOutp::MNode_getLif(aType);
    return res;
}

MIface* TrBase::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MVert_getLif(aType);
    return res;
}

MIface* TrBase::MOwner_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MUnit>(aType));  // IFR from inputs
    else res = CpStateOutp::MOwner_getLif(aType);
    return res;
}

void TrBase::AddInput(const string& aName)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), aName, mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

template<typename T> bool TrBase::GetInpSdata(int aId, T& aRes)
{
    bool res = false;
    MNode* inp = getNode(GetInpUri(aId));
    if (inp) {
	res =  GetSData(inp, aRes);
    } else {
	Logger()->Write(EDbg, this, "Cannot get input [%s]", GetInpUri(aId).c_str());
    }
    return res;
}

template<typename T> bool TrBase::GetInpData(int aId, T& aRes)
{
    bool res = false;
    MNode* inp = getNode(GetInpUri(aId));
    if (inp) {
	res =  GetGData(inp, aRes);
    } else {
	Logger()->Write(EDbg, this, "Cannot get input [%s]", GetInpUri(aId).c_str());
    }
    return res;
}

void TrBase::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aName.c_str()); // Local
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    } else if (aName == MDesInpObserver::Type()) {
	// Enable MDesInpObserver resolution for inputs only
	// We cannot resolve inputs atm (it requires inputs registry)
	// So checking components instead of inputs
	MIfReq::TIfReqCp* req = aReq->binded()->firstPair();
	if (req) {
	    const MIfProvOwner* reqo = req->provided()->rqOwner();
	    MNode* reqn = const_cast<MNode*>(reqo ? reqo->lIf(reqn) : nullptr); // Current requestor as node
	    if (isNodeOwned(reqn)) {
		ifr = dynamic_cast<MDesInpObserver*>(this);
		addIfpLeaf(ifr, aReq);
	    }
	}
    } else {
	CpStateOutp::resolveIfc(aName, aReq);
    }
}

void TrBase::onInpUpdated()
{
    // Just rederect to call to pairs
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	auto ifcs = pe ? pe->getIfs<MDesInpObserver>() : nullptr;
	if (ifcs) for (auto ifc : *ifcs) {
	    MDesInpObserver* obs = dynamic_cast<MDesInpObserver*>(ifc);
	    // assert(obs);
	    if (obs) obs->onInpUpdated();
	}
    }
}

Func::TInpIc* TrBase::GetInps(const string& aInpName, bool aOpt)
{
    MIfProv::TIfaces* res = nullptr;  
    MNode* inp = getNode(aInpName);
    if (inp) {
	MUnit* inpu = inp->lIf(inpu);
	MIfProv* ifp = inpu ? inpu->defaultIfProv(MDVarGet::Type()) : nullptr;
	res = ifp ? ifp->ifaces() : nullptr;
    } else if (!aOpt) {
	Log(TLog(EDbg, this) + "Cannot get input  [" + aInpName + "]");
    }
    return reinterpret_cast<Func::TInpIc*>(res);
}







///// TrVar

TrVar::TrVar(const string &aType, const string &aName, MEnv* aEnv): TrBase(aType, aName, aEnv), mFunc(nullptr)
{
}

string TrVar::VarGetIfid() const
{
    if (!mFunc) {
	// Function isn't still resolved. Try to resolve basing on func inputs only
	auto self = const_cast<TrVar*>(this);
	self->Init(string());
    }
    return mFunc ? mFunc->IfaceGetId() : string();
}

string TrVar::GetInpUri(int aId) const 
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else if (aId == Func::EInp3) return "Inp3";
    else if (aId == Func::EInp4) return "Inp4";
    else return string();
}

MIfProv::TIfaces* TrVar::GetInps(int aId, const string& aIfName, bool aOpt)
{
    MIfProv::TIfaces* res = nullptr;  
    MNode* inp = getNode(GetInpUri(aId));
    if (inp) {
	MUnit* inpu = inp->lIf(inpu);
	MIfProv* ifp = inpu ? inpu->defaultIfProv(aIfName) : nullptr;
	res = ifp ? ifp->ifaces() : nullptr;
    } else if (!aOpt) {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(aId) + "]");
    }
    return res;
}

Func::TInpIc* TrVar::GetInps(int aId, bool aOpt)
{
    MIfProv::TIfaces* res = nullptr;  
    MNode* inp = getNode(GetInpUri(aId));
    if (inp) {
	MUnit* inpu = inp->lIf(inpu);
	MIfProv* ifp = inpu ? inpu->defaultIfProv(MDVarGet::Type()) : nullptr;
	res = ifp ? ifp->ifaces() : nullptr;
    } else if (!aOpt) {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(aId) + "]");
    }
    return reinterpret_cast<Func::TInpIc*>(res);
}



void TrVar::OnFuncContentChanged()
{
    //OnChanged(this);
}

void TrVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
}

void TrVar::log(int aCtg, const string& aMsg)
{
    Log(TLog(aCtg, this) + aMsg);
}

const DtBase* TrVar::VDtGet(const string& aType)
{
    const DtBase* res = NULL;
    if (!mFunc) {
	Init(aType);
    }
    if (mFunc) {
	res = mFunc->FDtGet();
    }
    return res;
}

void TrVar::MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    auto self = const_cast<TrVar*>(this);
    int icnt = GetInpCpsCount();
    aOs << "Inputs [" << icnt << "]" << endl;
    for (auto ii = 0; ii < icnt; ii++) {
	MIfProv::TIfaces* ifaces = self->GetInps(ii, MDVarGet::Type(), false);
	aOs << "Inp [" << GetInpUri(ii) << "]" << endl;
	for (auto iface : *ifaces) {
	    Ifu::offset(aIdt, aOs); aOs << "Iface " << iface->Uid() << endl;
	    iface->doDump(aLevel, aIdt + 4, aOs);
	}
    }
    if (mFunc) {
	string result;
	//mFunc->GetResult(result);
	aOs << "Result: " << result << endl;
    } else {
	aOs << "Func: NULL" << endl;
    }
}


///// TrAddVar, ver.2

TrAddVar::TrAddVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
    AddInput("InpN");
}

void TrAddVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FAddDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

string TrAddVar::GetInpUri(int aId) const 
{
    if (aId == FAddBase::EInp) return "Inp";
    else if (aId == FAddBase::EInpN) return "InpN";
    else return string();
}

///// TrMplVar

TrMplVar::TrMplVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
}

void TrMplVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMplDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

string TrMplVar::GetInpUri(int aId) const 
{
    if (aId == FMplBase::EInp) return "Inp"; else return string();
}

///// TrDivVar

TrDivVar::TrDivVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(FDivBase::EInp));
    AddInput(GetInpUri(FDivBase::EInp2));
}

void TrDivVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FDivDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

string TrDivVar::GetInpUri(int aId) const 
{
    if (aId == FDivBase::EInp) return "Inp";
    else if (aId == FDivBase::EInp2) return "Inp2";
    else return string();
}



///// TrMinVar

TrMinVar::TrMinVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
}

void TrMinVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMinDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

string TrMinVar::GetInpUri(int aId) const
{
    if (aId == FMaxBase::EInp) return "Inp";
    else return string();
}



///// TrMaxVar

TrMaxVar::TrMaxVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
}

void TrMaxVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMaxDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

string TrMaxVar::GetInpUri(int aId) const
{
    if (aId == FMaxBase::EInp) return "Inp";
    else return string();
}


////    TrCmpVar

TrCmpVar::TrCmpVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
    AddInput("Inp2");
}

void TrCmpVar::Init(const string& aIfaceName)
{
    TrVar::Init(aIfaceName);
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (aIfaceName == Sdata<bool>::TypeSig() && inp1 != NULL && inp2 != NULL) {
	string t1 = inp1->VarGetIfid();
	string t2 = inp2->VarGetIfid();
	FCmpBase::TFType ftype = GetFType();
	if (mFunc = FCmp<Sdata<int> >::Create(this, t1, t2, ftype));
	else if (mFunc = FCmp<Enum>::Create(this, t1, t2, ftype));
	else if (mFunc = FCmp<Sdata<string> >::Create(this, t1, t2, ftype));
	else if (mFunc = FCmp<DGuri>::Create(this, t1, t2, ftype));
	else {
	    Log(TLog(EErr, this) + "Failed init, inputs [" + t1 + "], [" + t2 + "]");
	}
    }
}

string TrCmpVar::GetInpUri(int aId) const 
{
    if (aId == Func::EInp1) return "Inp";
    else if (aId == Func::EInp2) return "Inp2";
    else return string();
}

FCmpBase::TFType TrCmpVar::GetFType()
{
    FCmpBase::TFType res = FCmpBase::EEq;
    if (name().find("_Lt") != string::npos) res = FCmpBase::ELt;
    else if (name().find("_Le") != string::npos) res = FCmpBase::ELe;
    else if (name().find("_Eq") != string::npos) res = FCmpBase::EEq;
    else if (name().find("_Neq") != string::npos) res = FCmpBase::ENeq;
    else if (name().find("_Gt") != string::npos) res = FCmpBase::EGt;
    else if (name().find("_Ge") != string::npos) res = FCmpBase::EGe;
    else {
	Logger()->Write(EErr, this, "Incorrect type of function [%s]", name().c_str());
    }
    return res;
}


////  TrSwitchBool

const string TrSwitchBool::K_InpInp1 = "Inp1";
const string TrSwitchBool::K_InpInp2 = "Inp2";
const string TrSwitchBool::K_InpSel = "Sel";

TrSwitchBool::TrSwitchBool(const string &aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp1Ic(nullptr), mInpInp2Ic(nullptr), mInpSelIc(nullptr)
{
    AddInput(K_InpInp1);
    AddInput(K_InpInp2);
    AddInput(K_InpSel);
}

const DtBase* TrSwitchBool::VDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    const Sdata<bool>* sel = GetInpData(mInpSelIc, K_InpSel, sel);
    if (sel) {
	const DtBase* inp1 = GetInpData(mInpInp1Ic, K_InpInp1, inp1);
	if (inp1) {
	    const DtBase* inp2 = GetInpData(mInpInp2Ic, K_InpInp2, inp2);
	    if (inp2) {
		if (sel->IsValid()) {
		    res = sel->mData ? inp2 : inp1;
		} else {
		    Log(TLog(EErr, this) + "Invalid sel");
		}
	    } else {
		Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp2 + "]");
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp1 + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpSel + "]");
    }
    return res;
}

string TrSwitchBool::VarGetIfid() const
{
    auto res = const_cast<TrSwitchBool*>(this)->VDtGet(string());
    return res->GetTypeSig();
}


/// Bool transition base

const string TrBool::K_InpInp = "Inp";

TrBool::TrBool(const string &aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr)
{
    AddInput(K_InpInp);
}


///// TrAndVar


const DtBase* TrAndVar::VDtGet(const string& aType)
{
    mRes.mValid = true;
    if (!mInpIc) mInpIc = GetInps(K_InpInp);
    bool first = true;
    if (mInpIc && mInpIc->size() > 0) {
	for (auto dget : *mInpIc) {
	    const TData* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes = *arg; first = false;
		} else { mRes.mData = mRes.mData && arg->mData; }
	    } else {
		mRes.mValid = false; break;
	    }
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
    }
    return &mRes;
}


///// TrOrVar


const DtBase* TrOrVar::VDtGet(const string& aType)
{
    mRes.mValid = true;
    if (!mInpIc) mInpIc = GetInps(K_InpInp);
    bool first = true;
    if (mInpIc && mInpIc->size() > 0) {
	for (auto dget : *mInpIc) {
	    const TData* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes = *arg; first = false;
		} else { mRes.mData = mRes.mData || arg->mData; }
	    } else {
		mRes.mValid = false; break;
	    }
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
    }
    return &mRes;
}



///// TrNegVar

const DtBase* TrNegVar::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const TData* inp = GetInpData(mInpIc, K_InpInp, inp);
    if (inp) {
	if (inp->IsValid()) {
	    mRes.mData = !inp->mData;
	    mRes.mValid = true;
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
    }
    return &mRes;
}



///// TrToUriVar

const string TrToUriVar::K_InpInp = "Inp";

TrToUriVar::TrToUriVar(const string &aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr)

{
    AddInput(K_InpInp);
}

const DtBase* TrToUriVar::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const TInp* inp = GetInpData(mInpIc, K_InpInp, inp);
    if (inp) {
	if (inp->IsValid()) {
	    mRes.mData = GUri(inp->mData);
	    mRes.mValid = true;
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
    }
    return &mRes;
}


///// Append

TrApndVar::TrApndVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(Func::EInp1));
    AddInput(GetInpUri(Func::EInp2));
}

void TrApndVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp) {
	string t_inp = inp->VarGetIfid();
	if ((mFunc = FApnd<Sdata<string>>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FApnd<DGuri>::Create(this, aIfaceName, t_inp)));
	else {
	    Logger()->Write(EErr, this, "Failed init function");
	}
    }
}

string TrApndVar::GetInpUri(int aId) const
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else return string();
}


// Transition "Select valid"

const string TrSvldVar::K_InpInp1 = "Inp1";
const string TrSvldVar::K_InpInp2 = "Inp2";

TrSvldVar::TrSvldVar(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInp1Ic(nullptr), mInp2Ic(nullptr)
{
    AddInput(K_InpInp1);
    AddInput(K_InpInp2);
}

string TrSvldVar::VarGetIfid() const
{
    string res;
    auto* inp = const_cast<TrSvldVar*>(this)->VDtGet(string());
    res = inp->GetTypeSig();
    return res;
}

const DtBase* TrSvldVar::VDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    const DtBase* inp1 = GetInpData(mInp1Ic, K_InpInp1, inp1);
    if (inp1) {
	const DtBase* inp2 = GetInpData(mInp2Ic, K_InpInp2, inp2);
	if (inp2) {
	    if (inp1->IsValid() && inp2->IsValid()) {
		res = inp1;
	    } else if (inp1->IsValid()) {
		res = inp1;
	    } else if (inp2->IsValid()) {
		res = inp2;
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp2 + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp1 + "]");
    }
    return res;
}


///// Tail

TrTailVar::TrTailVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(FTailBase::EInp));
    AddInput(GetInpUri(FTailBase::EHead));
}

void TrTailVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FTailUri::Create(this, aIfaceName)));
    else {
	Logger()->Write(EErr, this, "Failed init function");
    }
}

string TrTailVar::GetInpUri(int aId) const
{
    if (aId == FTailBase::EInp) return "Inp";
    else if (aId == FTailBase::EHead) return "Head";
    else return string();
}


///// Head

TrHeadVar::TrHeadVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(FHeadBase::EInp));
    AddInput(GetInpUri(FHeadBase::ETail));
}

void TrHeadVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FHeadUri::Create(this, aIfaceName)));
    else {
	Logger()->Write(EErr, this, "Failed init function");
    }
}

string TrHeadVar::GetInpUri(int aId) const
{
    if (aId == FHeadBase::EInp) return "Inp";
    else if (aId == FHeadBase::ETail) return "Tail";
    else return string();
}

///// Tail as num of elems

TrTailnVar::TrTailnVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(FTailnBase::EInp));
    AddInput(GetInpUri(FTailnBase::ENum));
}

void TrTailnVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    string ifaceName(aIfaceName);
    if (ifaceName.empty()) {
	MDVarGet* inp = GetInp(FTailnBase::EInp);
	if (inp) {
	    ifaceName = inp->VarGetIfid();
	}
    }
    if ((mFunc = FTailnUri::Create(this, ifaceName)));
    else {
	Logger()->Write(EErr, this, "Failed init function");
    }
}

string TrTailnVar::GetInpUri(int aId) const
{
    if (aId == FTailnBase::EInp) return "Inp";
    else if (aId == FTailnBase::ENum) return "Num";
    else return string();
}





// Transition agent "Getting container size"

TrSizeVar::TrSizeVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
}

void TrSizeVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp = GetInp(Func::EInp1);
    // TODO Redesing to avoid overhead, ref ds_rdr_itr_ovst
    if (inp) {
	string t_inp = inp->VarGetIfid();
	if ((mFunc = FSizeVect<string>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FSizeVect<Pair<DGuri>>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FSizeVect<DGuri>::Create(this, aIfaceName, t_inp)));
    }
}

string TrSizeVar::GetInpUri(int aId) const
{
    if (aId == Func::EInp1) return "Inp";
    else return string();
}

string TrSizeVar::VarGetIfid() const
{
    return MDtGet<Sdata<int>>::Type();
}


// Agent transition "Get n-coord of Vector, wrapping it by Sdata"

TrAtVar::TrAtVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
    AddInput("Index");
}

void TrAtVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
     }
    MDVarGet* inp_ind = GetInp(Func::EInp2);
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp_ind && inp) {
	 string t_inp = inp->VarGetIfid();
	if ((mFunc = FAtVect<string>::Create(this, aIfaceName, t_inp)));
    }
}

string TrAtVar::GetInpUri(int aId) const
{
    if (aId == Func::EInp2) return "Index";
    else if (aId == Func::EInp1) return "Inp";
    else return string();
}


// Agent transition "Get n-coord of Vector, generic, no wrapping by Sdata"

TrAtgVar::TrAtgVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Inp");
    AddInput("Index");
}

void TrAtgVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
     }
    MDVarGet* inp_ind = GetInp(Func::EInp2);
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp_ind && inp) {
	 string t_inp = inp->VarGetIfid();
	if ((mFunc = FAtgVect<DGuri>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FAtgVect<Pair<DGuri>>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FAtgPair<DGuri>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FAtgPair<Sdata<int>>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FAtgPair<DGuri>::Create(this, aIfaceName, t_inp)));
    }
}

string TrAtgVar::GetInpUri(int aId) const
{
    if (aId == Func::EInp2) return "Index";
    else if (aId == Func::EInp1) return "Inp";
    else return string();
}



// Agent functions "Tuple composer"
//
const string TrTuple::K_InpName = "Inp";

TrTuple::TrTuple(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv), 
	mInpIc(nullptr)
{
    AddInput(K_InpName);
}

string TrTuple::VarGetIfid() const
{
    return NTuple::TypeSig();
}

// TODO The design is ugly and bulky. To redesing, ref ds_ibc_dgit
const DtBase* TrTuple::VDtGet(const string& aType)
{
    mRes.mValid = true;
    if (!mInpIc) mInpIc = GetInps(K_InpName);
    auto* dfget = (mInpIc && mInpIc->size() == 1) ? mInpIc->at(0) : nullptr;
    if (dfget) {
	const NTuple* arg = dfget->DtGet(arg);
	mRes = *arg;
	auto compo = owner()->firstPair();
	while (compo) {
	    MNode* compn = compo->provided()->lIf(compn);
	    assert(compn);
	    if (compn->name() != K_InpName) {
		DtBase* elem = mRes.GetElem(compn->name());
		if (elem) {
		    MUnit* inpu = compn->lIf(inpu);
		    MDVarGet* inpvg = inpu ? inpu->getSif(inpvg) : nullptr;
		    if (inpvg) {
			auto* edata = inpvg->VDtGet(elem->GetTypeSig());
			if (edata) {
			    *elem = *edata;
			} else {
			    elem->mValid = false;
			    Log(TLog(EErr, this) + "Input [" + compn->name() + "] is incompatible with tuple component");
			}
		    } else {
			elem->mValid = false;
			Log(TLog(EDbg, this) + "Cannot get input  [" + compn->name() + "]");
		    }
		} else {
		    Log(TLog(EErr, this) + "No such component [" + compn->name() + "] of tuple");
		}
	    }
	    compo = owner()->nextPair(compo);
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpName + "]");
	mRes.mValid = false;
    }
    return &mRes;
}


// Agent transition "Tuple component selector"
//
const string TrTupleSel::K_InpName = "Inp";
const string TrTupleSel::K_InpCompName = "Comp";

TrTupleSel::TrTupleSel(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr), mInpCIc(nullptr)
{
    AddInput(K_InpName);
    AddInput(K_InpCompName);
}

const DtBase* TrTupleSel::VDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    if (!mInpIc) mInpIc = GetInps(K_InpName);
    auto* dfget = (mInpIc && mInpIc->size() == 1) ? mInpIc->at(0) : nullptr;
    if (dfget) {
	const NTuple* arg = dfget->DtGet(arg);
	if (!mInpCIc) mInpCIc = GetInps(K_InpCompName);
	auto* dcget = (mInpCIc && mInpCIc->size() == 1) ? mInpCIc->at(0) : nullptr;
	if (dcget) {
	    const Sdata<string>* idx = dcget->DtGet(idx);
	    res = arg->GetElem(idx->mData);
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpCompName + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpName + "]");
    }
    return res;
}

string TrTupleSel::VarGetIfid() const
{
    string res;
    const DtBase* dt = const_cast<TrTupleSel*>(this)->VDtGet("");
    if (dt) {
	res = dt->GetTypeSig();
    }
    return res;
}


// Pair composer

TrPair::TrPair(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(Func::EInp1));
    AddInput(GetInpUri(Func::EInp2));
}

void TrPair::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (inp1 && inp2) {
	string t1 = inp1->VarGetIfid();
	string t2 = inp2->VarGetIfid();
	if (t1 == t2) {
	    if ((mFunc = FPair<DGuri>::Create(this, aIfaceName, t1)));
	    else if ((mFunc = FPair<Sdata<int>>::Create(this, aIfaceName, t1)));
	    else {
		Log(TLog(EErr, this) + "Failed init, iface [" + aIfaceName + "]");
	    }
	} else {
	    Log(TLog(EErr, this) + "Failed init, incompatible inputs [" + t1 + "], [" + t2 + "]");
	}
    } else {
	Log(TLog(EErr, this) + "Missing input [" + (inp2 ? GetInpUri(Func::EInp1) : GetInpUri(Func::EInp1)) + "]");
    }
}

string TrPair::GetInpUri(int aId) const
{
    if (aId == Func::EInp1) return "First";
    else if (aId == Func::EInp2) return "Second";
    else return string();
}




///// To string

const string TrTostrVar::K_InpInp = "Inp";

TrTostrVar::TrTostrVar(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr)
{
    AddInput(K_InpInp);
}

const DtBase* TrTostrVar::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const DtBase* inp = GetInpData(mInpIc, K_InpInp, inp);
    if (inp) {
	if (inp->IsValid()) {
	    mRes.mData = inp->ToString(false);
	    mRes.mValid = true;
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
    }
    return &mRes;
}


// Transition "Inputs counter"

const string TrInpCnt::K_InpInp = "Inp";

TrInpCnt::TrInpCnt(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr)
{
    AddInput(K_InpInp);
}

string TrInpCnt::VarGetIfid() const
{
    return Sdata<int>::TypeSig();
}

const DtBase* TrInpCnt::VDtGet(const string& aType)
{
    // TODO Make optimisation to not check type each cycle
    if (aType == Sdata<int>::TypeSig()) {
	if (!mInpIc) mInpIc = GetInps(K_InpInp);
	if (mInpIc) {
	    mRes.mData = mInpIc->size();
	    mRes.mValid = true;
	} else {
	    mRes.mValid = false;
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
	}
	return &mRes;
    } else {
	return nullptr;
    }
}


// Transition "Input selector"

const string TrInpSel::K_InpInp = "Inp";
const string TrInpSel::K_InpIdx = "Idx";

TrInpSel::TrInpSel(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr), mInpIdxIc(nullptr)
{
    AddInput(K_InpInp);
    AddInput(K_InpIdx);
}

string TrInpSel::VarGetIfid() const
{
    string res;
    MDVarGet* inp = const_cast<TrInpSel*>(this)->GetInp();
    res = inp->VarGetIfid();
    return res;
}

MDVarGet* TrInpSel::GetInp()
{
    MDVarGet* res = nullptr;
    if (!mInpIdxIc) mInpIdxIc = GetInps(K_InpIdx);
    auto* idxget = (mInpIdxIc && mInpIdxIc->size() == 1) ? mInpIdxIc->at(0) : nullptr;
    const Sdata<int>* idx = idxget ? idxget->DtGet(idx) : nullptr;
    if (idx) {
	if (!mInpIc) mInpIc = GetInps(K_InpInp);
	if (mInpIc) {
	    if (idx->IsValid()) {
		if (idx->mData >= 0 || idx->mData < mInpIc->size()) {
		    res = mInpIc->at(idx->mData);
		} else {
		    Log(TLog(EErr, this) + "Incorrect index  [" + to_string(idx->mData) + "], inps num: " + to_string(mInpIc->size()));
		}
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpIdx + "]");
    }
    return res;
}

const DtBase* TrInpSel::VDtGet(const string& aType)
{
    MDVarGet* inp = GetInp();
    return inp ? inp->VDtGet(aType) : nullptr;
}




// Agent functions "Mut composer" base


TrMut::TrMut(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
}

string TrMut::VarGetIfid() const
{
    return DMut::TypeSig();
}



// Agent function "Mut Node composer"

const string TrMutNode::K_InpParent = "Parent";
const string TrMutNode::K_InpName = "Name";

TrMutNode::TrMutNode(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv),
    mInpParentIc(nullptr), mInpNameIc(nullptr)
{
    AddInput(K_InpParent);
    AddInput(K_InpName);
}

const DtBase* TrMutNode::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const Sdata<string>* name = GetInpData(mInpNameIc, K_InpName, name);
    if (name) {
	const Sdata<string>* parent = GetInpData(mInpParentIc, K_InpParent, parent);
	if (parent) {
	    mRes.mData = TMut(ENt_Node, ENa_Parent, parent->mData, ENa_Id, name->mData);
	    mRes.mValid = true;
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpParent + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpName + "]");
    }
    return &mRes;
}


// Agent function "Mut Connect composer"
//
const string TrMutConn::K_InpCp1 = "Cp1";
const string TrMutConn::K_InpCp2 = "Cp2";

TrMutConn::TrMutConn(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv),
    mInpCp1Ic(nullptr), mInpCp2Ic(nullptr)
{
    AddInput(K_InpCp1);
    AddInput(K_InpCp2);
}

const DtBase* TrMutConn::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const TInp* cp1 = GetInpData(mInpCp1Ic, K_InpCp1, cp1);
    if (cp1) {
	const TInp* cp2 = GetInpData(mInpCp2Ic, K_InpCp2, cp2);
	if (cp2) {
	    if (cp1->IsValid() && cp2->IsValid()) {
		mRes.mData = TMut(ENt_Conn, ENa_P, cp1->mData, ENa_Q, cp2->mData);
		mRes.mValid = true;
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpCp2 + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpCp1 + "]");
    }
    return &mRes;
}


// Agent function "Mut Content composer"

const string TrMutCont::K_InpName = "Name";
const string TrMutCont::K_InpValue = "Value";
const string TrMutCont::K_InpTarget = "Target";

TrMutCont::TrMutCont(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv),
    mInpNameIc(nullptr), mInpValueIc(nullptr), mInpTargetIc(nullptr)
{
    AddInput(K_InpName);
    AddInput(K_InpValue);
    AddInput(K_InpTarget);
}

const DtBase* TrMutCont::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const Sdata<string>* name = GetInpData(mInpNameIc, K_InpName, name);
    if (name && name->IsValid()) {
	const Sdata<string>* value = GetInpData(mInpValueIc, K_InpValue, value);
	if (value && value->IsValid()) {
	    const Sdata<string>* target = GetInpData(mInpTargetIc, K_InpTarget, target);
	    // Target is optional
	    if (target) {
		if (target->IsValid()) {
		    mRes.mData = TMut(ENt_Cont, ENa_Targ, target->mData, ENa_Id, name->mData, ENa_MutVal, value->mData);
		    mRes.mValid = true;
		}
	    } else {
		mRes.mData = TMut(ENt_Cont, ENa_Id, name->mData, ENa_MutVal, value->mData);
		mRes.mValid = true;
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpValue + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpName + "]");
    }
    return &mRes;
}



// Agent function "Mut Disconnect composer"

const string TrMutDisconn::K_InpCp1 = "Cp1";
const string TrMutDisconn::K_InpCp2 = "Cp2";

TrMutDisconn::TrMutDisconn(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv),
    mInpCp1Ic(nullptr), mInpCp2Ic(nullptr)
{
    AddInput(K_InpCp1);
    AddInput(K_InpCp2);
}

const DtBase* TrMutDisconn::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const TInp* cp1 = GetInpData(mInpCp1Ic, K_InpCp1, cp1);
    if (cp1) {
	const TInp* cp2 = GetInpData(mInpCp2Ic, K_InpCp2, cp2);
	if (cp2) {
	    if (cp1->IsValid() && cp2->IsValid()) {
		mRes.mData = TMut(ENt_Disconn, ENa_P, cp1->mData, ENa_Q, cp2->mData);
		mRes.mValid = true;
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpCp2 + "]");
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpCp1 + "]");
    }
    return &mRes;
}



// Agent function "Chromo2 composer"

const string TrChr::K_InpBase = "Base";
const string TrChr::K_InpMut = "Mut";

TrChr::TrChr(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpBaseIc(nullptr), mInpMutIc(nullptr)
{
    AddInput(K_InpBase);
    AddInput(K_InpMut);
}

string TrChr::VarGetIfid() const
{
    return DChr2::TypeSig();
}

const DtBase* TrChr::VDtGet(const string& aType)
{
    mRes.mValid = true;
    // Base
    const DChr2* base = GetInpData(mInpBaseIc, K_InpBase, base);
    // Enable the base is optional
    if (base) {
	mRes = *base;
    }
    if (mRes.IsValid()) {
	// Mut
	if (!mInpMutIc) mInpMutIc = GetInps(K_InpMut);
	if (mInpMutIc && mInpMutIc->size() > 0) {
	    for (auto mutget: *mInpMutIc) {
		const DMut* mut = mutget ? mutget->DtGet(mut) : nullptr;
		if (mut) {
		    if (mut->mValid) {
			mRes.mData.Root().AddChild(mut->mData);
		    } else {
			mRes.mValid = false;
			break;
		    }
		} else {
		    mRes.mValid = false;
		    break;
		}
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpMut + "]");
	    mRes.mValid = false;
	}
    }
    return &mRes;
}



// Agent function "Chromo2 composer form chromo"

const string TrChrc::K_InpInp = "Inp";

TrChrc::TrChrc(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr)
{
    AddInput(K_InpInp);
}

string TrChrc::VarGetIfid() const
{
    return DChr2::TypeSig();
}

const DtBase* TrChrc::VDtGet(const string& aType)
{
    if (!mInpIc) mInpIc = GetInps(K_InpInp);
    if (mInpIc && mInpIc->size() > 0) {
	mRes.mValid = true;
	for (auto* get : *mInpIc) {
	    const DChr2* item = get->DtGet(item);
	    if (item && item->IsValid()) {
		mRes.mData.Root().AddChild(item->mData.Root());
	    } else {
		mRes.mValid = false;
		break;
	    }
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
	mRes.mValid = false;
    }
    return &mRes;
}


/// Agent function "Data is valid"

const string TrIsValid::K_InpInp = "Inp";

TrIsValid::TrIsValid(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr)
{
    AddInput(K_InpInp);
}

const DtBase* TrIsValid::VDtGet(const string& aType)
{
    mRes.mValid = false;
    const DtBase* inp = GetInpData(mInpIc, K_InpInp, inp);
    if (inp) {
	mRes.mData = inp->IsValid();
	mRes.mValid = true;
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + K_InpInp + "]");
    }
    return &mRes;
}


// Transition "Type enforcing"

TrType::TrType(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInp));
}

MIface* TrType::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

string TrType::GetInpUri(int aId) const
{
    if (aId == EInp) return "Inp";
    else return string();
}

MIface* TrType::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    // TODO Implement
    return res;
}

string TrType::VarGetIfid() const
{
    // TODO Implement
    return string();
}



/// Transition "Hash of data", ref ds_dcs_iui_tgh

const string TrHash::K_InpInp = "Inp";

TrHash::TrHash(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpIc(nullptr)
{
    AddInput(K_InpInp);
}

const DtBase* TrHash::VDtGet(const string& aType)
{
    mRes.mValid = false;
    if (!mInpIc) mInpIc = GetInps(K_InpInp);
    if (mInpIc && mInpIc->size() > 0) {
	int hash = 0;
	for (auto inp : *mInpIc) {
	    const DtBase* data = inp->VDtGet(string());
	    hash += data->Hash();
	}
	mRes.mData = hash;
	mRes.mValid = true;
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(EInp) + "]");
    }
    return &mRes;
}


