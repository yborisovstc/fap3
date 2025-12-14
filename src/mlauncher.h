#ifndef __FAP3_MLAUNCHER_H
#define __FAP3_MLAUNCHER_H

#include <miface.h>

/** @brief Model launcher interface
 * */
class MLauncher: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MLauncher"sv;}
	inline static constexpr TIdHash idHash() { return 0x7b1e7681b5caa8a1;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
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

