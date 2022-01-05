
#ifndef __FAP3_DBG_H
#define __FAP3_DBG_H

#include "mnode.h"
#include "munit.h"


/** @brief Debugging utilities
 * */
class Dbg
{
    public:
	static MUnit* getUnit(MNode* aNode);
	//static MDesSyncable* getDsync(MNode* aNode);
	template <typename T>
	    static T* getIface(MNode* aNode);
};


template <typename T>
T* Dbg::getIface(MNode* aNode)
{
    T* res = aNode->lIf(res);
    return res;
}

#endif 
