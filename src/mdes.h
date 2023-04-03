
#ifndef __FAP3_MDES_H
#define __FAP3_MDES_H

#include "miface.h"

#include "nconn.h"

using namespace std;

class MDesSyncable;

/** @brief Components Observer. Upper layer is observer of components 
 *
 * Components notifies upper layer of status changes
 * */
class MDesObserver: public MIface
{
    public:
	static const char* Type() { return "MDesObserver";};
	// From MIface
	virtual string Uid() const override { return MDesObserver_Uid();}
	virtual string MDesObserver_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesObserver_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	/** @brief Notification that component was activated */
	virtual void onActivated(MDesSyncable* aComp) = 0;
	/** @brief Notification that component was changed */
	virtual void onUpdated(MDesSyncable* aComp) = 0;
};

/** @brief Inputs Observer
 *
 * State is notified by its inputs of inputs update
 * */
class MDesInpObserver: public MIface
{
    public:
	static const char* Type() { return "MDesInpObserver";};
	// From MIface
	virtual string Uid() const override { return MDesInpObserver_Uid();}
	virtual string MDesInpObserver_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesInpObserver_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	/** @brief Notification that input state was changed */
	virtual void onInpUpdated() = 0;
};

/** @brief Interface of DES syncable
 * */
class MDesSyncable: public MIface
{
    public:
	static const char* Type() { return "MDesSyncable";};
	// From MIface
	virtual string Uid() const override { return MDesSyncable_Uid();}
	virtual string MDesSyncable_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesSyncable_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	virtual MIface* getLif(const char *aType) { return MDesSyncable_getLif(aType); }
	virtual MIface* MDesSyncable_getLif(const char *aType) = 0;
	// Local
	virtual void update() = 0;
	virtual void confirm() = 0;
	virtual void setUpdated() = 0; // TODO not used with ds_mdc_sw
	virtual void setActivated() = 0;
	/** @brief Debug. Returns the count of active synched owneds */
	virtual int countOfActive(bool aLocal = false) const = 0;
};

class MDesCtxCsm;
class MVert;

/* @brief DES context supplier, ref ds_dctx
 * Gets access to common states for components
 * */
class MDesCtxSpl : public MIface
{
    public:
	using TCp = MNcpp<MDesCtxSpl, MDesCtxCsm>;
    public:
	static const char* Type() { return "MDesCtxSpl";};
	// From MIface
	virtual string Uid() const override { return MDesCtxSpl_Uid();}
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesCtxSpl_doDump(aLevel, aIdt, std::cout);}
	virtual MIface* getLif(const char *aType) { return MDesCtxSpl_getLif(aType); }
	virtual string MDesCtxSpl_Uid() const = 0;
	virtual void MDesCtxSpl_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	virtual MIface* MDesCtxSpl_getLif(const char *aType) = 0;
	// Local
	virtual string getSplId() const = 0;
	/* @brief Gets head of suppliers stack, ref ds_dctx_dic_cs
	 * */
	virtual MDesCtxSpl* getSplsHead() = 0;
	virtual bool registerCsm(MNcpp<MDesCtxCsm, MDesCtxSpl>* aCsm) = 0;
	/* @brief binds context with given ID
	 * @param aCtxId  ID of the context
	 * @param aCtx    the context
	 * */
	virtual bool bindCtx(const string& aCtxId, MVert* aCtx) = 0;
	virtual bool unbindCtx(const string& aCtxId) = 0;
	virtual TCp* splCp() = 0;
};

/* @brief DES context consumer
 * */
class MDesCtxCsm : public MIface
{
    public:
	using TCp = MNcpp<MDesCtxCsm, MDesCtxSpl>;
    public:
	static const char* Type() { return "MDesCtxCsm";};
	// From MIface
	virtual string Uid() const override { return MDesCtxCsm_Uid();}
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesCtxCsm_doDump(aLevel, aIdt, std::cout);}
	virtual string MDesCtxCsm_Uid() const = 0;
	virtual void MDesCtxCsm_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual string getCsmId() const = 0;
	/* @brief Handles context addition */
	virtual void onCtxAdded(const string& aCtxId) = 0;
	// TODO Is it needed. Spl can simply disconnect the ctx
	virtual void onCtxRemoved(const string& aCtxId) = 0;
};


/** @brief DES Service point client
 * */
class MDesSpc: public MIface
{
    public:
	static const char* Type() { return "MDesSpc";};
	// From MIface
	virtual string Uid() const override { return MDesSpc_Uid();}
	virtual string MDesSpc_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesSpc_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesSpc_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	virtual MIface* getLif(const char *aType) { return MDesSpc_getLif(aType); }
	virtual MIface* MDesSpc_getLif(const char *aType) = 0;
	// Local
	/** @brief Provides service/client ID
	 * */
	virtual string getId() const = 0;
};

#endif
