#ifndef __FAP3_MECONT_H
#define __FAP3_MECONT_H

#include "miface.h"



/** @brief Iface of native hier embedded content
 * */
class MContent: public MIface
{
    public:
	static const char* Type() { return "MContent";};
	// From MIface
	virtual string Uid() const override { return MContent_Uid();}
	virtual string MContent_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MContent_getLif(aType);}
	virtual MIface* MContent_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { MContent_doDump(aLevel, aIdt, aOs);}
	virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual bool getData(string& aData) const = 0;
	virtual bool setData(const string& aData) = 0;
};


#endif
