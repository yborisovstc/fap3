
#include <assert.h>
#include "guri.h"

const char KSep = '.';
const char KSelf = '$';

const string GUri::nil = "nil";
const string GUri::K_Self= "_$";


static bool isElemName(string& aSrc)
{
    bool res = true;
    if (aSrc.size() > 0) {
	for (size_t ind = 0; ind < aSrc.size() && res; ind++) {
	    char c = aSrc.at(ind);
	    if (ind == 0) {
		res = isalpha(c);
	    } else {
		res = isalnum(c) || c == '_';
	    }
	}
    } else {
	res = false;
    }
    return res;
}

void GUri::parse(const string& aSrc)
{
    bool res = true;
    mElems.clear();
    if (aSrc.size() > 0) {
	size_t beg = 0, pos = 0;
	while (pos != string::npos && res) {
	    pos = aSrc.find_first_of(KSep, beg); 
	    string elem = aSrc.substr(beg, pos - beg);
	    if (beg == 0) {
		res = elem.empty() || isElemName(elem) || elem == K_Self;
	    } else {
		res = isElemName(elem);
	    }
	    if (res) {
		mElems.push_back(elem);
	    }
	    beg = (pos == string::npos) ? string::npos : pos + 1;
	}
    } else {
	// TODO Enable the empty URI meanwhile. Consider URI design
	//res = false;
    }
    mErr = !res;
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
    aTail.clear();
    for (int i = aHead.mElems.size(); i < mElems.size(); i++) {
	aTail.mElems.push_back(mElems.at(i));
    }
}

bool GUri::isHead(const GUri& aHead) const
{
    bool res = (aHead.size() < size());
    for (int i = 0; i < aHead.size() && res; i++) {
	res = (aHead.at(i) == at(i));
    }
    return res;
}

bool GUri::isTail(const GUri& aTail) const
{
    bool res = (aTail.size() < size());
    for (int i = 0; i < aTail.size() && res; i++) {
	res = (aTail.at(i) == at(size() - aTail.size() + i));
    }
    return res;
}

bool GUri::getTail(const GUri& aHead, GUri& aTail) const
{
    bool res = isHead(aHead);
    aTail.clear();
    if (res) {
	for (int i = aHead.size(); i < size(); i++) {
	    aTail.appendElem(at(i));
	}
    }
    return res;
}

bool GUri::getHead(const GUri& aTail, GUri& aHead) const
{
    bool res = isTail(aHead);
    aHead.clear();
    if (res) {
	for (int i = 0; i < (size() - aTail.size()); i++) {
	    aHead.appendElem(at(i));
	}
    }
    return res;
}

GUri GUri::head(int aIdx) const
{
    GUri res;
    for (int i = 0; i < aIdx; i++) {
	res.mElems.push_back(at(i));
    }
    return res;
}

GUri GUri::tailn(int aNum) const
{
    GUri res;
    for (int i = 0; i < aNum; i++) {
	res.mElems.push_back(at(size() - 1 - i));
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


