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




