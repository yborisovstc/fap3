#ifndef __FAP3_IFR_H
#define __FAP3_IFR_H

#include  "mifr.h"
#include  "ifu.h"

#include "nconn.h"


/** @brief Interface resolution tree node
 * */
class IfrNode : public NTnnp<MIfProv, MIfReq>, public MIfProv, protected MIfReq
{
    public:
	IfrNode(MIfProvOwner* aOwner): NTnnp<MIfProv, MIfReq>(this, this), mValid(false), mOwner(aOwner) {}
	virtual ~IfrNode() {}
	// From MIfProv
	virtual string MIfProv_Uid() const override { return mOwner->Uid() + Ifu::KUidSepIc + MIfProv::Type();}
	virtual string name() const override;
	virtual MIfProv* first() const override;
	virtual MIfProv* next() const override;
	virtual void MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool resolve(const string& aName) override;
	virtual bool isValid() const override { return mValid;}
	virtual void setValid(bool aValid) override;
	virtual MIface* iface() override { return nullptr;}
	virtual const MIfProvOwner* owner() const override { return mOwner;}
	// From MIfReq
	virtual string MIfReq_Uid() const override { return mOwner->Uid() + Ifu::KUidSepIc + MIfReq::Type();}
	virtual MIfProv* next(MIfProv::TCp* aProvCp) const override;
	virtual bool isRequestor(MIfProvOwner* aOwner) const override;
    public:
	MIfProv* findIface(const MIface* aIface);
	MIfProv* findOwner(const MIfProvOwner* aOwner);
	void eraseInvalid();
    protected:
	bool mValid;
	MIfProvOwner* mOwner;
};

/** @brief Interface resolution tree leaf
 * */
class IfrLeaf : public NCpOnp<MIfProv, MIfReq>, public MIfProv
{
    public:
	IfrLeaf(MIfProvOwner* aOwner, MIface* aIface): NCpOnp<MIfProv, MIfReq>(this), mValid(true), mOwner(aOwner), mIface(aIface) {}
	virtual ~IfrLeaf() {}
	// From MIfProv
	virtual string MIfProv_Uid() const override { return MIfProv::Type();}
	virtual string name() const override;
	virtual MIfProv* first() const override { return const_cast<IfrLeaf*>(this);}
	virtual MIfProv* next() const override;
	virtual bool resolve(const string& aName) override {return false;}
	virtual MIface* iface() override { return mIface;}
	virtual const MIfProvOwner* owner() const override { return mOwner;}
	virtual void MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool isValid() const override { return mValid;}
	virtual void setValid(bool aValid) override;
    protected:
	bool mValid;
	MIfProvOwner* mOwner;
	MIface* mIface;
};


/** @brief Root iface resolution tree node
 * */
class IfrNodeRoot : public IfrNode
{
    public:
	IfrNodeRoot(MIfProvOwner* aOwner, const string& aIfName): IfrNode(aOwner), mName(aIfName) {};
	// From MIfProv
	virtual string name() const override { return mName;}
    protected:
	string mName;

};

#endif
