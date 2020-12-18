#ifndef __FAP3_IFR_H
#define __FAP3_IFR_H

#include  "mifr.h"

#include "nconn.h"


/** @brief Interface resolution tree node
 * */
class IfrNode : public NTnnp<MIfProv, MIfReq>, public MIfProv, protected MIfReq
{
    public:
	IfrNode(): NTnnp<MIfProv, MIfReq>(this, this), mValid(false) {}
	virtual ~IfrNode() {}
	// From MIfProv
	virtual string MIfProv_Uid() const override { return MIfProv::Type();}
	virtual string name() const override;
	virtual MIfProv* first() const override;
	virtual MIfProv* next() const override;
	//virtual const MIfReq* requestor() const override { return mCnode.provided(); }
	virtual bool resolve(const string& aName) override;
	// From MIfReq
	virtual string MIfReq_Uid() const override { return MIfReq::Type();}
	virtual const MIfProv* owner() const { return this;}
	virtual MIfProv* next(MIfProv::TCp* aProvCp) const override;
	virtual MIface* iface() override { return nullptr;}
    protected:
	bool mValid;
};

/** @brief Interface resolution tree leaf
 * */
class IfrLeaf : public NCpOnp<MIfProv, MIfReq>, public MIfProv
{
    public:
	IfrLeaf(MIface* aIface): NCpOnp<MIfProv, MIfReq>(this), mValid(true), mIface(aIface) {}
	virtual ~IfrLeaf() {}
	// From MIfProv
	virtual string MIfProv_Uid() const override { return MIfProv::Type();}
	virtual string name() const override;
	virtual MIfProv* first() const override { return nullptr;}
	virtual MIfProv* next() const override { return nullptr;}
	virtual bool resolve(const string& aName) override {return false;}
	virtual MIface* iface() override { return mIface;}
    protected:
	bool mValid;
	MIface* mIface;
};


/** @brief Root iface resolution tree node
 * */
class IfrNodeRoot : public IfrNode
{
    public:
	IfrNodeRoot(const string& aIfName): IfrNode(), mName(aIfName) {};
	// From MIfProv
	virtual string name() const override { return mName;}
    protected:
	string mName;

};

#endif
