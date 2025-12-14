#ifndef __FAP3_MCONTENT2_H
#define __FAP3_MCONTENT2_H

#include "miface.h"

#include "curi.h"

class MContNode2;

/** @brief Iface of content provider
 * Can be leaf (keeps the content value) or node (keeps another content)
 * The best design would be that Cont resolve iface for leave of for content
 * Here the nimimized variant is used: if node is not resolved then it is leaf
 * 
 * */
//class MCont2 : public MCIface2<MCont2, MContNode2>
class MCont2: public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MCont2"sv;}
	inline static constexpr TIdHash idHash() { return 0xc19593acbbdbb3d4;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MCont2_Uid();}
	virtual string MCont2_Uid() const = 0;
	virtual MIface* getLif(TIdHash aTid) override { return MCont2_getLif(aTid);}
	virtual MIface* MCont2_getLif(TIdHash aTid) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { MCont2_doDump(aLevel, aIdt, aOs);}
	virtual void MCont2_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual bool getData(string& aData) const = 0;
	virtual bool setData(const string& aData) = 0;
	virtual MContNode2* node() = 0;
	virtual const MContNode2* node() const = 0;
};

/** @brief Iface of content owner
 * */
class MContOwner2 : public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MContOwner2"sv;}
	inline static constexpr TIdHash idHash() { return 0x8cb38c1858bfb929;}
    public:
	// From MIface
	TIdHash id() const override { return idHash();}
	virtual string Uid() const override { return MContOwner2_Uid();}
	virtual string MContOwner2_Uid() const = 0;
	// Local
	virtual bool onContChanged(MCont2* aCnt) = 0;
};

/** @brief Iface of content node in content tree
* */
class MContNode2 : public MIface
{
    public:
	inline static constexpr std::string_view idStr() { return "MContNode2"sv;}
	inline static constexpr TIdHash idHash() { return 0xc6d32f78e57267ac;}
    public:
	TIdHash id() const override { return idHash();}
	// From MIface
	virtual string Uid() const override { return MContNode2_Uid();}
	virtual string MContNode2_Uid() const = 0;
	// Local
	virtual bool addCont(const CUri& aUri, bool aLeaf = false) = 0;
	virtual int size() const = 0;
	virtual MCont2* at(int aIdx) const = 0;
	virtual MCont2* at(const string& aName) const = 0;
	virtual MCont2* getContent(const CUri& aUri) const = 0;
};




#endif
