
#include "dest.h"
#include "syst.h"
#include "data.h"
#include "prof_ids.h"


TrBase::TrBase(const string &aType, const string& aName, MEnv* aEnv): CpStateOutp(aType, aName, aEnv), mCInv(true)
{
}

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

void TrBase::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aName.c_str()); // Local
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    } else if (aName == MDesInpObserver::Type()) {
	// Enable MDesInpObserver resolution for inputs only
	// We cannot resolve inputs atm (it requires inputs registry)
	// So checking components instead of inputs
	//MIfReq::TIfReqCp* req = aReq->binded()->firstPair();
	MIfReq::TIfReqCp* req = *aReq->binded()->pairsBegin();
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
    // Invalidate data cache
    mCInv = true;
    // Redirect to call to pairs
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	auto ifcs = pe ? pe->getTIfs<MDesInpObserver>() : nullptr;
	if (ifcs) for (auto ifc : *ifcs) {
	    ifc->onInpUpdated();
	}
    }
}

Func::TInpIc* TrBase::GetInps(FInp& aInp)
{
    MIfProv::TIfaces* res = nullptr;  
    if (!aInp.mIfp) {
	PFL_DUR_STAT_START(PEvents::EDurStat_Tmp2);
	MNode* inp = getNode(aInp.mName);
	MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	aInp.mIfp = inpu ? inpu->defaultIfProv(MDVarGet::Type()) : nullptr;
	res = aInp.mIfp ? aInp.mIfp->ifaces() : nullptr;
	PFL_DUR_STAT_REC(PEvents::EDurStat_Tmp2);
    } else {
	PFL_DUR_STAT_START(PEvents::EDurStat_Tmp);
	res = aInp.mIfp ? aInp.mIfp->ifaces() : nullptr;
	PFL_DUR_STAT_REC(PEvents::EDurStat_Tmp);
    }
    if (!res || res->size() == 0) {
	LOGN(EDbg, "Cannot get input [" + aInp.mName + "]");
    }
    return reinterpret_cast<Func::TInpIc*>(res);
}

const DtBase* TrBase::VDtGet(const string& aType)
{
    if (mCInv) {
	PFL_DUR_STAT_START(PEvents::EDurStat_Trans);
	mResp = doVDtGet(aType);
	mCInv = false;
	PFL_DUR_STAT_REC(PEvents::EDurStat_Trans);
    }
    return mResp;
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

Func::TInpIc* TrVar::GetInps(int aInpId)
{
    return TrBase::GetInps(*GetFinp(aInpId));
}

void TrVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
}

string TrVar::GetInpUri(int aId) const
{
    FInp* inp = const_cast<TrVar*>(this)->GetFinp(aId);
    assert(inp);
    return inp->mName;
}

const DtBase* TrVar::doVDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    if (!mFunc) {
	Init(aType);
    }
    if (mFunc) {
	res = mFunc->FDtGet();
    } else {
	LOGN(EErr, "Failed initializing");
    }
    return res;
}

MDVarGet* TrVar::GetInp(int aInpId) {
    MDVarGet* res = nullptr;
    auto inps = GetInps(aInpId);
    if (inps && !inps->empty()) {
	res = inps->at(0);
    }
    return res;
}

void TrVar::MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    auto self = const_cast<TrVar*>(this);
    int icnt = GetInpCpsCount();
    aOs << "Inputs [" << icnt << "]" << endl;
    for (auto ii = 0; ii < icnt; ii++) {
	auto* ifaces = self->GetInps(ii);
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


//// Transition "Addition of Var data, negative inp, multi-connecting inputs"

const string TrAddVar::K_InpInp = "Inp";
const string TrAddVar::K_InpInpN = "InpN";

TrAddVar::TrAddVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp), mInpN(K_InpInpN)
{
    AddInput(K_InpInp);
    AddInput(K_InpInpN);
}

void TrAddVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FAddDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
    else {
	LOGN(EWarn, "Failed init, outp [" + aIfaceName + "]");
    }
}

FInp* TrAddVar::GetFinp(int aId)
{
    if (aId == FAddBase::EInp) return &mInp;
    else if (aId == FAddBase::EInpN) return &mInpN;
    else return nullptr;
}


//// Transition "Addition of Var data, single connection inputs"

const string TrAdd2Var::K_InpInp = "Inp";
const string TrAdd2Var::K_InpInp2 = "Inp2";

TrAdd2Var::TrAdd2Var(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp), mInp2(K_InpInp2)
{
    AddInput(K_InpInp);
    AddInput(K_InpInp2);
}

void TrAdd2Var::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FAddDt2<Sdata<int>>::Create(this, aIfaceName)) != NULL);
    else {
	LOGN(EWarn, "Failed init, outp [" + aIfaceName + "]");
    }
}

FInp* TrAdd2Var::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInp;
    else if (aId == Func::EInp2) return &mInp2;
    else return nullptr;
}


//// Transition "Subtraction of Var data, single connection inputs"

const string TrSub2Var::K_InpInp = "Inp";
const string TrSub2Var::K_InpInp2 = "Inp2";

TrSub2Var::TrSub2Var(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp), mInp2(K_InpInp2)
{
    AddInput(K_InpInp);
    AddInput(K_InpInp2);
}

void TrSub2Var::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FSubDt2<Sdata<int>>::Create(this, aIfaceName)) != NULL);
    else {
	LOGN(EWarn, "Failed init, outp [" + aIfaceName + "]");
    }
}

FInp* TrSub2Var::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInp;
    else if (aId == Func::EInp2) return &mInp2;
    else return nullptr;
}



///// TrMplVar

const string TrMplVar::K_InpInp = "Inp";

TrMplVar::TrMplVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp)
{
    AddInput(K_InpInp);
}

void TrMplVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMplDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

FInp* TrMplVar::GetFinp(int aId)
{
    if (aId == FMplBase::EInp) return &mInp; else return nullptr;
}

///// TrDivVar

const string TrDivVar::K_InpInp = "Inp";
const string TrDivVar::K_InpInp2 = "Inp2";

TrDivVar::TrDivVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp), mInp2(K_InpInp2)
{
    AddInput(K_InpInp);
    AddInput(K_InpInp2);
}

void TrDivVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FDivDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

FInp* TrDivVar::GetFinp(int aId)
{
    if (aId == FDivBase::EInp) return &mInp;
    else if (aId == FDivBase::EInp2) return &mInp2;
    else return nullptr;
}




///// TrMinVar

const string TrMinVar::K_InpInp = "Inp";

TrMinVar::TrMinVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp)
{
    AddInput(K_InpInp);
}

void TrMinVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMinDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

FInp* TrMinVar::GetFinp(int aId)
{
    if (aId == FMaxBase::EInp) return &mInp;
    else return nullptr;
}



///// TrMaxVar

const string TrMaxVar::K_InpInp = "Inp";

TrMaxVar::TrMaxVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp)
{
    AddInput(K_InpInp);
}

void TrMaxVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMaxDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

FInp* TrMaxVar::GetFinp(int aId)
{
    if (aId == FMaxBase::EInp) return &mInp;
    else return nullptr;
}



////    TrCmpVar

const string TrCmpVar::K_InpInp = "Inp";
const string TrCmpVar::K_InpInp2 = "Inp2";

TrCmpVar::TrCmpVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp), mInp2(K_InpInp2)
{
    AddInput(K_InpInp);
    AddInput(K_InpInp2);
}

void TrCmpVar::Init(const string& aIfaceName)
{
    TrVar::Init(aIfaceName);
    if (mName == "IsOutput_Eq") {
	    LOGN(EWarn, "");
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (/*aIfaceName == Sdata<bool>::TypeSig() && */inp1 != NULL && inp2 != NULL) {
	string t1 = inp1->VarGetIfid();
	string t2 = inp2->VarGetIfid();
	FCmpBase::TFType ftype = GetFType();
	if (mFunc = FCmp<Sdata<int> >::Create(this, t1, t2, ftype));
	else if (mFunc = FCmp<Enum>::Create(this, t1, t2, ftype));
	else if (mFunc = FCmp<Sdata<string> >::Create(this, t1, t2, ftype));
	else if (mFunc = FCmp<DGuri>::Create(this, t1, t2, ftype));
	else {
	    LOGN(EWarn, "Failed init, inputs [" + t1 + "], [" + t2 + "]");
	}
    }
}

FInp* TrCmpVar::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInp;
    else if (aId == Func::EInp2) return &mInp2;
    else return nullptr;
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
	LOGN(EErr, "Incorrect type of function [" + name() + "]");
    }
    return res;
}

////    TrEqVar
// TODO Not completed. To complete

const string TrEqVar::K_InpInp = "Inp";
const string TrEqVar::K_InpInp2 = "Inp2";

TrEqVar::TrEqVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp(K_InpInp), mInp2(K_InpInp2)
{
    AddInput(K_InpInp);
    AddInput(K_InpInp2);
}

void TrEqVar::Init(const string& aIfaceName)
{
    TrVar::Init(aIfaceName);
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (inp1 != NULL && inp2 != NULL) {
	string t1 = inp1->VarGetIfid();
	string t2 = inp2->VarGetIfid();
    }
}

FInp* TrEqVar::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInp;
    else if (aId == Func::EInp2) return &mInp2;
    else return nullptr;
}



////  TrSwitchBool

const string TrSwitchBool::K_InpInp1 = "Inp1";
const string TrSwitchBool::K_InpInp2 = "Inp2";
const string TrSwitchBool::K_InpSel = "Sel";

TrSwitchBool::TrSwitchBool(const string &aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInp1(K_InpInp1), mInp2(K_InpInp2), mSel(K_InpSel)
{
    AddInput(K_InpInp1);
    AddInput(K_InpInp2);
    AddInput(K_InpSel);
}

const DtBase* TrSwitchBool::doVDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    const Sdata<bool>* sel = GetInpData(mSel, sel);
    const DtBase* inp1 = GetInpData(mInp1, inp1);
    const DtBase* inp2 = GetInpData(mInp2, inp2);
    if (sel && inp1 && inp2)
    {
	if (sel->IsValid()) {
	    auto* data = sel->mData ? inp2 : inp1;
	    res = (aType.empty() || aType == data->GetTypeSig()) ? data : nullptr;
	}
    }
    LOGN(EDbg, "Sel: " + (sel ? sel->ToString(true) : "nul") + ", Inp1: " + (inp1 ? inp1->ToString(true) : "nil") + ", Inp2: " + (inp2 ? inp2->ToString(true) : "nil"));
    return res;
}

string TrSwitchBool::VarGetIfid() const
{
    auto res = const_cast<TrSwitchBool*>(this)->VDtGet(string());
    return res ? res->GetTypeSig() : string();
}


////  TrSwitchBool, v.2

const string TrSwitchBool2::K_InpInp1 = "Inp1";
const string TrSwitchBool2::K_InpInp2 = "Inp2";
const string TrSwitchBool2::K_InpSel = "Sel";

TrSwitchBool2::TrSwitchBool2(const string &aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInp1(K_InpInp1), mInp2(K_InpInp2), mSel(K_InpSel), mPx1(this), mPx2(this)
{
    AddInput(K_InpInp1);
    AddInput(K_InpInp2);
    AddInput(K_InpSel);
}

const DtBase* TrSwitchBool2::doVDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    const Sdata<bool>* sel = GetInpData(mSel, sel);
    const DtBase* inp1 = GetInpData(mInp1, inp1);
    const DtBase* inp2 = GetInpData(mInp2, inp2);
    if (sel && inp1 && inp2)
    {
	if (sel->IsValid()) {
	    auto* data = sel->mData ? inp2 : inp1;
	    res = (aType.empty() || aType == data->GetTypeSig()) ? data : nullptr;
	    mSelV = sel->mData;
	    mSelected = true;
	}
    }
    LOGN(EDbg, "Sel: " + (sel ? sel->ToString(true) : "nul") + ", Inp1: " + (inp1 ? inp1->ToString(true) : "nil") + ", Inp2: " + (inp2 ? inp2->ToString(true) : "nil"));
    return res;
}

string TrSwitchBool2::VarGetIfid() const
{
    auto res = const_cast<TrSwitchBool2*>(this)->VDtGet(string());
    return res ? res->GetTypeSig() : string();
}

void TrSwitchBool2::notifyInpsUpdated(const IobsPx* aPx)
{
    //const Sdata<bool>* sel = GetInpData(mSel, sel);
    //if (sel && sel->IsValid()) {
    if (mSelected) {
	auto* px = mSelV ? &mPx2 : &mPx1;
	if (aPx == px) {
	    onInpUpdated();
	}
    } else {
	onInpUpdated();
    }
}

void TrSwitchBool2::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    MIface* ifr = MNode_getLif(aName.c_str()); // Local
    if (ifr) {
	addIfpLeaf(ifr, aReq);
    } else if (aName == MDesInpObserver::Type()) {
	// Enable MDesInpObserver resolution for inputs only
	// We cannot resolve inputs atm (it requires inputs registry)
	// So checking components instead of inputs
	//MIfReq::TIfReqCp* req = aReq->binded()->firstPair();
	MIfReq::TIfReqCp* req = *aReq->binded()->pairsBegin();
	if (req) {
	    const MIfProvOwner* reqo = req->provided()->rqOwner();
	    MNode* reqn = const_cast<MNode*>(reqo ? reqo->lIf(reqn) : nullptr); // Current requestor as node
	    if (reqn && isNodeOwned(reqn)) {
		if (reqn->name() == mInp1.mName) {
		    ifr = &mPx1;
		} else if (reqn->name() == mInp2.mName) {
		    ifr = &mPx2;
		} else {
		    ifr = dynamic_cast<MDesInpObserver*>(this);
		}
		addIfpLeaf(ifr, aReq);
	    }
	}
    } else {
	CpStateOutp::resolveIfc(aName, aReq);
    }
}





/// Bool transition base

const string TrBool::K_InpInp = "Inp";

TrBool::TrBool(const string &aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInp(K_InpInp)
{
    AddInput(K_InpInp);
}


///// TrAndVar


const DtBase* TrAndVar::doVDtGet(const string& aType)
{
    mRes.mValid = true;
    bool first = true;
    auto* Ic = GetInps(mInp);
    if (Ic && Ic->size() > 0) {
	for (auto dget : *Ic) {
	    const TData* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes = *arg; first = false;
		} else {
		    mRes.mData = mRes.mData && arg->mData;
		}
		LOGN(EDbg, "Inp [" +dget->Uid() + "]: " + arg->ToString());
	    } else {
		mRes.mValid = false; break;
	    }
	}
    }
    LOGN(EDbg, "Res: " + mRes.ToString(true));
    return &mRes;
}


///// TrOrVar


const DtBase* TrOrVar::doVDtGet(const string& aType)
{
    mRes.mValid = true;
    auto* Ic = GetInps(mInp);
    bool first = true;
    if (Ic && Ic->size() > 0) {
	for (auto dget : *Ic) {
	    const TData* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes = *arg; first = false;
		} else {
		    mRes.mData = mRes.mData || arg->mData;
		}
	    } else {
		mRes.mValid = false; break;
	    }
	}
    }
    LOGN(EDbg, "Res: " + mRes.ToString(true));
    return &mRes;
}



///// TrNegVar

const DtBase* TrNegVar::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const TData* inp = GetInpData(mInp, inp);
    if (inp && inp->IsValid()) {
	mRes.mData = !inp->mData;
	mRes.mValid = true;
    }
    return &mRes;
}



///// TrToUriVar

const string TrToUriVar::K_InpInp = "Inp";

TrToUriVar::TrToUriVar(const string &aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInp(K_InpInp)

{
    AddInput(K_InpInp);
}

const DtBase* TrToUriVar::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const TInp* inp = GetInpData(mInp, inp);
    if (inp && inp->IsValid()) {
	mRes.mData = GUri(inp->mData);
	mRes.mValid = true;
    }
    return &mRes;
}


///// Append

const string TrApndVar::K_InpInp1 = "Inp1";
const string TrApndVar::K_InpInp2 = "Inp2";

TrApndVar::TrApndVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInp1(K_InpInp1), mInp2(K_InpInp2)
{
    AddInput(K_InpInp1);
    AddInput(K_InpInp2);
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
	    LOGN(EWarn, "Failed init function");
	}
    }
}

FInp* TrApndVar::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInp1;
    else if (aId == Func::EInp2) return &mInp2;
    else return nullptr;
}



// Transition "Select valid"

const string TrSvldVar::K_InpInp1 = "Inp1";
const string TrSvldVar::K_InpInp2 = "Inp2";

TrSvldVar::TrSvldVar(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInp1(K_InpInp1), mInp2(K_InpInp2)
{
    AddInput(K_InpInp1);
    AddInput(K_InpInp2);
}

string TrSvldVar::VarGetIfid() const
{
    string res;
    auto* inp = const_cast<TrSvldVar*>(this)->VDtGet(string());
    if (inp) {
	res = inp->GetTypeSig();
    }
    return res;
}

const DtBase* TrSvldVar::doVDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    const DtBase* inp1 = GetInpData(mInp1, inp1);
    const DtBase* inp2 = GetInpData(mInp2, inp2);
    if (inp1 && inp2) {
	if (inp1->IsValid() && inp2->IsValid()) {
	    res = inp1;
	    LOGN(EDbg, "Both inputs are valid, res (inp1): " + res->ToString(true));
	} else if (inp1->IsValid()) {
	    res = inp1;
	    LOGN(EDbg, "Inp1 is valid, res (inp1): " + res->ToString(true));
	} else if (inp2->IsValid()) {
	    res = inp2;
	    LOGN(EDbg, "Inp2 is valid, res (inp2): " + res->ToString(true));
	} else {
	    res = inp1;
	    LOGN(EDbg, "Both inputs are invalid, res (inp1): " + res->ToString(true));
	}
    } else {
	LOGN(EErr, "Null input [" + (inp1 ? mInp2.mName : mInp1.mName) + "]");
    }
    return res;
}


///// Tail

const string TrTailVar::K_InpInp = "Inp";
const string TrTailVar::K_InpHead = "Head";

TrTailVar::TrTailVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpHead(K_InpHead)
{
    AddInput(K_InpInp);
    AddInput(K_InpHead);
}

void TrTailVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FTailUri::Create(this, aIfaceName)));
    else {
	LOGN(EWarn, "Failed init function");
    }
}

FInp* TrTailVar::GetFinp(int aId)
{
    if (aId == FTailBase::EInp) return &mInpInp;
    else if (aId == FTailBase::EHead) return &mInpHead;
    else return nullptr;
}



///// Head

const string TrHeadVar::K_InpInp = "Inp";
const string TrHeadVar::K_InpTail = "Tail";

TrHeadVar::TrHeadVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpTail(K_InpTail)
{
    AddInput(K_InpInp);
    AddInput(K_InpTail);
}

void TrHeadVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    string ifaceName(aIfaceName);
    if (ifaceName.empty()) {
	// Output is empty, try to deduct from input
	MDVarGet* inp = GetInp(FHeadBase::EInp);
	if (inp) {
	    ifaceName = inp->VarGetIfid();
	}
    }
    if ((mFunc = FHeadUri::Create(this, ifaceName)));
    else {
	LOGN(EWarn, "Failed init function");
    }
}

FInp* TrHeadVar::GetFinp(int aId)
{
    if (aId == FHeadBase::EInp) return &mInpInp;
    else if (aId == FHeadBase::ETail) return &mInpTail;
    else return nullptr;
}

///// Head by tail len

const string TrHeadtnVar::K_InpInp = "Inp";
const string TrHeadtnVar::K_InpTailn = "Tlen";

TrHeadtnVar::TrHeadtnVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpTailn(K_InpTailn)
{
    AddInput(K_InpInp);
    AddInput(K_InpTailn);
}

void TrHeadtnVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    string ifaceName(aIfaceName);
    if (ifaceName.empty()) {
	// Output is empty, try to deduct from input
	MDVarGet* inp = GetInp(FHeadBase::EInp);
	if (inp) {
	    ifaceName = inp->VarGetIfid();
	}
    }
    if ((mFunc = FHeadTnUri::Create(this, ifaceName)));
    else {
	LOGN(EWarn, "Failed init function");
    }
}

FInp* TrHeadtnVar::GetFinp(int aId)
{
    if (aId == FHeadTnBase::EInp) return &mInpInp;
    else if (aId == FHeadTnBase::ETailn) return &mInpTailn;
    else return nullptr;
}



///// Tail as num of elems

const string TrTailnVar::K_InpInp = "Inp";
const string TrTailnVar::K_InpNum = "Num";

TrTailnVar::TrTailnVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpNum(K_InpNum)
{
    AddInput(K_InpInp);
    AddInput(K_InpNum);
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
	LOGN(EWarn, "Failed init function");
    }
}

FInp* TrTailnVar::GetFinp(int aId)
{
    if (aId == FTailnBase::EInp) return &mInpInp;
    else if (aId == FTailnBase::ENum) return &mInpNum;
    else return nullptr;
}





// Transition agent "Getting container size"

const string TrSizeVar::K_InpInp = "Inp";

TrSizeVar::TrSizeVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
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

FInp* TrSizeVar::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInpInp;
    else return nullptr;
}

string TrSizeVar::VarGetIfid() const
{
    return Sdata<int>::TypeSig();
}


// Agent transition "Get n-coord of Vector, wrapping it by Sdata"

const string TrAtVar::K_InpInp = "Inp";
const string TrAtVar::K_InpIndex = "Index";

TrAtVar::TrAtVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpIndex(K_InpIndex)
{
    AddInput(K_InpInp);
    AddInput(K_InpIndex);
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

FInp* TrAtVar::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInpInp;
    else if (aId == Func::EInp2) return &mInpIndex;
    else return nullptr;
}


// Agent transition "Get n-coord of Vector, generic, no wrapping by Sdata"

const string TrAtgVar::K_InpInp = "Inp";
const string TrAtgVar::K_InpIndex = "Index";

TrAtgVar::TrAtgVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpIndex(K_InpIndex)
{
    AddInput(K_InpInp);
    AddInput(K_InpIndex);
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

FInp* TrAtgVar::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInpInp;
    else if (aId == Func::EInp2) return &mInpIndex;
    else return nullptr;
}


// Transition "Find in Vert<Pair<T>> by first element of pair"

const string TrFindByP::K_InpInp = "Inp";
const string TrFindByP::K_InpSample = "Sample";

TrFindByP::TrFindByP(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpSample(K_InpSample)
{
    AddInput(K_InpInp);
    AddInput(K_InpSample);
}

void TrFindByP::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
     }
    MDVarGet* inp_sample = GetInp(Func::EInp2);
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp_sample && inp) {
	 string t_inp = inp->VarGetIfid();
	if ((mFunc = FFindByP<DGuri>::Create(this, aIfaceName, t_inp)));
    }
}

FInp* TrFindByP::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInpInp;
    else if (aId == Func::EInp2) return &mInpSample;
    else return nullptr;
}



// Agent functions "Tuple composer"
//
const string TrTuple::K_InpInp = "Inp";

TrTuple::TrTuple(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv), 
	mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
}

string TrTuple::VarGetIfid() const
{
    return NTuple::TypeSig();
}

// TODO The design is ugly and bulky. To redesing, ref ds_ibc_dgit
const DtBase* TrTuple::doVDtGet(const string& aType)
{
    mRes.mValid = true;
    const NTuple* arg = GetInpData(mInpInp, arg);
    if (arg) {
	mRes = *arg;
	for (auto it = owner()->pairsBegin(); it != owner()->pairsEnd(); it++) {
	    auto compo = *it;
	    MNode* compn = compo->provided()->lIf(compn);
	    assert(compn);
	    if (compn->name() != K_InpInp) {
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
			    LOGN(EErr, "Input [" + compn->name() + "] is incompatible with tuple component");
			}
		    } else {
			elem->mValid = false;
			LOGN(EDbg, "Cannot get input [" + compn->name() + "]");
		    }
		} else {
		    LOGN(EErr, "No such component [" + compn->name() + "] of tuple");
		}
	    }
	}
    } else {
	mRes.mValid = false;
    }
    return &mRes;
}


// Agent transition "Tuple component selector"
//
const string TrTupleSel::K_InpInp = "Inp";
const string TrTupleSel::K_InpComp= "Comp";

TrTupleSel::TrTupleSel(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp(K_InpInp), mInpComp(K_InpComp)
{
    AddInput(K_InpInp);
    AddInput(K_InpComp);
}

const DtBase* TrTupleSel::doVDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    const NTuple* arg = GetInpData(mInpInp, arg);
    const Sdata<string>* idx = GetInpData(mInpComp, idx);
    if (arg && arg->IsValid() && idx && idx->IsValid()) {
	res = arg->GetElem(idx->mData);
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

const string TrPair::K_InpFirst = "First";
const string TrPair::K_InpSecond = "Second";

TrPair::TrPair(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv),
    mInpFirst(K_InpFirst), mInpSecond(K_InpSecond)
{
    AddInput(K_InpFirst);
    AddInput(K_InpSecond);
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
		LOGN(EWarn, "Failed init, iface [" + aIfaceName + "]");
	    }
	} else {
	    LOGN(EWarn, "Failed init, incompatible inputs [" + t1 + "], [" + t2 + "]");
	    t1 = inp1->VarGetIfid();
	    t2 = inp2->VarGetIfid();
	}
    } else {
	LOGN(EWarn, "Missing input [" + (inp2 ? GetInpUri(Func::EInp1) : GetInpUri(Func::EInp1)) + "]");
    }
}

FInp* TrPair::GetFinp(int aId)
{
    if (aId == Func::EInp1) return &mInpFirst;
    else if (aId == Func::EInp2) return &mInpSecond;
    else return nullptr;
}





///// To string

const string TrTostrVar::K_InpInp = "Inp";

TrTostrVar::TrTostrVar(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
}

const DtBase* TrTostrVar::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const DtBase* inp = GetInpData(mInpInp, inp);
    if (inp && inp->IsValid()) {
	mRes.mData = inp->ToString(false);
	mRes.mValid = true;
    }
    return &mRes;
}


// Transition "Inputs counter"

const string TrInpCnt::K_InpInp = "Inp";

TrInpCnt::TrInpCnt(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
}

string TrInpCnt::VarGetIfid() const
{
    return Sdata<int>::TypeSig();
}

const DtBase* TrInpCnt::doVDtGet(const string& aType)
{
    // TODO Make optimisation to not check type each cycle
    if (aType == TRes::TypeSig()) {
	mRes.mValid = false;
	auto* Ic = GetInps(mInpInp);
	if (Ic) {
	    mRes.mData = Ic->size();
	    mRes.mValid = true;
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
    mInpInp(K_InpInp), mInpIdx(K_InpIdx)
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
    const Sdata<int>* idx = GetInpData(mInpIdx, idx);
    if (idx && idx->IsValid()) {
	auto* Ic = GetInps(mInpInp);
	if (Ic) {
	    if (idx->mData >= 0 && idx->mData < Ic->size()) {
		res = Ic->at(idx->mData);
	    } else {
		LOGN(EErr, "Incorrect index  [" + to_string(idx->mData) + "], inps num: " + to_string(Ic->size()));
	    }
	}
    }
    return res;
}

const DtBase* TrInpSel::doVDtGet(const string& aType)
{
    const DtBase* res = nullptr;
    MDVarGet* inp = GetInp();
    res = inp ? inp->VDtGet(aType) : nullptr;
    return res;
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
    mInpParent(K_InpParent), mInpName(K_InpName)
{
    AddInput(K_InpParent);
    AddInput(K_InpName);
}

const DtBase* TrMutNode::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const Sdata<string>* name = GetInpData(mInpName, name);
    const Sdata<string>* parent = GetInpData(mInpParent, parent);
    if (name && name->IsValid() && parent && parent->IsValid()) {
	mRes.mData = TMut(ENt_Node, ENa_Parent, parent->mData, ENa_Id, name->mData);
	mRes.mValid = true;
    }
    return &mRes;
}


// Agent function "Mut Connect composer"
//
const string TrMutConn::K_InpCp1 = "Cp1";
const string TrMutConn::K_InpCp2 = "Cp2";

TrMutConn::TrMutConn(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv),
    mInpCp1(K_InpCp1), mInpCp2(K_InpCp2)
{
    AddInput(K_InpCp1);
    AddInput(K_InpCp2);
}

const DtBase* TrMutConn::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const TInp* cp1 = GetInpData(mInpCp1, cp1);
    const TInp* cp2 = GetInpData(mInpCp2, cp2);
    if (cp1 && cp2 && cp1->IsValid() && cp2->IsValid()) {
	mRes.mData = TMut(ENt_Conn, ENa_P, cp1->mData, ENa_Q, cp2->mData);
	mRes.mValid = true;
    }
    return &mRes;
}


// Agent function "Mut Content composer"

const string TrMutCont::K_InpName = "Name";
const string TrMutCont::K_InpValue = "Value";
const string TrMutCont::K_InpTarget = "Target";

TrMutCont::TrMutCont(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv),
    mInpName(K_InpName), mInpValue(K_InpValue), mInpTarget(K_InpTarget)
{
    AddInput(K_InpName);
    AddInput(K_InpValue);
    AddInput(K_InpTarget);
}

const DtBase* TrMutCont::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const Sdata<string>* name = GetInpData(mInpName, name);
    const Sdata<string>* value = GetInpData(mInpValue, value);
    if (name && value && name->IsValid() && value->IsValid()) {
	const Sdata<string>* target = GetInpData(mInpTarget, target);
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
    }
    return &mRes;
}



// Agent function "Mut Disconnect composer"

const string TrMutDisconn::K_InpCp1 = "Cp1";
const string TrMutDisconn::K_InpCp2 = "Cp2";

TrMutDisconn::TrMutDisconn(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv),
    mInpCp1(K_InpCp1), mInpCp2(K_InpCp2)
{
    AddInput(K_InpCp1);
    AddInput(K_InpCp2);
}

const DtBase* TrMutDisconn::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const TInp* cp1 = GetInpData(mInpCp1, cp1);
    const TInp* cp2 = GetInpData(mInpCp2, cp2);
    if (cp1 && cp2 && cp1->IsValid() && cp2->IsValid()) {
	mRes.mData = TMut(ENt_Disconn, ENa_P, cp1->mData, ENa_Q, cp2->mData);
	mRes.mValid = true;
    }
    return &mRes;
}



// Agent function "Chromo2 composer"

const string TrChr::K_InpBase = "Base";
const string TrChr::K_InpMut = "Mut";

TrChr::TrChr(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpBase(K_InpBase), mInpMut(K_InpMut)
{
    AddInput(K_InpBase);
    AddInput(K_InpMut);
}

string TrChr::VarGetIfid() const
{
    return DChr2::TypeSig();
}

const DtBase* TrChr::doVDtGet(const string& aType)
{
    mRes.mValid = true;
    // Base
    const DChr2* base = GetInpData(mInpBase, base);
    // Enable the base is optional
    if (base) {
	mRes = *base;
    }
    if (mRes.IsValid()) {
	// Mut
	auto* MutIc = GetInps(mInpMut);
	if (MutIc && MutIc->size() > 0) {
	    for (auto mutget: *MutIc) {
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
	    mRes.mValid = false;
	}
    }
    return &mRes;
}



// Agent function "Chromo2 composer form chromo"

const string TrChrc::K_InpInp = "Inp";

TrChrc::TrChrc(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
}

string TrChrc::VarGetIfid() const
{
    return DChr2::TypeSig();
}

const DtBase* TrChrc::doVDtGet(const string& aType)
{
    auto* InpIc = GetInps(mInpInp);
    if (InpIc && InpIc->size() > 0) {
	mRes.mValid = true;
	for (auto* get : *InpIc) {
	    const DChr2* item = get->DtGet(item);
	    if (item && item->IsValid()) {
		mRes.mData.Root().AddChild(item->mData.Root());
	    } else {
		mRes.mValid = false;
		break;
	    }
	}
    } else {
	mRes.mValid = false;
    }
    return &mRes;
}


/// Agent function "Data is valid"

const string TrIsValid::K_InpInp = "Inp";

TrIsValid::TrIsValid(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
}

const DtBase* TrIsValid::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const DtBase* inp = GetInpData(mInpInp, inp);
    if (inp) {
	mRes.mData = inp->IsValid();
    } else {
	mRes.mData = false;
    }
    mRes.mValid = true;
    LOGN(EDbg, "Inp [" + (inp ? inp->ToString(true) : "nil") + "], res [" + mRes.ToString(true) + "]");
    return &mRes;
}


/// Agent function "Data is invalid"

const string TrIsInvalid::K_InpInp = "Inp";

TrIsInvalid::TrIsInvalid(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
}

const DtBase* TrIsInvalid::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    const DtBase* inp = GetInpData(mInpInp, inp);
    if (inp) {
	mRes.mData = !inp->IsValid();
    } else {
	mRes.mData = true;
    }
    mRes.mValid = true;
    LOGN(EDbg, "Inp [" + (inp ? inp->ToString(true) : "nil") + "], res [" + mRes.ToString(true) + "]");
    return &mRes;
}



// Transition "Type enforcing"

const string TrType::K_InpInp = "Inp";

TrType::TrType(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(K_InpInp);
}

string TrType::VarGetIfid() const
{
    // TODO Implement
    return string();
}



/// Transition "Hash of data", ref ds_dcs_iui_tgh

const string TrHash::K_InpInp = "Inp";

TrHash::TrHash(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv),
    mInpInp(K_InpInp)
{
    AddInput(K_InpInp);
}

const DtBase* TrHash::doVDtGet(const string& aType)
{
    mRes.mValid = false;
    auto* InpIc = GetInps(mInpInp);
    if (InpIc && InpIc->size() > 0) {
	int hash = 0;
	for (auto inp : *InpIc) {
	    const DtBase* data = inp->VDtGet(string());
	    if (data) {
		hash += data->Hash();
	    } else {
		data = inp->VDtGet(string()); // Debug
	    }
	}
	mRes.mData = hash;
	mRes.mValid = true;
    }
    return &mRes;
}


