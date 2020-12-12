
#include "curi.h"

const char KSep = '.';

void CUri::parse(const string& aSrc)
{
    if (aSrc.size() > 0) {
	mElems.clear();
	size_t beg = 0;
	size_t pos = aSrc.find_first_of(KSep, beg); 
	do {
	    string elem = aSrc.substr(beg, pos);
	    mElems.push_back(elem);
	    beg = (pos == string::npos) ? string::npos : pos + 1;
	    pos = aSrc.find_first_of(KSep, beg); 
	} while (pos == string::npos);
    }
}
