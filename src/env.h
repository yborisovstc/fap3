#ifndef __FAP3_ENV_H
#define __FAP3_ENV_H

#include "menv.h"
#include "mnode.h"
#include "factr.h"

class Logrec;

/** @brief Execution environment
 * */
class Env: public MEnv
{
    public:
	static const char* Type() { return "Env";};
	Env(const string& aSpecFile, const string& aLogFileName = string());
	Env(const string& aSpec, const string& aLogFileName, bool aOpt);
	virtual ~Env();
	// From MEnv.MIface
	virtual string MEnv_Uid() const override { return Type();}
	virtual MIface* MEnv_getLif(const char *aType) override { return nullptr;}
	virtual void MEnv_doDump(int aLevel, int aIdt, ostream& aOs) const override {}
	// From MEnv
	virtual MProvider* provider() const override {return mProvider;}
	virtual void constructSystem() override;
	virtual MLogRec* Logger() override;
	virtual MNode* Root() const override;
    protected:
	MNode* mRoot;
	string mSpecFile;
	string mSpec;
	MChromo* mChromo;
	Factory* mProvider;
	Logrec* mLogger;

};

#endif
