
#include "curi.h"

const char KSep = '.';

void CUri::parse(const string& aSrc)
{
    if (aSrc.size() > 0) {
	mElems.clear();
	size_t beg = 0, pos = 0;
	while (pos != string::npos) {
	    pos = aSrc.find_first_of(KSep, beg); 
	    string elem = aSrc.substr(beg, pos - beg);
	    mElems.push_back(elem);
	    beg = (pos == string::npos) ? string::npos : pos + 1;
	}
    }
}

CUri CUri::tail(int aIdx) const
{
    CUri res;
    for (int i = aIdx; i < size(); i++) {
	res.mElems.push_back(at(i));
    }
    return res;
}

CUri::operator string()
{
    string res;
    for (int i = 0; i < size(); i++) {
	res.append(mElems.at(i));
	if (i < size()) {
	    res.append(1, KSep);
	}
    }
    return res;
}
