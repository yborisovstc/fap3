#include "rdatauri.h"


template<> const char* Vector<DGuri>::TypeSig() { return  "VDG";};
template<> const char* Pair<DGuri>::TypeSig() { return  "PU";};
template<> const char* Vector<Pair<DGuri>>::TypeSig() { return  "VPU";};

const char* DGuri::TypeSig() { return  "URI";};

bool DGuri::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

bool DGuri::IsDataFit(const DGuri& aData)
{
    bool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

bool DGuri::DataFromString(istringstream& aStream, bool& aRes)
{
    bool changed = true;
    GUri uri(aStream.str());
    aRes = uri.isValid();
    if (aRes) {
	if (mData != uri) {
	    mData = uri;
	}
    } else {
	changed = false;
    }
    return changed;
}

void DGuri::DataToString(stringstream& aStream) const
{
    aStream << mData.operator string();
}


std::ostream& operator<<(std::ostream& os, const DGuri& aDuri) {
    os << aDuri.mData.toString();
    return os;
}

std::istream& operator>>(std::istream& is, DGuri& aDuri) {
    string str; is >> str;
    aDuri.mData = GUri(str);
    aDuri.mValid = aDuri.mData.isValid();
    return is;
}




