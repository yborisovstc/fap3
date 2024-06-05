#ifndef __FAP3_FACT_H
#define __FAP3_FACT_H

#include "map"

#include "mprov.h"


/** @brief Native entities factory
 * */
class Factory: public MProvider
{
    public:
	typedef map<string, MProvider*> TProviders;
	typedef pair<string, MProvider*> TProvidersElem;
    public:
	static const char* Type() { return "Factory";};
	Factory(const string& aName, MEnv* aEnv);
	virtual ~Factory();
	bool LoadPlugin(const string& aName);
	void LoadPlugins();
	bool AddProvider(MProvider* aProv);
	void RemoveProvider(MProvider* aProv);
	// From MProvider/MIface
	virtual string MProvider_Uid() const override { return Type();}
	virtual MIface* MProvider_getLif(const char *aType) override { return nullptr;}
	virtual void MProvider_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MProvider
	virtual const string& providerName() const { return iName;};
	virtual MNode* createNode(const string& aType, const string& aName, MEnv* aEnv);
	virtual MNode* provGetNode(const string& aUri);
	virtual bool isProvided(const MNode* aElem) const;
	virtual void setChromoRslArgs(const string& aRargs);
	virtual void getChromoRslArgs(string& aRargs);
	virtual MChromo* createChromo(const string& aRargs = string());
	virtual void getNodesInfo(vector<string>& aInfo);
	virtual const string& modulesPath() const;
	virtual void setEnv(MEnv* aEnv);
	virtual DtBase* createData(const string& aType) override;
    protected:
	static int FilterPlgDirEntries(const struct dirent *aEntry);
    protected:
	TProviders iProviders;
	MEnv* iEnv;
	string iName;
	string mChromoRargs; //!<  Chromo resolutin arguments
};



#endif
