#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <stdexcept> 
#include <stdarg.h> 
#include <iomanip>

#include "log.h"
#include "guri.h"
#include "mnode.h"
#include "chromo.h"

const int TLog::KPrecision = 6;
const int Logrec::KLogRecBufSize = 1400;

const char* KColSep = ";";
const char KFieldSep = ';';

static const string CtgText(int aCtg)
{
    string txt;
    if (aCtg >= EErr && aCtg < EWarn) txt = "ERR";
    else if (aCtg >= EWarn && aCtg < EInfo) txt = "WRN";
    else if (aCtg >= EInfo && aCtg < EDbg) txt = "INF";
    else if (aCtg >= EDbg) txt = "DBG";
    return txt;
}

string TLogGetField(const string& aPack, size_t& aBeg, bool aESep = true)
{
    size_t end = aPack.find_first_of(KFieldSep, aBeg);
    assert(!aESep || end != string::npos);
    string res = aPack.substr(aBeg, end - aBeg);
    aBeg = end + 1;
    return res;
}

TLog::TLog(int aCtg, const MNode* aAgt): mCtg(aCtg)
{
    stringstream ssn;
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
    ssn << setfill('0') << setw(9) << ts.tv_nsec;
    mTimestampS = string(buff) + "." + ssn.str().substr(0, KPrecision);
    mCtgS = CtgText(mCtg);
    if (aAgt != NULL) {
	GUri uri;
	aAgt->getUri(uri);
	mNodeUriS = uri.toString();
    }
}

TLog::TLog(int aCtg, const MNode* aAgt, const string& aContent): TLog(aCtg, aAgt)
{
    mContent = aContent;
}

TLog::TLog(const MNode* aAgt): TLog(EAll, aAgt) {}

TLog::TLog(int aCtg, const MNode* aAgt, const ChromoNode& aMut): TLog(aCtg, aAgt)
{
    stringstream ss;
    ss << aMut.LineId();
    mMutIdS = ss.str();
}

TLog::TLog(const MNode* aAgt, const ChromoNode& aMut): TLog(EAll, aAgt, aMut) {}

TLog::TLog(const string& aString)
{
    size_t beg = 0;
    mTimestampS = TLogGetField(aString, beg);
    mCtgS = TLogGetField(aString, beg);
    mMutIdS = TLogGetField(aString, beg);
    mNodeUriS = TLogGetField(aString, beg);
    mContent = TLogGetField(aString, beg);
}

TLog::operator string() const
{
    return mTimestampS + KFieldSep + mCtgS + KFieldSep + mMutIdS + KFieldSep + mNodeUriS + KFieldSep + mContent;
}

TLog& TLog::operator +(const string& aString)
{
    mContent += aString;
    return *this;
}

void TLog::SetCtg(int aCtg)
{
    mCtg = aCtg;
    mCtgS = CtgText(mCtg);
}

string TLog::TimestampS() const
{
    return mTimestampS;
}

string TLog::NodeUriS() const
{
    return mNodeUriS;
}

string TLog::MutIdS() const
{
    return mMutIdS;
}

int TLog::MutId() const
{
    int res;
    istringstream ss(mMutIdS);
    ss >> res;
    return res;
}

string TLog::CtgS() const
{
    return mCtgS;
}


Logrec::Logrec(const string& aLogFileName): iLogFileName(aLogFileName), iLogFileValid(),
    iObs(NULL), mCtxMutId(-1)
{
    for (int ct = 0; ct < ECtg_Max; ct++) {
	mStat[ct] = 0;
    }
    //remove(iLogFileName.c_str()); 
    rename(iLogFileName.c_str(), (iLogFileName + "~old").c_str()); 
    iLogFile = fopen(iLogFileName.c_str(), "w+");
    if(iLogFile)
    {
	iLogFileValid=true;
	//fputs("----------New Log----------\015\012", iLogFile);
	fflush(iLogFile);
    }
    else
	iLogFileValid = false;
};

Logrec::~Logrec()
{
    if (iObs != NULL) {
	iObs->OnLogRecDeleting(this);
    }
    if (iLogFileValid) 
	fclose(iLogFile);
}

void Logrec::WriteRecord(const char* aText)
{
    if (iLogFile)
    {
	fputs(aText, iLogFile);
	fputs("\015\012", iLogFile);
	fflush(iLogFile);
    }
}

void Logrec::WriteRecord(const string& aText)
{
    WriteRecord(aText.c_str());
}

void Logrec::WriteFormat(const char* aFmt,...)
{
    char buf[KLogRecBufSize] = "";
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf, aFmt, list);
    int len = strlen(buf);
    WriteRecord(buf);
    va_end(list);
}

void Logrec::Write(TLogRecCtg aCtg, const MNode* aNode, const char* aFmt,...)
{
    char buf1[KLogRecBufSize] = "";
    stringstream ss, ssn;
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    long int ms = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));

    ssn << setfill('0') << setw(9) << ts.tv_nsec;
    ss << buff << "." << ssn.str().substr(0, TLog::KPrecision) << KColSep;
    ss << CtgText(aCtg) << KColSep;
    int mutid = mCtxMutId;
    if (mutid != -1) {
	ss << mutid << KColSep;
    }
    else {
	ss << KColSep;
    }
    if (aNode != NULL) {
	GUri fullpath;
	aNode->getUri(fullpath);
	ss << fullpath.toString() << KColSep;
    }
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf1, aFmt, list);
    ss << buf1;
    WriteRecord(ss.str().c_str());
    if (iObs) {
	iObs->OnLogAdded(ms, aCtg, aNode, buf1, mutid);
    }
    va_end(list);
}

void Logrec::Write(TLogRecCtg aCtg, MNode* aNode, const ChromoNode& aMut, const char* aFmt,...)
{
    char buf1[KLogRecBufSize] = "";
    stringstream ss;
    ss << CtgText(aCtg) << KColSep;
    int lineid = aMut.LineId();
    ss << lineid << KColSep;
    if (aNode != NULL) {
	GUri fullpath;
	aNode->getUri(fullpath);
	ss << fullpath.toString() << KColSep;
    }
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf1, aFmt, list);
    ss << buf1;
    WriteRecord(ss.str().c_str());
    mStat[aCtg]++;
    if (iObs != NULL) {
	iObs->OnLogAdded(0, aCtg, aNode, buf1, lineid);
    }
    va_end(list);
}

void Logrec::Write(const TLog& aRec)
{
    WriteRecord(aRec);
    if (iObs != NULL) {
	iObs->OnLogAdded(aRec);
    }
}

void Logrec::Flush()
{
    if (iLogFile)
	fflush(iLogFile);
}

bool Logrec::AddLogObserver(MLogObserver* aObs)
{
    bool res = false;
    if (aObs != NULL && iObs == NULL) {
	iObs = aObs;
	iObs->AddObservable(this);
	res = true;
    }
    return res;
}

void Logrec::RemoveLogObserver(MLogObserver* aObs)
{
    assert(aObs == iObs);
    iObs->RemoveObservable(this);
    iObs = NULL;
}

void Logrec::SetContextMutId(int aMutId)
{
    if (mCtxMutId != aMutId) {
	mCtxMutId = aMutId;
    }
}

int Logrec::GetStat(TLogRecCtg aCtg) const
{
    return mStat[aCtg];
}

