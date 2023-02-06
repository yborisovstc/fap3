
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

void DMut::DataFromString(istringstream& aStream)
{
    mChanged = false;
    TMut mut(aStream.str());
    bool valid = mut.IsValid();
    if (valid) {
	mData = mut;
    }
    if (mValid) { mChanged = true; mValid = false;}
}

void DMut::DataToString(ostringstream& aStream) const
{
    aStream << mData.ToString();
}



const char* DChr2::TypeSig() { return  "CHR2";};

DChr2::DChr2(): DtBase()
{
};

bool DChr2::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

bool DChr2::IsDataFit(const DChr2& aData)
{
    bool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

void DChr2::DataFromString(istringstream& aStream)
{
    bool valid = false;
    mData.Reset();
    string str;
    if (RdpUtil::sstring(aStream, str)) {
	mData.SetFromSpec(str);
	valid = !mData.IsError();
    }
    if (mValid != valid) { mValid = valid; mChanged = true; }
}

void DChr2::DataToString(ostringstream& aStream) const
{
    string spec;
    DChr2* self = const_cast<DChr2*>(this);
    self->mData.GetSpec(spec);
    aStream << spec;
}

DtBase& DChr2::operator=(const DtBase& b)
{
    if (IsCompatible(b) && b.IsValid()) {
	this->DtBase::operator=(b);
	const DChr2& bp = reinterpret_cast<const DChr2&>(b);
	mData = bp.mData;
    } else {
	mValid = false;
    }
    return *this;
}



#if 0

const char* DMutSeg::TypeSig() { return  "MSEG";};

bool DMutSeg::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

bool DMutSeg::IsDataFit(const DMutSeg& aData)
{
    bool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

bool DMutSeg::DataFromString(istringstream& aStream, bool& aRes)
{
    // Not supported 
    aRes = false;
    return false;
}

void DMutSeg::DataToString(stringstream& aStream) const
{
    for (auto item : mData) {
	aStream << item.ToString();
	aStream << endl;
    }
}

#endif


