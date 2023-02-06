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
	DGuri(const GUri& aSrc): DGuri() {  mData = aSrc; mValid = true; }
	DGuri(const DGuri& aSrc): DtBase(aSrc), mData(aSrc.mData) {}
	static const char* TypeSig();
	static bool IsSrepFit(const string& aString);
	static bool IsDataFit(const DGuri& aData);
    public:
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual void DataFromString(istringstream& aStream) override;
	virtual void DataToString(ostringstream& aStream) const override;
	virtual DtBase& operator=(const DtBase& b) override {
	    if (IsCompatible(b) && b.IsValid()) {
		this->DtBase::operator=(b);
		const DGuri& bp = reinterpret_cast<const DGuri&>(b);
		mData = bp.mData;
	    } else {
		mValid = false;
	    }
	    return *this;
	}
	DGuri& operator+=(const DGuri& b) { mData.append(b.mData); return *this;}
	virtual bool operator==(const MDtBase& b) const override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DGuri&>(b).mData);}
	virtual bool operator!=(const MDtBase& b) const override { return !DGuri::operator==(b);}
	bool operator>(const DGuri& b) const { return mData > b.mData;};
	bool operator>=(const DGuri& b) const { return mData >= b.mData;};
	bool operator<(const DGuri& b) const { return mData < b.mData;};
	bool operator<=(const DGuri& b) const { return mData <= b.mData;};
    public:
	friend std::ostream& operator<<(std::ostream& os, const DGuri& aDuri);
	friend std::istream& operator>>(std::istream& is, DGuri& aDuri);
    public:
	GUri mData;
};




#endif

