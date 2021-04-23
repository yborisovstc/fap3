#ifndef __FAP3_MSYST_H
#define __FAP3_MSYST_H

#include "miface.h"
#include "mecont.h"

#include "nconn.h"
#include "mifr.h"


/** @brief Connection point interface
 * */
class MConnPoint: public MIface
{
    public:
	static const char* Type() { return "MConnPoint";};
	// From MIface
	virtual string Uid() const override { return MConnPoint_Uid();}
	virtual string MConnPoint_Uid() const = 0;
	virtual string provName() const = 0;
	virtual string reqName() const = 0;
};

class MNode;

/** @brief Socket interface
 * */
class MSocket: public MIface
{
    public:
	static const char* Type() { return "MSocket";};
	// From MIface
	virtual string Uid() const override { return MSocket_Uid();}
	virtual string MSocket_Uid() const = 0;
	// Local
	virtual int PinsCount() const = 0;
	virtual MNode* GetPin(int aInd) = 0;
	/** @brief Gets pin in IFR context
	 * */
	virtual MNode* GetPin(MIfReq::TIfReqCp* aReq) = 0;
};


class MAhost;

/** @brief System embedded managing agent
 * System delegates agent handling the requests
 * */
class MAgent: public MIface
{
    public:
	using TCp = MNcpp<MAgent, MAhost>;
    public:
	static const char* Type() { return "MAgent";};
	// From MIface
	virtual string Uid() const override { return MAgent_Uid();}
	virtual string MAgent_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MAgent_getLif(aType);}
	virtual MIface* MAgent_getLif(const char *aType) = 0;
	// Local
	/** @brief Handles host content changed */
	virtual void onHostContentChanged(const MContent* aCont) = 0;
};


/** @brief System host (manageable) of embedded managing agent
 * System delegates agent handling the requests thru this iface
 * */
class MAhost: public MIface
{
    public:
	static const char* Type() { return "MAhost";};
	// From MIface
	virtual string Uid() const override { return MAhost_Uid();}
	virtual string MAhost_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MAhost_getLif(aType);}
	virtual MIface* MAhost_getLif(const char *aType) = 0;
};


/** @brief System host agents controller
 * System exposes this iface to agent that the agent can request the attaching
 * */
class MActr: public MIface
{
    public:
	static const char* Type() { return "MActr";};
	// From MIface
	virtual string Uid() const override { return MActr_Uid();}
	virtual string MActr_Uid() const = 0;
	// Local
	virtual bool attachAgent(MAgent::TCp* aAgt) = 0;
	virtual bool detachAgent(MAgent::TCp* aAgt) = 0;
};






#endif
