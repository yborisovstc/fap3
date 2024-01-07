#ifndef __FAP3_MLAUNCHER_H
#define __FAP3_MLAUNCHER_H

#include <miface.h>

/** @brief Model launcher interface
 * */
class MLauncher: public MIface
{
    public:
	static const char* Type() { return "MLauncher";};
	// From MIface
	virtual string Uid() const override { return MLauncher_Uid();}
	virtual string MLauncher_Uid() const = 0;
	// Local
	virtual bool Run(int aCount = 0, int aIdleCount = 0) = 0;
	virtual bool Stop() = 0;
        /** @brief Gets runnning steps counter value
         * */
	virtual int GetCounter() const = 0;
};

#endif

