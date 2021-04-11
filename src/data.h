
#ifndef __FAP3_DATA_H
#define __FAP3_DATA_H


#include "mdata.h"


/** @brief Interface of data host
 * */
class DHost
{
    public:
	virtual void HUpdateProp() = 0;
	virtual void HNotifyUpdate() = 0;
};

// Data handler base
class HBase: public MIface
{
    public:
	HBase(DHost* aHost): mHost(*aHost) {}
	virtual ~HBase() {}
	// From MIface
	virtual string Uid() const override { return "HBase";}
	virtual MIface* getLif(const char *aType) override { return HBase_getLif(aType);}
	virtual MIface* HBase_getLif(const char *aType) = 0;
	// Local
	virtual bool FromString(const string& aString) = 0;
	virtual void ToString(string& aString) = 0;
	virtual bool Set(MDVarGet* aInp) = 0;
	//!< Gets Id of data getter iface
	virtual string IfaceGetId() const = 0;
	//!< Gets Id of data setter iface
	virtual string IfaceSGetId() const = 0;
	virtual bool IsValid() const { return true;}
	virtual bool IsSigOK() const { return false;}
	DHost& mHost;
};


// Matrix
template <class T> class HMtr: public HBase, public MMtrGet<T> {
    public:
	HMtr(DHost* aHost, const string& aCont);
	HMtr(DHost* aHost, const Mtr<T>& aData);
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface* HBase_getLif(const char *aType) override;
	virtual string IfaceGetId() const { return MMtrGet<T>::Type();};
	virtual string IfaceSGetId() const { return string();}
	// From MMtrdGet
	virtual void MtrGet(Mtr<T>& aData);
	// From HBase
	virtual bool FromString(const string& aString);
	virtual void ToString(string& aString);
	virtual bool Set(MDVarGet* aInp);
	virtual bool IsValid() const { return mData.mValid;};
    protected:
	Mtr<T> mData;
};

/** @brief Generic data
 * @tparam  T Underlying data type, DtBase inherited 
 * */
template <class T> class HDt: public HBase, public MDtGet<T>, public MDtSet<T> {
    public:
	HDt(DHost* aHost, const string& aCont);
	HDt(DHost* aHost, const T& aData);
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface* HBase_getLif(const char *aType) override;
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual string IfaceSGetId() const { return string();}
	// From MDtGet
	virtual void DtGet(T& aData) override;
	// From MDtSet
	virtual void DtSet(const T& aData) override;
	// From HBase
	virtual bool FromString(const string& aString);
	virtual void ToString(string& aString);
	virtual bool Set(MDVarGet* aInp);
	virtual bool IsValid() const { return mData.mValid;};
	virtual bool IsSigOK() const { return mData.mSigTypeOK;};
    protected:
	T mData;
};


/** @brief Variant base data
 *
 * This data can be used in two phase data processing, like DES state
 * */
class BdVar: public MDVar, public MUpdatable, public MDVarGet, public MDVarSet, public DHost
{
    public:
	static const char* Type() { return "BdVar";};
	BdVar(MBdVarHost *aHost);
	virtual ~BdVar();
	bool ToString(string& aType, string& aData);
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	virtual bool IsValid() const;
	// From MUpdatable
	virtual string MUpdatable_Uid() const override { return mHost->GetDvarUid(this);}
	virtual bool update() override;
	// From MDVarGet
	virtual string MDVarGet_Uid() const override {return mHost->GetDvarUid(this);}
	virtual string VarGetIfid() const override;
	virtual MIface* DoGetDObj(const char *aName) override;
	// From MDVarSet
	virtual string MDVarSet_Uid() const override { return mHost->GetDvarUid(this);}
	virtual string VarGetSIfid();
	virtual MIface *DoGetSDObj(const char *aName) override;
	// From MDVar
	virtual string MDVar_Uid() const override { return mHost->GetDvarUid(this);}
	virtual MIface* MDVar_getLif(const char *aType) override;
	// From DHost
	virtual void HUpdateProp();
	virtual void HNotifyUpdate();
	// Debug
	string GetValue();
    protected:
	bool Init(const string& aString, MDVarGet* aInp = NULL);
	void NotifyUpdate();
    protected:
	HBase* mData;  //<! Data
	MBdVarHost* mHost; //<! Host
};


#endif
