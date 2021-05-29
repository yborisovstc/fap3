
#include "dest.h"
#include "syst.h"


TrBase::TrBase(const string& aName, MEnv* aEnv): CpStateOutp(aName, aEnv)
{
    if (aName.empty()) mName = Type();
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




///// TrVar

TrVar::TrVar(const string &aName, MEnv* aEnv): TrBase(aName, aEnv), mFunc(nullptr)
{
    if (aName.empty()) mName = Type();
}

string TrVar::VarGetIfid() const
{
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
	Log(TLog(EErr, this) + "Cannot get input  [" + GetInpUri(aId) + "]");
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

void TrVar::log(TLogRecCtg aCtg, const string& aMsg)
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






///// TrAddVar

TrAddVar::TrAddVar(const string& aName, MEnv* aEnv): TrVar(aName, aEnv)
{
    if (aName.empty()) mName = Type();
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


///// TrMaxVar

TrMaxVar::TrMaxVar(const string& aName, MEnv* aEnv): TrVar(aName, aEnv)
{
    if (aName.empty()) mName = Type();
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

TrCmpVar::TrCmpVar(const string& aName, MEnv* aEnv): TrVar(aName, aEnv)
{
    if (aName.empty()) mName = Type();
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
	//else if (mFunc = FCmp<DGuri>::Create(this, t1, t2, ftype));
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


TrSwitchBool::TrSwitchBool(const string& aName, MEnv* aEnv): TrVar(aName, aEnv)
{
    if (aName.empty()) mName = Type();
    AddInput("Sel");
    AddInput("Inp1");
    AddInput("Inp2");
}

void TrSwitchBool::Init(const string& aIfaceName)
{
    TrVar::Init(aIfaceName);
    MDVarGet* inpCase = GetInp(FSwithcBase::EInp_Sel);
    MDVarGet* inp2 = GetInp(FSwithcBase::EInp_1);
    MDVarGet* inp3 = GetInp(FSwithcBase::EInp_1 + 1);
    if (inpCase && inp2 && inp3) {
	MDtGet<Sdata<bool>>* inpCaseD = inpCase->GetDObj(inpCaseD);
	void* inp2d = inp2->DoGetDObj(aIfaceName.c_str());
	void* inp3d = inp3->DoGetDObj(aIfaceName.c_str());
	if (inpCaseD && inp2d && inp3d) {
	    string t_case = inpCase->VarGetIfid();
	    mFunc = FSwitchBool::Create(this, aIfaceName, t_case);
	}
    } else {
	string inp = GetInpUri(FSwithcBase::EInp_Sel);
	if (!inp2) inp = GetInpUri(FSwithcBase::EInp_1);
	else if (!inp3) inp = GetInpUri(FSwithcBase::EInp_1 + 1);
	Logger()->Write(EErr, this, "Cannot get input [%s]", inp.c_str());
    }

}

string TrSwitchBool::GetInpUri(int aId) const 
{
    if (aId == FSwithcBase::EInp_Sel) return "Sel";
    else if (aId > FSwithcBase::EInp_Sel) {
	stringstream ss;
	ss <<  "Inp" << (aId - FSwithcBase::EInp_Sel);
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





