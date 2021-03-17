#ifndef __FAP3_MENV_H
#define __FAP3_MENV_H

#include "miface.h"
#include "mprov.h"
#include "mlog.h"

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
	/** @brief Gets root node */
	virtual MNode* Root() const = 0;
};




#endif // __FAP3_MENV_H
