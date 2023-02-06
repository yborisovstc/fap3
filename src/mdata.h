#ifndef __FAP3_MDATA_H
#define __FAP3_MDATA_H

#include <set>
#include <vector>
#include <string>
#include "rdata.h"
#include "miface.h"

using namespace std;


/** @brief Interface of updatable data
 * */
class MUpdatable: public MIface
{
    public:
	static const char* Type() { return "MUpdatable";};
	// From MIface
	virtual string Uid() const override { return MUpdatable_Uid();}
	virtual string MUpdatable_Uid() const = 0;
	// Local
	virtual bool update() = 0;
};


class MDVar: public MIface
{
    public:
	static const char* Type() { return "MDVar";};
	// From MIface
	virtual string Uid() const override { return MDVar_Uid();}
	virtual string MDVar_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MDVar_getLif(aType);}
	virtual MIface* MDVar_getLif(const char *aType) = 0;
};



/** @brief Interface of getter of variable type data
 * */
class MDVarGet: public MIface
{
    public:
	static const char* Type() { return "MDVarGet";};
	// From MIface
	virtual string Uid() const override { return MDVarGet_Uid();}
	virtual string MDVarGet_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDVarGet_doDump(aLevel, aIdt, std::cout);}
	virtual void MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const {}
	// Local
	template <class T> T* GetDObj(T* aInst) { return aInst = static_cast<T*>(DoGetDObj(aInst->Type()));};
	virtual MIface* DoGetDObj(const char *aName) = 0;
	/** @brief Gets an iface id. Is used for type negotiation from root to leafs */
	virtual string VarGetIfid() const = 0;
	/** @brief Gets data by type
	 * */
	virtual bool VDtGet(const DtBase& aData) { return false;}
	virtual const DtBase* VDtGet(const string& aType) { return nullptr;}
	template<class T> const T* DtGet(const T* aDType) { return reinterpret_cast<const T*>(VDtGet(aDType->TypeSig()));}
	template<class T> const T* DtGet() { return reinterpret_cast<const T*>(VDtGet(T::TypeSig()));}
};


/** @brief Generic data getter base
 * Temporary solution for ds_rdi
 * */
class MDtGetBase: public MIface
{
    public:
	virtual bool DtbGet(MDtBase& aData) { return false;}
	/** @brief Duplicate (allocate and copy) data
	 * */
	virtual MDtBase* DtbDup() { return nullptr;}
	static const string MDtGetType(const string& aDataType) { return string("MDtGet_") + aDataType; }
};

// Generic data getter

template <class T> class MDtGet: public MDtGetBase
{
    public:
	static const string mType;
	static const char* Type() { return mType.c_str();};
	// From MIface
	virtual string Uid() const override { return MDtGet_Uid();}
	virtual string MDtGet_Uid() const { return mType;}
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDtGet_doDump(aLevel, aIdt, std::cout);}
	virtual void MDtGet_doDump(int aLevel, int aIdt, ostream& aOs) const {}
	// From MDtGetBase
	virtual bool DtbGet(MDtBase& aData) override {
	    bool res = false;
	    T* data = dynamic_cast<T*>(&aData);
	    if (data) {
		DtGet(*data); res = true;
	    }
	    return res;
	}
	virtual MDtBase* DtbDup() override {
	    MDtBase* data = new T; DtbGet(*data); return data;
	}
	// Local
	virtual void DtGet(T& aData) = 0;
};

template<class T> const string MDtGet<T>::mType = string("MDtGet_") + T::TypeSig();


/** @brief Generic data setter interface
 * */
template <class T> class MDtSet: public MIface
{
    public:
	static const string mType;
	static const char* Type() { return mType.c_str();};
	// From MIface
	virtual string Uid() const override { return mType;}
	// Local
	virtual void DtSet(const T& aData) = 0;
};

template<class T> const string MDtSet<T>::mType = string("MDtSet_") + T::TypeSig();

// Matrix
template <class T> class MMtrGet: public MIface
{
    public:
	static const char* Type();
	static const char* TypeSig();
	virtual void MtrGet(Mtr<T>& aData) = 0;
};


/** @brief Interface of variant base data host
 * */
class MBdVarHost
{
    public:
	virtual MDVarGet* HGetInp(const void* aRmt) = 0;
	virtual void HOnDataChanged(const void* aRmt) = 0;
	virtual string GetUid() { return "?";}
	virtual string GetDvarUid(const MDVar* aComp) const = 0;
};

template <typename T>
class MVectorGet : public MIface
{
    public:
	static const char* Type();
    public: 
	virtual int Size() const = 0;
	virtual bool GetElem(int aInd, T& aElem) const = 0;
	virtual string MVectorGet_Mid() const = 0;
	virtual MIface* MVectorGet_Call(const string& aSpec, string& aRes) = 0;
	// From MIface
	MIface* Call(const string& aSpec, string& aRes) override {return MVectorGet_Call(aSpec, aRes);}
	string Mid() const override { return MVectorGet_Mid();}
};


#endif
