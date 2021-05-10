
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



// Just to keep templated methods in cpp
class Fhost: public Func::Host { };

void Init()
{
    Fhost* host = nullptr;
    FAddDt<Sdata<int>>::Create(host, "");
    FMaxDt<Sdata<int>>::Create(host, "");
}


