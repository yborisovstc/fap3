
#ifndef __FAP3_MLINK_H
#define __FAP3_MLINK_H


#include "miface.h"
#include "ifu.h"

using namespace std;


class MNode;

/** @brief One-way relation interface
 *
 * */
class MLink: public MIface
{
    public:
	static const char* Type() { return "MLink";}
	// From MIface
	virtual string Uid() const override { return MLink_Uid();}
	virtual string MLink_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MLink_getLif(aType);}
	virtual MIface* MLink_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MLink_doDump(aLevel, aIdt, std::cout);}
	virtual void MLink_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual bool connect(MNode* aPair) = 0;
	virtual bool disconnect(MNode* aPair) = 0;
	virtual MNode* pair() = 0;
};






#endif
