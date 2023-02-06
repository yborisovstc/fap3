
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
class TrBase: public CpStateOutp, public MDVarGet, protected MDesInpObserver
{
    public:
	static const char* Type() { return "TrBase";}
	TrBase(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node
	virtual MIface* MNode_getLif(const char *aType) override;
	virtual MIface* MOwner_getLif(const char *aType) override;
	// From MVert
	virtual MIface *MVert_getLif(const char *aType) override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const override {return getUid<MConnPoint>();}
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
    protected:
	// Local
	Func::TInpIc* GetInps(const string& aInpName, bool aOpt = false);
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	/** @brief Gets input data
	 * */
	template <class T> inline const T* GetInpData(Func::TInpIc*& aIc, const string& aName, const T* aData);
    protected:
	void AddInput(const string& aName);
	virtual string GetInpUri(int aId) const { assert(false);}
	template<typename T> bool GetInpSdata(int aId, T& aRes);
	template<typename T> bool GetInpData(int aId, T& aRes);
};

template <class T> inline const T* TrBase::GetInpData(Func::TInpIc*& aIc, const string& aName, const T* aData)
{
    if (!aIc) aIc = GetInps(aName);
    auto* get = (aIc && aIc->size() == 1) ? aIc->at(0) : nullptr;
    const T* data = get ? get->DtGet(data) : nullptr;
    return data;
}

// Agent base of Var transition function
class TrVar: public TrBase, public Func::Host
{
    public:
	static const char* Type() { return "TrVar";};
	TrVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual void MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
	// From Func::Host
	virtual void OnFuncContentChanged() override;
	virtual int GetInpCpsCount() const {return 0;}
	virtual void log(int aCtg, const string& aMsg);
	virtual string getHostUri() const { return getUriS(nullptr);}
	virtual MIfProv::TIfaces* GetInps(int aId, const string& aIfName, bool aOpt) override;
	virtual Func::TInpIc* GetInps(int aInpId, bool aOpt) override;
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

/** @brief Transition "Division of Var data"
 * */
class TrDivVar: public TrVar
{
    public:
	static const char* Type() { return "TrDivVar";};
	TrDivVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	virtual int GetInpCpsCount() const override {return 2;}
};



/** @brief Agent function "Min of Var data"
 * */
class TrMinVar: public TrVar
{
    public:
	static const char* Type() { return "TrMinVar";};
	TrMinVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
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
class TrSwitchBool: public TrBase
{
    public:
	static const char* Type() { return "TrSwitchBool";};
	TrSwitchBool(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
	virtual string VarGetIfid() const override;
    protected:
	MDVarGet* GetInp();
    protected:
	Func::TInpIc* mInpInp1Ic;
	Func::TInpIc* mInpInp2Ic;
	Func::TInpIc* mInpSelIc;
	const static string K_InpInp1;
	const static string K_InpInp2;
	const static string K_InpSel;
};

class TrBool: public TrBase
{
    public:
	using TData = Sdata<bool>;
    public:
	TrBool(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TData::TypeSig();}
    protected:
	Func::TInpIc* mInpIc;
	const static string K_InpInp;
	TData mRes;
};




/** @brief Agent function "Boolena AND of Var data"
 * */
class TrAndVar: public TrBool
{
    public:
	static const char* Type() { return "TrAndVar";};
	TrAndVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): TrBool(aType, aName, aEnv) {}
	virtual const DtBase* VDtGet(const string& aType) override;
};

/** @brief Agent function "Boolena OR of Var data"
 * */
class TrOrVar: public TrBool
{
    public:
	static const char* Type() { return "TrOrVar";};
	TrOrVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): TrBool(aType, aName, aEnv) {}
	virtual const DtBase* VDtGet(const string& aType) override;
};


/** @brief Agent function "Boolena negation of Var data"
 * */
class TrNegVar: public TrBool
{
    public:
	static const char* Type() { return "TrNegVar";};
	TrNegVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): TrBool(aType, aName, aEnv) {}
	virtual const DtBase* VDtGet(const string& aType) override;
};


/** @brief Transition agent "Convert to URI"
 * */
class TrToUriVar: public TrBase
{
    public:
	using TRes = DGuri;
	using TInp = Sdata<string>;
    public:
	static const char* Type() { return "TrToUriVar";};
	TrToUriVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual string VarGetIfid() const override { return TRes::TypeSig();}
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpIc;
	const static string K_InpInp;
	TRes mRes;
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
class TrSvldVar: public TrBase
{
    public:
	static const char* Type() { return "TrSvldVar";};
	TrSvldVar(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInp1Ic;
	Func::TInpIc* mInp2Ic;
	static const string K_InpInp1;
	static const string K_InpInp2;
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

/** @brief Transition agent "Tail as num of elems"
 * */
class TrTailnVar: public TrVar
{
    public:
	static const char* Type() { return "TrTailnVar";}
	TrTailnVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
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

/** @brief Agent function "Getting component, wrapping it by Sdata"
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


/** @brief Agent function "Getting component, generic, no wrapping by Sdata"
 * */
class TrAtgVar: public TrVar
{
    public:
	static const char* Type() { return "TrAtgVar";};
	TrAtgVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From Func::Host
	virtual int GetInpCpsCount() const override {return 2;}
};



/** @brief Transition agent "Tuple composer"
 * */
class TrTuple: public TrBase
{
    public:
	static const char* Type() { return "TrTuple";};
	TrTuple(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	virtual string GetInpUri(int aId) const override { return string();}
    protected:
	Func::TInpIc* mInpIc;
	NTuple mRes;
	const static string K_InpName;
};

/** @brief Transition agent "Tuple component selector"
 * */
class TrTupleSel: public TrBase
{
    public:
	static const char* Type() { return "TrTupleSel";};
	TrTupleSel(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	virtual string GetInpUri(int aId) const override { return string();}
    protected:
	Func::TInpIc* mInpIc;
	Func::TInpIc* mInpCIc;
	const static string K_InpName;
	const static string K_InpCompName;
};



/** @brief Transition agent "To string"
 * */
class TrTostrVar: public TrBase
{
    using TRes = Sdata<string>;
    public:
	static const char* Type() { return "TrTostrVar";};
	TrTostrVar(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TRes::TypeSig();}
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	TRes mRes;
	Func::TInpIc* mInpIc;
	const static string K_InpInp;
};


/** @brief Transition "Input selector"
 * */
class TrInpSel: public TrBase
{
    public:
	static const char* Type() { return "TrInpSel";};
	TrInpSel(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	MDVarGet* GetInp();
    protected:
	Func::TInpIc* mInpIc;
	Func::TInpIc* mInpIdxIc;
	const static string K_InpInp;
	const static string K_InpIdx;
};

/** @brief Transition "Inputs counter"
 * */
class TrInpCnt: public TrBase
{
    public:
	static const char* Type() { return "TrInpCnt";};
	TrInpCnt(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpIc;
	Sdata<int> mRes;
	const static string K_InpInp;
};

/** @brief Agent function "Pair composer"
 * */
class TrPair: public TrVar
{
    public:
	static const char* Type() { return "TrPair";};
	TrPair(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(int aId) const override;
	// From Func::Host
	virtual int GetInpCpsCount() const override {return 2;}
};



/** @brief Agent functions "Mut composer" base
 * */
class TrMut: public TrBase
{
    public:
	static const char* Type() { return "TrMut";};
	TrMut(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual string GetInpUri(int aId) const {return string();}
	// From MDVarGet
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
	// From MDtGet
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpParentIc;
	Func::TInpIc* mInpNameIc;
	static const string K_InpParent;
	static const string K_InpName;
};

/** @brief Agent function "Mut connect composer"
 * */
class TrMutConn: public TrMut
{
    public:
	using TInp = Sdata<string>;
	static const char* Type() { return "TrMutConn";};
	TrMutConn(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpCp1Ic;
	Func::TInpIc* mInpCp2Ic;
	static const string K_InpCp1;
	static const string K_InpCp2;
};

/** @brief Agent function "Mut content composer"
 * */
class TrMutCont: public TrMut
{
    public:
	static const char* Type() { return "TrMutCont";};
	TrMutCont(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpNameIc;
	Func::TInpIc* mInpValueIc;
	Func::TInpIc* mInpTargetIc;
	static const string K_InpName;
	static const string K_InpValue;
	static const string K_InpTarget;
};


/** @brief Agent function "Mut disconnect composer"
 * */
class TrMutDisconn: public TrMut
{
    public:
	using TInp = Sdata<string>;
	static const char* Type() { return "TrMutDisconn";};
	TrMutDisconn(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpCp1Ic;
	Func::TInpIc* mInpCp2Ic;
	static const string K_InpCp1;
	static const string K_InpCp2;
};



/** @brief Agent function "Chromo composer"
 * */
class TrChr: public TrBase
{
    public:
	static const char* Type() { return "TrChr";};
	TrChr(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpBaseIc;
	Func::TInpIc* mInpMutIc;
	DChr2 mRes;  /*<! Cached result */
	static const string K_InpBase;
	static const string K_InpMut;
};

/** @brief Agent function "Chromo composer from chromo"
 * */
class TrChrc: public TrBase
{
    public:
	static const char* Type() { return "TrChrc";};
	TrChrc(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpIc;
	DChr2 mRes;  /*<! Cached result */
	static const string K_InpInp;
};


/** @brief Transition "Data is valid"
 * */
class TrIsValid: public TrBase
{
    public:
	using TRes = Sdata<bool>;
    public:
	static const char* Type() { return "TrIsValid";};
	TrIsValid(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TRes::TypeSig(); }
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpIc;
	TRes mRes;
	static const string K_InpInp;
};


/** @brief Transition "Type enforcing"
 * !! NOT COMPLETED
 * */
class TrType: public TrBase
{
    public:
	enum {EInp};
    public:
	static const char* Type() { return "TrType";};
	TrType(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override;
	virtual string VarGetIfid() const override;
    protected:
	// From TrBase
	virtual string GetInpUri(int aId) const override;
};


/** @brief Transition "Hash of data", ref ds_dcs_iui_tgh
 * */
class TrHash: public TrBase
{
    public:
	using TRes = Sdata<int>;
    public:
	static const char* Type() { return "TrHash";};
	TrHash(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TRes::TypeSig();}
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	Func::TInpIc* mInpIc;
	TRes mRes;
	static const string K_InpInp;
};

#endif
