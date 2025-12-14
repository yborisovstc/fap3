
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
	inline static constexpr std::string_view idStr() { return "MDesAdapter"sv;}
	inline static constexpr TIdHash idHash() { return 0x760233857087ee2f;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MDesAdapter_Uid();}
	virtual string MDesAdapter_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesAdapter_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesAdapter_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual MNode* getMag() = 0;
};


#endif
