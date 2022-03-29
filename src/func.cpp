
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
	    MDtGet<T>* dfget = dget->GetDObj(dfget);
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
	mHost.log(EDbg, string("Result: ") + (mRes.mValid ? to_string(aData.mData) : "err"));
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

template <class T>
void FAddDt<T>::MDtGet_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    auto self = const_cast<FAddDt<T>*>(this);
    T data;
    self->DtGet(data);
    string str;
    data.ToString(str);
    aOs << "Data: " << str << endl;
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
	mHost.log(EDbg, string("Result: ") + (res ? (aData.mData ? "true" : "false") : "err"));
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


/// Boolean Base

MIface* FBBase::getLif(const char *aName)
{
    MIface* res = NULL;
    if (res = checkLif<MDtGet<Sdata<bool>>>(aName));
    return res;
}

string FBBase::GetInpExpType(int aId) const
{
    return  MDtGet<Sdata<bool>>::Type();
}

void FBBase::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}




/// Boolean OR

Func* FBOrDt::Create(Host* aHost, const string& aString)
{
    return new FBOrDt(*aHost);
}

void FBOrDt::DtGet(Sdata<bool>& aData)
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
		    aData.mData = aData.mData || arg.mData;
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
	mHost.log(EDbg, string("Result: ") + (res ? (aData.mData ? "true" : "false") : "err"));
    }
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
    MDtGet<TInpData>* dfget = dget ? dget->GetDObj(dfget) : nullptr;
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


/// Append
//
template<class T>
Func* FApnd<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == MDtGet<TData>::Type() && aInpIid == MDtGet<TInpData>::Type()) {
	    res = new FApnd<T>(*aHost);
	}
    } else {
	// Weak negotiation - wrong case here
	//mHost.log(EErr, "Creating instance, wrong outp [" + aOutIid + "] or inp [" + aInpIid + "] types");
    }
    return res;
}

template<class T>
MIface* FApnd<T>::getLif(const char *aName)
{
    MIface* res = NULL;
    if (res = checkLif<MDtGet<TData>>(aName));
    return res;
}

template<class T>
void FApnd<T>::DtGet(TData& aData)
{
    bool res = true;
    // Inp1
    MDVarGet* dget1 = mHost.GetInp(EInp1);
    MDtGet<TInpData>* dfget1 = dget1 ? dget1->GetDObj(dfget1) : nullptr;
    if (dfget1) {
	TInpData arg1;
	dfget1->DtGet(arg1);
	if (arg1.mValid) {
	    aData.mData = arg1.mData;
	    aData.mValid = true;
	    // Inp2
	    MDVarGet* dget2 = mHost.GetInp(EInp2);
	    MDtGet<TInpData>* dfget2 = dget2 ? dget2->GetDObj(dfget2) : nullptr;
	    if (dfget2) {
		TInpData arg2;
		dfget2->DtGet(arg2);
		if (arg2.mValid) {
		    aData += arg2;
		    aData.mValid = true;
		} else {
		    mHost.log(EErr, "Incorrect Inp2 data");
		    res = false;
		}
	    } else {
		mHost.log(EErr, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
		res = false;
	    }
	} else {
	    mHost.log(EErr, "Incorrect Inp2 data");
	    res = false;
	}
    } else {
	mHost.log(EErr, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
	res = false;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T>
string FApnd<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1 || aId == EInp2) {
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
	mHost.log(EDbg, string("Result: ") +  (res ? (aData.mData ? "true" : "false") : "err"));
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


// Getting size of container: vector

template <class T> Func* FSizeVect<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == MDtGet<Sdata<int>>::Type() && aInpIid == MDtGet<Vector<T>>::Type()) {
	    res = new FSizeVect<T>(*aHost);
	}
    } else {
	// Weak negotiation - just base on input type
	if (aInpIid == MDtGet<Vector<T>>::Type()) {
	    res = new FSizeVect<T>(*aHost);
	}
    }
    return res;
}

template<class T> MIface *FSizeVect<T>::getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<TOutp>::Type()) == 0) res = (MDtGet<TOutp>*) this;
    return res;
}

template <class T> void FSizeVect<T>::DtGet(TOutp& aData)
{
    bool res = true;
    MDVarGet* dget = mHost.GetInp(EInp1);
    MDtGet<Vector<T>>* dfget = dget->GetDObj(dfget);
    if (dfget) {
	Vector<T> arg;
	dfget->DtGet(arg);
	if (arg.mValid) {
	    aData.mData = arg.Size();
	    aData.mValid = true;
	} else {
	    mHost.log(EErr, "Incorrect input data");
	    res = false;
	}
    } else {
	mHost.log(EErr, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
	res = false;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template <class T> string FSizeVect<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1) {
	res = MDtGet<Vector<T>>::Type();
    }
    return res;
}


// Getting component of container: vector
//
template <class T>
Func* FAtVect<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == MDtGet<Sdata<T>>::Type() && aInpIid == MDtGet<Vector<T>>::Type()) {
	    res = new FAtVect<T>(*aHost);
	}
    } else {
	// Weak negotiation - just base on input type
	if (aInpIid == MDtGet<Vector<T>>::Type()) {
	    res = new FAtVect<T>(*aHost);
	}
    }
    return res;
}

template<class T>
MIface *FAtVect<T>::getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<Sdata<T>>::Type()) == 0) res = (MDtGet<Sdata<T>>*) this;
    return res;
}

template <class T>
void FAtVect<T>::DtGet(Sdata<T>& aData)
{
    bool res = false;
    MDVarGet* dget = mHost.GetInp(EInp1);
    MDtGet<Vector<T>>* dfget = dget->GetDObj(dfget);
    dget = mHost.GetInp(EInp2);
    MDtGet<Sdata<int>>* diget = dget->GetDObj(diget);
    if (dfget && diget) {
	Vector<T> arg;
	dfget->DtGet(arg);
	Sdata<int> ind;
	diget->DtGet(ind);
	if (arg.mValid && ind.mValid ) {
	    if (ind.mData < arg.Size()) {
		aData.mValid = arg.GetElem(ind.mData, aData.mData);
		res = true;
	    } else {
		mHost.log(EErr, "Index is exceeded");
	    }
	} else {
	    mHost.log(EErr, "Incorrect argument");
	}
    } else if (!dfget) {
	mHost.log(EErr, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
    } else if (!diget) {
	mHost.log(EErr, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template <class T> string FAtVect<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1) {
	res = MDtGet<Vector<T>>::Type();
    } else if (aId == EInp2) {
	res = MDtGet<Sdata<int>>::Type();
    }
    return res;
}


/// Conversion to string base

MIface* FToStrBase::getLif(const char *aName)
{
    MIface* res = NULL;
    if (res = checkLif<MDtGet<Sdata<string>>>(aName));
    return res;
}

void FToStrBase::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}


/// Conversion to string

template <class T>
Func* FSToStr<T>::Create(Host* aHost, const string& aIface, const string& aInpTSig)
{
    Func* res = nullptr;
    if (aIface == MDtGet<Sdata<string>>::Type() && aInpTSig == MDtGet<Sdata<T>>::Type()) {
	res = new FSToStr<T>(*aHost);
    }
    return res;
}

template <class T>
void FSToStr<T>::DtGet(Sdata<string>& aData)
{
    bool res = true;
    MDVarGet* dget = mHost.GetInp(EInp1);
    if (dget) {
	MDtGet<Sdata<T>>* dfget = dget->GetDObj(dfget);
	if (dfget) {
	    Sdata<T> arg;
	    dfget->DtGet(arg);
	    arg.ToString(aData.mData, false);
	    aData.mValid = true;
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
    FSizeVect<string>::Create(host, string(), string());
    FAtVect<string>::Create(host, string(), string());
    FApnd<Sdata<string>>::Create(host, string(), string());
    FApnd<DGuri>::Create(host, string(), string());
    FSToStr<int>::Create(host, string(), string());
}


