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
	virtual string provName() const = 0;
	virtual string reqName() const = 0;
};

/** @brief System embedded managing agent
 * System delegates agent handling the requests
 * */
class MAgent: public MIface
{
    public:
	static const char* Type() { return "MAgent";};
	// From MIface
	virtual string Uid() const override { return MAgent_Uid();}
	virtual string MAgent_Uid() const = 0;
};




#endif
