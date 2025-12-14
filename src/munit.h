#ifndef __FAP3_MUNIT_H
#define __FAP3_MUNIT_H

#include "miface.h"
#include "mifr.h"
#include "nconn.h"
#include "guri.h"

class MIfProv;
class MIfReq;



/** @brief Interface of element of interface resolution mechanism (IRM)
 * Don't confuse with MIfProvOwner - it is for provider only whereas 
 * MUnit is for some external client
 * */
class MUnit: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MUnit"sv;}
	inline static constexpr TIdHash idHash() { return 0x37bf1002aa5295dc;}
    public:
	virtual ~MUnit() {} // TODO to consider policy of system destruction
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MUnit_Uid();}
	virtual string MUnit_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) { return MUnit_getLif(aTid);}
	virtual MIface* MUnit_getLif(TIdHash aTid) = 0;
	virtual void doDump(int aLevel, int aIdt, std::ostream& aOs) const { MUnit_doDump(aLevel, aIdt, aOs);}
	virtual void MUnit_doDump(int aLevel, int aIdt, std::ostream& aOs) const = 0;
	// Local
	/** @brief Returns unit iface default IFR provider
	 * This is to simplify requesting iface in case if the client don't have its own requestor
	 * */
	virtual MIfProv* defaultIfProv(TIdHash aTid) = 0;
	/** @brief Resolves requested interface
	 * It is only used by client requestor if the requestor hasn't resolution via this unit, i.e. hasn't 
	 * connection to this unit iface provider
	 * */
	virtual void resolveIface(TIdHash aTid, MIfReq::TIfReqCp* aReq) = 0;
	/** @brief Gets first iface resolved
	 * */
	template<class T> T* getSif(T* aInst, bool aCheck = true) {
	    auto prov = defaultIfProv(T::idHash());
	    MIface* ifc = (prov && prov->ifaces()) ? (prov->ifaces()->empty() ? nullptr : prov->ifaces()->at(0)) : nullptr;
	    if (aCheck) assert(prov->ifaces()->size() <= 1);
	    return aInst = reinterpret_cast<T*>(ifc);
	}
	/** @brief Gets ifaces resolved
	 * */
	template<class T> MIfProv::TIfaces* getIfs() {
	    auto prov = defaultIfProv(T::idHash());
	    return prov ? prov->ifaces() : nullptr;
	}
	template<class T> MIfProv::TTIfaces<T>* getTIfs() {
	    auto prov = defaultIfProv(T::idHash());
	    return prov ? reinterpret_cast<MIfProv::TTIfaces<T>*>(prov->ifaces()) : nullptr;
	}
};


#endif  //  __FAP3_MUNIT_H
