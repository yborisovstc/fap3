
#ifndef __FAP3_DESSP_H
#define __FAP3_DESSP_H

#include "mdes.h"
#include "syst.h"

/** @brief DES Servicing point, ref ds_ssp
 * NOT COMPLETED. Not proved with high-level use-cases
 * */

/** @brief DES Service Pont (extender), ref ds_ssp
 * */
class DesSpe : public Extd
{
    public:
	static const char* Type() { return "DesSpe";}
	DesSpe(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
};


/** @brief DES Service Pont (extended), ref ds_ssp
 * */
class DesSp : public Socket
{
    public:
	static const char* Type() { return "DesSp";};
	DesSp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
};


/** @brief DES Service terminal Pont, ref ds_ssp
 * */
class DesSpt : public Socket
{
    public:
	static const char* Type() { return "DesSpt";};
	DesSpt(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
    protected:
	// From MNode
	virtual string parentName() const override { return Type(); }
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
};



/** @brief DES service point client agent
 * */
class ADesSpc : public Unit, public MAgent, public MDesSpc
{
    public:
	using TAgtCp = NCpOnp<MAgent, MAhost>;  /*!< Agent conn point */
    public:
	static const char* Type() { return "ADesSpc";};
	ADesSpc(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Unit.MIfProvOwner
	//virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MDesSpc
	virtual string MDesSpc_Uid() const override {return getUid<MDesSpc>();}
	virtual void MDesSpc_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual MIface* MDesSpc_getLif(const char *aType) override { return nullptr; }
	virtual string getId() const override;
	// From MAgent
	virtual string MAgent_Uid() const override {return getUid<MAgent>();}
	virtual MIface* MAgent_getLif(const char *aType) override;
	// From Node.MOwned
	virtual void onOwnerAttached() override;
    protected:
	TAgtCp mAgtCp;                   /*!< Agent connpoint */ 
};

#endif
