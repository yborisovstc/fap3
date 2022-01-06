
#ifndef __FAP3_DBG_H
#define __FAP3_DBG_H

#include <string>

class MNode;
class MUnit;

/** @brief Debugging utilities
 * */
class Dbg
{
    public:
	static MUnit* getUnit(MNode* aNode);
	//static MDesSyncable* getDsync(MNode* aNode);
	template <typename T>
	    static T* getIface(MNode* aNode);
    public:
	static std::string mLog;
	static int mEnableLog;
};

#endif 
