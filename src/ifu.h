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
	// Dump masks
	enum TDm {
	    EDM_Recursive = 0x01,
	    EDM_Base = 0x02,
	    EDM_Comps = 0x04,
	    EDM_Opt1 = 0x10,
	    EDM_Opt2 = 0x20,
	    EDM_Opt3 = 0x40,
	};
    public:
	Ifu();
	static size_t FindFirstCtrl(const string& aString, const char aCtrl, size_t aPos); 
	static size_t FindFirstCtrl(const string& aString, const string& aCtrls, size_t aPos); 
	static size_t FindRightDelim(const string& aString, char LeftDelim, char RightDelim, size_t aPos);
	static void offset(int aIndent, ostream& aOs);
    public:
	static char KEsc;
	static char KUidSep;
	static char KUidSepIc;  /*!< UID separator of internal component (not native hier) */
	static int  KDumpIndent;
};


#endif
