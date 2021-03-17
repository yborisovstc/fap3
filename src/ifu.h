#ifndef __FAP3_IFU_H
#define __FAP3_IFU_H

#include <vector>
#include <string>
#include <sstream>
#include <map>


using namespace std;

/** @brief Interface support utility
 * */
class Ifu
{
    public:
	Ifu();
	static size_t FindFirstCtrl(const string& aString, const char aCtrl, size_t aPos); 
	static size_t FindFirstCtrl(const string& aString, const string& aCtrls, size_t aPos); 
	static size_t FindRightDelim(const string& aString, char LeftDelim, char RightDelim, size_t aPos);
    public:
	static char KEsc;
	static char KUidSep;
	static int  KDumpIndent;
};


#endif
