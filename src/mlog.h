#ifndef __FAP3_MLOG_H
#define __FAP3_MLOG_H

#include "miface.h"
#include "ifu.h"
#include <string>

class MNode;
class MLogObserver;
class ChromoNode;

/** @brief Scope of the logging parameters
 * */
enum TLogScope {
    ELsLocal = 0,   //<!  Local scope, applied to the current native hier node
    ELsGlobas = 1,  //<!  Global scope, applied to the whole owned tree
};

enum TLogRecCtg {
    ELcUndef = 0,
    ENoLog = 1,
    EErr = 2,
    EWarn = 10,
    EInfo = 20,
    EDbg = 30,
    EDbg2 = 31,
    EAll = 50,
    ECtg_Max
};

/** @brief Log record structure
 * */
class TLog
{
    public:
	TLog(int aCtg, const MNode* aAgt);
	TLog(int aCtg, const MNode* aAgt, const ChromoNode& aMut);
	TLog(const MNode* aAgt);
	TLog(const MNode* aAgt, const ChromoNode& aMut);
	TLog(const string& aString);
	TLog& operator +(const string& aString);
	operator string() const;
	string TimestampS() const;
	string NodeUriS() const;
	int MutId() const;
	int Ctg() const { return mCtg;}
	string MutIdS() const;
	string CtgS() const;
	string Content() const { return mContent;};
	void SetCtg(int aCtg);
    protected:
	int mCtg;
	string mTimestampS;
	string mCtgS;
	string mNodeUriS;
	string mMutIdS;
	string mContent;
};


/** @brief Log recorder interface
 * */
class MLogRec: public MIface
{
    public:
	// From MIface
	virtual string Uid() const override { return MLogRec_Uid();}
	virtual string MLogRec_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MLogRec_getLif(aType);}
	virtual MIface* MLogRec_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MLogRec_doDump(aLevel, aIdt, std::cout);}
	virtual void MLogRec_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual void WriteFormat(const char* aFmt,...) = 0;
	virtual void Write(TLogRecCtg aCtg, const MNode* aNode, const char* aFmt,...) = 0;
	virtual void Write(TLogRecCtg aCtg, MNode* aNode, const ChromoNode& aMut, const char* aFmt,...) = 0;
	virtual void SetContextMutId(int aMutId = -1) = 0;
	virtual void Flush() = 0;
	virtual bool AddLogObserver(MLogObserver* aObs) = 0;
	virtual void RemoveLogObserver(MLogObserver* aObs) = 0;
	virtual int GetStat(TLogRecCtg aCtg) const = 0;
	virtual void Write(const TLog& aRec) = 0;
};

// Log observer
// TODO [YB] Thre observer-observable relation here is actually of one-way. So do we need
// OnLogRecDeleting, AddObservable, RemoveObservable?
class MLogObserver
{
    public:
	virtual void AddObservable(MLogRec* aObservable) = 0;
	virtual void RemoveObservable(MLogRec* aObservable) = 0;
	virtual void OnLogAdded(long aTimeStamp, TLogRecCtg aCtg, const MNode* aNode, const std::string& aContent, int aMutId = 0) = 0;
	virtual void OnLogAdded(const TLog& aLog) = 0;
	virtual void OnLogRecDeleting(MLogRec* aLogRec) = 0;
};

#endif
