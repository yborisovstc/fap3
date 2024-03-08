#ifndef __FAP3_PROV_H
#define __FAP3_PROV_H

#include <unordered_map>
#include <map>
#include <unordered_map>

#include "mprov.h"


class MNode;

/** @brief Base provider
 *
 * */
class Provider: public MProvider
{
    public:
	typedef pair<string, MNode*> TRegVal;
	typedef map<string, MNode*> TReg;
    public:
	static const char* Type() { return "Provider";};
	Provider(const string& aName, MEnv* aEnv);
	virtual ~Provider();
	// From MProvider/MIface
	virtual string MProvider_Uid() const override { return mName;};
	virtual MIface* MProvider_getLif(const char *aType) override;
	virtual void MProvider_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MProvider
	virtual const string& providerName() const override { return mName;}
	virtual MNode* createNode(const string& aType, const string& aName, MEnv* aEnv) override;
	virtual MNode* provGetNode(const string& aUri) override;
	virtual bool isProvided(const MNode* aElem) const override;
	virtual MChromo* createChromo(const string& aRargs = string()) override;
	virtual void getNodesInfo(vector<string>& aInfo) override;
	virtual const string& modulesPath() const override;
	virtual void setEnv(MEnv* aEnv) override;
    protected:
	MEnv* mEnv;
	TReg mReg;
	string mName;
};

/** @brief Provider base using factory regiser
 * */
class ProvBase: public Provider
{
    public:
	/** Native agent factory function */
	using TFact = MNode* (const string &aName, MEnv* aEnv);
	/** Registry of native agents factory function */
	using TFReg = unordered_map<string, TFact*>;
	/** Data factory function */
	using TDtFact = DtBase* ();
	/** Registry of data factory function */
	using TDtFReg = map<string, TDtFact*>;
    public:
	static const char* Type() { return "ProvBase";};
	ProvBase(const string& aName, MEnv* aEnv);
	virtual ~ProvBase();
	virtual const TFReg& FReg() const = 0;
	virtual const TDtFReg& FDtReg() const = 0;
	// From MProvider
	virtual MNode* createNode(const string& aType, const string& aName, MEnv* aEnv);
	virtual MNode* provGetNode(const string& aUri);
	virtual MChromo* createChromo();
	virtual DtBase* createData(const string& aType) override;
    protected:
	/** Creates native agent */
	MNode* CreateAgent(const string& aType, const string& aName, MEnv* aEnv) const;
	DtBase* CreateData(const string& aType) const;
	template<typename T> static pair<string, ProvBase::TFact*> Item();
	template<typename T> static pair<string, ProvBase::TDtFact*> DItem();
    private:
	vector<string> iNodesInfo;
};

/** Generator of native agent factory registry item */
template<typename T> pair<string, ProvBase::TFact*> ProvBase::Item() {
    return pair<string, ProvBase::TFact*>
	(T::Type(), [](const string &name, MEnv* env)->MNode* { return new T(T::Type(), name, env);});
}

/** Generator of data factory registry item */
template<typename T> pair<string, ProvBase::TDtFact*> ProvBase::DItem() {
    return pair<string, ProvBase::TDtFact*>
	(T::TypeSig(), []()->DtBase* { return new T();});
}



#endif
