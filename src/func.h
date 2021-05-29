
#ifndef __FAP3_FUNC_H
#define __FAP3_FUNC_H

#include "mdata.h"
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
	    virtual bool IsLogLevel(int aLevel) const { return false; }
	    virtual void log(TLogRecCtg aCtg, const string& aMsg) = 0;
	};
    public:
	Func(Host& aHost): mHost(aHost) {};
	virtual ~Func() {}
	virtual MIface* getLif(const char *aName) { return nullptr;}
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
	T mRes;
};


// Maximum, variable type

class FMaxBase: public Func {
    public:
	enum { EInp = Func::EInp1 };
	FMaxBase(Host& aHost): Func(aHost) {};
};

/** @brief Addintion, generic data
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



#endif
