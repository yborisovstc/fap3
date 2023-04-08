
#include <cstring>
#include "func.h"
#include "prof_ids.h"

static int KDbgLog_Value = 9;

const MDVarGet* Func::GetInp(Host* aHost, int aInpId)
{
    const MDVarGet* res = nullptr;
    TInpIc* Ic = aHost->GetInps(aInpId);
    if (Ic) {
	res = (Ic->size() == 1) ? Ic->at(0) : nullptr;
	if (!res) {
	    aHost->log(EDbg, "Cannot get input [" + aHost->GetInpUri(aInpId) + "]");
	}
    }
    return res;
}

const MDVarGet* Func::GetInp(int aInpId)
{
    const MDVarGet* res = nullptr;
    TInpIc* Ic = mHost.GetInps(aInpId);
    if (Ic) {
	res = (Ic->size() == 1) ? Ic->at(0) : nullptr;
	if (!res) {
	    LOGF(EDbg, "Cannot get input [" + mHost.GetInpUri(aInpId) + "]");
	}
    }
    return res;
}

///// FAddDt

template<class T> Func* FAddDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
	bool inpok = true;
	auto* inps = aHost->GetInps(EInp);
	for (auto dget : *inps) {
	    inpok = dget->VarGetIfid() == T::TypeSig();
	    if (!inpok) { break; }
	}
	if (inpok) {
	    res = new FAddDt<T>(*aHost);
	}
    } else if (aString == T::TypeSig()) {
	    res = new FAddDt<T>(*aHost);
	}
    return res;
}

template<class T> const DtBase* FAddDt<T>::FDtGet()
{
    mRes.mValid = true;
    bool resSet = false;
    TInpIc* InpIc = mHost.GetInps(EInp);
    bool first = true;
    if (InpIc && InpIc->size() > 0) {
	for (auto dget : *InpIc) {
	    const T* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes.mData = arg->mData; first = false;
		} else { mRes.mData = mRes.mData + arg->mData; }
		resSet = true;
	    } else {
		mRes.mValid = false; break;
	    }
	}
    }
    TInpIc* InpNIc = mHost.GetInps(EInpN);
    if (InpNIc) for (auto dget : *InpNIc) {
	const T* arg = dget->DtGet(arg);
	if (arg && arg->mValid) {
	    if (resSet) {
		mRes.mData = mRes.mData - arg->mData;
	    } else {
		mRes.mData = -arg->mData;
	    }
	    resSet = true;
	} else {
	    mRes.mValid = false; break;
	}
    }
    if (!resSet) {
	mRes.mValid = false;
    }
    LOGF(EDbg, "Inp count: " + to_string(InpIc->size()) + ", InpN count: " + to_string(InpNIc->size()) + ", res [" + mRes.ToString(true) + "]");
    return &mRes;
}

template <class T> string FAddDt<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp) {
	res = T::TypeSig();
    }
    return res;
}


//// Addition, single connecting inputs (ref iss_014), generic data

template<class T> Func* FAddDt2<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
	bool inpok = true;
	auto* inpvg = GetInp(aHost, EInp1);
	auto* inp2vg = GetInp(aHost, EInp2);
	inpok = inpvg && inpvg->VarGetIfid() == T::TypeSig() && inp2vg && inp2vg->VarGetIfid() == T::TypeSig();
	if (inpok) {
	    res = new FAddDt2<T>(*aHost);
	}
    } else if (aString == T::TypeSig()) {
	    res = new FAddDt2<T>(*aHost);
	}
    return res;
}

template<class T> const DtBase* FAddDt2<T>::FDtGet()
{
    mRes.mValid = false;
    const T* arg1 = GetInpData(EInp1, arg1);
    const T* arg2 = GetInpData(EInp2, arg2);
    if (arg1 && arg2 && arg1->IsValid() && arg2->IsValid()) {
	mRes.mData = arg1->mData + arg2->mData;
	mRes.mValid = true;
    }
    LOGF(EDbg, "Inp [" + (arg1 ? arg1->ToString(true) : "nil") + "], Inp2 [" + (arg2 ? arg2->ToString(true) : "nil")  + "], res [" + mRes.ToString(true) + "]");
    return &mRes;
}

//// Subtraction, single connecting inputs (ref iss_014), generic data

template<class T> Func* FSubDt2<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
	bool inpok = true;
	auto* inpvg = GetInp(aHost, EInp1);
	auto* inp2vg = GetInp(aHost, EInp2);
	inpok = inpvg && inpvg->VarGetIfid() == T::TypeSig() && inp2vg && inp2vg->VarGetIfid() == T::TypeSig();
	if (inpok) {
	    res = new FSubDt2<T>(*aHost);
	}
    } else if (aString == T::TypeSig()) {
	    res = new FSubDt2<T>(*aHost);
	}
    return res;
}

template<class T> const DtBase* FSubDt2<T>::FDtGet()
{
    mRes.mValid = false;
    const T* arg1 = GetInpData(EInp1, arg1);
    const T* arg2 = GetInpData(EInp2, arg2);
    if (arg1 && arg2 && arg1->IsValid() && arg2->IsValid()) {
	mRes.mData = arg1->mData - arg2->mData;
	mRes.mValid = true;
    }
    LOGF(EDbg, "Inp [" + (arg1 ? arg1->ToString(true) : "nil") + "], Inp2 [" + (arg2 ? arg2->ToString(true) : "nil")  + "], res [" + mRes.ToString(true) + "]");
    return &mRes;
}



///// FMplDt

template<class T> Func* FMplDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
	bool inpok = true;
	auto* inps = aHost->GetInps(EInp);
	for (auto get : *inps) {
	    auto* idata = get->VDtGet(T::TypeSig());
	    if (!idata) { inpok = false; break; }
	}
	if (inpok) {
	    res = new FMplDt<T>(*aHost);
	}
    } else if (aString == T::TypeSig()) {
	    res = new FMplDt<T>(*aHost);
	}
    return res;
}

template<class T> const DtBase* FMplDt<T>::FDtGet()
{
    mRes.mValid = true;
    TInpIc* InpIc = mHost.GetInps(EInp);
    bool first = true;
    if (InpIc && InpIc->size() > 0) {
	for (auto dget : *InpIc) {
	    const T* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes = *arg; first = false;
		} else { mRes.mData = mRes.mData * arg->mData; }
	    } else {
		mRes.mValid = false; break;
	    }
	}
    } else {
	mRes.mValid = false;
    }
    return &mRes;
}


///// FDivDt

template<class T> Func* FDivDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
	bool inpok = true;
	auto* inpvg = GetInp(aHost, EInp);
	auto* inp2vg = GetInp(aHost, EInp2);
	inpok = inpvg->VarGetIfid() == T::TypeSig() && inp2vg->VarGetIfid() == T::TypeSig();
	if (inpok) {
	    res = new FDivDt<T>(*aHost);
	}

    } else if (aString == T::TypeSig()) {
	res = new FDivDt<T>(*aHost);
    }
    return res;
}

template<class T> const DtBase* FDivDt<T>::FDtGet()
{
    mRes.mValid = false;
    const T* arg1 = GetInpData(EInp, arg1);
    const T* arg2 = GetInpData(EInp2, arg2);
    if (arg1 && arg2 && arg1->IsValid() && arg2->IsValid()) {
	mRes.mData = arg1->mData / arg2->mData;
	mRes.mValid = true;
    }
    return &mRes;
}


/// Append
//
template<class T>
Func* FApnd<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == T::TypeSig() && aInpIid == T::TypeSig()) {
	    res = new FApnd<T>(*aHost);
	}
    } else if (!aInpIid.empty()) {
	// TODO Resolution basing only on input type. Is this acceptable?
	if (aInpIid == T::TypeSig()) {
	    res = new FApnd<T>(*aHost);
	}
    } else {
	// Weak negotiation - wrong case here
	//LOGF(EErr, "Creating instance, wrong outp [" + aOutIid + "] or inp [" + aInpIid + "] types");
    }
    return res;
}

template<class T> const DtBase* FApnd<T>::FDtGet()
{
    mRes.mValid = false;
    const T* arg1 = GetInpData(EInp1, arg1);
    const T* arg2 = GetInpData(EInp2, arg2);
    if (arg1 && arg1->IsValid() && arg2 && arg2->IsValid()) {
	mRes.mData = arg1->mData + arg2->mData;
	mRes.mValid = true;
    }
    LOGF(EDbg, "Inp1 [" + (arg1 ? arg1->ToString(true) : "nil") + "], Inp2 [" + (arg2 ? arg2->ToString(true) : "nil")  + "], res [" + mRes.ToString(true) + "]");
    return &mRes;
}

template<class T>
string FApnd<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1 || aId == EInp2) {
	res = T::TypeSig();
    }
    return res;
}



// Min: Generic data

template<class T> Func* FMinDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == T::TypeSig()) {
	res = new FMinDt<T>(*aHost);
    }
    return res;
}

template<class T> const DtBase* FMinDt<T>::FDtGet()
{
    mRes.mValid = true;
    TInpIc* InpIc = mHost.GetInps(EInp);
    bool first = true;
    if (InpIc && InpIc->size() > 0) {
	for (auto dget : *InpIc) {
	    const T* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes.mData = arg->mData; first = false;
		} else { if (arg->mData < mRes.mData)  mRes.mData = arg->mData; }
	    } else {
		mRes.mValid = false; break;
	    }
	}
    }
    return &mRes;
}

static void FMinDtFact() {
    FMinDt<Sdata<int>>::Create(NULL, "");
}





// Max: Generic data

template<class T> Func* FMaxDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == T::TypeSig()) {
	res = new FMaxDt<T>(*aHost);
    }
    return res;
}

template<class T> const DtBase* FMaxDt<T>::FDtGet()
{
    mRes.mValid = true;
    TInpIc* InpIc = mHost.GetInps(EInp);
    bool first = true;
    if (InpIc && InpIc->size() > 0) {
	for (auto dget : *InpIc) {
	    const T* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes.mData = arg->mData; first = false;
		} else { if (arg->mData > mRes.mData)  mRes.mData = arg->mData; }
	    } else {
		mRes.mValid = false; break;
	    }
	}
    }
    return &mRes;
}

static void FMaxDtFact() {
    FMaxDt<Sdata<int>>::Create(NULL, "");
}



// Comparition  function

string FCmpBase::IfaceGetId() const
{ 
    return Sdata<bool>::TypeSig();
}


template <class T> Func* FCmp<T>::Create(Host* aHost, const string& aInp1Iid, const string& aInp2Iid, FCmpBase::TFType aFType)
{
    Func* res = NULL;
    if (aInp1Iid == T::TypeSig() && aInp2Iid == T::TypeSig()) {
	res = new FCmp<T>(*aHost, aFType);
    }
    return res;
}

template <class T> const DtBase* FCmp<T>::FDtGet()
{
    mRes.mValid = false;
    auto* arg1d = GetInpData(EInp1);
    auto* arg2d = GetInpData(EInp2);
    const T* arg1 = arg1d ? reinterpret_cast<const T*>(arg1d) : nullptr;
    const T* arg2 = arg2d ? reinterpret_cast<const T*>(arg2d) : nullptr;
    if (arg1 && arg2) {
	if (arg1->IsCompatible(*arg2)) {
	    if (arg1->mValid && arg2->mValid) {
		bool sres;
		if (mFType == ELt) sres = *arg1 < *arg2;
		else if (mFType == ELe) sres = *arg1 <= *arg2;
		else if (mFType == EEq) sres = *arg1 == *arg2;
		else if (mFType == ENeq) sres = *arg1 != *arg2;
		else if (mFType == EGt) sres = *arg1 > *arg2;
		else if (mFType == EGe) sres = *arg1 >= *arg2;
		mRes.mData = sres;
		mRes.mValid = true;
	    } else {
		// Enable comparing invalid data
		if (mFType == EEq) { mRes.mData = (*arg1 == *arg2); mRes.mValid = true;}
		else if (mFType == ENeq) { mRes.mData = (*arg1 != *arg2); mRes.mValid = true;}
	    }
	}
    }
    LOGF(EDbg, "Inp [" + (arg1 ? arg1d->ToString(true) : "nil") + "], Inp2 [" + (arg2 ? arg2d->ToString(true) : "nil")  + "], res [" + mRes.ToString(true) + "]");
    return &mRes;
}



// Getting size of container: vector

template <class T> Func* FSizeVect<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == Sdata<int>::TypeSig() && aInpIid == Vector<T>::TypeSig()) {
	    res = new FSizeVect<T>(*aHost);
	}
    } else {
	// Weak negotiation - just base on input type
	if (aInpIid == Vector<T>::TypeSig()) {
	    res = new FSizeVect<T>(*aHost);
	}
    }
    return res;
}

template <class T> const DtBase* FSizeVect<T>::FDtGet()
{
    mRes.mValid = false;
    const Vector<T>* arg = GetInpData(EInp1, arg);
    if (arg && arg->IsValid()) {
	mRes.mData = arg->Size();
	mRes.mValid = true;
    }
    return &mRes;
}

template <class T> string FSizeVect<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1) {
	res = Vector<T>::TypeSig();
    }
    return res;
}


// Getting component of container: vector, wrapping comp by Sdata
//
template <class T>
Func* FAtVect<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == Sdata<T>::TypeSig() && aInpIid == Vector<T>::TypeSig()) {
	    res = new FAtVect<T>(*aHost);
	}
    } else {
	// Weak negotiation - just base on input type
	if (aInpIid == Vector<T>::TypeSig()) {
	    res = new FAtVect<T>(*aHost);
	}
    }
    return res;
}

template <class T> const DtBase* FAtVect<T>::FDtGet()
{
    mRes.mValid = false;
    const Vector<T>* inp = GetInpData(EInp1, inp);
    const Sdata<int>* ind = GetInpData(EInp2, ind);
    if (inp && ind && inp->IsValid() && ind->IsValid()) {
	if (ind->mData < inp->Size()) {
	    mRes.mValid = inp->GetElem(ind->mData, mRes.mData);
	} else {
	    LOGF(EWarn, "Index is exceeded: " + ind->ToString(false));
	}
    }
    return &mRes;
}

template <class T> string FAtVect<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1) {
	res = Vector<T>::TypeSig();
    } else if (aId == EInp2) {
	res = Sdata<int>::TypeSig();
    }
    return res;
}


// Getting component of container: vector, generic, NOT wrapping comp by Sdata
//
template <class T>
Func* FAtgVect<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == T::TypeSig() && aInpIid == Vector<T>::TypeSig()) {
	    res = new FAtgVect<T>(*aHost);
	}
    } else {
	// Weak negotiation - just base on input type
	if (aInpIid == Vector<T>::TypeSig()) {
	    res = new FAtgVect<T>(*aHost);
	}
    }
    return res;
}

template <class T> const DtBase* FAtgVect<T>::FDtGet()
{
    mRes.mValid = false;
    const Vector<T>* inp = GetInpData(EInp1, inp);
    const Sdata<int>* ind = GetInpData(EInp2, ind);
    if (inp && ind && inp->IsValid() && ind->IsValid()) {
	if (ind->mData < inp->Size()) {
	    mRes.mValid = inp->GetElem(ind->mData, mRes);
	} else {
	    LOGF(EWarn, "Index is exceeded: " + ind->ToString(false));
	}
    }
    return &mRes;
}

template <class T> string FAtgVect<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1) {
	res = Vector<T>::TypeSig();
    } else if (aId == EInp2) {
	res = Sdata<int>::TypeSig();
    }
    return res;
}

// Getting component of container: Pair, generic, NOT wrapping comp by Sdata
//
template <class T>
Func* FAtgPair<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == T::TypeSig() && aInpIid == Pair<T>::TypeSig()) {
	    res = new FAtgPair<T>(*aHost);
	}
    } else {
	// Weak negotiation - just base on input type
	if (aInpIid == Pair<T>::TypeSig()) {
	    res = new FAtgPair<T>(*aHost);
	}
    }
    return res;
}

template <class T> const DtBase* FAtgPair<T>::FDtGet()
{
    mRes.mValid = false;
    const Pair<T>* inp = GetInpData(EInp1, inp);
    const Sdata<int>* ind = GetInpData(EInp2, ind);
    if (inp && ind && inp->IsValid() && ind->IsValid()) {
	if (ind->mData == 0) {
	    mRes = inp->mData.first;
	} else if (ind->mData == 1) {
	    mRes = inp->mData.second;
	} else {
	    string inds = ind->ToString(false);
	    LOGF(EWarn, "Index is exceeded: " + inds);
	}
    }
    return &mRes;
}

template <class T> string FAtgPair<T>::GetInpExpType(int aId) const
{
    string res;
    if (aId == EInp1) {
	res = Pair<T>::TypeSig();
    } else if (aId == EInp2) {
	res = Sdata<int>::TypeSig();
    }
    return res;
}


/// Getting tail, URI

Func* FTailUri::Create(Host* aHost, const string& aOutId)
{
    Func* res = NULL;
    if (aOutId == TData::TypeSig()) {
	res = new FTailUri(*aHost);
    }
    return res;
}

const DtBase* FTailUri::FDtGet()
{
    mRes.mValid = false;
    const TData* inp = GetInpData(EInp, inp);
    const TData* head = GetInpData(EHead, head);
    if (inp && head && inp->IsValid() && head->IsValid()) {
	mRes.mValid = inp->mData.getTail(head->mData, mRes.mData);
    }
    LOGF(EDbg, "Inp [" + (inp ? inp->ToString(true) : "nil") + "], Head [" + (head ? head->ToString(true) : "nil")  + "], res [" + mRes.ToString(true) + "]");
    return &mRes;
}


/// Getting Head, URI

Func* FHeadUri::Create(Host* aHost, const string& aOutId)
{
    Func* res = NULL;
    if (aOutId == TData::TypeSig()) {
	res = new FHeadUri(*aHost);
    }
    return res;
}

const DtBase* FHeadUri::FDtGet()
{
    mRes.mValid = false;
    const TData* inp = GetInpData(EInp, inp);
    const TData* tail = GetInpData(ETail, tail);
    if (inp && tail && inp->IsValid() && tail->IsValid()) {
	mRes.mValid = inp->mData.getHead(tail->mData, mRes.mData);
    }
    return &mRes;
}


/// Getting tail as num of elems, URI

Func* FTailnUri::Create(Host* aHost, const string& aOutId)
{
    Func* res = NULL;
    if (aOutId == TData::TypeSig()) {
	res = new FTailnUri(*aHost);
    }
    return res;
}

const DtBase* FTailnUri::FDtGet()
{
    mRes.mValid = false;
    const TData* inp = GetInpData(EInp, inp);
    const TNum* num = GetInpData(ENum, num);
    if (inp && num && inp->IsValid() && num->IsValid()) {
	mRes.mData = inp->mData.tailn(num->mData);
	mRes.mValid = mRes.mData.isValid();
    }
    return &mRes;
}


/// Compose Pair
//
template<class T>
Func* FPair<T>::Create(Host* aHost, const string& aOutIid, const string& aInpId)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == TData::TypeSig()) {
	    res = new FPair<T>(*aHost);
	}
    } else if (!aInpId.empty()) {
	if (aInpId == TInpData::TypeSig()) {
	    res = new FPair<T>(*aHost);
	}
    }
    return res;
}

template<class T> const DtBase* FPair<T>::FDtGet()
{
    mRes.mValid = true;
    const TInpData* inp1 = GetInpData(EInp1, inp1);
    if (inp1) {
	mRes.mData.first = *inp1;
	const TInpData* inp2 = GetInpData(EInp2, inp2);
	if (inp2) {
	    mRes.mData.second = *inp2;
	} else {
	    mRes.mData.second.mValid = false;
	}
    } else {
	mRes.mData.first.mValid = false;
    }
    return &mRes;
}




// Just to keep templated methods in cpp
class Fhost: public Func::Host { };

void Init()
{
    Fhost* host = nullptr;
    FAddDt<Sdata<int>>::Create(host, "");
    FAddDt2<Sdata<int>>::Create(host, "");
    FSubDt2<Sdata<int>>::Create(host, "");
    FMplDt<Sdata<int>>::Create(host, "");
    FDivDt<Sdata<int>>::Create(host, "");
    FMaxDt<Sdata<int>>::Create(host, "");
    FCmp<Sdata<int> >::Create(host, "", "", FCmpBase::ELt);
    FCmp<Sdata<string> >::Create(host, "", "", FCmpBase::ELt);
    FCmp<Enum>::Create(host, "", "", FCmpBase::ELt);
    FCmp<DGuri>::Create(host, "", "", FCmpBase::ELt);
    FSizeVect<string>::Create(host, string(), string());
    FSizeVect<Pair<DGuri>>::Create(host, string(), string());
    FSizeVect<DGuri>::Create(host, string(), string());
    FAtVect<string>::Create(host, string(), string());
    FAtgVect<DGuri>::Create(host, string(), string());
    FAtgVect<Pair<DGuri>>::Create(host, string(), string());
    FAtgPair<DGuri>::Create(host, string(), string());
    FAtgPair<Sdata<int>>::Create(host, string(), string());
    FApnd<Sdata<string>>::Create(host, string(), string());
    FApnd<DGuri>::Create(host, string(), string());
    FPair<DGuri>::Create(host, "", "");
    FPair<Sdata<int>>::Create(host, "", "");
}


