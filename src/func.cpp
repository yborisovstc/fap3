
#include <cstring>
#include "func.h"

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
	//if (mHost.IsLogLevel(KDbgLog_Value)) {
	//    mHost.LogWrite(EDbg, "Result: %s", res ? (aData.mData ? "true" : "false") : "err");
	//}
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
}


