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
	static const char* Type() { return "MCont2";};
	// From MIface
	virtual string Uid() const override { return MCont2_Uid();}
	virtual string MCont2_Uid() const = 0;
	// Local
	virtual bool getData(string& aData) const = 0;
	virtual bool setData(const string& aData) = 0;
	virtual MContNode2* node() = 0;
	virtual const MContNode2* node() const = 0;
	virtual void dump(int aIdt) const override { MCont2_dump(aIdt);}
	virtual void MCont2_dump(int aIdt) const = 0;
};

/** @brief Iface of content owner
 * */
class MContOwner2 : public MIface
{
    public:
	static const char* Type() { return "MContOwner2";};
	// From MIface
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
	static const char* Type() { return "MContNode2";};
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
