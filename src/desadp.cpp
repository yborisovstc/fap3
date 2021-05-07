
#include "des.h"
#include "desadp.h"


// Agents DES adaptation - ADP

// DES adapter base

const string KCont_AgentUri = "AgentUri";
const string K_CpUriInpMagUri = "./InpMagUri";

AAdp::AAdp(const string& aName, MEnv* aEnv): Unit(aName, aEnv), mMag(NULL), mObrCp(this), mAgtCp(this), mNotified(false)
{
    if (aName.empty()) mName = Type();
}

AAdp::~AAdp()
{
}

MIface* AAdp::MAgent_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MUnit>(aType)); // To allow client to request IFR
    else if (res = checkLif<MDesObserver>(aType));
    return res;
}

bool AAdp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = true;
    if (aName == MDesInpObserver::Type()) {
	MNode* inp = ahostGetNode(K_CpUriInpMagUri);
	if (isRequestor(aReq, inp)) {
	    MIface* iface = dynamic_cast<MDesInpObserver*>(&mIapMagUri);
	    addIfpLeaf(iface, aReq);
	}
    } else {
	Unit::resolveIfc(aName, aReq);
    }
    return res;
}

void AAdp::onObsContentChanged(MObservable* aObl, const MContent* aCont)
{
    string data;
    aCont->getData(data);
    MAhost* host = aHost();
    MContentOwner* cow = host ? host->lIf(cow) : nullptr;
    if (cow) {
	if (aCont == cow->getCont(KCont_AgentUri)) {
	    string magUri;
	    cow->getContent(KCont_AgentUri, magUri);
	    bool sres = UpdateMag(magUri);
	}
    }
}

void AAdp::NotifyInpsUpdated()
{
    // It is AMnodeAdp responsibility to provide the ifaces
    /*
    TIfRange range = GetIfi(MDesInpObserver::Type());
    for (TIfIter it = range.first; it != range.second; it++) {
	MDesInpObserver* mobs = dynamic_cast<MDesInpObserver*>(*it);
	if (mobs) {
	    mobs->OnInpUpdated();
	}
    }
    */
}

MIface* AAdp::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MDesObserver>(aType));
    else if (res = checkLif<MDesInpObserver>(aType));
    else res = Unit::MNode_getLif(aType);
    return res;
}

void AAdp::OnMagCompDeleting(const MUnit* aComp, bool aSoft, bool aModif)
{
}

void AAdp::OnMagCompAdding(const MUnit* aComp, bool aModif)
{
}

bool AAdp::OnMagCompChanged(const MUnit* aComp, const string& aContName, bool aModif)
{
    return true;
}

bool AAdp::OnMagChanged(const MUnit* aComp)
{
    return true;
}

bool AAdp::OnMagCompRenamed(const MUnit* aComp, const string& aOldName)
{
    return true;
}

void AAdp::OnMagCompMutated(const MUnit* aNode)
{
}

void AAdp::OnMagError(const MUnit* aComp)
{
}

void AAdp::setActivated()
{
    // Propagate activation to owner
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs) {
	obs->onActivated(this);
    }
}

void AAdp::update()
{
    if (mInpMagUriUpdated) {
	UpdateMag();
	mInpMagUriUpdated = false;
    }
    setUpdated();
}

void AAdp::confirm()
{
}

void AAdp::setUpdated()
{
    // Propagate updated to owner
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs && !mNotified) {
	obs->onUpdated(this);
	mNotified = true;
    }
}

void AAdp::onActivated(MDesSyncable* aComp)
{
    // Propagate activation to owner
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs) {
	obs->onActivated(this);
    }
}

void AAdp::onUpdated(MDesSyncable* aComp)
{
    if (!mNotified) { // Notify owner
	MDesObserver* obs = Owner() ? Owner()->lIf(obs) : nullptr;
	if (obs) {
	    obs->onUpdated(this);
	    mNotified = true;
	}
    }
}

void AAdp::NotifyInpsUpdated(MNode* aCp)
{
    /*
    TIfRange range = aCp->GetIfi(MDesInpObserver::Type());
    for (TIfIter it = range.first; it != range.second; it++) {
	MDesInpObserver* mobs = (MDesInpObserver*) (*it);
	if (mobs != NULL) {
	    mobs->OnInpUpdated();
	}
    }
    */
}

void AAdp::onInpUpdated()
{
    setActivated();
}

void AAdp::UpdateMag()
{
    MNode* inp = ahostGetNode(K_CpUriInpMagUri);
    if (inp) {
	string magUri;
	bool res = GetSData(inp, magUri);
	if (res) {
	    UpdateMag(magUri);
	}
    }
}

bool AAdp::UpdateMag(const string& aMagUri)
{
    bool res = false;
    if (aMagUri != mMagUri) {
	mMagUri = aMagUri;
	MNode* mag = ahostGetNode(mMagUri);
	if (mag) {
	    mMag = mag;
	    res = true;
	    OnMagUpdated();
	    MObservable* magob = mMag->lIf(magob);
	    magob->addObserver(&mObrCp);
	    NotifyInpsUpdated();
	    Logger()->Write(EInfo, this, "Managed agent attached [%s]", mMagUri.c_str());
	} else {
	    Logger()->Write(EErr, this, "Cannot find managed agent [%s]", mMagUri.c_str());
	}
    }
    return res;
}

void AAdp::OnInpMagUri()
{
    mInpMagUriUpdated = true;
    setActivated();
}

MNode* AAdp::ahostGetNode(const GUri& aUri)
{
    MAhost* ahost = aHost();
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    MNode* res = hostn ? hostn->getNode(aUri, this) : nullptr;
    return res;
}

MAhost* AAdp::aHost()
{
    MAhost* ahost = nullptr;
    auto ahostCp = mAgtCp.firstPair();
    ahost = ahostCp ? ahostCp->provided() : nullptr;
    return ahost;
}





// Input access point

void AAdp::AdpIap::onInpUpdated()
{
    mHandler();
}






// MNode DES adapter

const string K_CpUriCompNames = "./CompNames";
const string K_CpUriCompCount = "./CompsCount";
const string K_CpUriOwner = "./Owner";

AMnodeAdp::AMnodeAdp(const string& aName, MEnv* aEnv): AAdp(aName, aEnv)
{
    // TODO This iName init seems far from elegant. To find solution, ref ds_i_icnau 
    if (aName.empty()) mName = Type();
    mCompNames.mValid = true;
}

/*YB!!
void AMnodeAdp::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);

    if (aName == MDVarGet::Type()) {
	MUnit* cmpCount = Host()->GetNode(K_CpUriCompCount);
	if (ctx.IsInContext(cmpCount)) {
	    MIface* iface = dynamic_cast<MDVarGet*>(&mApCmpCount);
	    InsertIfCache(aName, aCtx, cmpCount, iface);
	} else {
	    MUnit* cmpNames = Host()->GetNode(K_CpUriCompNames);
	    if (ctx.IsInContext(cmpNames)) {
		MIface* iface = dynamic_cast<MDVarGet*>(&mApCmpNames);
		InsertIfCache(aName, aCtx, cmpNames, iface);
	    } else {
		MUnit* out = Host()->GetNode(K_CpUriOwner);
		if (ctx.IsInContext(out)) {
		    MIface* iface = dynamic_cast<MDVarGet*>(&mPapOwner);
		    InsertIfCache(aName, aCtx, out, iface);
		}
	    }
	}
    } else  {
	AAdp::UpdateIfi(aName, aCtx);
    }
}
*/
void AMnodeAdp::GetCompsCount(Sdata<int>& aData)
{
    aData.mData = mCompNames.mData.size();
    aData.mValid = true;
}

void AMnodeAdp::GetOwner(Sdata<string>& aData)
{
    if (mMag) {
	//aData.mData = mMag->GetMan()->GetUri(NULL, true);
	aData.mValid = true;
    } else {
	aData.mData = GUri::nil;
	aData.mValid = true;
    }
}


void AMnodeAdp::confirm()
{
    if (mMag) {
	if (mCompNamesUpdated) {
	    // Comps names
	    mCompNames.mData.clear();
	    /*
	    for (int i = 0; i < mMag->CompsCount(); i++) {
		mCompNames.mData.push_back(mMag->GetComp(i)->Name());
	    }
	    */
	    mCompNames.mValid = true;
	    mCompNamesUpdated = false;
	    MNode* cp = ahostGetNode(K_CpUriCompNames);
	    NotifyInpsUpdated(cp);
	    // Comps count
	    cp = ahostGetNode(K_CpUriCompCount);
	    NotifyInpsUpdated(cp);
	}
	if (mOwnerUpdated) {
	    MNode* cp = ahostGetNode(K_CpUriOwner);
	    NotifyInpsUpdated(cp);
	    mOwnerUpdated = false;
	}
    } else {
	Logger()->Write(EErr, this, "Managed agent is not attached");
    }
}

void AMnodeAdp::OnMagCompDeleting(const MUnit* aComp, bool aSoft, bool aModif)
{
    mCompNamesUpdated = true;
}

void AMnodeAdp::OnMagCompAdding(const MUnit* aComp, bool aModif)
{
    mCompNamesUpdated = true;
}

bool AMnodeAdp::OnMagCompChanged(const MUnit* aComp, const string& aContName, bool aModif)
{
    return true;
}

bool AMnodeAdp::OnMagChanged(const MUnit* aComp)
{
    return true;
}

bool AMnodeAdp::OnMagCompRenamed(const MUnit* aComp, const string& aOldName)
{
    mCompNamesUpdated = true;
    return true;
}

void AMnodeAdp::OnMagCompMutated(const MUnit* aNode)
{
}

void AMnodeAdp::OnMagError(const MUnit* aComp)
{
}

void AMnodeAdp::OnMagUpdated()
{
    mCompNamesUpdated = true;
    mOwnerUpdated = true;
    setUpdated();
}




