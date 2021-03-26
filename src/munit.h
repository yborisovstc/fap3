#ifndef __FAP3_MUNIT_H
#define __FAP3_MUNIT_H

#include "miface.h"
#include "mifr.h"
#include "nconn.h"
#include "guri.h"

class MIfProv;



/** @brief Interface of element of interface resolution mechanism (IRM)
 * */
class MUnit: public MIface
{
    public:
	using TIfReqCp = MNcpp<MIfReq, MIfProv>; /*!< IFR requestor connpoint type */
    public:
	static const char* Type() { return "MUnit";}
	virtual ~MUnit() {} // TODO to consider policy of system destruction
	// From MIface
	virtual string Uid() const override { return MUnit_Uid();}
	virtual string MUnit_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MUnit_getLif(aType);}
	virtual MIface* MUnit_getLif(const char *aType) = 0;
	// Local
	/** @brief Returns unit iface default IFR provider
	 * This is to simplify requesting iface in case if the client don't have its own requestor
	 * */
	virtual MIfProv* defaultIfProv(const string& aName) = 0;
	/** @brief Resolves requested interface
	 * It is only used by client requestor if the requestor hasn't resolution via this unit, i.e. hasn't 
	 * connection to this unit iface provider
	 * */
	virtual bool resolveIface(const string& aName, TIfReqCp* aReq) = 0;
};


#endif  //  __FAP3_MUNIT_H
