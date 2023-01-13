

#include <stdlib.h>
#include <stdio.h>
#include "mvert.h"
#include "data.h"
#include "vert.h"
#include "guri.h"
#include "rmutdata.h"
#include "rdatauri.h"



// Row data signatures
//
// Composite data
template<> const char* Vector<string>::TypeSig() { return  "VS";};
template<> const char* Vector<Pair<string>>::TypeSig() { return  "VPS";};
template<> const char* Pair<string>::TypeSig() { return  "PS";};
template<> const char* Pair<Sdata<string>>::TypeSig() { return  "PSS";};
//template<> const char* Pair<GUri>::TypeSig() { return  "PU";};
template<> const char* Vector<DGuri>::TypeSig() { return  "VDU";};
template<> const char* Pair<DGuri>::TypeSig() { return  "PDU";};
template<> const char* Vector<Pair<DGuri>>::TypeSig() { return  "VPDU";};



// Matrix

template<> const char* MMtrGet<float>::Type() { return "MMtrGet_float";};

template<> const char* MMtrGet<float>::TypeSig() { return  Mtr<float>::TypeSig();};

template<class T>
MIface* HMtr<T>::HBase_getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MMtrGet<T>::Type()) == 0) res = (MMtrGet<T>*) this;
    return res;
}

template<class T> HBase* HMtr<T>::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && Mtr<T>::IsSrepFit(aString)) {
	res = new HMtr<T>(aHost, aString);
    }
    if (res == NULL && aInp != NULL) {
	MMtrGet<T>* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    Mtr<T> data;
	    dget->MtrGet(data);
	    if (Mtr<T>::IsDataFit(data)) {
		res = new HMtr<T>(aHost, data);
	    }
	}
    }
    return res;
}

template<class T> HMtr<T>::HMtr(DHost* aHost, const string& aCont): HBase(aHost) 
{
    mData.FromString(aCont);
}

template<class T> HMtr<T>::HMtr(DHost* aHost, const Mtr<T>& aData): HBase(aHost), mData(aData)
{
}

template<class T> bool HMtr<T>::FromString(const string& aString)
{
    return mData.FromString(aString);
}

template<class T> void HMtr<T>::ToString(string& aString)
{
    mData.ToString(aString);
}

template<class T> bool HMtr<T>::Set(MDVarGet* aInp)
{
    bool res = false;
    MMtrGet<T>* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	Mtr<T> data = mData;
	dget->MtrGet(data);
	if (mData != data) {
	    mData = data; res = true;
	}
    }
    return res;
}

template<class T> void HMtr<T>::MtrGet(Mtr<T>& aData)
{
    aData = mData;
}

// Generic data


template<class T> MIface* HDt<T>::HBase_getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = dynamic_cast<MDtGet<T>*>(this);
    else if (strcmp(aName, MDtSet<T>::Type()) == 0) {
	res = dynamic_cast<MDtSet<T>*>(this);
    }
    return res;
}

template<class T> HBase* HDt<T>::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && T::IsSrepFit(aString)) {
	res = new HDt<T>(aHost, aString);
    }
    if (res == NULL && aInp != NULL) {
	MDtGet<T>* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    T data;
	    dget->DtGet(data);
	    if (T::IsDataFit(data)) {
		res = new HDt<T>(aHost, data);
	    }
	}
    }
    return res;
}

template<class T> HDt<T>::HDt(DHost* aHost, const string& aCont): HBase(aHost) 
{
    istringstream is(aCont);
    mData.FromString(is);
}

template<class T> HDt<T>::HDt(DHost* aHost, const T& aData): HBase(aHost), mData(aData)
{
}

template<class T> bool HDt<T>::FromString(const string& aString)
{
    istringstream is(aString);
    mData.FromString(is);
    return mData.IsValid();
}

template<class T> void HDt<T>::ToString(string& aString)
{
    ostringstream oss;
    mData.ToString(oss);
    aString = oss.str();
}

template<class T> bool HDt<T>::Set(MDVarGet* aInp)
{
    bool res = false;
    MDtGet<T>* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	T data;
        data = mData;
	dget->DtGet(data);
	if (mData != data) {
	    mData = data;
	    mHost.HUpdateProp();
	    mHost.HNotifyUpdate();
	    res = true;
	}
    }
    else {
	res = mData.mValid != false;;
	mData.mValid = false;
    }
    return res;
}

template<class T> void HDt<T>::DtGet(T& aData)
{
    aData = mData;
}

template<class T> void HDt<T>::DtSet(const T& aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.HUpdateProp();
	mHost.HNotifyUpdate();
    }
}

// Variant base data

BdVar::BdVar(MBdVarHost *aHost): mData(NULL), mHost(aHost)
{
}

MIface* BdVar::MDVar_getLif(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDVar::Type()) == 0) {
	res = dynamic_cast<MDVar*>(this);
    } else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = dynamic_cast<MDVarGet*>(this);
    } else if (strcmp(aName, MDVarSet::Type()) == 0) {
	res = dynamic_cast<MDVarSet*>(this);
    }
    return res;
}

string BdVar::VarGetIfid() const
{
    return mData != NULL ? mData->IfaceGetId() : string();
}

string BdVar::VarGetSIfid()
{
    return mData != NULL ? mData->IfaceSGetId() : string();
}

MIface* BdVar::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
    if (mData == NULL) {
	Init(aName);
    }
    if (mData != NULL) {
	res = mData->HBase_getLif(aName);
    }
    return res;
}

MIface *BdVar::DoGetSDObj(const char *aName)
{
    MIface* res = NULL;
    if (mData != NULL) {
	res = mData->HBase_getLif(aName);
    }
    return res;
}


BdVar::~BdVar()
{
    if (mData != NULL) {
	delete mData;
    }
}

bool BdVar::Init(const string& aString, MDVarGet* aInpv)
{
    bool res = false;
    if (mData != NULL) {
	delete mData;
	mData = nullptr;
    }
    else if ((mData = HDt<Sdata <int> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <float> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <bool> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <string> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Mtr <int> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Mtr <float> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<NTuple>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Enum>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<DMut>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<DChr2>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Vector<string>>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Vector<DGuri>>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Pair<string>>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Pair<Sdata<string>>>::Create(this, aString, aInpv)) != NULL);
    //else if ((mData = HDt<Pair<GUri>>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Pair<DGuri>>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Vector<Pair<DGuri>>>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Vector<Pair<string>>>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<DGuri>::Create(this, aString, aInpv)) != NULL);
    /* Seems it is not required, Value init should be done on creation phase
       if (mData != NULL && !aString.empty()) {
       res = mData->FromString(aString);
       }
       */
    res = mData != NULL;
    return res;
}

bool BdVar::FromString(const string& aData) 
{
    bool res = false;
    if (mData == NULL) {
	res = Init(aData);
    } else {
	mData->FromString(aData);
	res = mData->IsValid();
	if (!mData->IsValid() && !mData->IsSigOK()) {
	    // Signature get's not fit, reinit
	    Init(aData);
	    res = true;
	}
    }
    if (res) {
	NotifyUpdate();
    }
    return res;
}

bool BdVar::ToString(string& aData) 
{
    bool res = false;
    if (mData == NULL) {
	res = Init(aData);
    }
    if (mData != NULL) {
	mData->ToString(aData);
    }
    return res;

}

bool BdVar::update()
{
    bool res = false;
    //string old_value; // Dbg
    //ToString(old_value); // Dbg
    MDVarGet* vget = mHost->HGetInp(this);
    if (vget) {
	if (!mData) {
	    res = Init(string(), vget);
	    if (res) {
		NotifyUpdate();
	    }
	}
	if (mData) {
	    res |= mData->Set(vget);
	}
    }
    return res;
}

void BdVar::NotifyUpdate()
{
    if (mHost != NULL) {
	mHost->HOnDataChanged(this);
    }
}

void BdVar::HUpdateProp()
{
}

void BdVar::HNotifyUpdate()
{
    NotifyUpdate();
}

string BdVar::GetValue()
{
    string res;
    ToString(res);
    return res;
}

bool BdVar::IsValid() const
{
    return mData ? mData->IsValid() : false;
}

bool BdVar::IsDsError() const
{
    return mData ? mData->IsDsErr() : false;
}

