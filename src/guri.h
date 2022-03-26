#ifndef __FAP3_GURI_H
#define __FAP3_GURI_H

#include <string>
#include <vector>

using namespace std;


/** @brief Generic URI
 * Uses owner-owned relation chain for identification
 * */
class GUri
{
    public:
	using TElem = string;
    public:
	GUri(): mErr(false) {}
	GUri(const GUri& aSrc): mElems(aSrc.mElems), mErr(aSrc.mErr) {}
	GUri(const string& aSrc) { parse(aSrc);}
	GUri(const char* aSrc): GUri(string(aSrc)) {}
	int size() const { return mElems.size();}
	const string& at(int aIdx) const { return mElems.at(aIdx);}
	GUri tail(int aIdx) const;
	void tail(const GUri& aHead, GUri& aTail) const;
	GUri head(int aIdx) const;
	const vector<TElem>& elems() const {return mElems;};
	void append(const GUri& aUri);
	void prepend(const GUri& aUri);
	void appendElem(const TElem& aElem);
	void prependElem(const TElem& aElem);
	bool operator==(const GUri& aSrc) const;
	bool operator!=(const GUri& aSrc) const { return !operator ==(aSrc);}
	bool operator<(const GUri& aSrc) const;
	bool operator<=(const GUri& aSrc) const { return *this < aSrc || *this == aSrc;}
	bool operator>(const GUri& aSrc) const { return aSrc < *this;}
	bool operator>=(const GUri& aSrc) const { return aSrc <= *this;}
	GUri& operator+=(const GUri& aSrc) { append(aSrc); return *this;}
	GUri operator+(const GUri& aSrc) const { GUri res(*this); res.append(aSrc); return res;}
	operator string() const { return toString();}
	string toString() const;
	bool isAbsolute() const;
	bool isValid() const { return !mErr;}
	bool isName() const;
    public:
	static const string nil; // TODO YB move to proper place
	static const string K_Self;
    private:
	void parse(const string& aSrc);
    private:
	vector<TElem> mElems;
	bool mErr;
};

#endif
