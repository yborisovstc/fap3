#ifndef __FAP3_CURI_H
#define __FAP3_CURI_H

#include <string>
#include <vector>

using namespace std;


/** @brief Content URI
 * Uses owner-owned relation chain for identification
 * */
class CUri
{
    public:
	CUri() {}
	CUri(const CUri& aSrc): mElems(aSrc.mElems) {}
	CUri(const string& aSrc) { parse(aSrc);}
	int size() const { return mElems.size();}
	const string& at(int aIdx) const { return mElems.at(aIdx);}
    private:
	void parse(const string& aSrc);
    private:
	vector<string> mElems;
};

#endif
