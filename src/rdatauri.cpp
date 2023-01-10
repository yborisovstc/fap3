#include "rdatauri.h"


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

void DGuri::DataFromString(istringstream& aStream)
{
    mChanged = false;
    string str;
    aStream >> str;
    GUri uri(str);
    bool valid = uri.isValid();
    if (valid) {
	if (mData != uri) {
	    mData = uri;
	    mChanged = true;
	}
    }
    if (mValid != valid) { mValid = valid; mChanged = true; }
}

void DGuri::DataToString(ostringstream& aStream) const
{
    aStream << mData.operator string();
}


std::ostream& operator<<(std::ostream& aOs, const DGuri& aDuri) {
    ostringstream& os = dynamic_cast<ostringstream&>(aOs);
    aDuri.ToString(os);
    return aOs;
}

std::istream& operator>>(std::istream& aIs, DGuri& aDuri) {
    istringstream& is = dynamic_cast<istringstream&>(aIs);
    aDuri.FromString(is);
    return is;
}




