
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

MIface* TrVar::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (!mFunc) {
	Init(aName);
	if (mFunc) {
	    res = mFunc->getLif(aName);
	}
    } else {
	res = mFunc->getLif(aName);
	if (!res) {
	    Init(aName);
	    if (mFunc) {
		res = mFunc->getLif(aName);
	    }
	}
    }
    return res;
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

MIface* TrVar::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = CpStateOutp::MNode_getLif(aType);
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
	mFunc->GetResult(result);
	aOs << "Result: " << result << endl;
    } else {
	aOs << "Func: NULL" << endl;
    }
}





///// TrAddVar

TrAddVar::TrAddVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), "Inp", mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
    cp = Provider()->createNode(CpStateInp::Type(), "InpN", mEnv);
    assert(cp);
    res = attachOwned(cp);
    assert(res);
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
    if (aIfaceName == MDtGet<Sdata<bool> >::Type() && inp1 != NULL && inp2 != NULL) {
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


TrSwitchBool::TrSwitchBool(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput("Sel");
    AddInput("Inp1");
    AddInput("Inp2");
}

void TrSwitchBool::Init(const string& aIfaceName)
{
    TrVar::Init(aIfaceName);
    MDVarGet* inpCase = GetInp(FSwitchBase::EInp_Sel);
    MDVarGet* inp2 = GetInp(FSwitchBase::EInp_1);
    MDVarGet* inp3 = GetInp(FSwitchBase::EInp_1 + 1);
    if (inpCase && inp2 && inp3) {
	MDtGet<Sdata<bool>>* inpCaseD = inpCase->GetDObj(inpCaseD);
	void* inp2d = inp2->DoGetDObj(aIfaceName.c_str());
	void* inp3d = inp3->DoGetDObj(aIfaceName.c_str());
	if (inpCaseD && inp2d && inp3d) {
	    string t_case = inpCase->VarGetIfid();
	    mFunc = FSwitchBool::Create(this, aIfaceName, t_case);
	}
    } else {
	string inp = GetInpUri(FSwitchBase::EInp_Sel);
	if (!inp2) inp = GetInpUri(FSwitchBase::EInp_1);
	else if (!inp3) inp = GetInpUri(FSwitchBase::EInp_1 + 1);
	Logger()->Write(EErr, this, "Cannot get input [%s]", inp.c_str());
    }
}

string TrSwitchBool::GetInpUri(int aId) const 
{
    if (aId == FSwitchBase::EInp_Sel) return "Sel";
    else if (aId > FSwitchBase::EInp_Sel) {
	stringstream ss;
	ss <<  "Inp" << (aId - FSwitchBase::EInp_Sel);
	return ss.str();
    }
    else return string();
}

MIface* TrSwitchBool::DoGetDObj(const char *aName)
{
    MIface* res = nullptr;
    if (!mFunc) {
	Init(aName);
	if (mFunc) {
	    MDVarGet* fvg = dynamic_cast<MDVarGet*>(mFunc->getLif(MDVarGet::Type()));
	    res = fvg ? fvg->DoGetDObj(aName) : NULL;
	}
    } else {
	MDVarGet* fvg = dynamic_cast<MDVarGet*>(mFunc->getLif(MDVarGet::Type()));
	res = fvg ? fvg->DoGetDObj(aName) : NULL;
	if (!res) {
	    Init(aName);
	    if (mFunc) {
		MDVarGet* fvg = dynamic_cast<MDVarGet*>(mFunc->getLif(MDVarGet::Type()));
		res = fvg ? fvg->DoGetDObj(aName) : NULL;
	    }
	}
    }
    return res;
}


///// TrAndVar

TrAndVar::TrAndVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), "Inp", mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

void TrAndVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FBAndDt::Create(this, aIfaceName)) != NULL);
}

string TrAndVar::GetInpUri(int aId) const 
{
    if (aId == FBAndDt::EInp) return "Inp";
    else return string();
}


///// TrOrVar

TrOrVar::TrOrVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), "Inp", mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

void TrOrVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FBOrDt::Create(this, aIfaceName)) != NULL);
}

string TrOrVar::GetInpUri(int aId) const 
{
    if (aId == FBOrDt::EInp) return "Inp";
    else return string();
}



///// TrNegVar

TrNegVar::TrNegVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(FBnegDt::EInp));
}

void TrNegVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FBnegDt::Create(this, aIfaceName)) != NULL);
}

string TrNegVar::GetInpUri(int aId) const 
{
    if (aId == FBnegDt::EInp) return "Inp";
    else return string();
}



///// TrToUriVar

TrToUriVar::TrToUriVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    MNode* cp = Provider()->createNode(CpStateInp::Type(), "Inp", mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

void TrToUriVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp) {
	string t_inp = inp->VarGetIfid();
	if ((mFunc = FUri::Create(this, aIfaceName, t_inp)));
	else {
	    Logger()->Write(EErr, this, "Failed init function");
	}
    }
}

string TrToUriVar::GetInpUri(int aId) const 
{
    if (aId == FUri::EInp) return "Inp";
    else return string();
}

string TrToUriVar::VarGetIfid() const
{
    return MDtGet<DGuri>::Type();
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


///// Select valid

TrSvldVar::TrSvldVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(Func::EInp1));
    AddInput(GetInpUri(Func::EInp2));
}

void TrSvldVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp) {
	string t_inp = inp->VarGetIfid();
	if ((mFunc = FSvld<Sdata<string>>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FSvld<DGuri>::Create(this, aIfaceName, t_inp)));
	else {
	    Logger()->Write(EErr, this, "Failed init function");
	}
    }
}

string TrSvldVar::GetInpUri(int aId) const
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else return string();
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

TrTuple::TrTuple(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(K_InpName);
}

MIface* TrTuple::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

MIface* TrTuple::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<NTuple>::Type()) == 0) {
	res = dynamic_cast<MDtGet<NTuple>*>(this);
    }
    return res;
}

string TrTuple::VarGetIfid() const
{
    return MDtGet<NTuple>::Type();
}

// TODO The design is ugly and bulky. To redesing, ref ds_ibc_dgit
void TrTuple::DtGet(NTuple& aData)
{
    bool res = false;
    MNode* tinp = getNode(K_InpName);
    GetGData(tinp, aData);
    auto compo = owner()->firstPair();
    while (compo) {
	MNode* compn = compo->provided()->lIf(compn);
	assert(compn);
	if (compn->name() != K_InpName) {
	    MUnit* inpu = compn->lIf(inpu);
	    MDVarGet* inpvg = inpu ? inpu->getSif(inpvg) : nullptr;
	    DtBase* elem = aData.GetElem(compn->name());
	    if (elem) {
		if (inpvg) {
		    string dgtype = string("MDtGet_") + elem->GetTypeSig();
		    MDtGetBase* inpdg = static_cast<MDtGetBase*>(inpvg->DoGetDObj(dgtype.c_str()));
		    if (inpdg) {
			res = inpdg->DtbGet(*elem);
			if (!res) {
			    Log(TLog(EErr, this) + "Failed getting [" + compn->name() + "] data");
			}
		    } else {
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
    mRes = aData;
}


#if 0
// Agent transition "Tuple component selector"
//
const string TrTupleSel::K_InpName = "Inp";
const string TrTupleSel::K_CompName = "Comp";

TrTupleSel::TrTupleSel(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(K_InpName);
    AddInput(K_CompName);
}

MIface* TrTupleSel::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

MIface* TrTupleSel::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    MNode* tinp = getNode(K_InpName);
    GetGData(tinp, mData);
    Sdata<string> compName;
    MNode* cnamen = getNode(K_CompName);
    GetGData(cnamen, compName);
    if (mData.IsValid() && compName.IsValid()) {
	DtBase* elem = mData.GetElem(compName.mData);
	string elemGetterType = string("MDtGet_") + elem->GetTypeSig();
	if (aName = elemGetterType) {
	}
	return res;
    }
#endif


// Transition agent "Tuple component selector"
//
//
const string TrTupleSel::K_InpName = "Inp";
const string TrTupleSel::K_CompName = "Comp";

TrTupleSel::TrTupleSel(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(K_InpName);
    AddInput(K_CompName);
}

void TrTupleSel::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
     }
    if ((mFunc = FTupleSel<Sdata<int>>::Create(this, aIfaceName)));
    else if ((mFunc = FTupleSel<Sdata<string>>::Create(this, aIfaceName)));
    else {
	Logger()->Write(EErr, this, "Failed init function");
    }
}

string TrTupleSel::GetInpUri(int aId) const
{
    if (aId == Func::EInp1) return K_InpName;
    else if (aId == Func::EInp2) return K_CompName;
    else return string();
}

string TrTupleSel::VarGetIfid() const
{
    string res;
    // Override TrVar behavior - we can assume ifid from input data
    MDVarGet* dget = const_cast<TrTupleSel*>(this)->GetInp(Func::EInp1);
    MDtGet<NTuple>* dfget = dget ? dget->GetDObj(dfget) : nullptr;
    dget = const_cast<TrTupleSel*>(this)->GetInp(Func::EInp2);
    MDtGet<Sdata<string>>* diget = dget ? dget->GetDObj(diget) : nullptr;
    if (dfget && diget) {
	NTuple arg;
	dfget->DtGet(arg);
	Sdata<string> ind;
	diget->DtGet(ind);
	if (arg.mValid && ind.mValid) {
	    DtBase* elem = arg.GetElem(ind.mData);
	    res = MDtGetBase::MDtGetType(elem->GetTypeSig());
	} else {
	    Log(TLog(EErr, this) + "Invalid argument");
	}
    }
    return res;
}



///// To string

TrTostrVar::TrTostrVar(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(Func::EInp1));
}

void TrTostrVar::Init(const string& aIfaceName)
{
    if (mFunc) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp) {
	string t_inp = inp->VarGetIfid();
	if ((mFunc = FSToStr<int>::Create(this, aIfaceName, t_inp)));
	else if ((mFunc = FUriToStr::Create(this, aIfaceName, t_inp)));
	else {
	    Logger()->Write(EErr, this, "Failed init function");
	}
    }
}

string TrTostrVar::GetInpUri(int aId) const
{
    if (aId == Func::EInp1) return "Inp";
    else return string();
}


// Transition "Inputs counter"

const string TrInpCnt::K_InpInp = "Inp";

TrInpCnt::TrInpCnt(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(K_InpInp);
}

MIface* TrInpCnt::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

MIface* TrInpCnt::DoGetDObj(const char *aName)
{
    return checkLif<MDtGet<Sdata<int>>>(aName);
}

string TrInpCnt::VarGetIfid() const
{
    return MDtGet<Sdata<int>>::Type();
}

void TrInpCnt::DtGet(Sdata<int>& aData)
{
    MNode* inpn = getNode(K_InpInp);
    if (inpn) {
	MUnit* inpu = inpn->lIf(inpu);
	MIfProv* ifp = inpu ? inpu->defaultIfProv(MDVarGet::Type()) : nullptr;
	MIfProv::TIfaces* ifaces = ifp ? ifp->ifaces() : nullptr;
	if (ifaces) {
	    aData.mData = ifaces->size();
	    aData.mValid = true;
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(EInpInp) + "]");
    }
    mRes = aData;
}

string TrInpCnt::GetInpUri(int aId) const
{
    if (aId == EInpInp) return K_InpInp;
    else assert(false);
}



// Transition "Input selector"

const string TrInpSel::K_InpInp = "Inp";
const string TrInpSel::K_InpIdx = "Idx";

TrInpSel::TrInpSel(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(K_InpInp);
    AddInput(K_InpIdx);
}

MIface* TrInpSel::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

MIface* TrInpSel::DoGetDObj(const char *aName)
{
    MDVarGet* inp = GetInp();
    MIface* ifr = inp ? inp->DoGetDObj(aName) : nullptr;
    return ifr;
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
    MDVarGet* inp = nullptr;
    int idx = -1;
    bool res = GetInpSdata(EInpIdx, idx);
    if (res) {
	MNode* inpn = getNode(K_InpInp);
	if (inpn) {
	    MUnit* inpu = inpn->lIf(inpu);
	    MIfProv* ifp = inpu ? inpu->defaultIfProv(MDVarGet::Type()) : nullptr;
	    MIfProv::TIfaces* ifaces = ifp ? ifp->ifaces() : nullptr;
	    if (ifaces) {
		if (idx < 0 || idx >= ifaces->size()) {
		    Log(TLog(EErr, this) + "Incorrect index  [" + to_string(idx) + "], inps num: " + to_string(ifaces->size()));
		} else {
		    inp = static_cast<MDVarGet*>(ifaces->at(idx));
		}
	    }
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(EInpInp) + "]");
	}
    }
    return inp;
}

string TrInpSel::GetInpUri(int aId) const
{
    if (aId == EInpInp) return K_InpInp;
    else if (aId == EInpIdx) return K_InpIdx;
    else assert(false);
}








// Agent functions "Mut composer" base


TrMut::TrMut(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
}

MIface* TrMut::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

MIface* TrMut::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<DMut>::Type()) == 0) {
	res = dynamic_cast<MDtGet<DMut>*>(this);
    }
    return res;
}

string TrMut::VarGetIfid() const
{
    return MDtGet<DMut>::Type();
}



// Agent function "Mut Node composer"

TrMutNode::TrMutNode(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInpParent));
    AddInput(GetInpUri(EInpName));
}

string TrMutNode::GetInpUri(int aId) const
{
    if (aId == EInpParent) return "Parent";
    else if (aId == EInpName) return "Name";
    else return string();
}

void TrMutNode::DtGet(DMut& aData)
{
    bool res = false;
    string name;
    res = GetInpSdata(EInpName, name);
    if (res) {
	string parent;
	res = GetInpSdata(EInpParent, parent);
	if (res) {
	    aData.mData = TMut(ENt_Node, ENa_Parent, parent, ENa_Id, name);
	    aData.mValid = true;
	} else {
	    aData.mValid = false;
	}
    } else {
	aData.mValid = false;
    }
    mRes = aData;
}


// Agent function "Mut Connect composer"

TrMutConn::TrMutConn(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInpCp1));
    AddInput(GetInpUri(EInpCp2));
}

string TrMutConn::GetInpUri(int aId) const
{
    if (aId == EInpCp1) return "Cp1";
    else if (aId == EInpCp2) return "Cp2";
    else return string();
}

void TrMutConn::DtGet(DMut& aData)
{
    bool res = false;
    string cp1;
    res = GetInpSdata(EInpCp1, cp1);
    if (res) {
	string cp2;
	res = GetInpSdata(EInpCp2, cp2);
	if (res) {
	    aData.mData = TMut(ENt_Conn, ENa_P, cp1, ENa_Q, cp2);
	    aData.mValid = true;
	} else {
	    aData.mValid = false;
	}
    } else {
	aData.mValid = false;
    }
    mRes = aData;
}


// Agent function "Mut Content composer"

TrMutCont::TrMutCont(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInpName));
    AddInput(GetInpUri(EInpValue));
    AddInput(GetInpUri(EInpTarget));
}

string TrMutCont::GetInpUri(int aId) const
{
    if (aId == EInpName) return "Name";
    else if (aId == EInpValue) return "Value";
    else if (aId == EInpTarget) return "Target";
    else return string();
}

void TrMutCont::DtGet(DMut& aData)
{
    bool res = false;
    string name;
    res = GetInpSdata(EInpName, name);
    if (res) {
	string value;
	res = GetInpSdata(EInpValue, value);
	if (res) {
	    string targ;
	    res = GetInpSdata(EInpTarget, targ);
	    if (res) {
		aData.mData = TMut(ENt_Cont, ENa_Targ, targ, ENa_Id, name, ENa_MutVal, value);
	    } else {
		aData.mData = TMut(ENt_Cont, ENa_Id, name, ENa_MutVal, value);
	    }
	    aData.mValid = true;
	} else {
	    aData.mValid = false;
	}
    } else {
	aData.mValid = false;
    }
    mRes = aData;
}



// Agent function "Mut Disconnect composer"

TrMutDisconn::TrMutDisconn(const string& aType, const string& aName, MEnv* aEnv): TrMut(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInpCp1));
    AddInput(GetInpUri(EInpCp2));
}

string TrMutDisconn::GetInpUri(int aId) const
{
    if (aId == EInpCp1) return "Cp1";
    else if (aId == EInpCp2) return "Cp2";
    else return string();
}

void TrMutDisconn::DtGet(DMut& aData)
{
    bool res = false;
    string cp1;
    res = GetInpSdata(EInpCp1, cp1);
    if (res) {
	string cp2;
	res = GetInpSdata(EInpCp2, cp2);
	if (res) {
	    aData.mData = TMut(ENt_Disconn, ENa_P, cp1, ENa_Q, cp2);
	    aData.mValid = true;
	} else {
	    aData.mValid = false;
	}
    } else {
	aData.mValid = false;
    }
    mRes = aData;
}




// Agent function "Chromo2 composer"

TrChr::TrChr(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInpBase));
    AddInput(GetInpUri(EInpMut));
}

MIface* TrChr::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

string TrChr::GetInpUri(int aId) const
{
    if (aId == EInpBase) return "Base";
    else if (aId == EInpMut) return "Mut";
    else return string();
}

MIface* TrChr::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<DChr2>::Type()) == 0) {
	res = dynamic_cast<MDtGet<DChr2>*>(this);
    }
    return res;
}

string TrChr::VarGetIfid() const
{
    return MDtGet<DChr2>::Type();
}

void TrChr::DtGet(DChr2& aData)
{
    // Base
    GetInpData(EInpBase, aData);
    // Mut
    MNode* inp = getNode(GetInpUri(EInpMut));
    if (inp) {
	MUnit* vgetu = inp->lIf(vgetu);
	auto* ifcs = vgetu ? vgetu->getIfs<MDVarGet>() : nullptr;
	if (ifcs) {
	    aData.mValid = true;
	    for (MIface* iface : *ifcs) {
		MDVarGet* vget = dynamic_cast<MDVarGet*>(iface);
		if (vget) {
		    MDtGet<DMut>* gsd = vget->GetDObj(gsd);
		    if (gsd) {
			DMut mut;
			gsd->DtGet(mut);
			if (mut.mValid) {
			    aData.mData.Root().AddChild(mut.mData);
			} else {
			    aData.mValid = false;
			    break;
			}
		    } else {
			aData.mValid = false;
			break;
		    }
		} else {
		    aData.mValid = false;
		    break;
		}
	    }
	} else {
	    aData.mValid = false;
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(EInpMut) + "]");
	aData.mValid = false;
    }
    mRes = aData;
}



// Agent function "Chromo2 composer form chromo"

TrChrc::TrChrc(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInp));
}

MIface* TrChrc::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

string TrChrc::GetInpUri(int aId) const
{
    if (aId == EInp) return "Inp";
    else return string();
}

MIface* TrChrc::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<DChr2>::Type()) == 0) {
	res = dynamic_cast<MDtGet<DChr2>*>(this);
    }
    return res;
}

string TrChrc::VarGetIfid() const
{
    return MDtGet<DChr2>::Type();
}

void TrChrc::DtGet(DChr2& aData)
{
    MNode* inp = getNode(GetInpUri(EInp));
    if (inp) {
	MUnit* vgetu = inp->lIf(vgetu);
	auto* ifcs = vgetu ? vgetu->getIfs<MDVarGet>() : nullptr;
	if (ifcs) {
	    aData.mValid = true;
	    for (MIface* iface : *ifcs) {
		MDVarGet* vget = dynamic_cast<MDVarGet*>(iface);
		if (vget) {
		    MDtGet<DChr2>* gsd = vget->GetDObj(gsd);
		    if (gsd) {
			DChr2 item;
			gsd->DtGet(item);
			if (item.mValid) {
			    aData.mData.Root().AddChild(item.mData.Root());
			} else {
			    aData.mValid = false;
			    break;
			}
		    } else {
			aData.mValid = false;
			break;
		    }
		} else {
		    aData.mValid = false;
		    break;
		}
	    }
	} else {
	    aData.mValid = false;
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(EInp) + "]");
	aData.mValid = false;
    }
    mRes = aData;
}


#if 0 // Attemtp to created solution that doesn't use paremetrized data iface.
/// Agent function "Data is valid"

TrIsValid::TrIsValid(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInp));
}
string TrIsValid::GetInpUri(int aId) const
{
    if (aId == EInp) return "Inp";
    else return string();
}

void TrIsValid::DtGet(TSdata& aData)
{
    MNode* inp = getNode(GetInpUri(EInp));
    if (inp) {
	MUnit* vgetu = inp->lIf(vgetu);
	MDVarGet* vget = vgetu ? vgetu->getSif<MDVarGet>(vget) : nullptr;
	if (vget) {
	    string difid = vget->VarGetIfid();
	    if (!difid.empty()) {
		MIface* dget = vget->DoGetDObj(difid.c_str());
		//MDtBase* data = dget ? dget->
	    }
	}
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(EInp) + "]");
	aData.mValid = false;
    }
}
#endif 

///// TrIsValid

TrIsValid::TrIsValid(const string &aType, const string& aName, MEnv* aEnv): TrVar(aType, aName, aEnv)
{
    AddInput(GetInpUri(Func::EInp1));
}

void TrIsValid::Init(const string& aIfaceName)
{
    TrVar::Init(aIfaceName);
    MDVarGet* inp1 = GetInp(Func::EInp1);
    if (aIfaceName == TOGetData::Type() && inp1) {
	string t1 = inp1->VarGetIfid();
	if (mFunc = FIsValid<DGuri>::Create(this, t1));
	else if (mFunc = FIsValid<Sdata<string>>::Create(this, t1));
	else if (mFunc = FIsValid<Sdata<int>>::Create(this, t1));
	else {
	    Log(TLog(EErr, this) + "Failed init, input [" + t1 + "]");
	}
    }
}

string TrIsValid::GetInpUri(int aId) const 
{
    if (aId == Func::EInp1) return "Inp";
    else return string();
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

TrHash::TrHash(const string& aType, const string& aName, MEnv* aEnv): TrBase(aType, aName, aEnv)
{
    AddInput(GetInpUri(EInp));
}

MIface* TrHash::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = TrBase::MNode_getLif(aType);
    return res;
}

string TrHash::GetInpUri(int aId) const
{
    if (aId == EInp) return "Inp";
    else return string();
}

MIface* TrHash::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<Sdata<int>>::Type()) == 0) {
	res = dynamic_cast<MDtGet<Sdata<int>>*>(this);
    }
    return res;
}

string TrHash::VarGetIfid() const
{
    return MDtGet<Sdata<int>>::Type();
}

void TrHash::DtGet(Sdata<int>& aData)
{
    MNode* inp = getNode(GetInpUri(EInp));
    if (inp) {
	MUnit* inpu = inp->lIf(inpu);
	MIfProv* ifp = inpu ? inpu->defaultIfProv(MDVarGet::Type()) : nullptr;
	MIfProv::TIfaces* inps= ifp ? ifp->ifaces() : nullptr;
	int hash = 0;
	for (auto dgeti : *inps) {
	    MDVarGet* dget = dynamic_cast<MDVarGet*>(dgeti);
	    MDtGetBase* dgetb = dynamic_cast<MDtGetBase*>(dget->DoGetDObj(dget->VarGetIfid().c_str()));
	    MDtBase* data = dgetb->DtbDup();
	    hash += data->Hash();
	    delete data;
	}
	aData.mData = hash;
	aData.mValid = true;
    } else {
	Log(TLog(EDbg, this) + "Cannot get input  [" + GetInpUri(EInp) + "]");
    }

}


