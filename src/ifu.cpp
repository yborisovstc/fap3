#include <stdlib.h>
#include <stdio.h>
#include <stdexcept> 
#include <sstream>
#include "ifu.h" 
#include "munit.h" 


char Ifu::KEsc = '\\';
char Ifu::KUidSep = '%';

Ifu::Ifu()
{
}

size_t Ifu::FindFirstCtrl(const string& aString, const char aCtrl, size_t aPos)
{
    size_t pos_beg = aPos;
    size_t pos;
    do {
	pos = aString.find_first_of(aCtrl, pos_beg); 
	pos_beg = pos + 1;
    } while(pos != string::npos && aString.at(pos - 1) == KEsc);
    return pos;
}

size_t Ifu::FindFirstCtrl(const string& aString, const string& aCtrls, size_t aPos)
{
    size_t pos_beg = aPos;
    size_t pos;
    do {
	char ctrl = 0x00;
	pos = aString.find_first_of(aCtrls, pos_beg); 
	pos_beg = pos + 1;
    } while(pos != string::npos && aString.at(pos - 1) == KEsc);
    return pos;
}

size_t Ifu::FindRightDelim(const string& aString, char aLeftDelim, char aRightDelim, size_t aPos)
{
    size_t res = string::npos;
    size_t pos_beg = aPos, pos = 0;
    int level = 0;
    do {
	string delims(1, aLeftDelim); delims += aRightDelim;
	pos = aString.find_first_of(delims, pos_beg);
	if (pos != string::npos) {
	    char delim =  aString.at(pos);
	    if (delim == aLeftDelim) {
		level++;
	    } else {
		if (level == 0) {
		    res = pos;
		} else {
		    level--;
		}
	    }
	    pos_beg = pos + 1;
	}
    } while (pos != string::npos && res == string::npos);
    return pos;
}

