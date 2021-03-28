
#ifndef __FAP3_DES_H
#define __FAP3_DES_H

#include "mdata.h"
#include "mdes.h"
#include "vert.h"
#include "syst.h"

class BdVar;

/** @brief Connection point - input of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateInp: public ConnPointu
{
    public:
	static const char* Type() { return "CpStateInp";};
	CpStateInp(const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief Connection point - output of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStateOutp: public ConnPointu
{
    public:
	static const char* Type() { return "CpStateOutp";};
	static string PEType();
	CpStateOutp(const string& aName = string(), MEnv* aEnv = NULL);
};


/** @brief State, non-inhritable, monolitic, using host unit base organs, combined chain
 *
 * Ref ds_uac for unit based orgars, ds_mae for monolitic agents, ds_mae_scc for combined chain design
 * TODO It is not obvious solution to expose MDVarSet iface, but I don't find more proper solution ATM. To redesign?
 * */
class State: public Vertu, public MConnPoint, public MDesSyncable, public MDesInpObserver, public MBdVarHost, public MDVarSet
{
    public:
	static const char* Type() { return "State";};
	State(const string& aName = string(), MEnv* aEnv = NULL);
	// From Node.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MDesSyncable
	virtual string MDesSyncable_Uid() const override {return getUid<MDesSyncable>();}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void update() override;
	virtual void confirm() override;
	virtual bool isUpdated() const override;
	virtual void setUpdated() override;
	virtual void resetUpdated() override;
	virtual bool isActive() const override;
	virtual void setActive() override;
	virtual void resetActive() override;
	// From MDesInpObserver
	virtual string MDesInpObserver_Uid() const {return getUid<MDesInpObserver>();}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	virtual void onInpUpdated() override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const {return getUid<MDesInpObserver>();}
	virtual string provName() const override;
	virtual string reqName() const override;
	// From MBdVarHost
	virtual MDVarGet* HGetInp(const void* aRmt) override;
	virtual void HOnDataChanged(const void* aRmt) override;
	// From MDVarSet
	virtual string MDVarSet_Uid() const override {return getUid<MDVarSet>();}
	virtual string VarGetSIfid();
	virtual MIface *DoGetSDObj(const char *aName) override;
    protected:
	/** @brief Notifies dependencies of input updated */
	void NotifyInpsUpdated();
	bool IsLogeventUpdate();
    private:
	bool mActive;
	bool mUpdated;
	BdVar* mPdata;   //<! Preparing (updating) phase data
	BdVar* mCdata;   //<! Confirming phase data
};


#endif
