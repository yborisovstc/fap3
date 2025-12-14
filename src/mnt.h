
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
	inline static constexpr std::string_view idStr() { return "AMntp"sv;}
    public:
	AMntp(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~AMntp();
	// From MMntp
	virtual string MMntp_Uid() const override {return getUid<MMntp>();}
	virtual MIface* MMntp_getLif(TIdHash aTid) override {return nullptr;}
	virtual MNode* root() const override;
	// From Node
	virtual MIface* MNode_getLif(TIdHash aTid) override;
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
    protected:
	bool CreateModel(const string& aSpecPath);
    private:
	Env* mMdlEnv;
	MMntp* mMMntpPtr = nullptr;
};



#endif
