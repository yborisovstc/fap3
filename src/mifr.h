#ifndef __FAP3_MIFR_H
#define __FAP3_MIFR_H

#include "miface.h"
#include "nconn.h"
#include "vector"

class MIfProv;
class MIfProvOwner;
class MIfReq;

/** @brief Interface of interface provider in iface resolution mechanism
 * */
class MIfProv: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MIfProv"sv;}
	inline static constexpr TIdHash idHash() { return 0x2715b72665f4234c;}
    public:
	using TCp = MNcpp<MIfProv, MIfReq>; /*!< Connpoint type */
	using TIfaces = vector<MIface*>;
	template <class T> using TTIfaces = vector<T*>;
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MIfProv_Uid();}
	virtual string MIfProv_Uid() const = 0;
	virtual TIdHash ifId() const = 0;
	virtual void resolve(TIdHash aTid) = 0;
	virtual MIface* iface() = 0;
	virtual TIfaces* ifaces() = 0;
	virtual const MIfProvOwner* owner() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { MIfProv_doDump(aLevel, aIdt, aOs);}
	virtual void MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const =0;
	virtual bool isValid() const = 0;
	virtual void setValid(bool aValid) = 0;
	virtual MIfProv* findIface(const MIface* aIface) = 0;
};

/** @brief Interface of interface requestor in iface resolution mechanism
 * */
class MIfReq: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MIfReq"sv;}
	inline static constexpr TIdHash idHash() { return 0xab15665aca00012f;}
    public:
	using TIfReqCp = MNcpp<MIfReq, MIfProv>; /*!< IFR requestor connpoint type */
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MIfReq_Uid();}
	virtual string MIfReq_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { MIfReq_doDump(aLevel, aIdt, aOs);}
	virtual void MIfReq_doDump(int aLevel, int aIdt, ostream& aOs) const =0;
	//virtual const MIfProv* owner() const = 0;
	/** @brief Indicates if giver owner is in back requestors chain on aPos position
	 * @parem aPos  the position checked, -1 means all positions
	 * */
	virtual bool isRequestor(MIfProvOwner* aOwner, int aPos = -1) const = 0;
	/** @brief Gets the owner */
	virtual const MIfProvOwner* rqOwner() const = 0;
	/** @brief Gets previous requestor in the chain */
	virtual MIfReq* prev() = 0;
	/** @brief Gets first (initial) requestor in the chain */
	virtual MIfReq* tail() = 0;
	virtual bool isResolved() { return false;}
	virtual void onProvInvalidated() = 0;
};

/** @brief Interface provider owner
 * Don't confuse with MUnit. MUnit is for some external client
 * but provider owner is for provider only.
 * */
class MIfProvOwner: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MIfProvOwner"sv;}
	inline static constexpr TIdHash idHash() { return 0xf2c0cfe42559fbf4;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MIfProvOwner_Uid();}
	virtual string MIfProvOwner_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) { return MIfProvOwner_getLif(aTid);}
	virtual MIface* MIfProvOwner_getLif(TIdHash aTid) = 0;
	// Local
	/** @resolve interface as provider owner */
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) = 0;
	virtual void onIfpDisconnected(MIfProv* aProv) = 0;
	// TODO not used, remove?
	virtual void onIfpInvalidated(MIfProv* aProv) = 0;
	
};


#endif
