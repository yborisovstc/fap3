
#ifndef __FAP3_MDADP_H
#define __FAP3_MDADP_H

/** @brief DES adapter interfaces
 * */

#include "miface.h"

class MNode;

/** @brief Interface of Composite DES adapter
 * */
class MDesAdapter: public MIface
{
    public:
	static const char* Type() { return "MDesAdapter";};
	// From MIface
	virtual string Uid() const override { return MDesAdapter_Uid();}
	virtual string MDesAdapter_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesAdapter_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesAdapter_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual MNode* getMag() = 0;
};


#endif
