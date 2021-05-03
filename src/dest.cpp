
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




