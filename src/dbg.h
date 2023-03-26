
#ifndef __FAP3_DBG_H
#define __FAP3_DBG_H

#include <string>

class MNode;
class MUnit;
class MElem;
class MVert;

/** @brief Debugging utilities
 * */
class Dbg
{
    public:
	static MUnit* getUnit(MNode* aNode);
	static MElem* getElem(MNode* aNode);
	static MVert* getVert(MNode* aNode);
	//static MDesSyncable* getDsync(MNode* aNode);
	template <typename T>
	    static T* getIface(MNode* aNode);
	/** @brief Gets data string representation
	 * aNode  the native hier context
	 * aCpUri aNode relative (or absolute) URI of connpoint
	 * */
	static std::string SoDs(MNode* aNode, const char* aCpUri);
    public:
	static std::string mLog;
	static int mEnableLog;
};

#endif 
