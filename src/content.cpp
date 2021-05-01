
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
    if (mData != aCont) {
	mData = aCont;
	mValid = true;
	notifyContentChanged(this);
    }
    return res;
}

void Content::mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string snode = aMut.Attr(ENa_MutNode);
    if (snode.empty()) {
	string sdata = aMut.Attr(ENa_MutVal);
	bool res = setData(sdata);
	if (!res) {
	    Log(TLog(EErr, this) + "Failed setting content [" + sdata + "]");
	}
    } else {
	Log(TLog(EErr, this) + "Changing content of [" + snode + "] - not supported");
    }
    if (!aUpdOnly) {
	notifyNodeMutated(aMut, aCtx);
    }
}

void Content::onContentChanged(const MContent* aCont)
{
    // Redirect to owner
    notifyContentChanged(aCont);
}

void Content::notifyContentChanged(const MContent* aCont)
{
    MContentOwner* cowner = Owner()->lIf(cowner);
    if (cowner) {
	cowner->onContentChanged(aCont);
    }
}

///// Cnt

void Cnt::MContent_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    Ifu::offset(aIdt, aOs);
    cout << mName << ": " << mData << endl;
}

bool Cnt::setData(const string& aData)
{
    bool res = true;
    if (mData != aData) {
	mData = aData;
	mHost.cntOwner()->onContentChanged(this);
    }
    return res;
}

