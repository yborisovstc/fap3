

#include <time.h>
#include <pthread.h>
#include "prof.h"
#include "mlog.h"

// PindBase

bool PindBase::getClockResolution(MPind::TClock& aRes) const {
    bool res = false;
    clockid_t cid;
    int rr = pthread_getcpuclockid(pthread_self(), &cid);
    if (rr == 0) {
	rr = clock_getres(cid, &aRes);
	if (rr == 0) {
	    res = true;
	}
    }
    return res;
}

bool PindBase::GetClock(MPind::TClock &aRes) {
    bool res = false;
//    clockid_t cid = CLOCK_REALTIME;
//    clockid_t cid = CLOCK_MONOTONIC;
    int s = 0;
    // TODO YB to consider configurable option to keep clockid from the first call
    // It will not work if the func measured called from different context

    clockid_t cid = -1;
    s = pthread_getcpuclockid(pthread_self(), &cid);
    if (s == 0) {
        //LOG(EInfo, "Pind: %d, ClockId: %d", getId(), mClockId);
    } else {
        //LOG(LogLevel::Error) << "getpuclockid failed: " << s;
    }
    if (s == 0 && cid != -1) {
        s = clock_gettime(cid, &aRes);
        if (s == 0) {
            res = true;
        } else {
            //LOG(LogLevel::Error) << "clock_gettime failed: " << s << ", " << cid;
        }
    }
    return res;
}

string PindBase::toString() const {
    return field(mDescr);
}

bool Pind::saveToFile(const std::string &aPath) {
    bool res = true;
    FILE *fp = fopen(aPath.c_str(), "w+");
    if (fp) {
	auto len = getBufLen();
	for (int count = 0; count <= len; count++) {
	    // Record data
	    string recs = recToString(count);
	    if (!recs.empty()) {
		recs += "\n";
		int fpres = fputs(recs.c_str(), fp);
		if (fpres == EOF) {
		    res = false;
		    break;
		}
	    }
	}
    } else {
	res = false;
    }
    return res;
}


// PindItem

string PindItem::toString() const {
    string res;
    res = field(to_string(mId));
    res = res + PindBase::toString();
    return res;
}





// ProfBase

void ProfBase::enable() {
}

void ProfBase::saveMetrics() {
    for (int i = 0; i < pindsCount(); i++) {
	auto* ind = getPind(i);
	if (ind) {
	    string efname = mPathBase + "~" + ind->getFileSuf() + ".csv";
	    ind->saveToFile(efname);
	}
    }
}



// Duration indicator

void PindDur::Start() {
    mErr = !GetClock(mIvStart);
}

void PindDur::Rec(TRecParam /* aPar */) {
    if (!mErr) {
	TClock cur;
	mErr = !GetClock(cur);
	if (!mErr) {
	    mDur = (cur.tv_sec - mIvStart.tv_sec) * 1000000000 + (cur.tv_nsec - mIvStart.tv_nsec);
	    if (mDur <= 10) {
		// TODO Handle error
		mErr = true;
	    }
	} else {
	    // TODO Handle error
	}
    }
}

string PindDur::toString() const {
    return PindItem::toString() + field(to_string(mDur));
}


