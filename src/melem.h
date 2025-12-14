
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
	inline static constexpr std::string_view idStr() { return "MElem"sv;}
	inline static constexpr TIdHash idHash() { return 0xd0cb09cd212045e1;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MElem_Uid();}
	virtual string MElem_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) { return MElem_getLif(aTid);}
	virtual MIface* MElem_getLif(TIdHash aTid) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MElem_doDump(aLevel, aIdt, std::cout);}
	virtual void MElem_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual MChromo& Chromos() = 0;
	virtual MChild* asChild() = 0;
	virtual MParent* asParent() = 0;
        /** @brief Get parents uri hierarchy
         * Allows to get inheritance upper tree w/o providing access to tree elem
         * Such approach is less vulnerable, but also need design analysis
         * Using MChild::cP() atm, ref SdoParents::VDtGet
         * */
	//virtual vector<GUri> getParentsUri() const = 0;
};


#endif  //  __FAP3_MELEM_H
