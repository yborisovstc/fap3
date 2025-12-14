#ifndef __FAP3_MSYST_H
#define __FAP3_MSYST_H

#include <vector>

#include "miface.h"
#include "mecont.h"

#include "nconn.h"
#include "mifr.h"


/** @brief Connection point interface
 * */
class MConnPoint: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MConnPoint"sv;}
	inline static constexpr TIdHash idHash() { return 0x4ab927ef8407add3;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MConnPoint_Uid();}
	virtual string MConnPoint_Uid() const = 0;
        virtual TIdHash idProvided() const = 0;
        virtual TIdHash idRequired() const = 0;
};

class MNode;
class MVert;

/** @brief Socket interface
 * */
class MSocket: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MSocket"sv;}
	inline static constexpr TIdHash idHash() { return 0xef54a3eb58d53b3c;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
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
	inline static constexpr std::string_view idStr() { return "MAgent"sv;}
	inline static constexpr TIdHash idHash() { return 0x253392665466b0a2;}
    public:
	using TCp = MNcpp<MAgent, MAhost>;
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MAgent_Uid();}
	virtual string MAgent_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) override { return MAgent_getLif(aTid);}
	virtual MIface* MAgent_getLif(TIdHash aTid) = 0;
};


/** @brief System host (manageable) of embedded managing agent
 * System delegates agent handling the requests thru this iface
 * */
class MAhost: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MAhost"sv;}
	inline static constexpr TIdHash idHash() { return 0x5daf5c9fd0239bfb;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MAhost_Uid();}
	virtual string MAhost_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) override { return MAhost_getLif(aTid);}
	virtual MIface* MAhost_getLif(TIdHash aTid) = 0;
};


/** @brief System host agents controller
 * System exposes this iface to agent that the agent can request the attaching
 * */
// TODO Do we really need it? System can observe owned attach and then do attach/detach agent
class MActr: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MActr"sv;}
	inline static constexpr TIdHash idHash() { return 0xfb77132ee5b6f81c;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MActr_Uid();}
	virtual string MActr_Uid() const = 0;
	// Local
	virtual bool attachAgent(MAgent::TCp* aAgt) = 0;
	virtual bool detachAgent(MAgent::TCp* aAgt) = 0;
};


/** @brief Edge spec
 * */
using TEdge = pair<MVert*, MVert*>;
using TEdges = std::vector<TEdge>;

/** @brief System as connected subsystems
 * */
class MSyst: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MSyst"sv;}
	inline static constexpr TIdHash idHash() { return 0x3558aa937bbb2f;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MSyst_Uid();}
	virtual string MSyst_Uid() const = 0;
	// Local
	/** @brief Get connections
	 * */
	virtual const TEdges& connections() const = 0;
};






#endif
