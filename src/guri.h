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
	void clear() { mElems.clear();}
	const string& at(int aIdx) const { return mElems.at(aIdx);}
	GUri head(int aIdx) const;
	/** @brief Tail formed by elems from aIdx */
	GUri tail(int aIdx) const;
	/** @brief Tail formed by aNum tail elemement */
	GUri tailn(int aNum) const;
	void tail(const GUri& aHead, GUri& aTail) const;
	bool isHead(const GUri& aHead) const;
	bool isTail(const GUri& aTail) const;
	bool getHead(const GUri& aTail, GUri& aHead) const;
	bool getTail(const GUri& aHead, GUri& aTail) const;
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
	GUri& operator=(const GUri& aSrc) { this->mElems = aSrc.mElems; this->mErr = aSrc.mErr; return *this;}
	operator string() const { return toString();}
	string toString() const;
	bool isAbsolute() const;
	bool isValid() const { return !mErr;}
	bool isName() const;
	friend ostream& operator<<(ostream& aStream, const GUri& aDt);
	friend istream& operator>>(istream& aStream, GUri& aDt);
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
