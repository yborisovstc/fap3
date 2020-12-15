#ifndef __FAP3_MUNIT_H
#define __FAP3_MUNIT_H

#include "miface.h"
#include "menv.h"

class MUnit: public MIface
{
    public:
	// From MIface
	constexpr static const char* mType = "MUnit";
	virtual string Uid() const override { return MUnit_Uid();}
	virtual string MUnit_Uid() const = 0;
	// Local
	virtual string name() const = 0;
	virtual bool getContent(string& aData, const string& aName = string()) const = 0;
	virtual bool setContent(const string& aData, const string& aName = string()) = 0;
	virtual bool addContent(const string& aName, bool aLeaf = false) = 0;
	// Debug
	virtual void dumpContent() const = 0;
};


#endif  //  __FAP3_MUNIT_H
