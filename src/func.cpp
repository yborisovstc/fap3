
#include <cstring>
#include "func.h"

static int KDbgLog_Value = 9;


///// FAddDt v.2

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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp);
    bool first = true;
    if (mInpIc && mInpIc->size() > 0) {
	for (auto dget : *mInpIc) {
	    const T* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes.mData = arg->mData; first = false;
		} else { mRes.mData = mRes.mData + arg->mData; }
		resSet = true;
	    } else {
		mRes.mValid = false; break;
	    }
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
    }
    if (!mInpNIc) mInpNIc = mHost.GetInps(EInpN);
    if (mInpNIc) for (auto dget : *mInpNIc) {
	const T* arg = dget->DtGet(arg);
	if (arg && arg->mValid) {
	    if (resSet) {
		mRes.mData = -arg->mData;
	    } else {
		mRes.mData = mRes.mData - arg->mData;
	    }
	    resSet = true;
	} else {
	    mRes.mValid = false; break;
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInpN) + "]");
    }
    if (!resSet) {
	mRes.mValid = false;
    }
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp);
    bool first = true;
    if (mInpIc) for (auto dget : *mInpIc) {
	const T* arg = dget->DtGet(arg);
	if (arg && arg->mValid) {
	    if (first) { mRes = *arg; first = false;
	    } else { mRes.mData = mRes.mData * arg->mData; }
	} else {
	    mRes.mValid = false; break;
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
    }
    return &mRes;
}


///// FDivDt

template<class T> Func* FDivDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
    } else if (aString == T::TypeSig()) {
	res = new FDivDt<T>(*aHost);
    }
    return res;
}

template<class T> const DtBase* FDivDt<T>::FDtGet()
{
    mRes.mValid = false;
    const T* arg1 = GetInpData(mInpIc, EInp, arg1);
    if (arg1) {
	if (arg1->IsValid()) {
	    mRes.mData = arg1->mData;
	    const T* arg2 = GetInpData(mInp2Ic, EInp2, arg2);
	    if (arg2) {
		if (arg2->IsValid()) {
		    mRes.mData = arg1->mData / arg2->mData;
		    mRes.mValid = true;
		}
	    } else {
		mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
	    }
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
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
	//mHost.log(EErr, "Creating instance, wrong outp [" + aOutIid + "] or inp [" + aInpIid + "] types");
    }
    return res;
}

template<class T> const DtBase* FApnd<T>::FDtGet()
{
    mRes.mValid = false;
    const T* arg1 = GetInpData(mInp1Ic, EInp1, arg1);
    if (arg1) {
	if (arg1->IsValid()) {
	    mRes.mData = arg1->mData;
	    const T* arg2 = GetInpData(mInp2Ic, EInp2, arg2);
	    if (arg2) {
		if (arg2->IsValid()) {
		    mRes.mData += arg2->mData;
		    mRes.mValid = true;
		}
	    } else {
		mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
	    }
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
    }
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp);
    bool first = true;
    if (mInpIc && mInpIc->size() > 0) {
	for (auto dget : *mInpIc) {
	    const T* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes.mData = arg->mData; first = false;
		} else { if (arg->mData < mRes.mData)  mRes.mData = arg->mData; }
	    } else {
		mRes.mValid = false; break;
	    }
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp);
    bool first = true;
    if (mInpIc && mInpIc->size() > 0) {
	for (auto dget : *mInpIc) {
	    const T* arg = dget->DtGet(arg);
	    if (arg && arg->mValid) {
		if (first) { mRes.mData = arg->mData; first = false;
		} else { if (arg->mData > mRes.mData)  mRes.mData = arg->mData; }
	    } else {
		mRes.mValid = false; break;
	    }
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
    }
    return &mRes;
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp1);
    if (!mInp2Ic) mInp2Ic = mHost.GetInps(EInp2);
    auto* av1 = (mInpIc && mInpIc->size() == 1) ? mInpIc->at(0) : nullptr;
    auto* av2 = (mInp2Ic && mInp2Ic->size() == 1) ? mInp2Ic->at(0) : nullptr;
    if (av1 != NULL && av2 != NULL) {
	const T* arg1 = av1->DtGet(arg1);
	const T* arg2 = av2->DtGet(arg2);
	if (arg1->IsCompatible(*arg2)) {
	    if (arg1->mValid && arg2->mValid) {
		bool sres;
		if (mFType == ELt) sres = arg1 < arg2;
		else if (mFType == ELe) sres = *arg1 <= *arg2;
		else if (mFType == EEq) sres = *arg1 == *arg2;
		else if (mFType == ENeq) sres = *arg1 != *arg2;
		else if (mFType == EGt) sres = *arg1 > *arg2;
		else if (mFType == EGe) sres = *arg1 >= *arg2;
		mRes.mData = sres;
		mRes.mValid = true;
	    } else {
		// Enable comparing invalid data
		if (mFType == EEq) mRes.mData = (*arg1 == *arg2);
		else if (mFType == ENeq) mRes.mData = (*arg1 != *arg2);
	    }
	}
    }
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp1);
    auto* dfget = (mInpIc && mInpIc->size() == 1) ? mInpIc->at(0) : nullptr;
    if (dfget) {
	const Vector<T>* arg = dfget->DtGet(arg);
	if (arg->mValid) {
	    mRes.mData = arg->Size();
	    mRes.mValid = true;
	} else {
	    mHost.log(EDbg, "Invalid input data");
	}
    } else {
	mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp1);
    if (!mInp2Ic) mInp2Ic = mHost.GetInps(EInp2);
    auto* dfget = (mInpIc && mInpIc->size() == 1) ? mInpIc->at(0) : nullptr;
    auto* diget = (mInp2Ic && mInp2Ic->size() == 1) ? mInp2Ic->at(0) : nullptr;
    if (dfget && diget) {
	const Vector<T>* arg = dfget->DtGet(arg);
	const Sdata<int>* ind = diget->DtGet(ind);
	if (arg->mValid && ind->mValid ) {
	    if (ind->mData < arg->Size()) {
		mRes.mValid = arg->GetElem(ind->mData, mRes.mData);
	    } else {
		string inds = ind->ToString(false);
		mHost.log(EWarn, "Index is exceeded: " + inds);
	    }
	} else {
	    mHost.log(EDbg, "Invalid argument");
	}
    } else if (!dfget) {
	mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
    } else if (!diget) {
	mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp1);
    if (!mInp2Ic) mInp2Ic = mHost.GetInps(EInp2);
    auto* dfget = (mInpIc && mInpIc->size() == 1) ? mInpIc->at(0) : nullptr;
    auto* diget = (mInp2Ic && mInp2Ic->size() == 1) ? mInp2Ic->at(0) : nullptr;
    if (dfget && diget) {
	const Vector<T>* arg = dfget->DtGet(arg);
	const Sdata<int>* ind = diget->DtGet(ind);
	if (arg->mValid && ind->mValid ) {
	    if (ind->mData < arg->Size()) {
		mRes.mValid = arg->GetElem(ind->mData, mRes);
	    } else {
		string inds = ind->ToString(false);
		mHost.log(EWarn, "Index is exceeded: " + inds);
	    }
	} else {
	    mHost.log(EDbg, "Invalid argument");
	}
    } else if (!dfget) {
	mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
    } else if (!diget) {
	mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
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
    if (!mInpIc) mInpIc = mHost.GetInps(EInp1);
    if (!mInp2Ic) mInp2Ic = mHost.GetInps(EInp2);
    auto* dfget = (mInpIc && mInpIc->size() == 1) ? mInpIc->at(0) : nullptr;
    auto* diget = (mInp2Ic && mInp2Ic->size() == 1) ? mInp2Ic->at(0) : nullptr;
    if (dfget && diget) {
	const Pair<T>* arg = dfget->DtGet(arg);
	const Sdata<int>* ind = diget->DtGet(ind);
	if (arg->mValid && ind->mValid ) {
	    if (ind->mData == 0) {
		mRes = arg->mData.first;
		mRes.mValid = true;
	    } else if (ind->mData == 1) {
		mRes = arg->mData.second;
		mRes.mValid = true;
	    } else {
		string inds = ind->ToString(false);
		mHost.log(EWarn, "Index is exceeded: " + inds);
	    }
	} else {
	    mHost.log(EDbg, "Invalid argument");
	}
    } else if (!dfget) {
	mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
    } else if (!diget) {
	mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
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
    const TData* inp = GetInpData(mInpIc, EInp, inp);
    if (inp) {
	const TData* head = GetInpData(mInpHeadIc, EHead, head);
	if (head) {
	    if (inp->IsValid() && head->IsValid()) {
		bool res = inp->mData.getTail(head->mData, mRes.mData);
		mRes.mValid = res;
	    }
	} else {
	    mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EHead) + "]");
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
    }
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
    const TData* inp = GetInpData(mInpIc, EInp, inp);
    if (inp) {
	const TData* tail = GetInpData(mInpTailIc, ETail, tail);
	if (tail) {
	    if (inp->IsValid() && tail->IsValid()) {
		bool res = inp->mData.getHead(tail->mData, mRes.mData);
		mRes.mValid = res;
	    }
	} else {
	    mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(ETail) + "]");
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
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
    const TData* inp = GetInpData(mInpIc, EInp, inp);
    if (inp) {
	const TNum* num = GetInpData(mInpNumIc, ENum, num);
	if (num) {
	    if (inp->IsValid() && num->IsValid()) {
		mRes.mData = inp->mData.tailn(num->mData);
		mRes.mValid = mRes.mData.isValid();
	    }
	} else {
	    mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(ENum) + "]");
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp) + "]");
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
    const TInpData* inp1 = GetInpData(mInp1Ic, EInp1, inp1);
    if (inp1) {
	mRes.mData.first = *inp1;
	const TInpData* inp2 = GetInpData(mInp2Ic, EInp2, inp2);
	if (inp2) {
	    mRes.mData.second = *inp2;
	} else {
	    mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(EInp2) + "]");
	    mRes.mData.second.mValid = false;
	}
    } else {
	mHost.log(EWarn, "Cannot get input [" + mHost.GetInpUri(EInp1) + "]");
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


