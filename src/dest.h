
#ifndef __FAP3_DEST_H
#define __FAP3_DEST_H

#include "mdata.h"
#include "mdes.h"
#include "msyst.h"
#include "vert.h"
#include "func.h"
#include "des.h"
#include "rmutdata.h"

/** @brief Transition input. Helper class.
 * */
class FInp
{
    public:
	FInp(const string& aName): mIfp(nullptr), mName(aName) {}
    public:
	MIfProv* mIfp;       //<! Interface provider, not owned
	const string& mName; //<! Input name
};

/** @brief Transition function base 
 * */ 
class TrBase: public CpStateOutp, public MDVarGet, protected MDesInpObserver
{
    public:
	inline static constexpr std::string_view idStr() { return "TrBase"sv;}
    public:
	static vector<GUri> getParentsUri();
	TrBase(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node
	virtual MIface* MNode_getLif(TIdHash aTid) override;
	virtual MIface* MOwner_getLif(TIdHash aTid) override;
	vector<GUri> parentsUri() const override { return getParentsUri(); }
	// From MVert
	virtual MIface *MVert_getLif(TIdHash aTid) override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const override {return getUid<MConnPoint>();}
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual MIface* DoGetDObj(const char *aName) override { return nullptr;}
	virtual const DtBase* VDtGet(const string& aType) override;
    protected:
	// Local
	Func::TInpIc* GetInps(FInp& aInp);
	// From Unit.MIfProvOwner
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
	/** @brief Gets input data
	 * */
	template <class T> inline const T* GetInpData(FInp& aInp, const T* aData);
	virtual const DtBase* doVDtGet(const string& aType) { return nullptr;}
    protected:
	void AddInput(const string& aName);
	bool mCInv;              //!< Sign of data cache invalidated
	const DtBase* mResp = nullptr;
	MIfProv* mIobsIfProv = nullptr;
	MDVarGet* mMDVarGetPtr = nullptr;
	MDesInpObserver* mMDesInpObserverPtr = nullptr;
};

template <class T> inline const T* TrBase::GetInpData(FInp& aInp, const T* aData)
{
    const T* data = nullptr;
    Func::TInpIc* Ic = GetInps(aInp);
    if (Ic) {
	auto* get = (Ic->size() == 1) ? Ic->at(0) : nullptr;
	data = get ? get->DtGet(data) : nullptr;
	if (get && !data) { // There already is the log in GetInps
	    LOGN(EDbg, "Cannot get input [" + aInp.mName + "]");
	}
    }
    return data;
}

// Agent base of Var transition function
class TrVar: public TrBase, public Func::Host
{
    public:
	TrVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string MDVarGet_Uid() const override { return getUid<MDVarGet>();}
	virtual void MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual string VarGetIfid() const override;
	// From Func::Host
	virtual int GetInpCpsCount() const {return 0;}
	virtual string GetInpUri(int aId) const override;
	inline void log(int aCtg, const string& aMsg) override { LOGN(aCtg, aMsg); }
	virtual bool hostIsLogLevel(int aLevel) const override { return isLogLevel(aLevel);}
	virtual string getHostUri() const { return getUriS(nullptr);}
	virtual Func::TInpIc* GetInps(int aInpId) override;
    protected:
	// Local
	virtual void Init(const string& aIfaceName);
	virtual FInp* GetFinp(int aId) { return nullptr; }
	/** @brief Gets value of MDVarGet MDtGet input */
	template<typename T> bool DtGetInp(T& aData, const string& aInpName);
	/** @brief Gets value of MDVarGet MDtGet Sdata<T> input */
	template<typename T> bool DtGetSdataInp(T& aData, const string& aInpName);
	MDVarGet* GetInp(int aInpId);
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	Func* mFunc;
};

/** @brief Transition "Addition of Var data, negative inp, multi-connecting inputs"
 * Note: inputs validity isn't detected proreply, ref iss_014
 * 
 * */
class TrAddVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrAddVar"sv;}
    public:
	TrAddVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
	virtual int GetInpCpsCount() const override {return 2;}
    protected:
	const static string K_InpInp;
	const static string K_InpInpN;
	FInp mInp;
	FInp mInpN;
};

/** @brief Transition "Addition of Var data, single connection inputs"
 * */
class TrAdd2Var: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrAdd2Var"sv;}
    public:
	TrAdd2Var(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual string parentName() const override { return string(idStr()); }
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
	virtual int GetInpCpsCount() const override {return 2;}
    protected:
	const static string K_InpInp;
	const static string K_InpInp2;
	FInp mInp;
	FInp mInp2;
};

/** @brief Transition "Subtraction of Var data, single connection inputs"
 * */
class TrSub2Var: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrSub2Var"sv;}
    public:
	TrSub2Var(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
	virtual int GetInpCpsCount() const override {return 2;}
    protected:
	const static string K_InpInp;
	const static string K_InpInp2;
	FInp mInp;
	FInp mInp2;
};





/** @brief Transition "Multiplication of Var data"
 * */
class TrMplVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrMplVar"sv;}
    public:
	TrMplVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
	virtual int GetInpCpsCount() const override {return 2;}
    protected:
	const static string K_InpInp;
	FInp mInp;
};

/** @brief Transition "Division of Var data"
 * */
class TrDivVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrDivVar"sv;}
    public:
	TrDivVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From TrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
	virtual int GetInpCpsCount() const override {return 2;}
    protected:
	const static string K_InpInp;
	const static string K_InpInp2;
	FInp mInp;
	FInp mInp2;
};



/** @brief Agent function "Min of Var data"
 * */
class TrMinVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrMinVar"sv;}
    public:
	TrMinVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	const static string K_InpInp;
	FInp mInp;
};


/** @brief Agent function "Max of Var data"
 * */
class TrMaxVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrMaxVar"sv;}
    public:
	TrMaxVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	const static string K_InpInp;
	FInp mInp;
};


/** @brief Transition "Compare"
 * */
class TrCmpVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrCmpVar"sv;}
    public:
	TrCmpVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	FCmpBase::TFType GetFType();
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
	virtual int GetInpCpsCount() const override {return 2;}
    protected:
	const static string K_InpInp;
	const static string K_InpInp2;
	FInp mInp;
	FInp mInp2;
};

/** @brief Transition "Is equal"
 * */
// TODO Not completed. To complete
class TrEqVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrEqVar"sv;}
    public:
	TrEqVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
	virtual int GetInpCpsCount() const override {return 2;}
    protected:
	const static string K_InpInp;
	const static string K_InpInp2;
	FInp mInp;
	FInp mInp2;
};



/** @brief Agent function "Switcher"
 * */
class TrSwitchBool: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrSwitchBool"sv;}
    public:
	TrSwitchBool(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* doVDtGet(const string& aType) override;
	virtual string VarGetIfid() const override;
    protected:
	MDVarGet* GetInp();
    protected:
	FInp mInp1, mInp2, mSel;
	const static string K_InpInp1, K_InpInp2, K_InpSel;
};

/** @brief Agent function "Switcher" ver.2
 * Updated for solving desinp observing from non-selected inp, ref ds_dsps
 * Doens't improve the performance, mitigated by trans cache (ds_desopt_tdc) so
 * TrSwitchBool can be used instead.
 * */
class TrSwitchBool2: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrSwitchBool2"sv;}
    public:
	class IobsPx : public MDesInpObserver {
	    public:
		IobsPx(TrSwitchBool2* aHost): mHost(aHost) {}
		virtual string MDesInpObserver_Uid() const {return mHost->getUid<MDesInpObserver>() + "Px";}
		virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
		virtual void onInpUpdated() override { mHost->notifyInpsUpdated(this); }
		TrSwitchBool2* mHost;
	};
    friend class IobsPx;
    public:
	TrSwitchBool2(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* doVDtGet(const string& aType) override;
	virtual string VarGetIfid() const override;
    protected:
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
	MDVarGet* GetInp();
	void notifyInpsUpdated(const IobsPx* aPx);
    protected:
	FInp mInp1, mInp2, mSel;
	const static string K_InpInp1, K_InpInp2, K_InpSel;
	bool mSelV = false;
	bool mSelected = false;
	IobsPx mPx1, mPx2;
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
	FInp mInp;
	const static string K_InpInp;
	TData mRes;
};


/** @brief Agent function "Boolena AND of Var data"
 * */
class TrAndVar: public TrBool
{
    public:
	inline static constexpr std::string_view idStr() { return "TrAndVar"sv;}
    public:
	TrAndVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): TrBool(aType, aName, aEnv) {}
	virtual string parentName() const override { return string(idStr()); }
	virtual const DtBase* doVDtGet(const string& aType) override;
};

/** @brief Agent function "Boolena OR of Var data"
 * */
class TrOrVar: public TrBool
{
    public:
	inline static constexpr std::string_view idStr() { return "TrOrVar"sv;}
    public:
	TrOrVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): TrBool(aType, aName, aEnv) {}
	virtual const DtBase* doVDtGet(const string& aType) override;
};


/** @brief Agent function "Boolean negation of Var data"
 * */
class TrNegVar: public TrBool
{
    public:
	inline static constexpr std::string_view idStr() { return "TrNegVar"sv;}
    public:
	TrNegVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL): TrBool(aType, aName, aEnv) {}
	virtual const DtBase* doVDtGet(const string& aType) override;
};


/** @brief Transition agent "Convert to URI"
 * */
class TrToUriVar: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrToUriVar"sv;}
    public:
	using TRes = DGuri;
	using TInp = Sdata<string>;
    public:
	TrToUriVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual string VarGetIfid() const override { return TRes::TypeSig();}
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInp;
	const static string K_InpInp;
	TRes mRes;
};


/** @brief Transition agent "Append"
 * */
class TrApndVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrApndVar"sv;}
    public:
	TrApndVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	const static string K_InpInp1, K_InpInp2;
	FInp mInp1, mInp2;
};

/** @brief Transition agent "Select valid"
 * */
class TrSvldVar: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrSvldVar"sv;}
    public:
	TrSvldVar(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInp1, mInp2;
	static const string K_InpInp1, K_InpInp2;
};


/** @brief Transition agent "Tail"
 * */
class TrTailVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrTailVar"sv;}
    public:
	TrTailVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp, mInpHead;
	static const string K_InpInp, K_InpHead;
};

/** @brief Transition agent "Head"
 * */
class TrHeadVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrHeadVar"sv;}
    public:
	TrHeadVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp, mInpTail;
	static const string K_InpInp, K_InpTail;
};

/** @brief Transition agent "Head by tail length"
 * */
class TrHeadtnVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrHeadtnVar"sv;}
    public:
	TrHeadtnVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp, mInpTailn;
	static const string K_InpInp, K_InpTailn;
};


/** @brief Transition agent "Tail as num of elems"
 * */
class TrTailnVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrTailnVar"sv;}
    public:
	TrTailnVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp, mInpNum;
	static const string K_InpInp, K_InpNum;
};


/** @brief Transition "Getting container size"
 * */
class TrSizeVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrSizeVar"sv;}
    public:
	TrSizeVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void Init(const string& aIfaceName) override;
	virtual int GetInpCpsCount() const override {return 1;}
	virtual string VarGetIfid() const override;
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp;
	static const string K_InpInp;
};

/** @brief Agent function "Getting component, wrapping it by Sdata"
 * */
class TrAtVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrAtVar"sv;}
    public:
	TrAtVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void Init(const string& aIfaceName) override;
	virtual int GetInpCpsCount() const override {return 2;}
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp, mInpIndex;
	static const string K_InpInp, K_InpIndex;
};


/** @brief Agent function "Getting component, generic, no wrapping by Sdata"
 * */
class TrAtgVar: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrAtgVar"sv;}
    public:
	TrAtgVar(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void Init(const string& aIfaceName) override;
	virtual int GetInpCpsCount() const override {return 2;}
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp, mInpIndex;
	static const string K_InpInp, K_InpIndex;
};

/** @brief Transition "Find in Vert<Pair<T>> by first element of pair"
 * */
class TrFindByP: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrFindByP"sv;}
    public:
	TrFindByP(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void Init(const string& aIfaceName) override;
	virtual int GetInpCpsCount() const override {return 2;}
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpInp, mInpSample;
	static const string K_InpInp, K_InpSample;
};




/** @brief Transition agent "Tuple composer"
 * */
class TrTuple: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrTuple"sv;}
    public:
	TrTuple(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpInp;
	const static string K_InpInp;
	NTuple mRes;
};

/** @brief Transition agent "Tuple component selector"
 * */
class TrTupleSel: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrTupleSel"sv;}
    public:
	TrTupleSel(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpInp, mInpComp;
	const static string K_InpInp, K_InpComp;
};



/** @brief Transition agent "To string"
 * */
class TrTostrVar: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrTostrVar"sv;}
    using TRes = Sdata<string>;
    public:
	TrTostrVar(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TRes::TypeSig();}
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	TRes mRes;
	FInp mInpInp;
	const static string K_InpInp;
};


/** @brief Transition "Input selector"
 * */
class TrInpSel: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrInpSel"sv;}
    public:
	TrInpSel(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	MDVarGet* GetInp();
    protected:
	FInp mInpInp, mInpIdx;
	const static string K_InpInp, K_InpIdx;
};

/** @brief Transition "Inputs counter"
 * */
class TrInpCnt: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrInpCnt"sv;}
    public:
	using TRes = Sdata<int>;
	TrInpCnt(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpInp;
	TRes mRes;
	const static string K_InpInp;
};

/** @brief Agent function "Pair composer"
 * */
class TrPair: public TrVar
{
    public:
	inline static constexpr std::string_view idStr() { return "TrPair"sv;}
    public:
	TrPair(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual void Init(const string& aIfaceName) override;
	virtual int GetInpCpsCount() const override {return 2;}
	virtual FInp* GetFinp(int aId) override;
    protected:
	FInp mInpFirst, mInpSecond;
	const static string K_InpFirst, K_InpSecond;
};



/** @brief Agent functions "Mut composer" base
 * */
class TrMut: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrMut"sv;}
    public:
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
	inline static constexpr std::string_view idStr() { return "TrMutNode"sv;}
    public:
	enum { EInpName, EInpParent };
    public:
	TrMutNode(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpParent, mInpName;
	static const string K_InpParent, K_InpName;
};

/** @brief Agent function "Mut connect composer"
 * */
class TrMutConn: public TrMut
{
    public:
	inline static constexpr std::string_view idStr() { return "TrMutConn"sv;}
    public:
	using TInp = Sdata<string>;
	TrMutConn(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpCp1, mInpCp2;
	static const string K_InpCp1, K_InpCp2;
};

/** @brief Agent function "Mut content composer"
 * */
class TrMutCont: public TrMut
{
    public:
	inline static constexpr std::string_view idStr() { return "TrMutCont"sv;}
    public:
	static const char* Type() { return "TrMutCont";};
	TrMutCont(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpName, mInpValue, mInpTarget;
	static const string K_InpName, K_InpValue, K_InpTarget;
};


/** @brief Agent function "Mut disconnect composer"
 * */
class TrMutDisconn: public TrMut
{
    public:
	inline static constexpr std::string_view idStr() { return "TrMutDisconn"sv;}
    public:
	using TInp = Sdata<string>;
	TrMutDisconn(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpCp1, mInpCp2;
	static const string K_InpCp1, K_InpCp2;
};


/** @brief Agent function "Chromo composer"
 * */
class TrChr: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrChr"sv;}
    public:
	TrChr(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpBase, mInpMut;
	DChr2 mRes;
	static const string K_InpBase, K_InpMut;
};

/** @brief Agent function "Chromo composer from chromo"
 * */
class TrChrc: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrChrc"sv;}
    public:
	TrChrc(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override;
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpInp;
	DChr2 mRes;
	static const string K_InpInp;
};


/** @brief Transition "Data is valid"
 * */
class TrIsValid: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrIsValid"sv;}
    public:
	using TRes = Sdata<bool>;
    public:
	TrIsValid(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TRes::TypeSig(); }
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpInp;
	TRes mRes;
	static const string K_InpInp;
};

/** @brief Transition "Data is invalid"
 * */
class TrIsInvalid: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrIsInvalid"sv;}
    public:
	using TRes = Sdata<bool>;
    public:
	TrIsInvalid(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TRes::TypeSig(); }
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpInp;
	TRes mRes;
	static const string K_InpInp;
};



/** @brief Transition "Type enforcing"
 * !! NOT COMPLETED
 * */
class TrType: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrType"sv;}
    public:
	TrType(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual string VarGetIfid() const override;
    protected:
	static const string K_InpInp;
};


/** @brief Transition "Hash of data", ref ds_dcs_iui_tgh
 * */
class TrHash: public TrBase
{
    public:
	inline static constexpr std::string_view idStr() { return "TrHash"sv;}
    public:
	using TRes = Sdata<int>;
    public:
	static const char* Type() { return "TrHash";};
	TrHash(const string& aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const override { return TRes::TypeSig();}
	virtual const DtBase* doVDtGet(const string& aType) override;
    protected:
	FInp mInpInp;
	TRes mRes;
	static const string K_InpInp;
};

#endif
