
#include "chromo.h"
#include "content.h"


Content::Content(const string &aName, MEnv* aEnv): Node(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

Content::~Content()
{
}

MIface* Content::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MContent>(aType));
    else if (res = checkLif<MContentOwner>(aType));
    return res;
}

MIface* Content::MContent_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MContentOwner>(aType));
    return res;
}

void Content::MContent_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    cout << (mValid ? mData : GUri::nil) << endl;
}

bool Content::getData(string& aCont) const
{
    bool res = false;
    if (mValid) {
	aCont = mData;
	res = true;
    }
    return res;
}

bool Content::setData(const string& aCont)
{
    bool res = true;
    mData = aCont;
    mValid = true;
    return res;
}

void Content::mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string snode = aMut.Attr(ENa_MutNode);
    Log(TLog(EErr, this) + "Changing content of [" + snode + "] - not supported");
    if (!aUpdOnly) {
	notifyNodeMutated(aMut, aCtx);
    }
}


