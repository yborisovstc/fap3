#ifndef __FAP3_MSYST_H
#define __FAP3_MSYST_H

#include "miface.h"


/** @brief Connection point interface
 * */
class MConnPoint: public MIface
{
    public:
	static const char* Type() { return "MConnPoint";};
	// From MIface
	virtual string Uid() const override { return MConnPoint_Uid();}
	virtual string MConnPoint_Uid() const = 0;
	virtual string provided() const = 0;
	virtual string required() const = 0;
};



#endif
