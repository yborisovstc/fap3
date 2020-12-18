#ifndef __FAP3_SYST_H
#define __FAP3_SYST_H

#include "menv.h"
#include "msyst.h"

#include "vert.h"

class ConnPointu: public Vertu, public MConnPoint
{
    public:
	ConnPointu(const string &aName, MEnv* aEnv);
	virtual ~ConnPointu() {}
	// From MVert::MCIface::MIface
	virtual MIface *getLif(const char *aType) override;
	// From MVert::MCIface
	virtual bool isCompatible(MCIface* aPair) const override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const { return MConnPoint::Type();}
	virtual string provided() const override;
	virtual string required() const override;
    protected:
	// From Unit
	virtual IfrNode* createDefaultIfProv(const string& aName) const override;
};

#endif
