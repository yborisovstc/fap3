
#ifndef __FAP3_DEST_H
#define __FAP3_DEST_H

#include "mdata.h"
#include "mdes.h"
#include "msyst.h"
#include "vert.h"
#include "func.h"
#include "des.h"


/** @brief Transition function base 
 * */ 
class TrBase: public CpStateOutp
{
    public:
	static const char* Type() { return "TrBase";}
	TrBase(const string& aName = string(), MEnv* aEnv = NULL);
	// From Node
	virtual MIface* MOwner_getLif(const char *aType) override;
	// From MVert
	virtual MIface *MVert_getLif(const char *aType) override;
	//virtual bool isCompatible(MVert* aPair, bool aExt) override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const override {return getUid<MConnPoint>();}
	//virtual string provName() const override;
	//virtual string reqName() const override;
    protected:
	void AddInput(const string& aName);
};


// Agent base of Var transition function
class TrVar: public TrBase, public MDVarGet, public Func::Host
{
    public:
	static const char* Type() { return "TrVar";};
	TrVar(const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override;
	virtual string VarGetIfid() const override;
	// From Func::Host
	virtual void OnFuncContentChanged() override;
	virtual int GetInpCpsCount() const {return 0;}
	virtual bool IsLogLevel(int aLevel) const override { return true; }
	virtual void log(TLogRecCtg aCtg, const string& aMsg);
	virtual MIfProv::TIfaces* GetInps(int aId, const string& aIfName, bool aOpt) override;
    protected:
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(int aId) const;
	/** @brief Gets value of MDVarGet MDtGet input */
	template<typename T> bool DtGetInp(T& aData, const string& aInpName);
	/** @brief Gets value of MDVarGet MDtGet Sdata<T> input */
	template<typename T> bool DtGetSdataInp(T& aData, const string& aInpName);
    protected:
	Func* mFunc;
};


/** @brief Agent function "Addition of Var data"
 * */
class TrAddVar: public TrVar
{
    public:
	static const char* Type() { return "TrAddVar";};
	TrAddVar(const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};

/** @brief Agent function "Max of Var data"
 * */
class TrMaxVar: public TrVar
{
    public:
	static const char* Type() { return "TrMaxVar";};
	TrMaxVar(const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};






#endif
