#ifndef __FAP3_MCONTENT_H
#define __FAP3_MCONTENT_H

#include "miface.h"

#include "curi.h"

/** @brief Iface of content provider
 * Can be leaf (keeps the content value) or node (keeps another content)
 * The best design would be that Cont resolve iface for leave of for content
 * Here the nimimized variant is used: if node is not resolved then it is leaf
 * 
 * */
class MCont : public MCIface
{
    public:
	static const char* Type() { return "MCont";};
	// From MIface
	virtual string Uid() const override { return MCont_Uid();}
	virtual string MCont_Uid() const = 0;
	virtual MIface* getLif(const char *aType) { return MCont_getLif(aType);}
	virtual MIface* MCont_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt = 0) const { return MCont_doDump(aLevel, aIdt, std::cout);}
	virtual void MCont_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual bool getContent(string& aCont) const = 0;
	virtual bool setContent(const string& aCont) = 0;
};

/** @brief Iface of content owner
 * */
class MContOwner : public MCIface
{
    public:
	static const char* Type() { return "MContOwner";};
	// From MIface
	virtual string Uid() const override { return MContOwner_Uid();}
	virtual string MContOwner_Uid() const = 0;
	// Local
	virtual bool onContChanged(MCont* aCnt) = 0;
};

/** @brief Iface of content node in content tree
 * Node 
 * */
class MContNode : public MContOwner
{
    public:
	static const char* Type() { return "MContNode";};
	// From MIface
	virtual string Uid() const override { return MContNode_Uid();}
	virtual string MContNode_Uid() const = 0;
	// Local
	virtual bool addCont(const string& aName, bool aLeaf = false) = 0;
	virtual int size() const = 0;
	virtual MCont* at(int aIdx) const = 0;
	virtual MCont* at(const string& aName) const = 0;
	virtual MCont* getContent(const CUri& aUri) const = 0;
};




#endif
