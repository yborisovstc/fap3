
#include "content.h"


Content::Content(const string &aName, MEnv* aEnv): Node(aName, aEnv)
{
}

Content::~Content()
{
}

MIface* Content::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MContent>(aType));
    return res;
}


string Content::MContent_Uid() const
{
    return string();
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
