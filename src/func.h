
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
	enum { EInp1 = 0, EInp2, EInp3, EInp4 };
    public:
	class Host {
	    public: 
	    MDVarGet* GetInp(int aInpId, bool aOpt = false) {
		MDVarGet* res = nullptr;
		auto inps = GetInps(aInpId, MDVarGet::Type(), aOpt);
		if (inps && !inps->empty()) {
		    res = dynamic_cast<MDVarGet*>(inps->at(0));
		}
		return res;
	    }
	    virtual MIfProv::TIfaces* GetInps(int aId, const string& aIfName, bool aOpt) = 0;
	    virtual void OnFuncContentChanged() = 0;
	    virtual string GetInpUri(int aId) const = 0;
	    virtual void log(int aCtg, const string& aMsg) = 0;
	};
    public:
	Func(Host& aHost): mHost(aHost) {};
	virtual ~Func() {}
	virtual MIface* getLif(const char *aName) { return nullptr;}
	template<class T> MIface* checkLif(const char* aType) { return (strcmp(aType, T::Type()) == 0) ? dynamic_cast<T*>(this) : nullptr;}
	virtual string IfaceGetId() const = 0;
	virtual void GetResult(string& aResult) const { aResult = "<?>";};
	virtual string GetInpExpType(int aId) const { return "<?>";};
	/** @brief Helper. Gets value from MDVarGet */
	template <typename T> static bool GetData(MDVarGet* aDvget, T& aData);
    protected:
	Host& mHost;
};


class FAddBase: public Func {
    public:
	enum { EInp = EInp1, EInpN = EInp2 };
	FAddBase(Host& aHost): Func(aHost) {};
};


/** @brief Addintion, generic data
 * */
template <class T> class FAddDt: public FAddBase, public MDtGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddDt(Host& aHost): FAddBase(aHost) {};
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual void DtGet(T& aData);
	virtual void GetResult(string& aResult) const;
	virtual string GetInpExpType(int aId) const;
	// From MDtGet
	virtual void MDtGet_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	T mRes;
};


/** @brief Boolean AND, generic data
 * */
class FBAndDt: public Func, public MDtGet<Sdata<bool>> {
    public:
	enum { EInp = EInp1 };
	static Func* Create(Host* aHost, const string& aString);
	FBAndDt(Host& aHost): Func(aHost) {};
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const { return MDtGet<Sdata<bool>>::Type();};
	virtual void DtGet(Sdata<bool>& aData);
	virtual void GetResult(string& aResult) const;
	virtual string GetInpExpType(int aId) const;
	Sdata<bool> mRes;
};


/** @brief Converting to GUri
 * @tparam T type of vector element
 * */
class FUri: public Func, public MDtGet<DGuri> {
    public:
	using TData = DGuri;
	using TInpData = Sdata<string>;
	enum { EInp = EInp1 };
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FUri(Host& aHost): Func(aHost) {}
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const { return MDtGet<TData>::Type();}
	virtual void DtGet(TData& aData);
	virtual void GetResult(string& aResult) const {mRes.ToString(aResult);}
	virtual string GetInpExpType(int aId) const;
	TData mRes;
};

/** @brief Append
 * */
template <class T>
class FApnd: public Func, public MDtGet<T> {
    public:
	using TData = T;
	using TInpData = T;
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FApnd(Host& aHost): Func(aHost) {}
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const { return MDtGet<TData>::Type();}
	virtual void DtGet(TData& aData);
	virtual void GetResult(string& aResult) const {mRes.ToString(aResult);}
	virtual string GetInpExpType(int aId) const;
	TData mRes;
};




// Maximum, variable type

class FMaxBase: public Func {
    public:
	enum { EInp = Func::EInp1 };
	FMaxBase(Host& aHost): Func(aHost) {};
};

/** @brief Getting maximum, generic data
 * */
template <class T> class FMaxDt: public FMaxBase, public MDtGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMaxDt(Host& aHost): FMaxBase(aHost) {};
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual void DtGet(T& aData);
	virtual void GetResult(string& aResult) const;
	T mRes;
};



/** @brief Comparition, generic data base
 * */
class FCmpBase: public Func, public MDtGet<Sdata<bool> >
{
    public:
	enum TFType {ELt, ELe, EEq, ENeq, EGe, EGt};
    public:
	FCmpBase(Host& aHost, TFType aFType): Func(aHost), mFType(aFType) {};
	virtual ~FCmpBase();
	// From Func
	virtual string IfaceGetId() const override;
	virtual void GetResult(string& aResult) const override;
	virtual MIface* getLif(const char *aName) override;
	// From MDtGet
	virtual void DtGet(Sdata<bool>& aData) override;
    protected:
	TFType mFType;
	Sdata<bool> mRes;
};

template <class T> class FCmp: public FCmpBase
{
    public:
	static Func* Create(Host* aHost, const string& aInp1Iid, const string& aInp2Iid, TFType aFType);
	FCmp(Host& aHost, TFType aFType): FCmpBase(aHost, aFType) {};
	virtual void DtGet(Sdata<bool>& aData);
};


/** @brief Switcher, generic data base
 * */
class FSwithcBase: public Func 
{
    public:
	enum { EInp_Sel = Func::EInp1, EInp_1 };
	FSwithcBase(Host& aHost): Func(aHost) {};
};

/** @brief Boolean switcher, selector should be of MDBoolGet or MDtGet<Sdata<bool> >
 * */
class FSwitchBool: public FSwithcBase, public MDVarGet
{
    public:
	static Func* Create(Func::Host* aHost, const string& aOutIid, const string& aInp1Id);
	FSwitchBool(Func::Host& aHost): FSwithcBase(aHost) {};
	// From Func
	virtual string IfaceGetId() const override { return MDVarGet::Type();};
	virtual string GetInpExpType(int aId) const override;
	virtual MIface* getLif(const char *aName) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return string();}
	virtual string VarGetIfid() const override;
	virtual MIface* DoGetDObj(const char *aName) override;
	// Local
	MDVarGet* GetCase() const;
    private:
	bool GetCtrl() const;
};


/** @brief Boolean negatiopn
 * */
class FBnegDt: public Func, public MDtGet<Sdata<bool>> {
    public:
	enum { EInp = EInp1 };
	static Func* Create(Host* aHost, const string& aString);
	FBnegDt(Host& aHost): Func(aHost) {}
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const override { return MDtGet<Sdata<bool>>::Type();}
	virtual void DtGet(Sdata<bool>& aData) override;
	virtual void GetResult(string& aResult) const override;
	Sdata<bool> mRes;
};


/** @brief Getting size of container: Vector
 * @tparam T type of vector element
 * */
// TODO YB Weak design. Size doesn't depend on container elem type, so better to have this class w/o template.
// But the current design doesn't allow to do it. HDt returns the exact MDtGet iface of owned data.
// To consider the design improvement
template <class T>
class FSizeVect: public Func, public MDtGet<Sdata<int>> {
    public:
	using TOutp = Sdata<int>;
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FSizeVect(Host& aHost): Func(aHost) {};
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const override { return MDtGet<TOutp>::Type();}
	virtual void DtGet(TOutp& aData) override;
	virtual void GetResult(string& aResult) const override {mRes.ToString(aResult);}
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


/** @briefGetting component of container: Vector
 * */
template <class T>
class FAtVect: public FAtBase, public MDtGet<Sdata<T>> {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FAtVect(Host& aHost): FAtBase(aHost) {};
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const override { return MDtGet<Sdata<T>>::Type();}
	virtual void DtGet(Sdata<T>& aData) override;
	virtual void GetResult(string& aResult) const override {mRes.ToString(aResult);}
	virtual string GetInpExpType(int aId) const override;
    protected:
	Sdata<T> mRes;
};


/** @brief Conversion to string
 * */
class FToStrBase: public Func, public MDtGet<Sdata<string>>
{
    public:
	enum { EInp = EInp1 };
	FToStrBase(Host& aHost): Func(aHost) {}
	virtual MIface* getLif(const char *aName) override;
	virtual string IfaceGetId() const override { return MDtGet<Sdata<string>>::Type();}
	virtual void GetResult(string& aResult) const override;
	Sdata<string> mRes;
};


/** @brief Conversion Sdata to string
 * @param T  type of original data
 * */
template <class T>
class FSToStr: public FToStrBase
{
    public:
	enum { EInp = EInp1 };
	static Func* Create(Host* aHost, const string& aIface, const string& aInpTSig);
	FSToStr(Host& aHost): FToStrBase(aHost) {}
	virtual void DtGet(Sdata<string>& aData) override;
};









#endif
