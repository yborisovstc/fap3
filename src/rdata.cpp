
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "rdata.h"

const char KDataSep = ' ';
const char KSigParsToDataSep = ' ';
const char KSigToParsSep = ',';
const char KParsSep = ',';


const char MDtBase::mKTypeToDataSep = KSigParsToDataSep;

// Scalar data
template<> const char* Sdata<int>::TypeSig() { return  "SI";};
template<> const char* Sdata<float>::TypeSig() { return  "SF";};
template<> const char* Sdata<bool>::TypeSig() { return  "SB";};
template<> const char* Sdata<string>::TypeSig() { return  "SS";};
template<> const char* Sdata<Vector<string>>::TypeSig() { return  "SVS";};

template<> void Sdata<int>::InpFromString(istringstream& aStream, int& aRes) { aStream >> std::boolalpha >> aRes; }
template<> void Sdata<float>::InpFromString(istringstream& aStream, float& aRes) { aStream >> std::boolalpha >> aRes; }
template<> void Sdata<bool>::InpFromString(istringstream& aStream, bool& aRes) { aStream >> std::boolalpha >> aRes; }
template<> void Sdata<string>::InpFromString(istringstream& aStream, string& aRes) { aRes = aStream.str(); }


DtBase::~DtBase()
{
}


int DtBase::ParseSigPars(const string& aCont, string& aSig)
{
    size_t res = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(KSigParsToDataSep);
	res = sigp_e;
	string sigp = aCont.substr(beg, sigp_e);
	int sig_e = sigp.find_first_of(KParsSep);
	aSig = sigp.substr(0, sig_e);
    }
    return res;
}

bool DtBase::IsSrepFit(const string& aString, const string& aTypeSig)
{
    string sig;
    ParseSigPars(aString, sig);
    return  sig == aTypeSig;
}

bool DtBase::IsDataFit(const DtBase& aData, const string& aTypeSig)
{
    return  aData.mValid && aData.GetTypeSig() == aTypeSig;
}

void DtBase::ToString(string& aString, bool aSig) const
{
    stringstream ss;
    if (aSig) {
	ss << GetTypeSig() << " ";
    }
    if (!mValid) {
	ss << "<ERR>";
    }
    else {
	DataToString(ss);
    }
    aString = ss.str();
}

bool DtBase::FromString(const string& aString)
{
    bool res = true;
    bool changed = false;
    string sig;
    int end = ParseSigPars(aString, sig);
    if (sig == GetTypeSig()) {
	mSigTypeOK = true;
	string ss;
	if (end != string::npos) {
	    int beg = end + 1;
	    //end = aString.find(' ', beg);
	    //ss = aString.substr(beg, end - beg);
	    ss = aString.substr(beg);
	}
	if (!ss.empty()) {
	    istringstream sstr(ss);
	    changed |= DataFromString(sstr, res);
	}
	/* YB Let's enable default data
	else {
	    res = false;
	}
	*/
    }
    else {
	res = false;
	mSigTypeOK = false;
    }
    if (mValid != res) { mValid = res; changed = true; }
    return changed;
}

// Matrix, base

template<> const char* Mtr<int>::TypeSig() { return  "MI";};
template<> const char* Mtr<float>::TypeSig() { return  "MF";};


int MtrBase::Ind(int r, int c) const
{
    int res = -1;
    if (mType == EMt_Regular) res = mDim.second*r + c;
    else if (mType == EMt_Diagonal) res = (r == c) ? r : -1;
    return res;
}

int MtrBase::ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim)
{
    size_t res = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(' ');
	res = sigp_e;
	string sigp = aCont.substr(beg, sigp_e);
	int sig_e = sigp.find_first_of(',');
	aSig = sigp.substr(0, sig_e);
	if (!aSig.empty()) {
	    beg = sig_e + 1;
	    int par_e = sigp.find_first_of(',', beg);
	    string par = sigp.substr(beg, par_e - beg);
	    if (par.length() == 1) {
		enum {ENone, EVect, EStr} opt = ENone;
		char ts = par.at(0);
		switch (ts) {
		    case 'R': aType = MtrBase::EMt_Regular; break;
		    case 'V': aType = MtrBase::EMt_Regular; opt = EVect; break;
		    case 'S': aType = MtrBase::EMt_Regular; opt = EStr; break;
		    case 'D': aType = MtrBase::EMt_Diagonal; break;
		    case 'U': aType = MtrBase::EMt_Utriang; break;
		    case 'B': aType = MtrBase::EMt_Btriang; break;
		    default: ;
		}
		beg = par_e + 1;
		par_e = sigp.find_first_of(',', beg);
		par = sigp.substr(beg, par_e - beg);
		if (!par.empty()) {
		    int dp1 = 0, dp2 = 0;
		    istringstream sstr(par);
		    sstr >> dp1;
		    if (dp1 != 0) {
			if (par_e != string::npos) {
			    beg = par_e + 1;
			    par = sigp.substr(beg);
			    istringstream sstr(par);
			    sstr >> dp2;
			}
			aDim.first = dp1; aDim.second = dp2; 
			if (dp2 == 0) {
			    aDim.second = dp1;
			    if (opt == EVect) {
				aDim.first = dp1; aDim.second = 1;
			    }
			    else if (opt == EStr) {
				aDim.first = 1; aDim.second = dp1;
			    }
			}
			else if (opt == EVect | opt == EStr || aType == MtrBase::EMt_Diagonal) {
			    aDim.first = 0; aDim.second = 0; aType = MtrBase::EMt_Unknown;
			}
		    }
		}
	    }
	}
    }
    return res;
}

int MtrBase::IntSize() const
{
    int res = mDim.first * mDim.second;
    if (mType == EMt_Diagonal) {
	res = mDim.first;
    } 
    return res;
}

bool MtrBase::IsSrepFit(const string& aString, const string& aTypeSig)
{
    TMtrType mtype;
    TMtrDim mdim;
    string sig;
    ParseSigPars(aString, sig, mtype, mdim);
    return  sig == aTypeSig &&  mtype != EMt_Unknown && mdim.first != 0 && mdim.second != 0;
}

bool MtrBase::IsDataFit(const MtrBase& aData, const string& aTypeSig)
{
    return  aData.mValid && aData.GetTypeSig() == aTypeSig &&  aData.mType != EMt_Unknown && aData.mDim.first != 0 && aData.mDim.second != 0;
}

void MtrBase::ToString(string& aString, bool aSig) const
{
    stringstream ss;
    if (aSig) {
	ss << GetTypeSig() << " ";
    }
    char mt = 'R';
    switch (mType) {
	case MtrBase::EMt_Unknown: mt = '?'; break;
	case MtrBase::EMt_Diagonal: mt = 'D'; break;
	case MtrBase::EMt_Utriang: mt = 'U'; break;
	case MtrBase::EMt_Btriang: mt = 'B'; break;
    }
    enum {ENone, EVect, EStr} opt = ENone;
    if (mDim.first == 1) { mt = 'S'; opt = EStr;}
    else if (mDim.second == 1) { mt = 'V'; opt = EVect;};
    ss << ',' << mt;
    if (mType == MtrBase::EMt_Diagonal || opt == EVect) {
	ss << ',' << mDim.first;
    }
    else if (opt == EStr) {
	ss << ',' << mDim.second;
    }
    else {
	ss << ',' << mDim.first << ',' << mDim.second;
    }
    if (!mValid) {
	ss << " <ERROR>";
    }
    else {
	for (int cnt = 0; cnt < IntSize(); cnt++) {
	    ss << " ";
	    ElemToString(cnt, ss);
	}
    }
    aString = ss.str();
}

bool MtrBase::FromString(const string& aString)
{
    bool res = true;
    bool changed = false;
    string ss;
    TMtrType mtype;
    TMtrDim mdim;
    string sig;
    int beg = 0, end = 0;
    end = ParseSigPars(aString, sig, mtype, mdim);
    if (sig == GetTypeSig() && mtype != EMt_Unknown) {
	if (mType != mtype) { mType = mtype; changed = true; }
	if (mDim != mdim) { mDim = mdim; changed = true; }
	string ss;
	int cnt = 0;
	do {
	    beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	    if (!ss.empty()) {
		istringstream sstr(ss);
		changed |= ElemFromString(cnt++, sstr, res);
	    }
	} while (end != string::npos && res && cnt < IntSize());
	if (cnt != IntSize() || end != string::npos) {
	    res = false;
	}
    }
    else {
	res = false;
    }
    if (mValid != res) { mValid = res; changed = true; }
    return changed;
}

MtrBase& MtrBase::operator+=(const MtrBase& b)
{
    bool res = true;
    if (mDim == b.mDim) {
	for (int cntr = 0; cntr < mDim.first; cntr++) {
	    for (int cntc = 0; cntc < mDim.second; cntc++) {
		int cnt = mDim.second*cntr + cntc;
		AddElem(b, cntr, cntc);
	    }
	}
    }
    else {
	res = false;
    }
    if (mValid != res) { mValid = res; }
    return *this;
}

MtrBase& MtrBase::operator-=(const MtrBase& b)
{
    bool res = true;
    if (mDim == b.mDim) {
	for (int cntr = 0; cntr < mDim.first; cntr++) {
	    for (int cntc = 0; cntc < mDim.second; cntc++) {
		int cnt = mDim.second*cntr + cntc;
		SubElem(b, cntr, cntc);
	    }
	}
    }
    else {
	res = false;
    }
    if (mValid != res) { mValid = res; }
    return *this;
}


MtrBase& MtrBase::Mpl(const void* b)
{
    if (mValid) {
	if (mType == EMt_Diagonal) {
	    for (int r = 0; r < mDim.first; r++) {
		MplElem(r, r, b);
	    }
	} else {
	    for (int r = 0; r < mDim.first; r++) {
		for (int c = 0; c < mDim.second; c++) {
		    MplElem(r, c, b);
		}
	    }
	}
    }
    return *this;
}

MtrBase& MtrBase::Mpl(const MtrBase& a, const MtrBase& b) 
{
    if (a.mValid && b.mValid) {
	if (mType == EMt_Unknown) 
	{ 
	    if (a.mType == EMt_Diagonal && b.mType == EMt_Diagonal) {
		mType = EMt_Diagonal; 
	    }
	    else {
		mType = EMt_Regular;
	    }
	}
	if (mDim.first == 0) {
	    mDim.first = a.mDim.first;
	}
	if (mDim.second == 0) {
	    mDim.second = b.mDim.second;
	}
	SetIntSize(IntSize());
	if (a.mDim.second == b.mDim.first && a.mDim.first == mDim.first && b.mDim.second == mDim.second) {
	    mType = EMt_Regular;
	    if (a.mType == EMt_Diagonal) {
		mType = b.mType;
	    }
	    else if (b.mType == EMt_Diagonal) {
		mType = a.mType;
	    }
	    if (mType == EMt_Diagonal) {
		for (int r = 0; r < mDim.first; r++) {
		    MplElems(r, r, a, r, r, b, r, r);
		}
	    }
	    else {
		for (int r = 0; r < mDim.first; r++) {
		    for (int c = 0; c < mDim.second; c++) {
			if (a.mType == EMt_Diagonal) {
			    MplElems(r, c, a, r, r, b, r, c);
			}
			else if (b.mType == EMt_Diagonal) {
			    MplElems(r, c, a, r, c, b, c, c);
			}
			else {
			    MplRtoC(r, c, a, b);
			}
		    }
		}
		mValid = true;
	    }
	}
	else {
	    mValid = false;
	}
    }
    else {
	mValid = false;
    }
    return *this;
}

void MtrBase::SetMplncArg1Hint(const DtBase& res, const DtBase& arg2)
{
    if (res.mValid) {
	const MtrBase* mres = dynamic_cast<const MtrBase*>(&res);
	assert(mres != NULL);
	mDim.first = mres->mDim.first;
	mValid = true;
    }
    if (arg2.mValid) {
	const MtrBase* marg2 = dynamic_cast<const MtrBase*>(&arg2);
	assert(marg2 != NULL);
	mDim.second = marg2->mDim.second;
    }
}

void MtrBase::SetMplncArg2Hint(const DtBase& res, const DtBase& arg1)
{
    if (res.mValid) {
	const MtrBase* mres = dynamic_cast<const MtrBase*>(&res);
	assert(mres != NULL);
	mDim.second = mres->mDim.second;
	mValid = true;
    }
    if (arg1.mValid) {
	const MtrBase* marg1 = dynamic_cast<const MtrBase*>(&arg1);
	assert(marg1 != NULL);
	mDim.first = marg1->mDim.second;
    }
}

MtrBase& MtrBase::Invm(const MtrBase& a)
{
    if (a.mType == EMt_Diagonal) {
	// Only diag mtr is supported for now
	if ((mDim.first == 0 || mDim.first == a.mDim.first) && (mDim.second == 0 || mDim.second == a.mDim.second)) {
	    mDim = a.mDim;
	    mType = EMt_Diagonal;
	    SetIntSize(IntSize());
	    for (int r = 0; r < mDim.first; r++) {
		InvmElem(r, r, a, r, r);
	    }
	}
	else {
	    mValid = false;
	}
    }
    else {
	mValid = false;
    }
    return *this;
}


// Tuple, named components
const char* NTuple::TypeSig() { return  "TPL";};

NTuple::~NTuple()
{
    for (tComps::iterator it = mData.begin(); it != mData.end(); it++) {
	DtBase* elem = it->second;
	delete elem;
    }
    mData.clear();
}

bool NTuple::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

bool NTuple::IsDataFit(const NTuple& aData)
{
    return  aData.mValid && aData.GetTypeSig() == TypeSig();
}

int NTuple::ParseSigPars(const string& aCont, string& aSig, tCTypes& aCTypes)
{
    size_t res = string::npos;
    size_t end = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(' ');
	string sigp = aCont.substr(beg, sigp_e);
	res = sigp_e;
	size_t sig_e = sigp.find_first_of(',');
	aSig = sigp.substr(0, sig_e);
	if (!aSig.empty()) {
	    aCTypes.clear();
	    end = sig_e;
	    do {
		beg = end + 1;
		end = sigp.find(',', beg);
		string compsig = sigp.substr(beg, end - beg);
		size_t cs1 = compsig.find(':');
		if (cs1 != string::npos) {
		    string cs_type = compsig.substr(0, cs1);
		    string cs_name = compsig.substr(cs1 + 1);
		    aCTypes.push_back(tCompSig(cs_type, cs_name));
		}
		else {
		    break;
		}
	    } while (end != string::npos);
	}
    }
    return res;
}

bool NTuple::IsCTypesFit(const tCTypes& aCt) const
{
    bool res = true;
    if (mData.size() == aCt.size()) {
	for (int i = 0; i < mData.size(); i++) {
	    tComp cp = mData.at(i);
	    DtBase* comp = cp.second;
	    const tCompSig& cpsig = aCt.at(i);
	    if (cp.first != cpsig.second || comp->GetTypeSig() != cpsig.first) {
		res = false; break;
	    };
	}
    }
    else {
	res = false;
    }
    return res;
}

void NTuple::TypeParsToString(stringstream& aStream) const
{
    for (tComps::const_iterator it = mData.begin(); it != mData.end(); it++) {
	if (it != mData.begin()) {
	    string sep(1, KParsSep);
	    aStream << sep;
	}
	const tComp& comp = *it;
	aStream << comp.second->GetTypeSig() << ":" << comp.first;
    }
}

void NTuple::DataToString(stringstream& aStream) const
{
    for (tComps::const_iterator it = mData.begin(); it != mData.end(); it++) {
	if (it != mData.begin()) {
	    string sep(1, KDataSep);
	    aStream << sep;
	}
	const tComp& comp = *it;
	comp.second->DataToString(aStream);
    }
}
void NTuple::ToString(string& aString, bool aSig) const
{
    stringstream ss;
    if (aSig) {
	ss << GetTypeSig() << KSigToParsSep;
    }
    if (!mValid) {
	ss << " <ERROR>";
    }
    else {
	TypeParsToString(ss);
	ss << KSigParsToDataSep;
	DataToString(ss);
    }
    aString = ss.str();
}


bool NTuple::FromString(const string& aString)
{
    bool res = true;
    bool changed = false;
    string ss;
    string sig;
    tCTypes ctypes;
    int beg = 0, end = 0;
    end = ParseSigPars(aString, sig, ctypes);
    if (sig == GetTypeSig()) {
	if (!IsCTypesFit(ctypes)) { 
	    Init(ctypes);
	    changed = true; 
	}
	string ss;
	int cnt = 0;
	do {
	    beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	    if (!ss.empty()) {
		tComp& cp =  mData.at(cnt);
		DtBase* comp = cp.second;
		istringstream sstr(ss);
		changed |= comp->DataFromString(sstr, res);
	    }
	    cnt++;
	} while (end != string::npos && res && cnt < mData.size());
	if (cnt != mData.size() || end != string::npos) {
	    res = false;
	}
    }
    else {
	res = false;
    }
    if (mValid != res) { mValid = res; changed = true; }
    return changed;
}

void NTuple::Init(const tCTypes& aCt)
{
    mData.clear();
    for (tCTypes::const_iterator it = aCt.begin(); it != aCt.end(); it++) {
	const tCompSig& ctype = *it;
	const string& sr = ctype.first;
	const string& name = ctype.second;
	DtBase* comp = NULL;
	if ((comp = Sdata<int>::Construct(sr)) != NULL);
	else if ((comp = Sdata<float>::Construct(sr)) != NULL);
	else if ((comp = Sdata<bool>::Construct(sr)) != NULL);
	else if ((comp = Sdata<string>::Construct(sr)) != NULL);
	if (comp != NULL) {
	    mData.push_back(tComp(name, comp));
	}
    }
}

DtBase* NTuple::GetElem(const string& aName)
{
    DtBase* res = NULL;
    for (tComps::iterator it = mData.begin(); it != mData.end(); it++) {
	tComp& cp = *it;
	if (cp.first == aName) {
	    res = cp.second;
	    break;
	}
    }
    return res;
}

bool NTuple::operator==(const MDtBase& sb)
{ 
    const NTuple& b = dynamic_cast<const NTuple&>(sb);
    bool res = true;
    if (!&b || DtBase::operator!=(b) || mData.size() != b.mData.size()) {
	res = false;
    } else {
	for (int ind = 0; ind < mData.size() && res; ind++) {
	    auto acomp = mData.at(ind);
	    auto bcomp = b.mData.at(ind);
	    if (acomp.first != bcomp.first || !acomp.second  || !bcomp.second || *acomp.second != *bcomp.second) {
		res = false;
	    }
	}
    }
    return res;
};

NTuple& NTuple::operator=(const NTuple& b)
{
    if (&b == this) return *this;
    mValid = b.mValid;
    mSigTypeOK = b.mSigTypeOK;
    mData.clear();
    for (tComps::const_iterator it = b.mData.begin(); it != b.mData.end(); it++) {
	const tComp& bcomp = *it;
	const string& name = bcomp.first;
	DtBase* comp = bcomp.second->Clone();
	if (comp != NULL) {
	    mData.push_back(tComp(name, comp));
	}
    }
    return *this;
}



// Enumeration
const char* Enum::TypeSig() { return  "E";};

Enum::~Enum()
{
}

bool Enum::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

bool Enum::IsDataFit(const Enum& aData)
{
    bool res = aData.mValid && aData.GetTypeSig() == TypeSig();
    return res;
}

int Enum::ParseSigPars(const string& aCont, string& aSig, tSet& aSet)
{
    size_t res = string::npos;
    size_t end = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(KSigParsToDataSep);
	string sigp = aCont.substr(beg, sigp_e);
	res = sigp_e;
	size_t sig_e = sigp.find_first_of(KSigToParsSep);
	aSig = sigp.substr(0, sig_e);
	if (sig_e != string::npos) {
	    if (!aSig.empty()) {
		aSet.clear();
		end = sig_e;
		do {
		    beg = end + 1;
		    end = sigp.find(KParsSep, beg);
		    string compsig = sigp.substr(beg, end - beg);
		    aSet.push_back(compsig);
		} while (end != string::npos);
	    }
	} else {
	    // No pars found, take as error
	    res = string::npos;
	}
    }
    return res;
}

int Enum::ParseSig(const string& aCont, string& aSig)
{
    size_t res = string::npos;
    size_t end = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(KParsSep);
	aSig = aCont.substr(beg, sigp_e);
	res = sigp_e;
    }
    return res;
}

bool Enum::AreTypeParsFit(const tSet& aSet) const
{
    bool res = true;
    if (mSet.size() != aSet.size()) {
	res = false;
    }
    else {
	for (int cnt = 0; cnt < mSet.size() && res; cnt++) {
	    res = mSet.at(cnt) == aSet.at(cnt);
	}
    }
    return res;
}

void Enum::Init(const tSet& aSet)
{
    mSet.clear();
    mSet = aSet;
}

bool Enum::FromString(const string& aString)
{
    bool res = true;
    bool changed = false;
    string ss;
    string sig;
    int beg = 0, end = 0;
    tSet set;
    end = ParseSigPars(aString, sig, set);
    if (end != string::npos && sig == GetTypeSig()) {
	if (!AreTypeParsFit(set)) { 
	    Init(set);
	    changed = true; 
	}
	if (end != string::npos) {
	    int beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	}
	if (!ss.empty()) {
	    istringstream sstr(ss);
	    changed |= DataFromString(sstr, res);
	}
    }
    else {
	res = false;
    }
    if (mValid != res) { mValid = res; changed = true; }
    return changed;
}

bool Enum::DataFromString(istringstream& aStream, bool& aRes)
{
    bool changed = false;
    string sdata;
    aStream >> sdata;
    if (aRes = !aStream.fail()) {
	bool valid = false; 
	int data = -1;
	// Check if data belongs to the set
	for (int ind = 0; ind != mSet.size() && !valid; ind++) {
	    string& selem = mSet.at(ind);
	    if (sdata == selem) {
		valid = true; data = ind;
	    }
	}
	if (mValid != valid) { 
	    mValid = valid; changed = true; 
	}
	if (mValid && mData != data) { 
	    mData = data; changed = true; 
	}
    }
    return changed;
}

void Enum::TypeParsToString(stringstream& aStream) const
{
    for (tSet::const_iterator it = mSet.begin(); it != mSet.end(); it++) {
	if (it != mSet.begin()) {
	    string sep(1, KParsSep);
	    aStream << sep;
	}
	aStream << *it;
    }
}

void Enum::ToString(string& aString, bool aSig) const
{
    stringstream ss;
    if (aSig) {
	ss << GetTypeSig() << KSigToParsSep;
    }
    if (!mValid) {
	ss << " <ERROR>";
    }
    else {
	TypeParsToString(ss);
	ss << KSigParsToDataSep;
	DataToString(ss);
    }
    aString = ss.str();
}

void Enum::DataToString(stringstream& aStream) const 
{ 
    if (mData < 0 || mData >= mSet.size()) {
	aStream << "ERROR";
    } else {
	aStream << mSet.at(mData);
    }
}

bool Enum::IsCompatible(const DtBase& sb)
{
    const Enum& b = dynamic_cast<const Enum& >(sb); 
    bool res = b.mValid && b.GetTypeSig() == TypeSig() && mSet.size() == b.mSet.size();
    if (res) {
	for (int i = 0; i < mSet.size() && res; i++) {
	    if (mSet.at(i) != b.mSet.at(i)) 
		res = false;
	}
    }
    return res;
}

bool Enum::operator==(const MDtBase& sb)
{ 
    const Enum& b = dynamic_cast<const Enum& >(sb); 
    return &b != NULL && DtBase::operator==(b)  && mData == b.mData;
};

// Vector base

void VectorBase::DataToString(stringstream& aStream) const
{
    for (int idx = 0; idx < Size(); idx++) {
	ElemToString(idx, aStream);
	aStream << KDataSep;
    }
}

bool VectorBase::DataFromString(istringstream& aStream, bool& aRes)
{
    bool res = true;
    bool changed = false;
    string ss = aStream.str();
    if (!ss.empty()) {
	int beg = 0;
	int elem_e = ss.find_first_of(KDataSep, beg);
	int idx = 0;
	do {
	    string elems = ss.substr(beg, elem_e);
	    istringstream ess(elems);
	    changed |= ElemFromString(idx++, ess, res);
	    beg = elem_e == string::npos ? string::npos : elem_e + 1;
	    elem_e = ss.find_first_of(KDataSep, beg);
	} while (res && beg != string::npos);
    }
    aRes = res;
    return changed;
}

bool VectorBase::IsCompatible(const MDtBase& sb)
{
    const VectorBase& b = dynamic_cast<const VectorBase& >(sb); 
    bool res = b.mValid && b.GetTypeSig() == GetTypeSig();
    return res;
}

// Vector


template<> const char* Vector<string>::TypeSig() { return  "VS";};

