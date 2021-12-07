
#include <cstring>
#include "func.h"

static int KDbgLog_Value = 9;


///// FAddDt

template<class T> Func* FAddDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
	bool inpok = true;
	MIfProv::TIfaces* inps = aHost->GetInps(EInp, MDVarGet::Type(), false);
	for (auto dgeti : *inps) {
	    MDVarGet* dget = dynamic_cast<MDVarGet*>(dgeti);
	    MDtGet<T>* dfget = dget->lIf(dfget);
	    if (!dfget) { inpok = false; break; }
	}
	if (inpok) {
	    res = new FAddDt<T>(*aHost);
	}
    } else if (aString == MDtGet<T>::Type()) {
	    res = new FAddDt<T>(*aHost);
	}
    return res;
}

template<class T> MIface *FAddDt<T>::getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = dynamic_cast<MDtGet<T>*>(this);
    return res;
}

template<class T> void FAddDt<T>::DtGet(T& aData)
{
    bool res = true;
    MIfProv::TIfaces* inps = mHost.GetInps(EInp, MDVarGet::Type(), false);
    bool first = true;
    if (inps) for (auto dgeti : *inps) {
	MDVarGet* dget = dynamic_cast<MDVarGet*>(dgeti);
	MDtGet<T>* dfget = dget->GetDObj(dfget);
	if (dfget) {
	    T arg = aData;
	    dfget->DtGet(arg);
	    if (arg.mValid) {
		if (first) { aData = arg; first = false;
		} else { aData += arg; }
	    } else {
		mHost.log(EErr, "Incorrect argument [" + mHost.GetInpUri(EInp) + "]");
		res = false; break;
	    }
	} else {
	    mHost.log(EErr, "Incorrect argument [" + mHost.GetInpUri(EInp) + "]");
	    res = false; break;
	}
    }
    inps = mHost.GetInps(EInpN, MDVarGet::Type(), false);
    if (inps) for (auto dgeti : *inps) {
	MDVarGet* dget = dynamic_cast<MDVarGet*>(dgeti);
	MDtGet<T>* dfget = dget->GetDObj(dfget);
	if (dfget) {
	    T arg = aData;
	    dfget->DtGet(arg);
	    if (arg.mValid) {
		aData -= arg;
	    } else {
		mHost.log(EErr, "Incorrect argument [" + mHost.GetInpUri(EInp) + "]");
		res = false; break;
	    }
	} else {
	    mHost.log(EErr, "Incorrect argument [" + mHost.GetInpUri(EInp) + "]");
	    res = false; break;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FAddDt<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

template <class T> string FAddDt<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp) {
	res = MDtGet<T>::Type();
    }
    return res;
}


/// Boolean AND

Func* FBAndDt::Create(Host* aHost, const string& aString)
{
    return new FBAndDt(*aHost);
}

MIface* FBAndDt::getLif(const char *aName)
{
    MIface* res = NULL;
    if (res = checkLif<MDtGet<Sdata<bool>>>(aName));
    return res;
}

void FBAndDt::DtGet(Sdata<bool>& aData)
{
    bool res = true;

    MIfProv::TIfaces* inps = mHost.GetInps(EInp, MDVarGet::Type(), false);
    bool first = true;
    if (inps) for (auto dgeti : *inps) {
	MDVarGet* dget = dynamic_cast<MDVarGet*>(dgeti);
	MDtGet<Sdata<bool>>* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    Sdata<bool> arg = aData;
	    dfget->DtGet(arg);
	    if (arg.mValid) {
		if (first) {
		    aData = arg;
		    first = false;
		} else {
		    aData.mData = aData.mData && arg.mData;
		}
	    } else {
		mHost.log(EErr, "Incorrect argument [" + mHost.GetInpUri(EInp) + "]");
		res = false; break;
	    }
	} else {
	    mHost.log(EErr, "Incompatible argument [" + mHost.GetInpUri(EInp) + "]");
	    dfget = dget->GetDObj(dfget);
	    res = false; break;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
	if (mHost.IsLogLevel(KDbgLog_Value)) {
	    mHost.log(EErr, string("Result: ") + (res ? (aData.mData ? "true" : "false") : "err"));
	}
    }
}

string FBAndDt::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp) {
	res = MDtGet<Sdata<bool>>::Type();
    }
    return res;
}


void FBAndDt::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}



/// Converting to GUri

Func* FUri::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == MDtGet<TData>::Type() && aInpIid == MDtGet<TInpData>::Type()) {
	    res = new FUri(*aHost);
	}
    } else {
	// Weak negotiation - wrong case here
	//mHost.log(EErr, "Creating instance, wrong outp [" + aOutIid + "] or inp [" + aInpIid + "] types");
    }
    return res;
}




MIface* FUri::getLif(const char *aName)
{
    MIface* res = NULL;
    if (res = checkLif<MDtGet<DGuri>>(aName));
    return res;
}

void FUri::DtGet(TData& aData)
{
    bool res = true;
    MDVarGet* dget = mHost.GetInp(EInp);
    MDtGet<TInpData>* dfget = dget ? dget->lIf(dfget) : nullptr;
    if (dfget) {
	TInpData arg;
	dfget->DtGet(arg);
	if (arg.mValid) {
	    aData.mData = GUri(arg.mData);
	    aData.mValid = true;
	} else {
	    mHost.log(EErr, "Incorrect input data");
	    TInpData arg;
	    dfget->DtGet(arg);
	    res = false;
	}
    } else {
	mHost.log(EErr, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
	res = false;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

string FUri::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp) {
	res = MDtGet<TInpData>::Type();
    }
    return res;
}





// Max: Generic data

template<class T> Func* FMaxDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDtGet<T>::Type()) {
	res = new FMaxDt<T>(*aHost);
    }
    return res;
}

template<class T> MIface *FMaxDt<T>::getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = dynamic_cast<MDtGet<T>*>(this);
    return res;
}

template<class T> void FMaxDt<T>::DtGet(T& aData)
{
    bool res = true;
    MIfProv::TIfaces* inps = mHost.GetInps(EInp, MDVarGet::Type(), false);
    bool first = true;
    if (inps) for (auto dgeti : *inps) {
	MDVarGet* dget = dynamic_cast<MDVarGet*>(dgeti);
	MDtGet<T>* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    T arg = aData;
	    dfget->DtGet(arg);
	    if (arg.mValid) {
		if (first) {
		    aData = arg;
		    first = false;
		} else {
		    if (arg.mData > aData.mData) {
			aData = arg;
		    }
		}
	    } else {
		mHost.log(EErr, "Incorrect argument [" + mHost.GetInpUri(EInp) + "]");
		res = false; break;
	    }
	} else {
	    mHost.log(EErr, "Incompatible argument [" + mHost.GetInpUri(EInp) + "]");
	    res = false; break;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMaxDt<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

static void FMaxDtFact() {
    FMaxDt<Sdata<int>>::Create(NULL, "");
}



// Comparition  function

FCmpBase::~FCmpBase()
{
}

string FCmpBase::IfaceGetId() const
{ 
    return MDtGet<Sdata<bool> >::Type();
}

void FCmpBase::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

void FCmpBase::DtGet(Sdata<bool>& aData)
{
    aData.Set(true);
}

template <class T> Func* FCmp<T>::Create(Host* aHost, const string& aInp1Iid, const string& aInp2Iid, FCmpBase::TFType aFType)
{
    Func* res = NULL;
    if (aInp1Iid == MDtGet<T>::Type() && aInp2Iid == MDtGet<T>::Type()) {
	res = new FCmp<T>(*aHost, aFType);
    }
    return res;
}

template <class T> void FCmp<T>::DtGet(Sdata<bool>& aData)
{
    bool res = true;
    MDVarGet* av1 = mHost.GetInp(EInp1);
    MDVarGet* av2 = mHost.GetInp(EInp2);
    if (av1 != NULL && av2 != NULL) {
	MDtGet<T>* a1 = av1->GetDObj(a1);
	MDtGet<T>* a2 = av2->GetDObj(a2);
	if (a1 != NULL && a2 != NULL) {
	    T arg1, arg2;
	    a1->DtGet(arg1);
	    a2->DtGet(arg2);
	    if (arg1.mValid && arg2.mValid && arg1.IsCompatible(arg2)) {
		bool res;
		if (mFType == ELt) res = arg1 < arg2;
		else if (mFType == ELe) res = arg1 <= arg2;
		else if (mFType == EEq) res = arg1 == arg2;
		else if (mFType == ENeq) res = arg1 != arg2;
		else if (mFType == EGt) res = arg1 > arg2;
		else if (mFType == EGe) res = arg1 >= arg2;
		aData.Set(res);
	    }
	    else {
		res = false;
	    }
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	if (mHost.IsLogLevel(KDbgLog_Value)) {
	    mHost.log(EDbg, string("Result: ") +  (res ? (aData.mData ? "true" : "false") : "err"));
	}
	mHost.OnFuncContentChanged();
    }
}

MIface *FCmpBase::getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<Sdata<bool> >::Type()) == 0) res = (MDtGet<Sdata<bool> >*) this;
    return res;
}


////    FSwitchBool

Func* FSwitchBool::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (aInpIid == MDtGet<Sdata<bool> >::Type()) {
	res = new FSwitchBool(*aHost);
    }
    return res;
}

MIface *FSwitchBool::getLif(const char *aName)
{
    MIface* res = NULL;
    // There are two approach of commutation. The first one is to commutate MDVarGet ifase, i.e.
    // that switcher result is this iface of selected case. The second is that switcher 
    // implements MDVarGet by itselt and does commutation in this iface methods.
    // The first approach requires iface cache refresh any time the swithcher ctrl is changed.
    // This is not what the cache is intended to and makes overhead. So let's select approach#2 for now.
    if (strcmp(aName, MDVarGet::Type()) == 0) res = (MDVarGet*) this;
    // if (strcmp(aName, MDVarGet::Type()) == 0) res = GetCase();
    return res;
}

bool FSwitchBool::GetCtrl() const
{
    bool res = false;
    MDVarGet* iv = mHost.GetInp(EInp1);
    if (iv != NULL) {
	MDtGet<Sdata<bool> >* getif= iv->GetDObj(getif);
	if (getif != NULL) {
	    Sdata<bool> data;
	    getif->DtGet(data);
	    if (data.mValid) {
		res = data.mData;
	    }
	}
    }
    return res;
}

MDVarGet* FSwitchBool::GetCase() const
{
    MDVarGet* res = NULL;
    bool case_id = GetCtrl();
    res = mHost.GetInp(case_id ? 2 : 1);
    return res;
}

string FSwitchBool::VarGetIfid() const
{
    MDVarGet* cs = GetCase();
    return cs->VarGetIfid();
}

MIface* FSwitchBool::DoGetDObj(const char *aName)
{
    MDVarGet* cs = GetCase();
    return cs ? cs->DoGetDObj(aName) : nullptr;
}

string FSwitchBool::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp_Sel) {
	res = MDtGet<Sdata<bool> >::Type();
    } else {
	res = "<any>";
    }
    return res;
}

/// Boolean negation

Func* FBnegDt::Create(Host* aHost, const string& aString)
{
    return new FBnegDt(*aHost);
}

MIface* FBnegDt::getLif(const char *aName)
{
    MIface* res = NULL;
    if (res = checkLif<MDtGet<Sdata<bool>>>(aName));
    return res;
}

void FBnegDt::DtGet(Sdata<bool>& aData)
{
    bool res = true;
    MDVarGet* dget = mHost.GetInp(EInp1);
    if (dget != NULL) {
	MDtGet<Sdata<bool> >* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    Sdata<bool> arg;
	    dfget->DtGet(arg);
	    aData = !arg;
	}
    } else {
	dget = mHost.GetInp(EInp1);
	res = false;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

void FBnegDt::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}






// Just to keep templated methods in cpp
class Fhost: public Func::Host { };

void Init()
{
    Fhost* host = nullptr;
    FAddDt<Sdata<int>>::Create(host, "");
    FMaxDt<Sdata<int>>::Create(host, "");
    FCmp<Sdata<int> >::Create(host, "", "", FCmpBase::ELt);
    FCmp<Sdata<string> >::Create(host, "", "", FCmpBase::ELt);
    FCmp<Enum>::Create(host, "", "", FCmpBase::ELt);
    FCmp<DGuri>::Create(host, "", "", FCmpBase::ELt);
}


