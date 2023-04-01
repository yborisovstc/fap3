#ifndef __FAP3_MENV_H
#define __FAP3_MENV_H

#include "miface.h"
#include "mprov.h"
#include "mlog.h"
#include "mprof.h"


/** @brief Imprort manager interface
 * */
class MImportMgr
{
    public:
	virtual void GetModulesNames(vector<string>& aModules) const = 0;
	virtual void ResetImportsPaths() = 0;
	virtual void AddImportsPaths(const string& aPaths) = 0;
	virtual string GetModulePath(const string& aModName) const = 0;
	virtual bool Import(const string& aUri) = 0;
};


class GUri;

/** @brief Execution environment interface
 * */
class MEnv : public MIface
{
    public:
	static const char* Type() { return "MEnv";};
	virtual ~MEnv() {}
    public:
	// From MIface
	virtual string Uid() const override { return MEnv_Uid();}
	virtual string MEnv_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MEnv_getLif(aType);}
	virtual MIface* MEnv_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MEnv_doDump(aLevel, aIdt, std::cout);}
	virtual void MEnv_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
        // Local
	virtual MProvider* provider() const = 0;
	/** @brief Constructs model with the chromo spec set */
	virtual void constructSystem() = 0;
	/** @brief Gets logger instance */
	virtual MLogRec* Logger() = 0;
	/** @brief Gets profiler iface */
	virtual MProfiler *profiler() = 0;
	/** @brief Gets import manager */
	virtual MImportMgr* ImpsMgr() = 0;
	/** @brief Gets root node */
	// TODO Security gap, remove
	virtual MNode* Root() const = 0;
	/** @brief Adds provider */
	virtual bool addProvider(MProvider* aProv) = 0;
	/** @brief Removes provider */
	virtual void removeProvider(MProvider* aProv) = 0;
	/** @brief Runs DES */
	virtual bool RunSystem(int aCount = 0, int aIdleCount = 0) = 0;
	/** @brief Stops DES */
	virtual bool StopSystem() = 0;
	/** @brier Gets modules relative Uri */
	virtual string modulesUri() const = 0;
	/** @brier Sets environment variable */
	virtual bool setEVar(const string& aName, const string& aValue) = 0;
	/** @brier Gets environment variable */
	virtual bool getEVar(const string& aName, string& aValue) const = 0;
};




#endif // __FAP3_MENV_H
