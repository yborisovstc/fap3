
#include "des.h"
#include "desadp.h"
#include "rmutdata.h"
#include "mlink.h"
#include "mmntp.h"


// Agents DES adaptation - ADP

// DES adapter base

const string KCont_AgentUri = "AgentUri";
const string KCont_SelfAsMagBase = "SelfAsMagBase";
const string K_CpUriInpMagUri = "InpMagUri";
const string K_CpUriInpMagBase = "InpMagBase";
const string K_MagOwnerLinkUri = "MagOwnerLink";

AAdp::AAdp(const string &aType, const string& aName, MEnv* aEnv): Unit(aType, aName, aEnv),
   mMagOwner(nullptr), mMag(NULL), mObrCp(this), mAgtCp(this), mUpdNotified(false), mActNotified(false),
    mMagUriValid(false), mSelfAsBase(false)
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

void AAdp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	MNode* inp = ahostGetNode(K_CpUriInpMagUri);
	if (isRequestor(aReq, inp)) {
	    MIface* iface = dynamic_cast<MDesInpObserver*>(&mIapMagUri);
	    addIfpLeaf(iface, aReq);
	} else {
	    MNode* inp = ahostGetNode(K_CpUriInpMagBase);
	    if (isRequestor(aReq, inp)) {
		MIface* iface = dynamic_cast<MDesInpObserver*>(&mIapMagBase);
		addIfpLeaf(iface, aReq);
	    }
	}
    } else {
	Unit::resolveIfc(aName, aReq);
    }
}

void AAdp::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    MObservable* hostobl = ahostNode() ? ahostNode()->lIf(hostobl) : nullptr;
    if (aObl == hostobl) {
	MNode* ownedn = aOwned->lIf(ownedn);
	if (ownedn->name() == K_MagOwnerLinkUri) {
	    // Start monitoring MagLink
	    MObservable* magolinkobl = ownedn->lIf(magolinkobl);
	    if (magolinkobl) {
		magolinkobl->addObserver(&mObrCp);
	    }
	}
    }
}

void AAdp::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
    MObservable* hostobl = ahostNode() ? ahostNode()->lIf(hostobl) : nullptr;
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
	} else if (aCont == cow->getCont(KCont_SelfAsMagBase)) {
	    string selfAsBase;
	    cow->getContent(KCont_SelfAsMagBase, selfAsBase);
	    mSelfAsBase= (selfAsBase == "y");
	    UpdateMag();
	}
    }
}

void AAdp::onObsChanged(MObservable* aObl)
{
    // Handling Mag owner link change
    MNode* magolinkn = ahostGetNode(K_MagOwnerLinkUri);
    MObservable* magolinkobl = magolinkn ? magolinkn->lIf(magolinkobl) : nullptr;
    if (aObl == magolinkobl) {
	MLink* magolinkl = magolinkn->lIf(magolinkl);
	MNode* magon = magolinkl->pair();
	bool res = UpdateMagOwner(magon);
	if (!res) {
	    Log(TLog(EErr, this) + "Failed to set MAG base [" + magon->Uid() + "]");
	}
    } else {
	// Handling Mag base input change
	// TODO this observing mechanism works just in some cases - when the connection
	// to MAG base is completed with observable input. Other cases (completion with
	// pair or connections chain) don't work). To consider design update.
	MNode* magbasen = ahostGetNode(K_CpUriInpMagBase);
	MObservable* magbaseobl = magbasen ? magbasen->lIf(magbaseobl) : nullptr;
	if (aObl == magbaseobl) {
	    // Apply MAG base immediatelly
	    ApplyMagBase();
	    mInpMagBaseUpdated = true;
	    setActivated();
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
	// Both observation and notification thru MDesInpObserver don't work
	// so try to updata MAG base befor update MAG
	ApplyMagBase();
	UpdateMag();
	mInpMagUriUpdated = false;
    }
    // TODO This doesn't work because of agent doesn't receives activation events
    // neither via MDesInpObserver nor via input observation. To redesign.
    if (mInpMagBaseUpdated) {
	ApplyMagBase();
	UpdateMag();
	mInpMagBaseUpdated = false;
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
    auto obsis = cpu->getIfs<MDesInpObserver>();
    for (auto obsi : *obsis) {
	MDesInpObserver* obs = dynamic_cast<MDesInpObserver*>(obsi);
	obs->onInpUpdated();
    }
}

void AAdp::onInpUpdated()
{
    setActivated();
}

bool AAdp::UpdateMagOwner(MNode* aMagOwner)
{
    bool res = false;
    // TODO to implement all use-cases
    // Handle mount point specifically
    MMntp* momp = aMagOwner->lIf(momp);
    MNode* mowner = momp ? momp->root() : aMagOwner;
    if (mMagOwner != mowner) {
	mMagOwner = mowner;
	UpdateMag();
	res = true;
    }
    return res;
}

void AAdp::UpdateMag()
{
    bool res = false;
    MNode* inp = ahostGetNode(K_CpUriInpMagUri);
    if (inp) {
	string magUri;
	res = GetSData(inp, magUri);
	if (res) {
	    UpdateMag(magUri);
	}
    }
    if (!res) {
	MNode* host = ahostNode();
	MContentOwner* cow = host ? host->lIf(cow) : nullptr;
	if (cow) {
	    if (cow->getCont(KCont_AgentUri)) {
		string magUri;
		cow->getContent(KCont_AgentUri, magUri);
		UpdateMag(magUri);
	    }
	}
    }
}

bool AAdp::UpdateMag(const string& aMagUri)
{
    bool res = false;
    if (!mMagUriValid || aMagUri != mMagUri) {
	mMagUri = aMagUri;
	mMagUriValid = true;
    }
    if (mMagUriValid) {
	MNode* mag = nullptr;
	bool hasMagOwner = true;
	if (mMagOwner) {
	    mag = mMagOwner->getNode(mMagUri);
	} else if (mSelfAsBase) {
	    mag = ahostGetNode(mMagUri);
	} else {
	    hasMagOwner = false;
	}
	if (mag) {
	    UpdateMag(mag);
	} else if (hasMagOwner) {
	    Logger()->Write(EErr, this, "Cannot find managed agent [%s]", mMagUri.c_str());
	}
    }
    return res;
}

bool AAdp::UpdateMag(MNode* aMag)
{
    bool res = false;
    if (mMag != aMag) {
	if (mMag) {
	    MObservable* prevmagob = mMag->lIf(prevmagob);
	    prevmagob->rmObserver(&mMagObs.mOcp);
	}
	mMag = aMag;
	res = true;
	OnMagUpdated();
	MObservable* magob = mMag->lIf(magob);
	magob->addObserver(&mMagObs.mOcp);
	NotifyInpsUpdated();
	Log(TLog(EInfo, this) + "Managed agent attached [" + mMag->Uid() + "]");
    }
    return res;
}

bool AAdp::ApplyMagBase()
{
    bool res = false;
    MNode* inp = ahostNode()->getNode(K_CpUriInpMagBase);
    if (inp) {
	MUnit* inpu = inp->lIf(inpu);
	if (inpu) {
	    // Resolve MLink first to avoid MNode wrong resolution
	    MLink* mmtl = inpu->getSif(mmtl);
	    if (mmtl) {
		MNode* mbase = mmtl->pair();
		// TODO to check if the base is not defined via link
		UpdateMagOwner(mbase);
		res = true;
	    }
	}
    } else {
	Log(TLog(EErr, this) + "Cannot get input [" + K_CpUriInpMagBase + "]");
    }
    return res;
}

void AAdp::OnInpMagUri()
{
    mInpMagUriUpdated = true;
    setActivated();
}

void AAdp::OnInpMagBase()
{
    mInpMagBaseUpdated = true;
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
    // Start monitoring MAG owner link
    MNode* magoln = ahostGetNode(K_MagOwnerLinkUri);
    MObservable* magolinkobl = magoln ? magoln->lIf(magolinkobl) : nullptr;
    if (magolinkobl) { 
	magolinkobl->addObserver(&mObrCp);
    } else {
	Log(TLog(EErr, this) + "Cannot find MAG owner link");
    }
    // Start monitoring MAG base input
    MNode* magbasen = ahostGetNode(K_CpUriInpMagBase);
    MObservable* magbaseobl = magbasen ? magbasen->lIf(magbaseobl) : nullptr;
    if (magbaseobl) {
	magbaseobl->addObserver(&mObrCp);
    } else {
	Log(TLog(EErr, this) + "Cannot find MAG base inp");
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
const string K_CpUriName = "Name";
const string K_InpMUtpUri = "InpMut";

AMnodeAdp::AMnodeAdp(const string &aType, const string& aName, MEnv* aEnv): AAdp(aType, aName, aEnv)
{
    mCompNames.mValid = false;
}

void AMnodeAdp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
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
		} else {
		    MNode* out = ahostGetNode(K_CpUriName);
		    if (isRequestor(aReq, out)) {
			MIface* iface = dynamic_cast<MDVarGet*>(&mPapName);
			addIfpLeaf(iface, aReq);
		    }
		}
	    }
	}
    } else if (aName == MDesInpObserver::Type()) {
	MNode* inpMut = ahostGetNode(K_InpMUtpUri);
	if (isRequestor(aReq, inpMut)) {
	    MIface* iface = dynamic_cast<MDesInpObserver*>(&mIapInpMut);
	    addIfpLeaf(iface, aReq);
	}
    }
    AAdp::resolveIfc(aName, aReq);
}

void AMnodeAdp::GetCompsCount(Sdata<int>& aData)
{
    aData.mData = mCompNames.mValid ? mCompNames.mData.size() : -1;
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
	MNode* base = mMagOwner ? mMagOwner : ahostNode();
	// TODO Isn't is better to have MNode method to get comps owner?
	GUri magUri;
	mMag->getUri(magUri, base);
	GUri ownUri = magUri.head(magUri.elems().size() - 1);
	aData.mData = ownUri;
	aData.mValid = true;
    } else {
	aData.mData = GUri::nil;
	aData.mValid = true;
    }
}

void AMnodeAdp::GetName(Sdata<string>& aData)
{
    if (mMag) {
	aData.mData = mMag->name();
	aData.mValid = true;
    } else {
	aData.mData = GUri::nil;
	aData.mValid = true;
    }
}

void AMnodeAdp::confirm() {
    if (mMag) {
	if (mInpMutChanged) {
	    mInpMutChanged = false;
	    ApplyMut();
	}
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
	if (mNameUpdated) {
	    MNode* cp = ahostGetNode(K_CpUriName);
	    NotifyInpUpdated(cp);
	    mNameUpdated = false;
	}
    } else {
	Logger()->Write(EErr, this, "Managed agent is not attached");
    }
    AAdp::confirm();
}

void AMnodeAdp::onMagOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    mCompNamesUpdated = true;
    mNameUpdated = true;
}

void AMnodeAdp::OnMagUpdated()
{
    mCompNamesUpdated = true;
    mOwnerUpdated = true;
    mNameUpdated = true;
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

void AMnodeAdp::OnInpMut()
{
    mInpMutChanged = true;
    setActivated();
}

void AMnodeAdp::ApplyMut()
{
    if (mMag) {
	bool eres = false;
	MNode* inp = ahostGetNode(K_InpMUtpUri);
	MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	MDVarGet* vget = inpu ? inpu->getSif(vget) : nullptr;
	if (vget) {
	    MDtGet<DMut>* gsd = vget->GetDObj(gsd);
	    if (gsd) {
		DMut dmut;
		gsd->DtGet(dmut);
		TMut& mut = dmut.mData;
		if (mut.IsValid() && mut.Type() != ENt_None && mut.Type() != ENt_Unknown) {
		    TNs ns; MutCtx mutctx(NULL, ns);
		    MChromo* chr = mEnv->provider()->createChromo();
		    chr->Init(ENt_Node);
		    chr->Root().AddChild(mut);
		    mMag->mutate(chr->Root(), false, mutctx, true);
		    delete chr;
		    string muts = mut.ToString();
		    Logger()->Write(EInfo, this, "Managed agent is mutated [%s]", muts.c_str());
		} else if (!mut.IsValid() || mut.Type() == ENt_Unknown) {
		    Logger()->Write(EErr, this, "Invalid mutation [%s]", mut.operator string().c_str());
		}
	    } else {
		MDtGet<DChr2>* gsd = vget->GetDObj(gsd);
		if (gsd) {
		    DChr2 data;
		    data.FromString("CHR2 {  }");
		    gsd->DtGet(data);
		    Chromo2& chromo = data.mData;
		    if (data.IsValid()) {
			TNs ns; MutCtx mutctx(NULL, ns);
			mMag->mutate(chromo.Root(), false, mutctx, true);
			string datas = chromo.Root();
			Logger()->Write(EInfo, this, "Managed agent is mutated [%s]", datas.c_str());
		    } else {
			string datas = chromo.Root();
			Logger()->Write(EErr, this, "Invalid mutations [%s]", datas.c_str());
		    }

		} else  {
		    Logger()->Write(EErr, this, "Cannot get data from Inp");
		}
	    }
	} else {
	    Logger()->Write(EErr, this, "Cannot get input");
	}
    }
}

void AMnodeAdp::update()
{
    /*
    if (mInpMutChanged) {
	ApplyMut();
	mInpMutChanged = false;
    }
    */
    AAdp::update();
}



// MElem DES adapter

const string K_MutApplOutpUri = "MutationApplied";

AMelemAdp::AMelemAdp(const string& aType, const string& aName, MEnv* aEnv): AAdp(aType, aName, aEnv)
{
    mMagChromo = mEnv->provider()->createChromo();
}

void AMelemAdp::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == MDesInpObserver::Type()) {
	MNode* inpMut = ahostGetNode(K_InpMUtpUri);
	if (isRequestor(aReq, inpMut)) {
	    MIface* iface = dynamic_cast<MDesInpObserver*>(&mIapInpMut);
	    addIfpLeaf(iface, aReq);
	}
    } else {
	AAdp::resolveIfc(aName, aReq);
    }
}



void AMelemAdp::ApplyMut()
{
    if (mMag) {
	bool eres = false;
	MNode* inp = ahostGetNode(K_InpMUtpUri);
	MUnit* inpu = inp ? inp->lIf(inpu) : nullptr;
	MDVarGet* vget = inpu ? inpu->getSif(vget) : nullptr;
	if (vget) {
	    MDtGet<DMut>* gsd = vget->GetDObj(gsd);
	    if (gsd) {
		DMut dmut;
		gsd->DtGet(dmut);
		TMut& mut = dmut.mData;
		MElem* mag = mMag->lIf(mag);
		if (mag) {
		    if (mut.IsValid() && mut.Type() != ENt_None && mut.Type() != ENt_Unknown) {
			TNs ns; MutCtx mutctx(NULL, ns);
			MChromo* chr = mEnv->provider()->createChromo();
			chr->Init(ENt_Node);
			chr->Root().AddChild(mut);
			mMag->mutate(chr->Root(), false, mutctx, true);
			delete chr;
			string muts = mut.ToString();
			Logger()->Write(EInfo, this, "Managed agent is mutated [%s]", muts.c_str());
		    } else if (!mut.IsValid() || mut.Type() == ENt_Unknown) {
			Logger()->Write(EErr, this, "Invalid mutation [%s]", mut.operator string().c_str());
		    }
		} else {
		    Logger()->Write(EErr, this, "Managed agent is not MElem");
		}
	    } else {
		MDtGet<DChr2>* gsd = vget->GetDObj(gsd);
		if (gsd) {
		    DChr2 data;
		    gsd->DtGet(data);
		    Chromo2& chromo = data.mData;
		    MElem* mag = mMag->lIf(mag);
		    if (mag) {
			if (data.IsValid()) {
			    TNs ns; MutCtx mutctx(NULL, ns);
			    mMag->mutate(chromo.Root(), false, mutctx);
			    string datas = chromo.Root();
			    Logger()->Write(EInfo, this, "Managed agent is mutated [%s]", datas.c_str());
			} else {
			    string datas = chromo.Root();
			    Logger()->Write(EErr, this, "Invalid mutations [%s]", datas.c_str());
			}
		    } else {
			Logger()->Write(EErr, this, "Managed agent is not MElem");
		    }

		} else  {
		    Logger()->Write(EErr, this, "Cannot get data from Inp");
		}
	    }
	} else {
	    Logger()->Write(EErr, this, "Cannot get input");
	}
    }
}

void AMelemAdp::OnInpMut()
{
    mInpMutChanged = true;
    setActivated();
}

void AMelemAdp::update()
{
    if (mInpMutChanged) {
	ApplyMut();
	mInpMutChanged = false;
    }
    AAdp::update();
}





