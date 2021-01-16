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
	GUri() {}
	GUri(const GUri& aSrc): mElems(aSrc.mElems) {}
	GUri(const string& aSrc) { parse(aSrc);}
	int size() const { return mElems.size();}
	const string& at(int aIdx) const { return mElems.at(aIdx);}
	GUri tail(int aIdx) const;
	operator string();
    private:
	void parse(const string& aSrc);
    private:
	vector<string> mElems;
};

#endif
