
#ifndef __FAP3_MLINK_H
#define __FAP3_MLINK_H


#include "miface.h"
#include "ifu.h"

using namespace std;


class MNode;

/** @brief One-way relation interface
 *
 * */
class MLink: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MLink"sv;}
	inline static constexpr TIdHash idHash() { return 0x4f0a000556332ad5;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MLink_Uid();}
	virtual string MLink_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) { return MLink_getLif(aTid);}
	virtual MIface* MLink_getLif(TIdHash aTid) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MLink_doDump(aLevel, aIdt, std::cout);}
	virtual void MLink_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual bool connect(MNode* aPair) = 0;
	virtual bool disconnect(MNode* aPair) = 0;
	virtual MNode* pair() = 0;
};






#endif
