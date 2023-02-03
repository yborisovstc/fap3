#ifndef __FAP3_PROVDEF_H
#define __FAP3_PROVDEF_H


#include "prov.h"


/** @brief Default provider
 * */
class ProvDef: public ProvBase
{
    public:
	ProvDef(const string& aName, MEnv* aEnv);
	virtual ~ProvDef();
	// From ProvBase
	virtual const TFReg& FReg() const override {return mReg;}
	virtual const TDtFReg& FDtReg() const override {return mDtReg;}
	// From MProvider
	virtual void getNodesInfo(vector<string>& aInfo);
	virtual const string& modulesPath() const;
	virtual void setChromoRslArgs(const string& aRargs) {}
	virtual void getChromoRslArgs(string& aRargs) {}
	virtual MChromo* createChromo(const string& aRargs = string());
    private:
	static const TFReg mReg;
	static const TDtFReg mDtReg;
};


#endif
