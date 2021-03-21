#ifndef __FAP3_MECONT_H
#define __FAP3_MECONT_H

#include "miface.h"



/** @brief Iface of native hier embedded content provider, ref ds_dce
 * */
class MContent: public MIface
{
    public:
	static const char* Type() { return "MContent";};
	// From MIface
	virtual string Uid() const override { return MContent_Uid();}
	virtual string MContent_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MContent_getLif(aType);}
	virtual MIface* MContent_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { MContent_doDump(aLevel, aIdt, aOs);}
	virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual string contName() const = 0;
	virtual bool getData(string& aData) const = 0;
	virtual bool setData(const string& aData) = 0;
};


class GUri;

/** @brief Iface of DCE content owner
 * */
class MContentOwner: public MIface
{
    public:
	static const char* Type() { return "MContentOwner";};
	// From MIface
	virtual string Uid() const override { return MContentOwner_Uid();}
	virtual string MContentOwner_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MContentOwner_getLif(aType);}
	virtual MIface* MContentOwner_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { MContentOwner_doDump(aLevel, aIdt, aOs);}
	virtual void MContentOwner_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	/** @brief Returns count of contents owner */
	virtual int contCount() const = 0;
	/** @brief Gets owned content by given index */
	virtual MContent* getCont(int aIdx) = 0;
	virtual const MContent* getCont(int aIdx) const = 0;
	virtual bool getContent(const GUri& aCuri, string& aRes) const = 0;
	virtual bool setContent(const GUri& aCuri, const string& aData) = 0;
};

#endif
