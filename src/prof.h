
#ifndef __FAP3_PROF_H
#define __FAP3_PROF_H

#include <string>
#include <map>
#include <vector>
#include <cmath>

#include "mprof.h"

using namespace std;

/** @brief Perf indicators IDs
 * */
enum {
    EPiid_Clock = 0,      //!< Clock
    EPiid_Dur = 1,        //!< Duration
    EPiid_DurStat = 2,    //!< Duration statistic
    EPiid_NUM ,            //!< NUM of IDs
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
    virtual string getDescription() override { return mDescr;}
    /** @brief Obtains clock resolution in nanoseconds */
    virtual bool getClockResolution(TClock& aRes) const override;
    virtual void dump() const override {}
    virtual string toString() const override;
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
protected:
    TInd mPrmInd;        //!< Primary indicator
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
    mAvg = 0.0;
    mM2 = 0.0;
    mSD = 0.0;
}

template <typename TInd, int Id>
string PindStat<TInd, Id>::toString() const {
    return PindItem::toString() + field(to_string(mCount)) + field(to_string(mPrmInd.getValue())) +
           field(to_string((TIndValue) mAvg)) +
           field(to_string((TIndValue)(mSD))) + field(to_string(mErrCount));
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
        for (auto itemInit : aIdata.mItems) {
            TItem *item = new TItem(itemInit);
            mItems.insert(TItemsElem(itemInit.mId, item));
        }
    }
    virtual ~PindCluster() { for (auto item : mItems) delete item.second;}
    // From MPind
    virtual TTid tid() const { return KTypeId; }
    virtual string getFileSuf() const override {return string();}
    virtual int getBufLen() const override { return mItems.size();}
    virtual string recToString(int aRecNum) const override {
	auto* item = getItem(aRecNum);
	return item ? item->toString() : string();
    }
    virtual string toString() const override {return string();}
    //virtual bool saveToFile(const std::string& aPath) override {return false;}
public:
    TPitem *getItem(PindItem::TId aId) const { return mItems.count(aId) > 0 ? mItems.at(aId) : NULL; }
protected:
    TItems mItems;
};

using PindDurStat = PindStat<PindDur, EPiid_DurStat>;


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
    using TPinds = array<PindBase*, Dim>;
    using TPindsElem = pair<MPind::TTid, PindBase*>;
public:
    DProf(MEnv* aEnv, const std::string& aPathBase): ProfBase(aEnv, aPathBase), mPinds{} {}
    virtual ~DProf() {
	for (auto ind : mPinds) {
	    delete ind;
	}
    }
    // From MProfiler
    virtual MPind* getPind(int aId) override { return mPinds.at(aId);}
    virtual int pindsCount() const override { return Dim;}
    template<typename T> void AddPind(const typename T::TIdata& aIdata) {
        MPind* curPind = getPind(T::KTypeId);
        if (!curPind) {
            mPinds[T::KTypeId] = new T(aIdata);
        }
    }
public:
    // Profiler helpers
    virtual void DurStatStart(TPItemId aId) { Pfi<PindDurStat>(aId)->Start();}
    virtual void DurStatRec(TPItemId aId) { Pfi<PindDurStat>(aId)->Rec(0);}
protected:
    TPinds mPinds;          //!< Performance indicators
};

#define LOG(...) mEnv->Logger()->WriteFormat(__VA_ARGS__)

template <typename T> inline T* ProfBase::Pfi(PindItem::TId aId) { return getPind<PindCluster<T>>().getItem(aId);}


#endif
