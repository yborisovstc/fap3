#ifndef __FAP3_SYST_H
#define __FAP3_SYST_H

#include "menv.h"
#include "msyst.h"

#include "vert.h"

class ConnPointu: public Vertu, public MConnPoint
{
    friend class CpIfrNode;
    public:
	ConnPointu(const string &aName, MEnv* aEnv);
	virtual ~ConnPointu() {}
	// From MUnit
	virtual MIface *MUnit_getLif(const char *aType) override;
	// From MVert::MCIface
	virtual bool isCompatible(MCIface* aPair) const override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const { return name() + Ifu::KUidSep + MConnPoint::Type();}
	virtual string provName() const override;
	virtual string reqName() const override;
    protected:
	// From Unit
	virtual IfrNode* createIfProv(const string& aName, TIfReqCp* aReq) const override;
};

#endif
