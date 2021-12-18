
#ifndef __FAP3_MMNTP_H
#define __FAP3_MMNTP_H

#include "miface.h"

/** @brief Model mount point interface
 * */
class MMntp : public MIface
{
    public:
	static const char* Type() { return "MMntp";}
    public:
	// From MIface
	virtual string Uid() const override { return MMntp_Uid();}
	virtual string MMntp_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MMntp_getLif(aType);}
	virtual MIface* MMntp_getLif(const char *aType) = 0;
	// Local
	virtual MNode* root() const = 0;
};



#endif
