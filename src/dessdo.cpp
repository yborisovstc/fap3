
#include "mdadp.h"
#include "dessdo.h"


SdoBase::InpBase::InpBase(SdoBase* aHost, const string& aName): mHost(aHost), mName(aName)
{
    // Add  input to the host
    MNode* cp = mHost->Provider()->createNode(CpStateInp::Type(), aName, mHost->mEnv);
    assert(cp);
    bool res = mHost->attachOwned(cp);
    assert(res);
}

SdoBase::SdoBase(const string &aType, const string& aName, MEnv* aEnv): CpStateOutp(aType, aName, aEnv),
    mObrCp(this), mSue(nullptr)
{
}

MIface* SdoBase::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = CpStateOutp::MNode_getLif(aType);
    return res;
}

void SdoBase::MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const
{
}

void SdoBase::onOwnerAttached()
{
    bool res = false;
    MObservable* obl = Owner()->lIf(obl);
    if (obl) {
	res = obl->addObserver(&mObrCp);
    }
    if (!res || !obl) {
	Logger()->Write(EErr, this, "Cannot attach to observer");
    } else {
	// Getting explorable
	// TODO to migrate to specific explorable iface
	UpdateMag();
    }
}

void SdoBase::UpdateMag()
{
    // Attempt MDesAdapter iface first
    MDesAdapter* desa = Owner()->lIf(desa);
    MNode* mag = nullptr;
    if (desa) {
	mag = desa->getMag();
    } else {
	// Then explorable of owner
	mag = Owner()->lIf(mag);
    }
    if (!mag) {
	//Log(TLog(EErr, this) + "Cannot get owners explorable");
    } else {
	// Set explored system
	if (mag != mSue) {
	    mSue = mag;
	    NotifyInpsUpdated();
	    Log(TLog(EInfo, this) + "Explorable is attached [" + mSue->Uid() + "]");
	}
    }
}

void SdoBase::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
}

void SdoBase::onObsOwnedDetached(MObservable* aObl, MOwned* aOwned)
{
}

void SdoBase::onObsContentChanged(MObservable* aObl, const MContent* aCont)
{
}

void SdoBase::onObsChanged(MObservable* aObl)
{
    UpdateMag();
}

void SdoBase::NotifyInpsUpdated()
{
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	// Don't add self to if request context to enable routing back to self
	MIfProv* ifp = pe->defaultIfProv(MDesInpObserver::Type());
	MIfProv* prov = ifp->first();
	while (prov) {
	    MDesInpObserver* obs = dynamic_cast<MDesInpObserver*>(prov->iface());
	    obs->onInpUpdated();
	    prov = prov->next();
	}
    }
}



///  SDO "Component exists"

SdoComp::SdoComp(const string &aType, const string& aName, MEnv* aEnv): Sdo<bool>(aType, aName, aEnv),
    mInpName(this, "Name")
{
}

void SdoComp::DtGet(Stype& aData)
{
    string name;
    bool res = mInpName.getData(name);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpName.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	aData.mData = mSue->getNode(name);
	aData.mValid = true;
    }
}


///  SDO "Components count"

SdoCompsCount::SdoCompsCount(const string &aType, const string& aName, MEnv* aEnv): Sdo<int>(aType, aName, aEnv) { }

void SdoCompsCount::DtGet(Stype& aData)
{
    string name;
    if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	int count = 0;
	auto owdCp = mSue->owner()->firstPair();
	while (owdCp) {
	    MNode* osn = owdCp->provided()->lIf(osn);
	    count++;
	    owdCp = mSue->owner()->nextPair(owdCp);
	}
	aData.mData = count;
	aData.mValid = true;
    }
}

///  SDO "Components names"

SdoCompsNames::SdoCompsNames(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<string>>(aType, aName, aEnv) { }

void SdoCompsNames::DtGet(Stype& aData)
{
    string name;
    if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	Stype cnames;
	auto owdCp = mSue->owner()->firstPair();
	while (owdCp) {
	    MNode* osn = owdCp->provided()->lIf(osn);
	    cnames.mData.push_back(osn->name());
	    owdCp = mSue->owner()->nextPair(owdCp);
	}
	aData.mData = cnames.mData;
	aData.mValid = true;
    }
}



///  SDO "Vertexes are connected"

SdoConn::SdoConn(const string &aType, const string& aName, MEnv* aEnv): Sdo<bool>(aType, aName, aEnv),
    mInpVp(this, "Vp"), mInpVq(this, "Vq")
{
}

void SdoConn::DtGet(Stype& aData)
{
    string vps, vqs;
    bool res = mInpVp.getData(vps);
    bool resq = mInpVq.getData(vqs) ;
    if (!res || !resq) {
	Log(TLog(EErr, this) + "Failed getting input [" + (res ? mInpVp.mName : mInpVq.mName) + "] data");
    } else if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	MNode* vpn = mSue->getNode(vps);
	MNode* vqn = mSue->getNode(vqs);
	if (vpn && vqn) {
	    MVert* vpv = vpn->lIf(vpv);
	    MVert* vqv = vqn->lIf(vqv);
	    if (vpv && vqv) {
		aData.mData = vpv->isConnected(vqv);
		aData.mValid = true;
	    }
	}
    }
}
