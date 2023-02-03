
#ifndef __FAP3_MPROV_H
#define __FAP3_MPROV_H


#include <string>
#include <vector>

#include "miface.h"

class MEnv;
class MNode;
class MChromo;


using namespace std;

class DtBase;

/** @brief Interface of native agents provicer
 *
 * */
class MProvider: public MIface
{
    public:
	static const char* Type() { return "MProvider";};
	virtual ~MProvider() {};
	// From MIface
	virtual string Uid() const override { return MProvider_Uid();}
	virtual string MProvider_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MProvider_getLif(aType);}
	virtual MIface* MProvider_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MProvider_doDump(aLevel, aIdt, std::cout);}
	virtual void MProvider_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual const string& providerName() const = 0;
	virtual MNode* createNode(const string& aType, const string& aName, MEnv* aEnv) = 0;
	virtual MNode* provGetNode(const string& aUri) = 0;
	virtual bool isProvided(const MNode* aElem) const = 0;
	/** @brief Sets chromo resolution arguments
	 * Just one argument, chromo spec file extension is used
	 * */
	virtual void setChromoRslArgs(const string& aRargs) = 0;
	virtual void getChromoRslArgs(string& aRargs) = 0;
	virtual MChromo* createChromo(const string& aRargs = string()) = 0;
	virtual void getNodesInfo(vector<string>& aInfo) = 0;
	virtual const string& modulesPath() const = 0;
	virtual void setEnv(MEnv* aEnv) = 0;
	/** @brief Creates data instnce by the type info
	 * */
	virtual DtBase* createData(const string& aType) {return nullptr;}
};



#endif
