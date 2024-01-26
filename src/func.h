
#ifndef __FAP3_FUNC_H
#define __FAP3_FUNC_H

#include "mdata.h"
#include "rdatauri.h"
#include "mifr.h"
#include "log.h"

/** @brief Executive part of function 
 * */
class Func
{
    public:
	using TInpIc = vector<MDVarGet*>;
	enum { EInp1 = 0, EInp2, EInp3, EInp4 };
    public:
	class Host {
	    public: 
	    virtual TInpIc* GetInps(int aInpId) = 0;
	    virtual string GetInpUri(int aId) const = 0;
	    virtual void log(int aCtg, const string& aMsg) = 0;
	    virtual string getHostUri() const = 0;
	    virtual bool hostIsLogLevel(int aLevel) const = 0;
	};
    public:
	Func(Host& aHost): mHost(aHost) {};
	virtual ~Func() {}
	virtual string IfaceGetId() const = 0;
	virtual string GetInpExpType(int aId) const { return "<?>";};
	virtual const DtBase* FDtGet() { return nullptr;}
	template <class T> inline const T* GetInpData(int aInpId, const T* aData);
	inline const DtBase* GetInpData(int aInpId);
	const MDVarGet* GetInp(int aInpId);
	static const MDVarGet* GetInp(Host* aHost, int aInpId);
    protected:
	Host& mHost;
};

#define LOGF(aLevel, aContent) \
    if (mHost.hostIsLogLevel(aLevel)) {\
	mHost.log(aLevel, aContent);\
    }\



template <class T> inline const T* Func::GetInpData(int aInpId, const T* aData)
{
    const T* data = nullptr;
    TInpIc* Ic = mHost.GetInps(aInpId);
    if (Ic) {
	if (Ic->size() == 1) {
	auto* get = Ic->at(0);
	data = get ? get->DtGet(data) : nullptr;
	if (!data) {
	    mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(aInpId) + "]");
	}
	} else {
	    LOGF(EErr, "More than one input [" + mHost.GetInpUri(aInpId) + "]");
	    for (auto ic : *Ic) {
		LOGF(EDbg, "Input [" + ic->Uid() + "]");
	    }
	}
	    
    }
    return data;
}

inline const DtBase* Func::GetInpData(int aInpId)
{
    const DtBase* data = nullptr;
    TInpIc* Ic = mHost.GetInps(aInpId);
    if (Ic) {
	auto* get = (Ic->size() == 1) ? Ic->at(0) : nullptr;
	data = get ? get->VDtGet(string()) : nullptr;
	if (!data) {
	    mHost.log(EDbg, "Cannot get input [" + mHost.GetInpUri(aInpId) + "]");
	}
    }
    return data;
}




class FAddBase: public Func {
    public:
	enum { EInp = EInp1, EInpN = EInp2 };
	FAddBase(Host& aHost): Func(aHost) {};
};

/** @brief Addition, v.2, generic data
 * */
template <class T> class FAddDt: public FAddBase
{
    public:
	static Func* Create(Host* aHost, const string& aOutId);
	FAddDt(Host& aHost): FAddBase(aHost) {};
	virtual string IfaceGetId() const { return T::TypeSig();}
	virtual string GetInpExpType(int aId) const;
	virtual const DtBase* FDtGet() override;
	T mRes;
};


/** @brief Addition, single connecting inputs (ref iss_014), generic data
 * */
template <class T> class FAddDt2: public Func
{
    public:
	static Func* Create(Host* aHost, const string& aOutId);
	FAddDt2(Host& aHost): Func(aHost) {};
	virtual string IfaceGetId() const { return T::TypeSig();}
	virtual string GetInpExpType(int aId) const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	T mRes;
};


/** @brief Subtraction, single connecting inputs (ref iss_014), generic data
 * */
template <class T> class FSubDt2: public Func
{
    public:
	static Func* Create(Host* aHost, const string& aOutId);
	FSubDt2(Host& aHost): Func(aHost) {};
	virtual string IfaceGetId() const { return T::TypeSig();}
	virtual string GetInpExpType(int aId) const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	T mRes;
};



/** @brief Multiplication, base
 * */
class FMplBase: public Func {
    public:
	enum { EInp = EInp1 };
	FMplBase(Host& aHost): Func(aHost) {};
};

/** @brief Multiplication, generic data
 * */
template <class T> class FMplDt: public FMplBase {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMplDt(Host& aHost): FMplBase(aHost) {};
	virtual string IfaceGetId() const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const override { return T::TypeSig();}
    protected:
	T mRes;
};


/** @brief Division, base
 * */
class FDivBase: public Func {
    public:
	enum { EInp = EInp1, EInp2 = EInp2 };
	FDivBase(Host& aHost): Func(aHost) {};
};

/** @brief Division, generic data
 * */
template <class T> class FDivDt: public FDivBase
{
    public:
	static Func* Create(Host* aHost, const string& aString);
	FDivDt(Host& aHost): FDivBase(aHost) {};
	virtual string IfaceGetId() const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const override { return T::TypeSig();}
    protected:
	T mRes;
};


/** @brief Append
 * */
template <class T>
class FApnd: public Func {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FApnd(Host& aHost): Func(aHost) {}
	virtual string IfaceGetId() const { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const;
    protected:
	T mRes;
};


// Minimum, variable type

class FMinBase: public Func {
    public:
	enum { EInp = Func::EInp1 };
	FMinBase(Host& aHost): Func(aHost) {};
};

/** @brief Getting minimum, generic data
 * */
template <class T> class FMinDt: public FMinBase
{
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMinDt(Host& aHost): FMinBase(aHost) {};
	virtual string IfaceGetId() const { return T::TypeSig();};
	virtual const DtBase* FDtGet() override;
    protected:
	T mRes;
};




// Maximum, variable type

class FMaxBase: public Func {
    public:
	enum { EInp = Func::EInp1 };
	FMaxBase(Host& aHost): Func(aHost) {};
};

/** @brief Getting maximum, generic data
 * */
template <class T> class FMaxDt: public FMaxBase
{
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMaxDt(Host& aHost): FMaxBase(aHost) {};
	virtual string IfaceGetId() const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
    protected:
	T mRes;
};



/** @brief Comparition, generic data base
 * */
class FCmpBase: public Func
{
    public:
	enum TFType {ELt, ELe, EEq, ENeq, EGe, EGt};
    public:
	FCmpBase(Host& aHost, TFType aFType): Func(aHost), mFType(aFType) {};
	// From Func
	virtual string IfaceGetId() const override;
    protected:
	TFType mFType;
	Sdata<bool> mRes;
};

template <class T> class FCmp: public FCmpBase
{
    public:
	static Func* Create(Host* aHost, const string& aInp1Iid, const string& aInp2Iid, TFType aFType);
	FCmp(Host& aHost, TFType aFType): FCmpBase(aHost, aFType) {};
	virtual const DtBase* FDtGet() override;
};



/** @brief Getting size of container: Vector
 * @tparam T type of vector element
 * */
// TODO YB Weak design. Size doesn't depend on container elem type, so better to have this class w/o template.
// But the current design doesn't allow to do it.
// To consider the design improvement
template <class T>
class FSizeVect: public Func {
    public:
	using TOutp = Sdata<int>;
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FSizeVect(Host& aHost): Func(aHost) {};
	virtual string IfaceGetId() const override { return TOutp::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const override;
    protected:
	TOutp mRes;
};

/** @brief Getting component of container: base
 * */
class FAtBase: public Func
{
    public:
	FAtBase(Host& aHost): Func(aHost) {};
};


/** @brief Getting component of container: Vector, wrapping by Sdata
 * It seems useful in case of having Vector of primitive scalar data (e.g. string)
 * */
template <class T>
class FAtVect: public FAtBase {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FAtVect(Host& aHost): FAtBase(aHost) {};
	virtual string IfaceGetId() const override { return Sdata<T>::TypeSig();}
	virtual string GetInpExpType(int aId) const override;
	virtual const DtBase* FDtGet() override;
    protected:
	Sdata<T> mRes;
};

/** @brief Getting component of container: Vector, generic variant
 * DOESN'T wrap component by Sdata
 * */
template <class T>
class FAtgVect: public FAtBase {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FAtgVect(Host& aHost): FAtBase(aHost) {};
	virtual string IfaceGetId() const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const override;
    protected:
	T mRes;
};

/** @brief Getting component of container: Pair, generic variant
 * DOESN'T wrap component by Sdata
 * */
template <class T>
class FAtgPair: public FAtBase {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FAtgPair(Host& aHost): FAtBase(aHost)  {};
	virtual string IfaceGetId() const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const override;
    protected:
	T mRes;
};

/** @brief Find in Vert<Pair<T>> by first element of pair
 * */
template <class T>
class FFindByP: public Func {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FFindByP(Host& aHost): Func(aHost) {};
	virtual string IfaceGetId() const override { return T::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const override;
    protected:
	T mRes;
};




/** @brief Gettng tail base
 * */
class FTailBase: public Func {
    public:
	enum { EInp = EInp1, EHead = EInp2 };
	FTailBase(Host& aHost): Func(aHost) {};
};

/** @brief Gettng tail base
 * */
template <typename T>
class FTail: public FTailBase {
    public:
        using TData = T;
    public:
	FTail(Host& aHost): FTailBase(aHost) {};
	virtual string GetInpExpType(int aId) const override { return TData::TypeSig(); }
	virtual string IfaceGetId() const { return TData::TypeSig();}
    public:
	TData mRes;
};

/** @brief Getting tail, URI
 * */
class FTailUri: public FTail<DGuri> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FTailUri(Host& aHost): FTail<DGuri>(aHost) {};
	virtual const DtBase* FDtGet();
};


/** @brief Gettng head base
 * */
class FHeadBase: public Func {
    public:
	enum { EInp = EInp1, ETail = EInp2 };
	FHeadBase(Host& aHost): Func(aHost) {};
};

/** @brief Gettng head
 * */
template <typename T>
class FHead: public FHeadBase {
    public:
	using TData = T;
    public:
	FHead(Host& aHost): FHeadBase(aHost) {};
	virtual string GetInpExpType(int aId) const override { return TData::TypeSig(); }
	virtual string IfaceGetId() const { return TData::TypeSig();}
    public:
	TData mRes;
};


/** @brief Getting head, URI
 * */
class FHeadUri: public FHead<DGuri> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FHeadUri(Host& aHost): FHead<DGuri>(aHost) {};
	virtual const DtBase* FDtGet() override;
};



/** @brief Gettng head by given tail len. Base
 * */
class FHeadTnBase: public Func {
    public:
	using TNum = Sdata<int>;
    public:
	enum { EInp = EInp1, ETailn = EInp2 };
	FHeadTnBase(Host& aHost): Func(aHost) {};
};


/** @brief Gettng head by given tail len
 * */
template <typename T>
class FHeadTn: public FHeadTnBase {
    public:
	using TData = T;
    public:
	FHeadTn(Host& aHost): FHeadTnBase(aHost) {};
	virtual string GetInpExpType(int aId) const override { return TData::TypeSig(); }
	virtual string IfaceGetId() const { return TData::TypeSig();}
    public:
	TData mRes;
};


/** @brief Getting head with given tail len, URI
 * */
class FHeadTnUri: public FHeadTn<DGuri> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FHeadTnUri(Host& aHost): FHeadTn<DGuri>(aHost) {};
	virtual const DtBase* FDtGet() override;
};



/** @brief Gettng tail as num of elems base
 * */
class FTailnBase: public Func {
    public:
	enum { EInp = EInp1, ENum = EInp2 };
	FTailnBase(Host& aHost): Func(aHost) {};
};


/** @brief Gettng tail as num of elems base
 * */
template <typename T>
class FTailn: public FTailnBase {
    public:
	using TData = T;
	using TNum = Sdata<int>;
    public:
	FTailn(Host& aHost): FTailnBase(aHost) {};
	virtual string GetInpExpType(int aId) const override { return TData::TypeSig(); }
	virtual string IfaceGetId() const { return TData::TypeSig();}
    public:
	TData mRes;
};


/** @brief Getting tail as num of elems, URI
 * */
class FTailnUri: public FTailn<DGuri> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FTailnUri(Host& aHost): FTailn<DGuri>(aHost) {};
	virtual const DtBase* FDtGet() override;
};


/** @brief Compose Pair from comps (DataBase)
 * */
template <class T>
class FPair: public Func {
    public:
	using TData = Pair<T>;
	using TInpData = T;
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FPair(Host& aHost): Func(aHost) {}
	virtual string IfaceGetId() const override { return TData::TypeSig();}
	virtual const DtBase* FDtGet() override;
	virtual string GetInpExpType(int aId) const override { return TInpData::TypeSig();}
    protected:
	TData mRes;
};

#endif
