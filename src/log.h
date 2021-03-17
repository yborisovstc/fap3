#ifndef __FAP3_LOG_H
#define __FAP3_LOG_H

#include "mlog.h"

    /** Log recorder
     * */
    class Logrec: public MLogRec
    {
	public:
	    static const char* Type() { return "Logrec";};
	    Logrec(const string& aLogFileName);
	    virtual ~Logrec();
	public:
	    // From MLogRec/MIface
	    virtual string MLogRec_Uid() const override { return Type();}
	    virtual MIface* MLogRec_getLif(const char *aType) override { return nullptr;}
	    virtual void MLogRec_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	    // From MLogRec
	    virtual void WriteFormat(const char* aFmt,...);
	    virtual void Write(TLogRecCtg aCtg, const MNode* aNode, const char* aFmt,...);
	    virtual void Write(TLogRecCtg aCtg, MNode* aNode, const ChromoNode& aMut, const char* aFmt,...);
	    virtual void Flush();
	    virtual void SetContextMutId(int aMutId = -1);
	    virtual bool AddLogObserver(MLogObserver* aObs);
	    virtual void RemoveLogObserver(MLogObserver* aObs);
	    virtual int GetStat(TLogRecCtg aCtg) const;
	    virtual void Write(const TLog& aRec);
	protected:
	    void WriteRecord(const char* aText);
	    void WriteRecord(const string& aText);
	protected:
	    FILE* iLogFile;
	    string iLogFileName;
	    bool iLogFileValid;
	    MLogObserver* iObs;
	    int mCtxMutId;
	    int mStat[ECtg_Max];
	public:
	    static const int KLogRecBufSize;
    };

#endif
