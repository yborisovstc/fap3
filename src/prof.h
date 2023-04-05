
#ifndef __FAP3_PROF_H
#define __FAP3_PROF_H

#include <string>
#include <map>
#include <vector>
#include <cmath>

#include "mprof.h"
#include "prof_ids.h"

using namespace std;

/** @brief Perf indicators type IDs
 * */
enum {
    EPiid_Clock = 0,      //!< Clock
    EPiid_Dur = 1,        //!< Duration
    EPiid_DurStat = 2,    //!< Duration statistic
    EPiid_NUM,            //!< NUM of IDs
    EPiid_Undef
};

/** @brief Performance indicator. Base
 * */
class PindBase: public MPind
{
public:
    /** @brief Initialization data */
    struct Idata {
        Idata(const string& aDescr): mDescr(aDescr) {}
        string mDescr; //!< Description
    };
public:
    PindBase(const string& aDescr): mDescr(aDescr) {}
    virtual ~PindBase() {}
    // From MPind
    virtual string getDescription() const override { return mDescr;}
    /** @brief Obtains clock resolution in nanoseconds */
    virtual bool getClockResolution(TClock& aRes) const override;
    virtual void dump() const override {}
    virtual string toString() const override;
    virtual string fieldToString(TPItemFId aFId) const override {return string();}
    virtual string getFileSuf() const override {return string();}
    /** Gets clock absolute value */
    bool GetClock(TClock& aClock);
    static string field(const string& aField) { return aField + PRec::KFieldSep;}

protected:
    string mDescr;            //!< Description
    clockid_t mClockId = -1;  //!< Clock ID
};


/** @brief Standard performance indicator. Linear buffer based
 * */
class Pind: public PindBase
{
public:
    Pind(const string& aDescription, int aBufLenLIm): PindBase(aDescription), mBufLenLim(aBufLenLIm) {}
    virtual ~Pind() {}
public:
    // From MPind
    virtual int getBufLen() const override { return -1;}
    virtual int getBufLenLimit() const override { return mBufLenLim;}
    virtual bool saveToFile(const std::string& aPath) override;
    virtual int getId() const override {return -1;}
protected:
    int mBufLenLim; //!< Limit of buf len - predefined size of buffer
    int mPos = -1;  //!< Insertion position in events buffer
};

/** @brief Performance indicator item
 * */
class PindItem: public PindBase
{
    public:
	using TId = TPItemId; //!< ID type
    public:
	static const TId KUndefId = -1; //!< Undefined id
    public:
	/** @brief Initialization data */
	struct Idata : public PindBase::Idata {
	    Idata(TId aId, const string& aDescr): PindBase::Idata(aDescr), mId(aId) {}
	    TId mId = KUndefId; //!< Event Id
	};
    public:
	PindItem(const Idata& aIdata): PindBase(aIdata.mDescr), mId(aIdata.mId) {}
	virtual ~PindItem() {}
	virtual int getBufLen() const override { return -1;}
	virtual int getBufLenLimit() const override { return -1;}
	virtual string recToString(PEvent::TId aRecId) const override {return string();}
	virtual string toString() const override;
	virtual int getId() const override {return mId;}
	virtual bool saveToFile(const std::string &aPath) override { return false; }
	bool isErr() const {return mErr;}
    protected:
	TId mId = KUndefId; /*!< Event Id */ // TODO YB move to PindBase
	bool mErr = false; /*!< Error detected */
};


/** @brief Entries counter performance ind item
 * */
class PindDur : public PindItem {
public:
    static const TTid KTypeId = EPiid_Dur;
public:
    using TIdata = PindItem::Idata;
    using TDur = long int; /*!< Duration type */
    using TVal = TDur; /*!< Value type */
    using TRecParam = int; /*!< Recording parameter type */
    static string toTime(TDur aNs);

public:
    PindDur(const TIdata &aIdata) : PindItem(aIdata) {}
    TVal getValue() const {return mDur;}
    virtual ~PindDur() {}
    virtual void Start();
    virtual void Rec(TRecParam aPar);
    // From MPind
    virtual TTid tid() const { return KTypeId; }
    virtual int getBufLen() const override { return 1; }
    virtual int getBufLenLimit() const override { return -1; }
    virtual string getFileSuf() const override { return string(); }
    virtual bool saveToFile(const std::string &aPath) override { return false; }
    virtual string toString() const override;
    virtual string fieldToString(TPItemFId aFId) const override;

protected:
    TClock mIvStart; /*!< Interval start */
    TDur mDur = 0;  /*!< Duration, ns */
};


/** @brief Statistic indicator item
 * */
template <typename TInd, int Id>
class PindStat: public PindItem
{
public:
    static const int KTypeId = Id;
    using TIndValue = typename TInd::TVal;

public:
    /** @brief Initialization data
     * @param aId           ID of item
     * @param aDesc         description of item
     * @param aWnd          statistic processing window size, in samples
     * @param aInvSd        invalidate basing on standard deviation
     * */
    struct Idata : public PindItem::Idata {
        Idata(TId aId, const string& aDescr, size_t aWnd, bool aInvSd):
        PindItem::Idata(aId, aDescr), mWnd(aWnd), mInvSd(aInvSd) {}
        size_t mWnd = 0;       //!< Statistic processing window size, in samples
        bool mInvSd = false;   //!< Invalidate basing on standard deviation
    };
    using TIdata = Idata;
public:
    PindStat(const TIdata& aIdata): PindItem(aIdata), mPrmInd(aIdata), mWnd(aIdata.mWnd), mInvSd(aIdata.mInvSd) {}
    virtual ~PindStat() {}
    virtual void Start() {mPrmInd.Start();}
    virtual void Rec(typename TInd::TRecParam aPar);
    /** @brief Returns the sign of window is processed */
    virtual bool isDone() const { return mCount >= mWnd; }
    /** @brief Starts collecting statistic in the window */
    virtual void StartWnd();
    // From MPind
    virtual TTid tid() const { return KTypeId; }
    virtual string toString() const override;
    virtual string fieldToString(TPItemFId aFId) const override;
    virtual string getFileSuf() const override {return string();}
protected:
    TInd mPrmInd;        //!< Primary indicator
    TIndValue mSum = 0;  //!< Sum
    TIndValue mMin = 0;  //!< Minimum
    TIndValue mMax = 0;  //!< Maximum
    double mAvg = 0.0;   //!< Average, ns
    size_t mWnd = 0;     //!< Statistic processing window size, in samples
    int mCount = 1;      //!< Events counter
    int mErrCount = 0;   //!< Count of errors happened
    double mM2 = 0.0;    //!< The sum of squares of differences from the current mean
    double mSD = 0.0;    //!< Standard deviation
    bool mInvSd = false; //!< Invalidate basing on standard deviation
};

template <typename TInd, int Id>
void PindStat<TInd, Id>::Rec(typename TInd::TRecParam aPar) {
    mPrmInd.Rec(aPar);
    mErr = mPrmInd.isErr();
    TIndValue pval = mPrmInd.getValue(); // Primary ind value
    if (mInvSd && (mCount > 2)) {
        if (abs(pval - mAvg) > 5*mSD) {
            mErr = true;
        }
    }
    if (!mErr) {
        mSum += pval;
        if (mCount == 1) {
	    mMin = mMax = pval;
        } else {
            if (mMin > pval) {
                mMin = pval;
            }
            if (mMax < pval) {
                mMax = pval;
            }
        }
        if (mCount < mWnd) {
            double count = mCount;
            double avg = ((count - 1.0) / count) * mAvg + ((double) pval) / count;
            mM2 = mM2 + (pval - mAvg) * (pval - avg);
            mSD = sqrt(mM2/mCount);
            mAvg = avg;
            mCount++;
        }
    } else {
        mErrCount++;
    }
}

template <typename TInd, int Id>
void PindStat<TInd, Id>::StartWnd() {
    mCount = 1;
    mErrCount = 0;
    mSum = 0;
    mMin = 0;
    mMax = 0;
    mAvg = 0.0;
    mM2 = 0.0;
    mSD = 0.0;
}

template <typename TInd, int Id>
string PindStat<TInd, Id>::toString() const {
    return PindItem::toString() + field(fieldToString(PIndFId::EStat_CNT)) +
           field(fieldToString(PIndFId::EStat_MIN)) + field(fieldToString(PIndFId::EStat_MAX)) +
           field(fieldToString(PIndFId::EStat_SUM)) + field(fieldToString(PIndFId::EStat_AVG)) +
           field(fieldToString(PIndFId::EStat_SD)) + field(fieldToString(PIndFId::EStat_ERRCNT));
}

template <typename TInd, int Id>
string PindStat<TInd, Id>::fieldToString(TPItemFId aFId) const {
    switch(aFId) {
	case PIndFId::EStat_CNT:
	    return to_string(mCount - 1);
	case PIndFId::EStat_MIN:
	    return to_string(mMin);
	case PIndFId::EStat_MAX:
	    return to_string(mMax);
	case PIndFId::EStat_SUM:
	    return to_string((TIndValue)mSum);
	case PIndFId::EStat_AVG:
	    return to_string((TIndValue)mAvg);
	case PIndFId::EStat_SD:
	    return to_string((TIndValue) mSD);
	case PIndFId::EStat_ERRCNT:
	    return to_string(mErrCount);
    }
    return PindItem::fieldToString(aFId);
}


/** @brief Collection of performance indicators
 */
template<typename TItem>
class PindCluster : public Pind {
public:
    static const int KTypeId = TItem::KTypeId;
public:
    using TPitem = TItem;
    using TItems = map<PindItem::TId, TPitem*>;
    using TItemsElem = pair<PindItem::TId, TPitem*>;
    using TInitItems = vector<typename TPitem::TIdata>;
    /** Initialization data */
    struct Idata : public Pind::Idata {
        Idata(const string &aDescr, const TInitItems &aItemss): Pind::Idata(aDescr), mItems(aItemss) {}
        TInitItems mItems;
    };
    using TIdata = Idata;
public:
    PindCluster(const Idata &aIdata) : Pind(aIdata.mDescr, -1) {
        addItems(aIdata);
    }
    virtual ~PindCluster() { for (auto item : mItems) delete item.second;}
    // From MPind
    virtual TTid tid() const { return KTypeId; }
    virtual string getFileSuf() const override { return getDescription();}
    virtual int getBufLen() const override { return mItems.size();}
    virtual string recToString(int aRecNum) const override {
	auto* item = getItemByIdx(aRecNum);
	return item ? item->toString() : string();
    }
    virtual string toString() const override {return string();}
    //virtual bool saveToFile(const std::string& aPath) override {return false;}
public:
    void addItems(const Idata &aIdata) {
        for (auto itemInit : aIdata.mItems) {
            TItem *item = new TItem(itemInit);
            mItems.insert(TItemsElem(itemInit.mId, item));
        }
    }
    TPitem *getItem(PindItem::TId aId) const { return mItems.count(aId) > 0 ? mItems.at(aId) : NULL; }
    TPitem *getItemByIdx(int aIdx) const {
	TPitem* res = nullptr;
	if (aIdx < mItems.size()) {
	    auto it = mItems.begin();
	    for (int i = 0; i < aIdx; i++) {
		it++;
	    }
	    res = it->second;
	}
	return res;
    }
protected:
    TItems mItems;
};

class PindDurStat: public PindStat<PindDur, EPiid_DurStat> {
public:
    PindDurStat(const TIdata& aIdata): PindStat<PindDur, EPiid_DurStat>(aIdata) {}
    virtual string fieldToString(TPItemFId aFId) const override;
};


class MEnv;

/** Profiler base */
class ProfBase: public MProfiler
{
public:
    ProfBase(MEnv* aEnv, const std::string& aPathBase): mEnv(aEnv),mPathBase(aPathBase) {}
    virtual ~ProfBase() {}
    // From MProfiler
    virtual void enable() override;
    virtual void saveMetrics() override;
public:
    template <typename T> inline T* Pfi(PindItem::TId aId);
protected:
    //void log(int aLevel, const string& aData) { mEnv->Logger()->WriteFormat("%s", aData.c_str()); }
protected:
    MEnv* mEnv;             //!< System environment
    std::string mPathBase;  //!< File path base
};


/** Default profiler */
template <int Dim>
class DProf: public ProfBase
{
public:
    /** Type of performance indicators register
     *  The key is id of indicator
     */
    using TPinds = array<MPind*, Dim>;
    using TPindsElem = pair<MPind::TTid, MPind*>;
public:
    static MProfiler* defaultProfiler();
    DProf(MEnv* aEnv, const std::string& aPathBase): ProfBase(aEnv, aPathBase), mPinds{} {}
    virtual ~DProf() {
	for (auto ind : mPinds) {
	    delete ind;
	}
    }
    // From MProfiler
    virtual MPind* getPind(int aId) override { return mPinds.at(aId);}
    virtual void setPind(int aId, MPind* aPind) override { mPinds[aId] = aPind; }
    virtual int pindsCount() const override { return Dim;}
protected:
    static DProf* mProf;    //!< Default profiler
    TPinds mPinds;          //!< Performance indicators
};

/** Default common profiler instance */
class DProfInst
{
public:
    static void init(const std::string& aPathBase);
    static void init(MProfiler *aProf);
    static MProfiler* profiler() { return prof(); }
protected:
    static MProfiler* prof();
    static MProfiler* mProf;    //!< Default profiler
};

// Profiler's routines
#ifdef PROFILING_ENABLED
#define PROF_SAVE(PFL) if (PFL) PFL->saveMetrics()
#define PROF_DUR_START(PFL, TYPE, ID) if (PFL) PFL->getPindItem<PindCluster, TYPE>(ID)->Start()
#define PROF_DUR_REC(PFL, TYPE, ID) if (PFL) PFL->getPindItem<PindCluster, TYPE>(ID)->Rec(0)
#define PROF_VALUE(PFL, TYPE, ID) ((PFL != nullptr) ? PFL->getPindItem<PindCluster, TYPE>(ID)->toString() : string())
#define PROF_FIELD(PFL, TYPE, ID, FID) ((PFL != nullptr) ? PFL->getPindItem<PindCluster, TYPE>(ID)->fieldToString(FID) : string())
#else
#define PROF_SAVE(PFL) ((void)0)
#define PROF_DUR_START(PFL, TYPE, ID) ((void)0)
#define PROF_DUR_REC(PFL, TYPE, ID) ((void)0)
#define PROF_VALUE(PFL, TYPE, ID) string()
#define PROF_FIELD(PFL, TYPE, ID, FID) string()
#endif

#define PROF_DUR PindDur
#define PROF_DUR_STAT PindDurStat

// Common profiler's routines
#ifdef PROFILING_ENABLED
#define PFLC() DProfInst::profiler()
#define PFLC_INIT(PROF) DProfInst::init(PROF)
#else
#define PFLC() ((void)0)
#define PFLC_INIT(PROF) ((void)0)
#endif
#define PFLC_SAVE() PROF_SAVE(PFLC())
#define PFLC_DUR_STAT(FUN, ID) PROF_DUR_##FUN(PFLC(), PROF_DUR_STAT, ID)
#define PFLC_DUR_STAT_F(FUN, ID, FID) PROF_DUR_##FUN(PFLC(), PROF_DUR_STAT, ID, FID)
#define PFLC_DUR(FUN, ID) PROF_DUR_##FUN(PFLC(), PROF_DUR, ID)

#define LOG(...) mEnv->Logger()->WriteFormat(__VA_ARGS__)

template <typename T> inline T* ProfBase::Pfi(PindItem::TId aId) { return getPind<PindCluster<T>>().getItem(aId);}

#endif
