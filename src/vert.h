#ifndef __FAP3_VERT_H
#define __FAP3_VERT_H

#include <set>

#include "mvert.h"
#include "unit.h"

using namespace std;


class Vertu : public Unit, public MVert
{
    public:
	using TPairs = set<MVert*>; 
    public:
	Vertu(const string &aName, MEnv* aEnv): Unit(aName, aEnv) {}
	virtual ~Vertu();
	// From MVert::MCIface
	virtual bool connect(MCIface* aPair) override;
	virtual bool disconnect(MCIface* aPair) override;
	virtual bool isConnected(MCIface* aPair) const override;
	virtual bool isCompatible(MCIface* aPair) const override;
	// From MVert
	virtual bool isCompatible(MVert* aPair, bool aExt = false) override {return true;}
	virtual MVert* getExtd() override {return nullptr;}
	virtual TDir getDir() const override { return ERegular;}
	virtual int pairsCount() const = 0;
	virtual MVert* getPair(int aInd) const = 0;
	virtual bool isPair(const MVert* aPair) const = 0;
	virtual bool isLinked(const MVert* aPair, bool aDirect = false) const = 0;
	virtual string MVert_Uid() const { return "Vertu";}
    protected:
    protected:
	TPairs mPairs;
};

#endif
