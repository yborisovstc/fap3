
#ifndef __FAP3_MELEM_H
#define __FAP3_MELEM_H


#include "miface.h"
#include "guri.h"
#include "menv.h"

class MChild;
class MParent;

/** @brief Interface of mutable agent
 *
 * */
class MElem: public MIface
{
    public:
	static const char* Type() { return "MElem";}
	// From MIface
	virtual string Uid() const override { return MElem_Uid();}
	virtual string MElem_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MElem_getLif(aType);}
	virtual MIface* MElem_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MElem_doDump(aLevel, aIdt, std::cout);}
	virtual void MElem_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual MChromo& Chromos() = 0;
	virtual MChild* asChild() = 0;
	virtual MParent* asParent() = 0;
};


#endif  //  __FAP3_MELEM_H
