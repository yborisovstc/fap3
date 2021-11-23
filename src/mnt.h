
#ifndef __FAP3_MNT_H
#define __FAP3_MNT_H

#include "node.h"
#include "env.h"

/** @brief Agent of mounting point
 * */
class AMntp : public Node
{
    public:
	static const char* Type() { return "AMntp";};
	AMntp(const string &aName, MEnv* aEnv);
	virtual ~AMntp();
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
    protected:
	bool CreateModel(const string& aSpecPath);
    private:
	Env* mMdlEnv;
};



#endif
