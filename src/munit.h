#ifndef __FAP3_MUNIT_H
#define __FAP3_MUNIT_H

#include "miface.h"
#include "menv.h"
#include "mifr.h"
#include "nconn.h"

class MIfProv;



#if 0
/** @brief Cooperative Interface provider
 * */
class MIfProvider
{
    public:
	/** @brief  Iface of Cooperative Ifaces provider iterator
	 * */
	class MIfIter {
	    public:
		virtual ~MIfIter() {};
		virtual MIfIter* Clone() const { return nullptr;};
		virtual MIfIter& operator=(const MIfIter& aIt) = 0;
		virtual MIfIter& operator++() =0;
		virtual bool operator==(const MIfIter& aIt) { return EFalse;};
		virtual MIface*  operator*() {return nullptr;};
	};

	/** @brief Cooperative Ifaces provider iterator 
	 * */
	class TIfIter {
	    public:
		TIfIter(): mImpl(nullptr), mCloned(EFalse) {};
		TIfIter(MIfIter& aImpl): mImpl(&aImpl), mCloned(false) {};
		TIfIter(const TIfIter& aIt): mImpl(nullptr), mCloned(true) { if (aIt.mImpl) mImpl = aIt.mImpl->Clone(); };
		~TIfIter() { if (mCloned) delete mImpl; mImpl = nullptr;};
		TIfIter& operator=(const TIfIter& aIt);
		TIfIter& operator++() { mImpl->operator++(); return *this;};
		TIfIter operator++(int) { TIfIter tmp(*this); operator++(); return tmp; };
		bool operator==(const TIfIter& aIt) { return (mImpl && aIt.mImpl) ? mImpl->operator==((*aIt.mImpl)) : mImpl == aIt.mImpl;};
		bool operator!=(const TIfIter& aIt) { return !operator==(aIt);};
		virtual MIface*  operator*() { return mImpl->operator*();};
	    protected:
		MIfIter* mImpl;
		bool mCloned;
	};

    public:
	virtual MIfIter getIfi(const string& aName, const MIfReq& aReq) = 0;
};
#endif




class MUnit: public MIface
{
    public:
	using TIfReqCp = MNcpp<MIfReq, MIfProv>; /*!< IFR requestor connpoint type */
    public:
	// From MIface
	static const char* Type() { return "MUnit";}
	virtual ~MUnit() {} // TODO to consider policy of system destruction
	virtual string Uid() const override { return MUnit_Uid();}
	virtual string MUnit_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MUnit_getLif(aType);}
	virtual MIface* MUnit_getLif(const char *aType) = 0;
	// Local
	virtual string name() const = 0;
	virtual bool getContent(string& aData, const string& aName = string()) const = 0;
	virtual bool setContent(const string& aData, const string& aName = string()) = 0;
	virtual bool addContent(const string& aName, bool aLeaf = false) = 0;
	/** @brief Returns unit iface default IFR provider
	 * This is to simplify requesting iface in case if the client don't have its own requestor
	 * */
	virtual MIfProv* defaultIfProv(const string& aName) = 0;
	/** @brief Resolves requested interface
	 * It is only used by client requestor if the requestor hasn't resolution via this unit, i.e. hasn't 
	 * connection to this unit iface provider
	 * */
	virtual bool resolveIface(const string& aName, TIfReqCp* aReq) = 0;
	// Debug
	virtual void dumpContent() const = 0;
};


#endif  //  __FAP3_MUNIT_H
