
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




#endif
