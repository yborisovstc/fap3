
#ifndef __FAP3_DEST_H
#define __FAP3_DEST_H

#include "mdata.h"
#include "mdes.h"
#include "msyst.h"
#include "vert.h"
#include "func.h"
#include "des.h"
#include "rmutdata.h"


/** @brief Transition function base 
 * */ 
class TrBase: public CpStateOutp, protected MDesInpObserver
{
    public:
	static const char* Type() { return "TrBase";}
	TrBase(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node
	virtual MIface* MOwner_getLif(const char *aType) override;
	// From MVert
	virtual MIface *MVert_getLif(const char *aType) override;
	//virtual bool isCompatible(MVert* aPair, bool aExt) override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const override {return getUid<MConnPoint>();}
	//virtual string provName() const override;
	//virtual string reqName() const override;
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
    protected:
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    protected:
	void AddInput(const string& aName);
	virtual string GetInpUri(int aId) const = 0;
	template<typename T> bool GetInpSdata(int aId, T& aRes);
	template<typename T> bool GetInpData(int aId, T& aRes);
};


// Agent base of Var transition function
class TrVar: public TrBase, public MDVarGet, public Func::Host
{
    public:
	static const char* Type() { return "TrVar";};
	TrVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual void MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual MIface* DoGetDObj(const char *aName) override;
	virtual string VarGetIfid() const override;
	// From Func::Host
	virtual void OnFuncContentChanged() override;
	virtual int GetInpCpsCount() const {return 0;}
	virtual void log(int aCtg, const string& aMsg);
	virtual MIfProv::TIfaces* GetInps(int aId, const string& aIfName, bool aOpt) override;
    protected:
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(int aId) const override;
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
	TrAddVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	virtual int GetInpCpsCount() const override {return 2;}
};


/** @brief Transition "Multiplication of Var data"
 * */
class TrMplVar: public TrVar
{
    public:
	static const char* Type() { return "TrMplVar";};
	TrMplVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	virtual int GetInpCpsCount() const override {return 2;}
};



/** @brief Agent function "Max of Var data"
 * */
class TrMaxVar: public TrVar
{
    public:
	static const char* Type() { return "TrMaxVar";};
	TrMaxVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};


/** @brief Transition "Compare"
 * */
class TrCmpVar: public TrVar
{
    public:
	static const char* Type() { return "TrCmpVar";};
	TrCmpVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	FCmpBase::TFType GetFType();
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From Func::Host
	virtual int GetInpCpsCount() const override {return 2;}
};


/** @brief Agent function "Switcher"
 * */
class TrSwitchBool: public TrVar
{
    public:
	static const char* Type() { return "TrSwitchBool";};
	TrSwitchBool(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrcVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From MDVarGet
	virtual MIface* DoGetDObj(const char *aName) override;
};

/** @brief Agent function "Boolena AND of Var data"
 * */
class TrAndVar: public TrVar
{
    public:
	static const char* Type() { return "TrAndVar";};
	TrAndVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};

/** @brief Agent function "Boolena OR of Var data"
 * */
class TrOrVar: public TrVar
{
    public:
	static const char* Type() { return "TrOrVar";};
	TrOrVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};


/** @brief Agent function "Boolena negation of Var data"
 * */
class TrNegVar: public TrVar
{
    public:
	static const char* Type() { return "TrNegVar";};
	TrNegVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};


/** @brief Transition agent "Convert to URI"
 * */
class TrUri: public TrVar
{
    public:
	static const char* Type() { return "TrUri";};
	TrUri(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From TrVar.MDVarGet
	virtual string VarGetIfid() const override;
};


/** @brief Transition agent "Append"
 * */
class TrApndVar: public TrVar
{
    public:
	static const char* Type() { return "TrApndVar";}
	TrApndVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};


/** @brief Transition agent "Select valid"
 * */
class TrSvldVar: public TrVar
{
    public:
	static const char* Type() { return "TrSvldVar";}
	TrSvldVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};

/** @brief Transition agent "Tail"
 * */
class TrTailVar: public TrVar
{
    public:
	static const char* Type() { return "TrTailVar";}
	TrTailVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};

/** @brief Transition agent "Head"
 * */
class TrHeadVar: public TrVar
{
    public:
	static const char* Type() { return "TrHeadVar";}
	TrHeadVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};






/** @brief Transition "Getting container size"
 * */
class TrSizeVar: public TrVar
{
    public:
	static const char* Type() { return "TrSizeVar";};
	TrSizeVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From Func::Host
	virtual int GetInpCpsCount() const override {return 1;}
	// From MDVarGet
	virtual string VarGetIfid() const override;
};

/** @brief Agent function "Getting component"
 * */
class TrAtVar: public TrVar
{
    public:
	static const char* Type() { return "TrAtVar";};
	TrAtVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From Func::Host
	virtual int GetInpCpsCount() const override {return 2;}
};


/** @brief Agent functions "Tuple composer"
 * */
class TrTuple: public TrBase, public MDVarGet, public MDtGet<NTuple>
{
    public:
	static const char* Type() { return "TrTuple";};
	TrTuple(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override;
	virtual string VarGetIfid() const override;
	// From MDtGet
	virtual void DtGet(NTuple& aData) override;
    protected:
	virtual string GetInpUri(int aId) const override { return string();} 
    protected:
	NTuple mRes;  /*<! Cached result */
	const static string K_InpName;
};

/** @brief Transition agent "To string"
 * */
class TrTostrVar: public TrVar
{
    public:
	static const char* Type() { return "TrTostrVar";}
	TrTostrVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
};





/** @brief Agent functions "Mut composer" base
 * */
class TrMut: public TrBase, public MDVarGet, public MDtGet<DMut>
{
    public:
	static const char* Type() { return "TrMut";};
	TrMut(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual string GetInpUri(int aId) const = 0;
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override;
	virtual string VarGetIfid() const override;
    protected:
	DMut mRes;  /*<! Cached result */
};


/** @brief Agent function "Mut Node composer"
 * */
class TrMutNode: public TrMut
{
    public:
	enum { EInpName, EInpParent };
    public:
	static const char* Type() { return "TrMutNode";};
	TrMutNode(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrcMut
	virtual string GetInpUri(int aId) const override;
	// From MDtGet
	virtual void DtGet(DMut& aData) override;
};

/** @brief Agent function "Mut connect composer"
 * */
class TrMutConn: public TrMut
{
    public:
	enum { EInpCp1, EInpCp2 };
    public:
	static const char* Type() { return "TrMutConn";};
	TrMutConn(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrcMut
	virtual string GetInpUri(int aId) const override;
	// From MDtGet
	virtual void DtGet(DMut& aData) override;
};

/** @brief Agent function "Mut content composer"
 * */
class TrMutCont: public TrMut
{
    public:
	enum { EInpTarget, EInpName, EInpValue };
    public:
	static const char* Type() { return "TrMutCont";};
	TrMutCont(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrcMut
	virtual string GetInpUri(int aId) const override;
	// From MDtGet
	virtual void DtGet(DMut& aData) override;
};


/** @brief Agent function "Mut disconnect composer"
 * */
class TrMutDisconn: public TrMut
{
    public:
	enum { EInpCp1, EInpCp2 };
    public:
	static const char* Type() { return "TrMutDisconn";};
	TrMutDisconn(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrcMut
	virtual string GetInpUri(int aId) const override;
	// From MDtGet
	virtual void DtGet(DMut& aData) override;
};



/** @brief Agent function "Chromo composer"
 * */
class TrChr: public TrBase, public MDVarGet, public MDtGet<DChr2>
{
    public:
	enum { EInpBase, EInpMut };
    public:
	static const char* Type() { return "TrChr";};
	TrChr(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override;
	virtual string VarGetIfid() const override;
	// From MDtGet
	virtual void DtGet(DChr2& aData) override;
    protected:
	// From TrBase
	virtual string GetInpUri(int aId) const override;
    protected:
	DChr2 mRes;  /*<! Cached result */
};

/** @brief Agent function "Chromo composer from chromo"
 * */
class TrChrc: public TrBase, public MDVarGet, public MDtGet<DChr2>
{
    public:
	enum { EInp };
    public:
	static const char* Type() { return "TrChrc";};
	TrChrc(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override;
	virtual string VarGetIfid() const override;
	// From MDtGet
	virtual void DtGet(DChr2& aData) override;
    protected:
	// From TrBase
	virtual string GetInpUri(int aId) const override;
    protected:
	DChr2 mRes;  /*<! Cached result */
};


#if 0
/** @brief Agent function "Data is valid"
 * */
class TrIsValid: public TrBase, public MDVarGet, public MDtGet<Sdata<bool>>
{
    public:
	enum { EInp };
	using TSdata = Sdata<bool>;
	using TDget = MDtGet<TSdata>;
    public:
	static const char* Type() { return "TrIsValid";};
	TrIsValid(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override {
	    return (string(TDget::Type()).compare(aName) == 0) ? dynamic_cast<TDget*>(this) : nullptr; }
	virtual string VarGetIfid() const override { return TDget::Type(); }
	// From MDtGet
	virtual void DtGet(TSdata& aData) override;
    protected:
	// From TrBase
	virtual string GetInpUri(int aId) const override;
    protected:
	TSdata mRes;  /*<! Cached result */
};

#endif

/** @brief Agent function "Is data valid"
 * */
class TrIsValid: public TrVar
{
    public:
	using TOData = Sdata<bool>;
	using TOGetData = MDtGet<TOData>;
	static const char* Type() { return "TrIsValid";};
	TrIsValid(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From Func::Host
	virtual int GetInpCpsCount() const override {return 1;}
};










#endif
