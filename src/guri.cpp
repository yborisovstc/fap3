
#include <assert.h>
#include "guri.h"

const char KSep = '.';

void GUri::parse(const string& aSrc)
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

GUri GUri::tail(int aIdx) const
{
    GUri res;
    for (int i = aIdx; i < size(); i++) {
	res.mElems.push_back(at(i));
    }
    return res;
}

void GUri::tail(const GUri& aHead, GUri& aTail) const
{
    assert(aHead <= *this);
    for (int i = aHead.mElems.size(); i < mElems.size(); i++) {
	aTail.mElems.push_back(mElems.at(i));
    }
}


GUri::operator string()
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

void GUri::append(const GUri& aUri)
{
    for (auto it = aUri.mElems.begin(); it != aUri.mElems.end(); it++) {
	appendElem(*it);
    }
}

void GUri::prepend(const GUri& aUri)
{
    for (auto it = aUri.mElems.rbegin(); it != aUri.mElems.rend(); it++) {
	prependElem(*it);
    }
}

void GUri::appendElem(const TElem& aElem)
{
    mElems.push_back(aElem);
}

void GUri::prependElem(const TElem& aElem)
{
    mElems.insert(mElems.begin(), aElem);
}

bool GUri::operator==(const GUri& s) const
{
    return mElems == s.mElems && mErr == s.mErr;
}

bool GUri::operator<(const GUri& aSrc) const
{
    bool res = !mErr && mElems.size() < aSrc.mElems.size();
    for (int i = 0; i < mElems.size() && res; i++) {
	res = mElems.at(i) == aSrc.mElems.at(i);
    }
    return res;
}



