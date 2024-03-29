#ifndef __FAP3_IFR_H
#define __FAP3_IFR_H

#include  <vector>

#include  "mifr.h"
#include  "ifu.h"

#include "nconn.h"


/** @brief Interface resolution tree node
 * TODO isn't this design overkill? Why don't just directly proxy node NTnnp to owner?
 * NOTE: nconn requestor doesn't own its pairs, provider owner ownes instead
 * */
class IfrNode : public NTnnp<MIfProv, MIfReq>, public MIfProv, protected MIfReq
{
    public:
	using TBase = NTnnp<MIfProv, MIfReq>;
    public:
	IfrNode(MIfProvOwner* aOwner): NTnnp<MIfProv, MIfReq>(this, this), mValid(false), mOwner(aOwner) {}
	virtual ~IfrNode() {}
	// From MIfProv
	virtual string MIfProv_Uid() const override { return mOwner->Uid() + Ifu::KUidSepIc + MIfProv::Type();}
	virtual string name() const override;
	virtual MIfProv* first() const override;
	virtual MIfProv* next() const override;
	virtual void MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void resolve(const string& aName) override;
	virtual bool isValid() const override { return mValid;}
	virtual void setValid(bool aValid) override;
	virtual MIface* iface() override { return nullptr;}
	virtual TIfaces* ifaces() override { return nullptr;}
	virtual const MIfProvOwner* owner() const override { return mOwner;}
	virtual MIfProv* findIface(const MIface* aIface) override;
	// From MIfReq
	virtual string MIfReq_Uid() const override { return mOwner->Uid() + Ifu::KUidSepIc + MIfReq::Type();}
	virtual void MIfReq_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual MIfProv* next(MIfProv::TCp* aProvCp) const override;
	virtual bool isRequestor(MIfProvOwner* aOwner, int aPos) const override;
	virtual const MIfProvOwner* rqOwner() const override { return mOwner;}
	virtual MIfReq* prev() override;
	virtual MIfReq* tail() override;
	virtual bool isResolved() override;
	virtual void onProvInvalidated() override;
	// From MNCpp
	virtual TSelf* firstLeafB() override;
	virtual TPair* nextLeaf(TPair* aLeaf) override;
	virtual bool detach(TPair* aPair) override;
    public:
	MIfProv* findOwner(const MIfProvOwner* aOwner);
	void eraseInvalid();
	void erase();
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
	virtual void resolve(const string& aName) override {}
	virtual MIface* iface() override { return mIface;}
	virtual TIfaces* ifaces() override { return nullptr;}
	virtual const MIfProvOwner* owner() const override { return mOwner;}
	virtual void MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool isValid() const override { return mValid;}
	virtual void setValid(bool aValid) override;
	virtual MIfProv* findIface(const MIface* aIface) override { return nullptr;}
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
	IfrNodeRoot(MIfProvOwner* aOwner, const string& aIfName): IfrNode(aOwner), mName(aIfName), mIcacheValid(false) {};
	// From MIfProv
	virtual string name() const override { return mName;}
	virtual void setValid(bool aValid) override;
	virtual TIfaces* ifaces() override;
	// From MIfReq
	virtual void onProvInvalidated() override;
    protected:
	void updateIcache();
    protected:
	string mName;
	TIfaces mIcache;  /*!< Cache of ifaces, ref ds_irm_cr */
	bool mIcacheValid;
};

#endif
