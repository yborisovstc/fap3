#ifndef __FAP3_MVERT_H
#define __FAP3_MVERT_H

#include "nconn.h"
#include "miface.h"
#include "ifu.h"

using namespace std;


/** @brief Interface of connectable
 * Represents generic connection of system level
 * TODO seems MCIface inheritance isn't useful here, consider simplify design
 * */
class MVert: public MCIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MVert"sv;}
	inline static constexpr TIdHash idHash() { return 0xedc4d46b41b2bbeb;}
    public:
	enum TDir { ERegular, EInp, EOut };
    public:
	// From MCIface 
	TIdHash id() const override { return idHash();}
	virtual string MVert_Uid() const = 0;
	virtual string Uid() const override { return MVert_Uid();}
	virtual MIface *getLif(TIdHash aTid) { return MVert_getLif(aTid);}
	virtual MIface *MVert_getLif(TIdHash aTid) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MVert_doDump(aLevel, aIdt, std::cout);}
	virtual void MVert_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	/** @brief Indicates if connnectable is compatible */
	virtual bool isCompatible(MVert* aPair, bool aExt) = 0;
	/** @brief Gets extended part in case if checked is extender */
	virtual MVert* getExtd() = 0;
	/** @brief Indicates direction */
	virtual TDir getDir() const = 0;
	virtual int pairsCount() const = 0;
	virtual MVert* getPair(int aInd) const = 0;
	virtual bool isPair(const MVert* aPair) const = 0;
	/** @brief Gets sign of vertex linked to given vertex
	 * Is shows not only direct linkage (pairing) but also the linkage via components
	 * */
	virtual bool isLinked(const MVert* aPair, bool aDirect = false) const = 0;
};

#endif
