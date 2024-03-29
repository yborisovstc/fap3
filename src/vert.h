#ifndef __FAP3_VERT_H
#define __FAP3_VERT_H

#include <set>

#include "mvert.h"
#include "unit.h"
#include "elem.h"

using namespace std;


/** @brief Vertex, not inheritable
 * */
class Vertu : public Unit, public MVert
{
    public:
	using TPairs = set<MVert*>; 
    public:
	static const char* Type() { return "Vertu";}
	Vertu(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Vertu();
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MVert::MCIface
	virtual bool connect(MCIface* aPair) override;
	virtual bool disconnect(MCIface* aPair) override;
	virtual bool isConnected(MCIface* aPair) const override;
	virtual bool isCompatible(MCIface* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	// From MVert
	virtual string MVert_Uid() const { return getUid<MVert>();}
	virtual MIface *MVert_getLif(const char *aType) override;
	virtual void MVert_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool isCompatible(MVert* aPair, bool aExt) override {return true;}
	virtual MVert* getExtd() override {return nullptr;}
	virtual TDir getDir() const override { return ERegular;}
	virtual int pairsCount() const override;
	virtual MVert* getPair(int aInd) const override;
	virtual bool isPair(const MVert* aPair) const override;
	virtual bool isLinked(const MVert* aPair, bool aDirect = false) const override;
    protected:
	// Local
	void disconnect();
	virtual void onConnected();
	virtual void onDisconnected();
    protected:
	TPairs mPairs;
};


/** @brief Vertex, inheritable
 * */
class Vert : public Elem, public MVert
{
    public:
	using TPairs = set<MVert*>; 
    public:
	static const char* Type() { return "Vert";}
	Vert(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Vert();
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MVert::MCIface
	virtual bool connect(MCIface* aPair) override;
	virtual bool disconnect(MCIface* aPair) override;
	virtual bool isConnected(MCIface* aPair) const override;
	virtual bool isCompatible(MCIface* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	// From MVert
	virtual string MVert_Uid() const { return getUid<MVert>();}
	virtual MIface *MVert_getLif(const char *aType) override;
	virtual void MVert_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual bool isCompatible(MVert* aPair, bool aExt) override {return true;}
	virtual MVert* getExtd() override {return nullptr;}
	virtual TDir getDir() const override { return ERegular;}
	virtual int pairsCount() const override;
	virtual MVert* getPair(int aInd) const override;
	virtual bool isPair(const MVert* aPair) const override;
	virtual bool isLinked(const MVert* aPair, bool aDirect = false) const override;
    protected:
	// Local
	void disconnect();
	virtual void onConnected();
	virtual void onDisconnected();
    protected:
	TPairs mPairs;
};


#endif
