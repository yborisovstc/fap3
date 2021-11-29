#ifndef __FAP3_RDATAURI_H
#define __FAP3_RDATAURI_H

#include "guri.h"
#include "rdata.h"


/** @brief Uri data
 * Wraps Guri to aline with HDt
 * */
class DGuri: public DtBase
{
    public:
	DGuri(): DtBase() {}
	DGuri(const DGuri& aSrc): DtBase(aSrc), mData(aSrc.mData) {}
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString);
	static bool IsDataFit(const DGuri& aData);
    public:
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual bool DataFromString(istringstream& aStream, bool& aRes) override;
	virtual void DataToString(stringstream& aStream) const override;
	virtual bool operator==(const MDtBase& b) override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DGuri&>(b).mData);}
	bool operator>(const DGuri& b) const { return mData > b.mData;};
	bool operator>=(const DGuri& b) const { return mData >= b.mData;};
	bool operator<(const DGuri& b) const { return mData < b.mData;};
	bool operator<=(const DGuri& b) const { return mData <= b.mData;};
    public:
	GUri mData;
};

#endif

