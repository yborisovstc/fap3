
#include "des.h"
#include "desadp.h"


// Agents DES adaptation - ADP

// DES adapter base

const string KCont_AgentUri = "AgentUri";
const string K_CpUriInpMagUri = "InpMagUri";

AAdp::AAdp(const string &aType, const string& aName, MEnv* aEnv): Unit(aType, aName, aEnv), mMag(NULL), mObrCp(this), mAgtCp(this), mUpdNotified(false), mActNotified(false)
{
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

MIface* AAdp::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MAgent>(aType));
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
    mActNotified = false;
    if (mInpMagUriUpdated) {
	UpdateMag();
	mInpMagUriUpdated = false;
    }
    setUpdated();
}

void AAdp::confirm()
{
    mUpdNotified = false;
}

// TODO Dup of ADes method, consider to inherit from ADes
MNode* AAdp::ahostNode()
{
    MAhost* ahost = mAgtCp.firstPair()->provided();
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    return hostn;
}

MDesObserver* AAdp::getDesObs()
{
    // Get access to owners owner via MAhost iface
    MNode* ahn = ahostNode();
    MOwner* ahno = ahn->owned()->pairAt(0) ? ahn->owned()->pairAt(0)->provided() : nullptr;
    MUnit* ahnou = ahno ? ahno->lIf(ahnou) : nullptr;
    MDesObserver* obs = ahnou ? ahnou->getSif(obs) : nullptr;
    return obs;
}

#if 0
void AAdp::setUpdated()
{
    // Propagate updated to owner
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs && !mUpdNotified) {
	obs->onUpdated(this);
	mUpdNotified = true;
    }
}
#endif

void AAdp::setUpdated()
{
    if (!mUpdNotified) { // Notify owner
	MDesObserver* obs = getDesObs();
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	}
    }
}


void AAdp::onActivated(MDesSyncable* aComp)
{
    if (!mActNotified) { // Notify owner
	// Propagate activation to owner
	MDesObserver* obs = getDesObs();
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
    }
}

void AAdp::onUpdated(MDesSyncable* aComp)
{
    if (!mUpdNotified) { // Notify owner
	MDesObserver* obs = Owner() ? Owner()->lIf(obs) : nullptr;
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	}
    }
}

void AAdp::NotifyInpUpdated(MNode* aCp)
{
    MUnit* cpu = aCp->lIf(cpu);
    MDesInpObserver* obs = cpu->getSif(obs);
    if (obs) {
	obs->onInpUpdated();
    }
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
	    if (mMag) {
		MObservable* prevmagob = mMag->lIf(prevmagob);
		prevmagob->rmObserver(&mMagObs.mOcp);
	    }
	    mMag = mag;
	    res = true;
	    OnMagUpdated();
	    MObservable* magob = mMag->lIf(magob);
	    magob->addObserver(&mMagObs.mOcp);
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

MIface* AAdp::MObserver_getLif(const char *aType)
{
    MIface* res = nullptr;
    return res;
}

void AAdp::onOwnerAttached()
{
    bool res = false;
    MObservable* obl = Owner()->lIf(obl);
    if (obl) {
	res = obl->addObserver(&mObrCp);
    }
    if (!res) {
	Logger()->Write(EErr, this, "Cannot attach to observer");
    }
    // Registering in agent host
    MActr* ac = Owner()->lIf(ac);
    res = ac->attachAgent(&mAgtCp);
    if (!res) {
	Logger()->Write(EErr, this, "Cannot attach to host");
    }
}





// Input access point

void AAdp::AdpIap::onInpUpdated()
{
    mHandler();
}






// MNode DES adapter

const string K_CpUriCompNames = "CompNames";
const string K_CpUriCompCount = "CompsCount";
const string K_CpUriOwner = "Owner";

AMnodeAdp::AMnodeAdp(const string &aType, const string& aName, MEnv* aEnv): AAdp(aType, aName, aEnv)
{
    mCompNames.mValid = true;
}


bool AMnodeAdp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = true;
    if (aName == MDVarGet::Type()) {
	MNode* cmpCount = ahostGetNode(K_CpUriCompCount);
	if (isRequestor(aReq, cmpCount)) {
	    MIface* iface = dynamic_cast<MDVarGet*>(&mApCmpCount);
	    addIfpLeaf(iface, aReq);
	} else {
	    MNode* cmpNames = ahostGetNode(K_CpUriCompNames);
	    if (isRequestor(aReq, cmpNames)) {
		MIface* iface = dynamic_cast<MDVarGet*>(&mApCmpNames);
		addIfpLeaf(iface, aReq);
	    } else {
		MNode* out = ahostGetNode(K_CpUriOwner);
		if (isRequestor(aReq, out)) {
		    MIface* iface = dynamic_cast<MDVarGet*>(&mPapOwner);
		    addIfpLeaf(iface, aReq);
		}
	    }
	}
    } else {
	AAdp::resolveIfc(aName, aReq);
    }
    return res;
}

void AMnodeAdp::GetCompsCount(Sdata<int>& aData)
{
    aData.mData = mCompNames.mData.size();
    aData.mValid = true;
}

void AMnodeAdp::GetCompNames(TCmpNames& aData)
{
    aData = mCompNames;
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


void AMnodeAdp::confirm() {
    if (mMag) {
	if (mCompNamesUpdated) {
	    // Comps names
	    mCompNames.mData.clear();
	    auto owdCp = mMag->owner()->firstPair();
	    while (owdCp) {
		MNode* osn = owdCp->provided()->lIf(osn);
		mCompNames.mData.push_back(osn->name());
		owdCp = mMag->owner()->nextPair(owdCp);
	    }
	    mCompNames.mValid = true;
	    mCompNamesUpdated = false;
	    MNode* cp = ahostGetNode(K_CpUriCompNames);
	    NotifyInpUpdated(cp);
	    // Comps count
	    cp = ahostGetNode(K_CpUriCompCount);
	    NotifyInpUpdated(cp);
	}
	if (mOwnerUpdated) {
	    MNode* cp = ahostGetNode(K_CpUriOwner);
	    NotifyInpUpdated(cp);
	    mOwnerUpdated = false;
	}
    } else {
	Logger()->Write(EErr, this, "Managed agent is not attached");
    }
    AAdp::confirm();
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

void AMnodeAdp::NotifyInpsUpdated()
{
    MNode* cp = ahostGetNode(K_CpUriCompNames);
    NotifyInpUpdated(cp);
    cp = ahostGetNode(K_CpUriCompCount);
    NotifyInpUpdated(cp);
    cp = ahostGetNode(K_CpUriOwner);
    NotifyInpUpdated(cp);
}




