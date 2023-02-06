#ifndef __FAP3_RMUTDATA_H
#define __FAP3_RMUTDATA_H

#include "chromo.h"
#include "rdata.h"
#include "chromo2.h"

/** @brief Mutation data
 * Wraps TMut to aline with HDt
 * */
class DMut: public DtBase
{
    public:
	DMut(): DtBase() {};
	DMut(const DMut& aSrc): DtBase(aSrc), mData(aSrc.mData) {};
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString);
	static bool IsDataFit(const DMut& aData);
    public:
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual void DataFromString(istringstream& aStream) override;
	virtual void DataToString(ostringstream& aStream) const override;
	virtual bool operator==(const MDtBase& b) const override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DMut&>(b).mData);}
	virtual bool operator!=(const MDtBase& b) const override { return !DMut::operator==(b);}
	//virtual bool IsCompatible(const DtBase& aSrc) override;
	virtual DtBase& operator=(const DtBase& b) override {
	    if (IsCompatible(b) && b.IsValid()) {
		this->DtBase::operator=(b);
		auto bp = reinterpret_cast<const DMut&>(b);
		mData = bp.mData;
	    } else {
		mValid = false;
	    }
	    return *this;
	}
    public:
	TMut mData;
};


/** @brief Chromo2 segment data
 * Wraps Chromo to aline with HDt
 * */
class DChr2: public DtBase
{
    public:
	using TData = Chromo2;
    public:
	DChr2();
	DChr2(const DChr2& aSrc): DtBase(aSrc), mData(aSrc.mData) {};
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString);
	static bool IsDataFit(const DChr2& aData);
    public:
	//DChr2& operator=(const DChr2& b) { this->DtBase::operator=(b); mData = b.mData; return *this;};
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual void DataFromString(istringstream& aStream) override;
	virtual void DataToString(ostringstream& aStream) const override;
	virtual DtBase& operator=(const DtBase& b) override;
	virtual bool operator==(const MDtBase& b) const override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DChr2&>(b).mData);}
	virtual bool operator!=(const MDtBase& b) const override { return !DChr2::operator==(b);}
    public:
	Chromo2 mData;
};


#if 0
/** @brief Chromo segment data
 * The segment combines with DMut
 * */
class DMutSeg : public DtBase
{
    public:
	using TData = vector<TMut>;
    public:
	DMutSeg(): DtBase() {}
	DMutSeg(const DMutSeg& aSrc): DtBase(aSrc), mData(aSrc.mData) {}
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString);
	static bool IsDataFit(const DMutSeg& aData);
    public:
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual bool DataFromString(istringstream& aStream, bool& aRes) override;
	virtual void DataToString(stringstream& aStream) const override;
	virtual bool operator==(const MDtBase& b) override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DMutSeg&>(b).mData);}
	virtual bool operator!=(const MDtBase& b) override { return !DMutSeg::operator==(b);}
    public:
	TData mData;
};
#endif

#endif

