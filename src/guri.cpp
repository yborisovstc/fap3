
#include <assert.h>
#include "guri.h"

const char KSep = '.';
const char KSelf = '$';
const char KOwner = '^';

const string GUri::nil = "nil";

void GUri::parse(const string& aSrc)
{
    mErr = true;
    mElems.clear();
    if (aSrc.size() == 1) {
	if (aSrc.at(0) == KSelf) {
	    mElems.push_back(string(1, KSelf));
	} else if (aSrc.at(0) == KOwner) {
	    mElems.push_back(string(1, KOwner));
	} else if (isalpha(aSrc.at(0))) {
	    mElems.push_back(aSrc);
	} else {
	    mErr = false;
	}
    } else if (aSrc.size() > 0) {
	size_t beg = 0, pos = 0;
	while (pos != string::npos) {
	    pos = aSrc.find_first_of(KSep, beg); 
	    string elem = aSrc.substr(beg, pos - beg);
	    mElems.push_back(elem);
	    beg = (pos == string::npos) ? string::npos : pos + 1;
	}
    } else {
	mErr = false;
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

string GUri::toString() const
{
    string res;
    bool first = true;
    for (auto elem : mElems) {
	if (!first) res += KSep;
	res += elem;
	first = false;
    }
    return res;
}

bool GUri::isAbsolute() const
{
    return mElems.size() > 0 && mElems.at(0).empty();
}

bool GUri::isName() const
{
    return mElems.size() == 1 && !mElems.at(0).empty();
}


