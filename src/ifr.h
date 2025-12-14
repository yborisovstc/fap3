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
	virtual string MIfProv_Uid() const override { return mOwner->Uid() + Ifu::KUidSepIc + string(MIfProv::idStr());}
	virtual TIdHash ifId() const override;
	virtual void MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual void resolve(TIdHash aTid) override;
	virtual bool isValid() const override { return mValid;}
	virtual void setValid(bool aValid) override;
	virtual MIface* iface() override { return nullptr;}
	virtual TIfaces* ifaces() override { return nullptr;}
	virtual const MIfProvOwner* owner() const override { return mOwner;}
	virtual MIfProv* findIface(const MIface* aIface) override;
	// From MIfReq
	virtual string MIfReq_Uid() const override { return mOwner->Uid() + Ifu::KUidSepIc + string(MIfReq::idStr());}
	virtual void MIfReq_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool isRequestor(MIfProvOwner* aOwner, int aPos) const override;
	virtual const MIfProvOwner* rqOwner() const override { return mOwner;}
	// TODO consider adding "root" method to nconn
	virtual MIfReq* prev() override;
	virtual MIfReq* tail() override;
	virtual bool isResolved() override;
	virtual void onProvInvalidated() override;
	// From MNCpp
	virtual bool detach(TPair* aPair) override;
	virtual LeafsIterator leafsBegin() override;
    public:
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
	virtual string MIfProv_Uid() const override { return mOwner->Uid() + Ifu::KUidSepIc + string(MIfProv::idStr());}
	virtual TIdHash ifId() const override;
	virtual void resolve(TIdHash aTid) override {}
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
	IfrNodeRoot(MIfProvOwner* aOwner, TIdHash aIfid): IfrNode(aOwner), mIfid(aIfid), mIcacheValid(false) {};
	// From MIfProv
	virtual TIdHash ifId() const override { return mIfid;}
	virtual void setValid(bool aValid) override;
	virtual TIfaces* ifaces() override;
	virtual void MIfProv_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MIfReq
	virtual void onProvInvalidated() override;
    protected:
	void updateIcache();
	void cleanIcache();
    protected:
	TIdHash mIfid;
	TIfaces mIcache;  /*!< Cache of ifaces, ref ds_irm_cr */
	bool mIcacheValid;
};

#endif
