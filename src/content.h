
#ifndef __FAP3_CONTENT_H
#define __FAP3_CONTENT_H

#include "mecont.h"
#include "env.h"
#include "node.h"


/** @brief Content agent
 * */
class Content : public Node, public MContent
{
    public:
	static const char* Type() { return "Content";}
	Content(const string &aName, MEnv* aEnv);
	virtual ~Content();
	// From Node.MNode.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MCont.MIface
	virtual string MContent_Uid() const override;
	virtual MIface* MContent_getLif(const char *aType) override { return nullptr;}
	virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MContent
	virtual bool getData(string& aData) const override;
	virtual bool setData(const string& aData) override;
    protected:
	string mData;
	bool mValid = false;
};


#endif
