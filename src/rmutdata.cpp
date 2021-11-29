
#include "rmutdata.h"

const char* DMut::TypeSig() { return  "MUT";};

bool DMut::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

bool DMut::IsDataFit(const DMut& aData)
{
    bool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

bool DMut::DataFromString(istringstream& aStream, bool& aRes)
{
    bool changed = true;
    TMut mut(aStream.str());
    aRes = mut.IsValid();
    if (aRes) {
    // changed = mData != mut;
	mData = mut;
    } else {
	changed = false;
    }
    return changed;
}

void DMut::DataToString(stringstream& aStream) const
{
    aStream << mData.ToString();
}



const char* DChr2::TypeSig() { return  "CHR2";};

bool DChr2::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

bool DChr2::IsDataFit(const DChr2& aData)
{
    bool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

bool DChr2::DataFromString(istringstream& aStream, bool& aRes)
{
    bool changed = true;
    mData.SetFromSpec(aStream.str());
    aRes = !mData.IsError();
    if (aRes) {
    } else {
	changed = false;
    }
    return changed;
}

void DChr2::DataToString(stringstream& aStream) const
{
    string spec;
    DChr2* self = const_cast<DChr2*>(this);
    self->mData.GetSpec(spec);
    aStream << spec;
}

