#ifndef __FAP3_MIFR_H
#define __FAP3_MIFR_H

#include "miface.h"
#include "nconn.h"

class MIfProv;
class MIfProvOwner;
class MIfReq;

/** @brief Interface of interface provider in iface resolution mechanism
 * */
class MIfProv: public MIface
{
    public:
	using TCp = MNcpp<MIfProv, MIfReq>; /*!< Connpoint type */
    public:
	static const char* Type() { return "MIfProv";}
	// From MIface
	virtual string Uid() const override { return MIfProv_Uid();}
	virtual string MIfProv_Uid() const = 0;
	virtual string name() const = 0;
	virtual MIfProv* first() const = 0;
	virtual MIfProv* next() const = 0;
	virtual bool resolve(const string& aName) = 0;
	virtual MIface* iface() = 0;
	virtual const MIfProvOwner* owner() const = 0;
	virtual void dump(int aIdt) const override { MIfProv_dump(aIdt);}
	virtual void MIfProv_dump(int aIdt) const =0;
	virtual bool isValid() const = 0;
	virtual void setValid(bool aValid) = 0;
};

/** @brief Interface of interface requestor in iface resolution mechanism
 * */
class MIfReq: public MIface
{
    public:
	static const char* Type() { return "MIfReq";}
	// From MIface
	virtual string Uid() const override { return MIfReq_Uid();}
	virtual string MIfReq_Uid() const = 0;
	//virtual const MIfProv* owner() const = 0;
	virtual MIfProv* next(MIfProv::TCp* aProvCp) const = 0;
};

/** @brief Interface provider owner
 * */
class MIfProvOwner: public MIface
{
    public:
	static const char* Type() { return "MIfProvOwner";}
	// From MIface
	virtual string Uid() const override { return MIfProvOwner_Uid();}
	virtual string MIfProvOwner_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MIfProvOwner_getLif(aType);}
	virtual MIface* MIfProvOwner_getLif(const char *aType) = 0;
	// Local
	virtual void onIfpDisconnected(MIfProv* aProv) = 0;
	
};


#endif
