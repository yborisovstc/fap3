
#ifndef __FAP3_MNT_H
#define __FAP3_MNT_H

#include "node.h"
#include "env.h"
#include "mmntp.h"

/** @brief Agent of mounting point
 * */
class AMntp : public Node, public MMntp
{
    public:
	static const char* Type() { return "AMntp";};
	AMntp(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~AMntp();
	// From MMntp
	virtual string MMntp_Uid() const override {return getUid<MMntp>();}
	virtual MIface* MMntp_getLif(const char *aType) override {return nullptr;}
	virtual MNode* root() const override;
	// From Node
	virtual MIface* MNode_getLif(const char *aType) override;
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
    protected:
	bool CreateModel(const string& aSpecPath);
    private:
	Env* mMdlEnv;
};



#endif
