#ifndef __FAP3_MVERT_H
#define __FAP3_MVERT_H

#include "nconn.h"
#include "miface.h"
#include "ifu.h"

using namespace std;


/** @brief Interface of connectable
 * Represents generic connection of system level
 * */
class MVert: public MCIface
{
    public:
	enum TDir { ERegular, EInp, EOut };
    public:
	static const char* Type() { return "MVert";}
	// From MCIface 
	//virtual string provided() const override { return Type();}
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
	// From MIface
	virtual string MVert_Uid() const = 0;
	virtual string Uid() const override { return MVert_Uid();}
    protected:
	//class EIfu: public Ifu { public: EIfu(); };
	//static EIfu mIfu;
};

#endif
