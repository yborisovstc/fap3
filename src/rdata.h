#ifndef __FAP3_RDATA_H
#define __FAP3_RDATA_H

#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <assert.h>

// Row data
//

using namespace std;


/** @brief Data base interface
 * */
class MDtBase
{
    public:
	static const char* Type() { return "MDtBase";};
    public:
	virtual void ToString(string& aString, bool aSig = true) const = 0;
	virtual bool FromString(const string& aString) = 0;
	virtual bool IsValid() const { return false;}
	virtual bool operator==(const MDtBase& b) const = 0;
	virtual bool operator!=(const MDtBase& b) const = 0;
    public:
	virtual string GetTypeSig() const { return "?";}
	virtual void DataToString(stringstream& aStream) const {aStream << "?";}
	// TODO to swap aRes and return data (sign of change)
	virtual bool DataFromString(istringstream& aStream, bool& aRes) { return false;}
	virtual bool IsCompatible(const MDtBase& b) const {return true;}
    public:
	static const char mKTypeToDataSep;
};


// Scalar data

class DtBase : public MDtBase
{
    public:
	DtBase(): mValid(false) {};
	DtBase(const DtBase& d): mValid(d.mValid), mSigTypeOK(false) {};
	virtual ~DtBase();
	string ToString(bool aSig = true) const { string res; ToString(res, aSig); return res; }
	static int ParseSigPars(const string& aCont, string& aSig);
	static bool IsSrepFit(const string& aString, const string& aTypeSig);
	static bool IsDataFit(const DtBase& aData, const string& aTypeSig);
	virtual void ToString(string& aString, bool aSig = true) const override;
	virtual bool FromString(const string& aString) override;
	virtual bool operator==(const MDtBase& b) const override { return mValid == b.IsValid();}
	virtual bool operator!=(const MDtBase& b) const override { return !DtBase::operator==(b);}
	virtual bool IsValid() const override { return mValid;}
    public:
	virtual DtBase* Clone() {return NULL;}
	virtual void SetMplncArg1Hint(const DtBase& res, const DtBase& arg2) {};
	virtual void SetMplncArg2Hint(const DtBase& res, const DtBase& arg1) {};
    public:
	bool mValid;
	// Invalidity reason: sigtype is invalis
	bool mSigTypeOK;
};

template<class T> class Sdata: public DtBase
{
    public:
	Sdata(): DtBase() {};
	Sdata(const T& aRdata): Sdata() { mData = aRdata; mValid = true;}
	Sdata(const Sdata& d): DtBase(d), mData(d.mData) {};
	template <class TA> Sdata(const Sdata<TA>& b): DtBase(b), mData(static_cast<T>(b.mData)) {};
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString) { return DtBase::IsSrepFit(aString, TypeSig());};
	static bool IsDataFit(const Sdata<T>& aData) { return DtBase::IsDataFit(aData, TypeSig());};
	static Sdata<T>* Construct(const string& aSrep) {Sdata<T>* res = NULL; if (IsSrepFit(aSrep)) { res = new Sdata<T>(); } else ;return res;};
	//bool Set(const Sdata& d);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void DataToString(stringstream& aStream) const override { aStream << std::boolalpha << mData;};
	virtual bool DataFromString(istringstream& aStream, bool& aRes) override;
	virtual DtBase* Clone() {return new Sdata<T>(*this);};
	virtual bool operator==(const MDtBase& sb) const override { const Sdata<T>& b = dynamic_cast<const Sdata<T>& >(sb);
	    return &b != NULL && DtBase::operator==(b) && mData == b.mData;};
	virtual bool operator!=(const MDtBase& b) const  override { return !Sdata<T>::operator==(b);}
	bool operator>(const Sdata<T>& b) const { return mData > b.mData;};
	bool operator>=(const Sdata<T>& b) const { return mData >= b.mData;};
	bool operator<(const Sdata<T>& b) const { return mData < b.mData;};
	bool operator<=(const Sdata<T>& b) const { return mData <= b.mData;};
	Sdata<T>& Invm(const Sdata<T>& b) { mData = 1 / b.mData; return *this;};
	Sdata<T>& operator=(const Sdata<T>& b) { this->DtBase::operator=(b); mData = b.mData; return *this;};
	//Sdata<T>& operator=(const Sdata<T>& b) { mData = b.mData; mValid = b.mValid; mSigTypeOK = b.mSigTypeOK; return *this;};
	Sdata<T>& operator+=(const Sdata<T>& b) { mData += b.mData; return *this;};
	Sdata<T>& operator-=(const Sdata<T>& b) { mData -= b.mData; return *this;};
	Sdata<T>& operator*=(const Sdata<T>& b) { mData *= b.mData; return *this;};
	Sdata<T>& operator!() { mData = !mData; return *this;};
	bool Set(const T& aData) { bool res = aData != mData; mData = aData; mValid = true; return res; };
    protected:
	void InpFromString(istringstream& aStream, T& aRes);
    public:
	T mData;
};

template<class T> bool Sdata<T>::DataFromString(istringstream& aStream, bool& aRes)
{
    bool changed = false;
    T sdata;
    InpFromString(aStream, sdata);
//    aStream >> std::boolalpha >> sdata;
    if (aRes = !aStream.fail()) {
	mValid = true; if (mData != sdata) { mData = sdata; changed = true; }
    }
    return changed;
}



// Matrix
// Operations required the result to provide the hints: type or dim required values.
// If there is no hint for particular par, the par can be redifined by the operaton
class MtrBase: public DtBase
{ 
    public: 
	// Type of matrix
	enum TMtrType {
	    EMt_Unknown,
	    EMt_Regular,
	    EMt_Diagonal,
	    EMt_Utriang, // Upper-triangular
	    EMt_Btriang, // Bottom-triangular
	};
	// Dimension
	typedef pair<int, int> TMtrDim;
    public: 
	MtrBase(): DtBase(), mType(EMt_Unknown), mDim(TMtrDim(0, 0)) {};
	MtrBase(TMtrType aType, TMtrDim aDim): DtBase(), mType(aType), mDim(aDim) {};
	MtrBase(const MtrBase& aMtr): DtBase(aMtr), mType(aMtr.mType), mDim(aMtr.mDim) {};
	virtual void ToString(string& aString, bool aSig = true) const;
	bool FromString(const string& aString);
	static bool IsSrepFit(const string& aString, const string& aTypeSig);
	static bool IsDataFit(const MtrBase& aData, const string& aTypeSig);
	static int ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim);
	int Ind(int r, int c) const;
	int IntSize() const;
	MtrBase& operator=(const MtrBase& b) { this->DtBase::operator=(b); mType = b.mType; mDim = b.mDim; return *this;};
	MtrBase& operator+=(const MtrBase& b);
	MtrBase& operator-=(const MtrBase& b);
	MtrBase& Mpl(const MtrBase& a, const MtrBase& b);
	MtrBase& Mpl(const void* b);
	MtrBase& Invm(const MtrBase& a);
	virtual void ElemToString(int aInd, stringstream& aStream) const { aStream << "?";};
	virtual bool ElemFromString(int aInd, istringstream& aStream, bool& aRes) { return false;}
	virtual void AddElem(const MtrBase& aB, int aRI, int aCI) {};
	virtual void SubElem(const MtrBase& aB, int aRI, int aCI) {};
	virtual void MplElems(int r, int c, const MtrBase& a, int ar, int ac, const MtrBase& b, int br, int bc) {};
	virtual void MplElem(int r, int c, const void* b) {};
	virtual void MplRtoC(int r, int c, const MtrBase& a, const MtrBase& b) {};
	virtual void InvmElem(int r, int c, const MtrBase& a, int ar, int ac) {};
	virtual void SetIntSize(int aSize) {};
	virtual void SetMplncArg1Hint(const DtBase& res, const DtBase& arg2);
	virtual void SetMplncArg2Hint(const DtBase& res, const DtBase& arg1);
    public:
	TMtrType mType;
	TMtrDim mDim;
};

template<class T> class Mtr: public MtrBase
{ 
    public:
	Mtr(): MtrBase() {};
	Mtr(TMtrType aType, TMtrDim aDim): MtrBase(aType, aDim) {};
	Mtr(const Mtr<T>& aMtr): MtrBase(aMtr), mData(aMtr.mData) {};
	template <class TA> Mtr(const Mtr<TA>& aMtr);
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString) { return MtrBase::IsSrepFit(aString, TypeSig());};
	static bool IsDataFit(const Mtr<T>& aData) { return MtrBase::IsDataFit(aData, TypeSig());};
	T GetElem(int r, int c) const { int i = Ind(r,c); return (i == -1) ? T(0) : mData.at(i);};
	T& Elem(int r, int c) { int i = Ind(r,c); assert(i >= 0 && i < mData.size()); return mData.at(i);};
	virtual bool operator==(const MDtBase& sb) const override { 
	    const Mtr<T>& b = dynamic_cast<const Mtr<T>& >(sb); return &b != NULL && DtBase::operator==(b) &&
		this->mType == b.mType && this->mDim == b.mDim && this->mData == b.mData;};
	virtual bool operator!=(const MDtBase& b) const override { return !Mtr<T>::operator==(b);}
	Mtr<T>& operator=(const Mtr<T>& b) { this->MtrBase::operator=(b); mData = b.mData; return *this;};
	template <class TA> void CastDown(const Mtr<TA>& a);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void ElemToString(int aInd, stringstream& aStream) const { aStream << mData.at(aInd);};
	virtual bool ElemFromString(int aInd, istringstream& aStream, bool& aRes);
	virtual void AddElem(const MtrBase& b, int r, int c);
	virtual void SubElem(const MtrBase& b, int r, int c);
	virtual void MplElems(int r, int c, const MtrBase& a, int ar, int ac, const MtrBase& b, int br, int bc);
	virtual void MplElem(int r, int c, const void* b);
	virtual void MplRtoC(int r, int c, const MtrBase& a, const MtrBase& b);
	virtual void InvmElem(int r, int c, const MtrBase& a, int ar, int ac) { T e = ((Mtr<T>&) a).GetElem(ar, ac); T res = 1/e; Elem(r, c) = res;
	    if (std::isinf(res)) mValid = false;};
	virtual void SetIntSize(int aSize) { if (mData.size() != aSize) mData.resize(aSize);};
    public:
	vector<T> mData;
};

template <class T> template <class TA> Mtr<T>::Mtr(const Mtr<TA>& aMtr): MtrBase(aMtr) {
    CastDown(aMtr);
}

template <class T> template<class TA>  void Mtr<T>::CastDown(const Mtr<TA>& a) 
{
    if (mType == EMt_Unknown) { mType = a.mType; }
    if (mDim.first == 0) mDim.first = a.mDim.first;
    if (mDim.second == 0) mDim.second = a.mDim.second;
    SetIntSize(IntSize());
    if (mType == a.mType && mDim == a.mDim && mData.size() == a.mData.size()) {
	for (int m = 0; m < mData.size(); m++) {
	    mData.at(m) = (T) a.mData.at(m);
	}
    }
    else mValid = false;
};

template<class T> void Mtr<T>::MplRtoC(int r, int c, const MtrBase& a, const MtrBase& b)
{
    for (int m = 0; m < a.mDim.second; m++) {
	Elem(r, c) += ((Mtr<T>&) a).GetElem(r, m) * ((Mtr<T>&) b).GetElem(m, c);
    }
}

template<class T> void Mtr<T>::MplElems(int r, int c, const MtrBase& a, int ar, int ac, const MtrBase& b, int br, int bc)
{
    //    if (Ind(r,c) == -1 || a.Ind(ar,ac) == -1 || b.Ind(br,bc) == -1) { mValid = false; return;} ; 
    Elem(r, c) = ((Mtr<T>&) a).GetElem(ar, ac) * ((Mtr<T>&) b).GetElem(br, bc);
}

template<class T> void Mtr<T>::MplElem(int r, int c, const void* b)
{
    Elem(r, c) *= *((const T*) b);
}

template<class T> void Mtr<T>::AddElem(const MtrBase& b, int r, int c) 
{ 
    if (Ind(r,c) == -1) {
	mValid = false; return;} ; 
    Elem(r, c) += ((Mtr<T>&) b).GetElem(r, c);
};

template<class T> void Mtr<T>::SubElem(const MtrBase& b, int r, int c)
{
    if (Ind(r,c) == -1) {
	mValid = false; return;} ;
    Elem(r, c) -= ((Mtr<T>&) b).GetElem(r, c);
};

template<class T> bool Mtr<T>::ElemFromString(int aInd, istringstream& aStream, bool& aRes)
{
    bool changed = false;
    T sdata;
    aStream >> sdata;
    if (aRes = !aStream.fail()) {
	if (aInd >= mData.size()) { mData.resize(IntSize()); changed = true; }
	T& data = mData.at(aInd);
	if (data != sdata) { data = sdata; changed = true; }
    }
    return changed;
}

// Tuple, with named components, adds the components on run-time
// Full_signature := TypeSig  ','   Paremeters ;
// Parameters := CompSig {"," CompSig} ;
// CompSig := CompTypeSig  ':' CompName ;
class NTuple: public DtBase
{ 
    public:
	// Components: Name, Data
	typedef pair<string, DtBase*> tComp;
	typedef vector<tComp> tComps;
	// Signature of component: Typesig, Name
	typedef pair<string, string> tCompSig;
	// Signature of components
	typedef vector<tCompSig> tCTypes;
    public:
	NTuple(): DtBase() {};
	NTuple(const NTuple& aTuple): DtBase(aTuple) {};
	virtual ~NTuple();
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString);
	static bool IsDataFit(const NTuple& aData);
	static int ParseSigPars(const string& aCont, string& aSig, tCTypes& aCTypes);
	void Init(const tCTypes& aCt);
	virtual void ToString(string& aString, bool aSig = true) const override;
	bool FromString(const string& aString);
	DtBase* GetElem(const string& aName);
	virtual bool operator==(const MDtBase& sb) const override;
	virtual bool operator!=(const MDtBase& b) const override { return !NTuple::operator==(b);}
	NTuple& operator=(const NTuple& b);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void DataToString(stringstream& aStream) const;
    protected:
	bool IsCTypesFit(const tCTypes& aCt) const;
	void TypeParsToString(stringstream& aStream) const;
    public:
	tComps mData;
};

// Enumeration, element of ordered finite set
// Data is represented by index in the set
class Enum: public DtBase
{
    public:
	typedef vector<string> tSet;
    public:
	Enum(): DtBase() {};
	Enum(const Enum& aData): DtBase(aData) {};
	virtual ~Enum();
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString);
	static bool IsDataFit(const Enum& aData);
	static int ParseSigPars(const string& aCont, string& aSig, tSet& aSet);
	static int ParseSig(const string& aCont, string& aSig);
	bool AreTypeParsFit(const tSet& aSet) const;
	virtual void ToString(string& aString, bool aSig = true) const override;
	bool FromString(const string& aString);
	void Init(const tSet& aSet);
    public:
	virtual string GetTypeSig() const { return TypeSig();};
	virtual bool DataFromString(istringstream& aStream, bool& aRes);
	virtual void DataToString(stringstream& aStream) const;
	virtual bool IsCompatible(const DtBase& b) const;
    public:
	virtual bool operator==(const MDtBase& sb) const override;
	virtual bool operator!=(const MDtBase& b) const override { return !Enum::operator==(b);}
	bool operator>(const Enum& b) const { return mData > b.mData;};
	bool operator>=(const Enum& b) const { return mData >= b.mData;};
	bool operator<(const Enum& b) const { return mData < b.mData;};
	bool operator<=(const Enum& b) const { return mData <= b.mData;};

    protected:
	void TypeParsToString(stringstream& aStream) const;
    public:
	tSet mSet;
	int mData;
};


/** @brief Vector base
 * */
class VectorBase : public DtBase
{
    public:
	VectorBase(): DtBase() {};
	VectorBase(int aSize): DtBase() {};
	VectorBase(const VectorBase& aSrc): DtBase(aSrc) {};
	virtual void ElemToString(int aInd, stringstream& aStream) const { aStream << "?";};
	virtual bool ElemFromString(int aIdx, istringstream& aStream, bool& aRes) { return false;}
	virtual int Size() const { return -1;}
	// From DtBase
	virtual void DataToString(stringstream& aStream) const override;
	virtual bool DataFromString(istringstream& aStream, bool& aRes);
	virtual bool IsCompatible(const MDtBase& b) const override;
};

/** @brief Typed vector
 * */
template <typename T>
class Vector : public VectorBase
{
    public:
	Vector(): VectorBase() {}
	Vector(int aSize): VectorBase(aSize) {}
	Vector(const Vector& aSrc): VectorBase(aSrc) {}
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString) { return DtBase::IsSrepFit(aString, TypeSig());};
	static bool IsDataFit(const Vector<T>& aData) { return DtBase::IsDataFit(aData, TypeSig());};
	// From VectorBase
	virtual int Size() const override { return mData.size();}
	virtual void ElemToString(int aIdx, stringstream& aStream) const override { aStream << mData.at(aIdx);}
	virtual bool ElemFromString(int aIdx, istringstream& aStream, bool& aRes) override {
	    assert(aIdx <= mData.size());
	    T data; aStream >> data; bool res = aIdx >= mData.size() || mData.at(aIdx) != data; 
	    if (aIdx < mData.size())
		mData.at(aIdx) = data;
	    else 
		mData.push_back(data);
	    aRes = true;
	    return true; // TODO YB get non trivial ret val
	}
	virtual bool operator==(const MDtBase& b) const override {
	    if (!IsCompatible(b)) return false;
	    const Vector<T>* vb = dynamic_cast<const Vector<T>*>(&b);
	    if (!vb) return false;
    	    return this->mData == vb->mData; }
	virtual bool operator!=(const MDtBase& b) const override { return !Vector::operator==(b);}
	// From MDtBase
	virtual string GetTypeSig() const { return TypeSig();};
	virtual DtBase* Clone() {return new Vector<T>(*this);};
	// Local
	virtual bool GetElem(int aInd, T& aElem) const {
	    bool res = true;
	    if (aInd < Size()) aElem = mData.at(aInd);
	    else res = false;
	    return res; }
    public:
	vector<T> mData;
};

#endif
