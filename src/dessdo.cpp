
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
    mObrCp(this), mEagObs(this), mSue(nullptr)
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
	    MObservable* obl = mSue->lIf(obl);
	    if (obl) {
		obl->addObserver(&mEagObs.mOcp);
	    }
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


///  SDO "Name"

SdoName::SdoName(const string &aType, const string& aName, MEnv* aEnv): Sdog<Sdata<string>>(aType, aName, aEnv) { }

const DtBase* SdoName::VDtGet(const string& aType)
{
    mRes.mValid = false;
    if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	mRes.mData = mSue->name();
	mRes.mValid = true;
    }
    return &mRes;
}

///  SDO "Uri"

SdoUri::SdoUri(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv) { }

const DtBase* SdoUri::VDtGet(const string& aType)
{
    mRes.mValid = false;
    if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	mSue->getUri(mRes.mData, nullptr);
	mRes.mValid = true;
    }
    return &mRes;
}


///  SDO "Parent"

SdoParent::SdoParent(const string &aType, const string& aName, MEnv* aEnv): Sdog<Sdata<string>>(aType, aName, aEnv) { }

const DtBase* SdoParent::VDtGet(const string& aType)
{
    mRes.mValid = false;
    if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	mRes.mData = mSue->parentName();
	mRes.mValid = true;
    }
    return &mRes;
}




///  SDO "Component exists"

SdoComp::SdoComp(const string &aType, const string& aName, MEnv* aEnv): Sdog<Sdata<bool>>(aType, aName, aEnv),
    mInpName(this, "Name")
{
}

const DtBase* SdoComp::VDtGet(const string& aType)
{
    mRes.mValid = false;
    string name;
    bool res = mInpName.getData(name);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpName.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	mRes.mData = mSue->getNode(name);
	mRes.mValid = true;
    }
    return &mRes;
}


///  SDO "Components count"

SdoCompsCount::SdoCompsCount(const string &aType, const string& aName, MEnv* aEnv): Sdog<Sdata<int>>(aType, aName, aEnv) { }

const DtBase* SdoCompsCount::VDtGet(const string& aType)
{
    string name;
    mRes.mValid = false;
    if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	int count = 0;
	auto owdCp = mSue->owner()->firstPair();
	while (owdCp) {
	    MNode* osn = owdCp->provided()->lIf(osn);
	    count++;
	    owdCp = mSue->owner()->nextPair(owdCp);
	}
	mRes.mData = count;
	mRes.mValid = true;
    }
    return &mRes;
}

void SdoCompsCount::onEagOwnedAttached(MOwned* aOwned)
{
    NotifyInpsUpdated();
}

void SdoCompsCount::onEagOwnedDetached(MOwned* aOwned)
{
    NotifyInpsUpdated();
}

///  SDO "Components names"

SdoCompsNames::SdoCompsNames(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<string>>(aType, aName, aEnv) { }

const DtBase* SdoCompsNames::VDtGet(const string& aType)
{
    string name;
    mRes.mValid = false;
    if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	Stype cnames;
	auto owdCp = mSue->owner()->firstPair();
	while (owdCp) {
	    MNode* osn = owdCp->provided()->lIf(osn);
	    cnames.mData.push_back(osn->name());
	    owdCp = mSue->owner()->nextPair(owdCp);
	}
	mRes.mData = cnames.mData;
	mRes.mValid = true;
    }
    return &mRes;
}

void SdoCompsNames::onEagOwnedAttached(MOwned* aOwned)
{
    NotifyInpsUpdated();
}

void SdoCompsNames::onEagOwnedDetached(MOwned* aOwned)
{
    NotifyInpsUpdated();
}

///  SDO "Component owner"

SdoCompOwner::SdoCompOwner(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpCompUri(this, "Comp")
{
}

const DtBase* SdoCompOwner::VDtGet(const string& aType)
{
    DGuri curi;
    mRes.mValid = false;
    bool res = mInpCompUri.getData(curi);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpCompUri.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	MNode* comp = mSue->getNode(curi.mData);
	if (comp) {
	    // TODO Comp URI reduction is used instead of comp MNode API. This is because
	    // of MNode not allowing to access owner. To re-design.
	    GUri ownerUri = curi.mData.head(curi.mData.size() - 1);
	    MNode* owner = mSue->getNode(ownerUri);
	    if (owner) {
		mRes.mData = ownerUri;
		mRes.mValid = true;
	    } else {
		Log(TLog(EErr, this) + "Couldn't get component [" + curi.ToString(false) + "] owner");
	    }
	} else {
	    Log(TLog(EErr, this) + "Couldn't get component [" + curi.ToString(false) + "]");
	}
    }
    return &mRes;
}


///  SDO "Component comp"

SdoCompComp::SdoCompComp(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpCompUri(this, "Comp"), mInpCompCompUri(this, "CompComp")
{
}

const DtBase* SdoCompComp::VDtGet(const string& aType)
{
    DGuri curi, ccuri;
    mRes.mValid = false;
    bool res = mInpCompUri.getData(curi);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpCompUri.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	res = mInpCompCompUri.getData(ccuri);
	if (!res) {
	    Log(TLog(EErr, this) + "Failed getting input [" + mInpCompCompUri.mName + "] data");
	} else {
	    MNode* comp = mSue->getNode(curi.mData);
	    if (comp) {
		MNode* ccomp = comp->getNode(ccuri.mData);
		if (ccomp) {
		    mRes.mData.clear();
		    ccomp->getUri(mRes.mData, mSue);
		    mRes.mValid = true;
		} else {
		    Log(TLog(EErr, this) + "Couldn't get component [" + mInpCompCompUri.mName + "] owner");
		}
	    } else {
		Log(TLog(EErr, this) + "Couldn't get component [" + mInpCompUri.mName + "]");
	    }
	}
    }
    return &mRes;
}




///  SDO "Vertexes are connected"

SdoConn::SdoConn(const string &aType, const string& aName, MEnv* aEnv): Sdog<Sdata<bool>>(aType, aName, aEnv),
    mInpVp(this, "Vp"), mInpVq(this, "Vq"), mVpUe(nullptr), mVqUe(nullptr)
{
}

const DtBase* SdoConn::VDtGet(const string& aType)
{
    mRes.mValid = false;
    string vps, vqs;
    bool res = mInpVp.getData(vps);
    bool resq = mInpVq.getData(vqs) ;
    if (!res || !resq) {
	Log(TLog(EErr, this) + "Failed getting input [" + (res ? mInpVp.mName : mInpVq.mName) + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	MNode* vpn = mSue->getNode(vps);
	MNode* vqn = mSue->getNode(vqs);
	if (vpn && vqn) {
	    if (vpn != mVpUe) {
		mVpUe = vpn;
		MObservable* obl = mVpUe->lIf(obl);
		if (obl) {
		    res = obl->addObserver(&mObrCp);
		}
		if (!res || !obl) {
		    Logger()->Write(EErr, this, "Cannot attach VertUe to observer");
		}
	    }
	    if (vqn != mVqUe) {
		mVqUe = vqn;
		MObservable* obl = mVqUe->lIf(obl);
		if (obl) {
		    res = obl->addObserver(&mObrCp);
		}
		if (!res || !obl) {
		    Logger()->Write(EErr, this, "Cannot attach VertUe to observer");
		}
	    }
	    MVert* vpv = vpn->lIf(vpv);
	    MVert* vqv = vqn->lIf(vqv);
	    if (vpv && vqv) {
		mRes.mData = vpv->isConnected(vqv);
		mRes.mValid = true;
	    }
	}
    }
    return &mRes;
}

void SdoConn::onObsChanged(MObservable* aObl)
{
    MObservable* op = mVpUe->lIf(op);
    MObservable* oq = mVqUe->lIf(oq);
    if (aObl == op || aObl == oq) {
	NotifyInpsUpdated();
    } else {
	SdoBase::onObsChanged(aObl);
    }
}


///  SDO "Vertex pairs count"

SdoPairsCount::SdoPairsCount(const string &aType, const string& aName, MEnv* aEnv): Sdog<Sdata<int>>(aType, aName, aEnv),
    mInpVert(this, "Vp"), mVertUe(nullptr)
{
}

const DtBase* SdoPairsCount::VDtGet(const string& aType)
{
    mRes.mValid = false;
    string verts;
    bool res = mInpVert.getData(verts);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpVert.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	MNode* vertn = mSue->getNode(verts);
	if (vertn != mVertUe) {
	    mVertUe = vertn;
	    // Start observing vertex under exploring
	    MObservable* obl = mVertUe->lIf(obl);
	    if (obl) {
		res = obl->addObserver(&mObrCp);
	    }
	    if (!res || !obl) {
		Logger()->Write(EErr, this, "Cannot attach VertUe to observer");
	    }
	}
	if (vertn) {
	    MVert* vertv = vertn->lIf(vertv);
	    if (vertv) {
		mRes.mData = vertv->pairsCount();
		mRes.mValid = true;
	    }
	}
    }
    return &mRes;
}

void SdoPairsCount::onObsChanged(MObservable* aObl)
{
    MObservable* obl = mVertUe ? mVertUe->lIf(obl) : nullptr;
    if (obl && aObl == obl) {
	//Log(TLog(EDbg, this) + "SdoPair::onObsChanged");
	NotifyInpsUpdated();
    } else {
	SdoBase::onObsChanged(aObl);
    }
}


///  SDO "Vertex pair URI"

SdoPair::SdoPair(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpTarg(this, "Vp"), mVertUe(nullptr)
{
}

const DtBase* SdoPair::VDtGet(const string& aType)
{
    string verts;
    mRes.mValid = false;
    bool res = mInpTarg.getData(verts);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpTarg.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	MNode* vertn = mSue->getNode(verts);
	if (vertn != mVertUe) {
	    mVertUe = vertn;
	    // Start observing vertex under exploring
	    MObservable* obl = mVertUe->lIf(obl);
	    if (obl) {
		res = obl->addObserver(&mObrCp);
	    }
	    if (!res || !obl) {
		Logger()->Write(EErr, this, "Cannot attach VertUe to observer");
	    }
	}
	if (vertn) {
	    MVert* vertv = vertn->lIf(vertv);
	    if (vertv) {
		if (vertv->pairsCount() == 1) {
		    MVert* pair = vertv->getPair(0);
		    // TODO Workaround used, ref ds_dcs_sdo_gmn. Create solid solution.
		    MUnit* pairu = pair->lIf(pairu);
		    MNode* pairn = pairu ? pairu->getSif(pairn) : nullptr;
		    if (pairn) {
			pairn->getUri(mRes.mData, mSue);
			mRes.mValid = true;
		    }
		}
	    } else {
		Log(TLog(EErr, this) + "Target [" + verts + "] is not vertex");
	    }
	}
    }
    return &mRes;
}


void SdoPair::onObsChanged(MObservable* aObl)
{
    MObservable* obl = mVertUe->lIf(obl);
    if (aObl == obl) {
	//Log(TLog(EDbg, this) + "SdoPair::onObsChanged");
	NotifyInpsUpdated();
    } else {
	SdoBase::onObsChanged(aObl);
    }
}

///  SDO "Single pair of targets comp"

SdoTcPair::SdoTcPair(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpTarg(this, "Targ"), mInpTargComp(this, "TargComp")
{
}

const DtBase* SdoTcPair::VDtGet(const string& aType)
{
    DGuri targUri;
    mRes.mValid = false;
    bool res = mInpTarg.getData(targUri);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpTarg.mName + "] data");
    } else {
	DGuri targCompUri;
	res = mInpTargComp.getData(targCompUri);
	if (!res) {
	    Log(TLog(EErr, this) + "Failed getting input [" + mInpTargComp.mName + "] data");
	} else if (!mSue)  {
	    Log(TLog(EWarn, this) + "Owner is not explorable");
	} else {
	    targUri += targCompUri;
	    MNode* vertn = mSue->getNode(targUri.mData);
	    if (vertn) {
		MVert* vertv = vertn->lIf(vertv);
		if (vertv) {
		    if (vertv->pairsCount() == 1) {
			MVert* pair = vertv->getPair(0);
			// TODO Workaround used, ref ds_dcs_sdo_gmn. Create solid solution.
			MUnit* pairu = pair->lIf(pairu);
			MNode* pairn = pairu ? pairu->getSif(pairn) : nullptr;
			if (pairn) {
			    GUri uri;
			    pairn->getUri(uri, mSue);
			    mRes.mData = uri;
			    mRes.mValid = true;
			}
		    }
		} else {
		    Log(TLog(EErr, this) + "Target [" + targUri.mData + "] is not vertex");
		}
	    }
	}
    }
    return &mRes;
}


///  SDO "Pairs"

SdoPairs::SdoPairs(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<DGuri>>(aType, aName, aEnv) { }

const DtBase* SdoPairs::VDtGet(const string& aType)
{
    if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	MVert* suev = mSue->lIf(suev);
	if (!suev) {
	    Log(TLog(EErr, this) + "Explorable isn't vertex");
	} else {
	    mRes.mValid = true;
	    mRes.mData.clear();
	    for (int ind = 0; ind < suev->pairsCount(); ind++) {
		MVert* pair = suev->getPair(ind);
		// TODO Workaround used, ref ds_dcs_sdo_gmn. Create solid solution.
		MUnit* pairu = pair->lIf(pairu);
		MNode* pairn = pairu ? pairu->getSif(pairn) : nullptr;
		if (!pairn) {
		    Log(TLog(EErr, this) + "Couldnt get URI for pair [" + pair->Uid() + "]");
		    mRes.mValid = false;
		    break;
		} else {
		    mRes.mValid = true;
		    GUri puri;
		    pairn->getUri(puri, mSue);
		    DGuri purid(puri);
		    mRes.mData.push_back(purid);
		}
	    }
	}
    }
    return &mRes;
}


///  SDO "Target pairs"

SdoTPairs::SdoTPairs(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<DGuri>>(aType, aName, aEnv),
    mInpTarg(this, "Targ") {}

const DtBase* SdoTPairs::VDtGet(const string& aType)
{
    DGuri turi;
    mRes.mValid = false;
    bool res = mInpTarg.getData(turi);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpTarg.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	MNode* targn = mSue->getNode(turi.mData);
	if (!targn) {
	    Log(TLog(EErr, this) + "Couldn't find target [" + mInpTarg.mName + "]");
	} else {
	    MVert* targv = targn->lIf(targv);
	    if (!targv) {
		Log(TLog(EErr, this) + "Target [" + mInpTarg.mName + "] isn't a vertex");
	    } else {
		mRes.mValid = true;
		for (int ind = 0; ind < targv->pairsCount(); ind++) {
		    MVert* pair = targv->getPair(ind);
		    // TODO Workaround used, ref ds_dcs_sdo_gmn. Create solid solution.
		    MUnit* pairu = pair->lIf(pairu);
		    MNode* pairn = pairu ? pairu->getSif(pairn) : nullptr;
		    if (!pairn) {
			Log(TLog(EErr, this) + "Couldnt get URI for pair [" + pair->Uid() + "]");
			mRes.mValid = false;
			break;
		    } else {
			mRes.mValid = true;
			GUri puri;
			pairn->getUri(puri, mSue);
			DGuri purid(puri);
			mRes.mData.push_back(purid);
		    }
		}
	    }
	}
    }
    return &mRes;
}


///  SDO "Edges"

SdoEdges::SdoEdges(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<Pair<DGuri>>>(aType, aName, aEnv) { }

const DtBase* SdoEdges::VDtGet(const string& aType)
{
    mRes.mValid = false;
    if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	MSyst* sues = mSue->lIf(sues);
	if (!sues) {
	    Log(TLog(EErr, this) + "Explorable isn't system");
	} else {
	    mRes.mValid = true;
	    mRes.mData.clear();
	    for (auto conn : sues->connections()) {
		MVert* p = conn.first;
		MUnit* pu = p->lIf(pu);
		MNode* pn = pu ? pu->getSif(pn) : nullptr;
		MVert* q = conn.second;
		MUnit* qu = q->lIf(qu);
		MNode* qn = qu ? qu->getSif(qn) : nullptr;
		if (!pn || !qn) {
		    Log(TLog(EErr, this) + "Couldnt get URI for vert [" + (pn ? p->Uid(): q->Uid()) + "]");
		    mRes.mValid = false;
		    break;
		} else {
		    mRes.mValid = true;
		    GUri puri;
		    pn->getUri(puri, mSue);
		    GUri quri;
		    qn->getUri(quri, mSue);
		    DGuri purid(puri);
		    DGuri qurid(quri);
		    Pair<DGuri> elem;
		    elem.mData.first = purid;
		    elem.mData.second = qurid;
		    elem.mValid = true;
		    mRes.mData.push_back(elem);
		}
	    }
	}
    }
    return &mRes;
}


#if 0 // NOT COMPLETED

/// Inputs update indicator, ref ds_dcs_iui_sla

SdoUpdateInd::SdoUpdateInd(const string &aType, const string& aName, MEnv* aEnv): CpStateOutp(aType, aName, aEnv)
{
    mData.mData = false;
    mData.mValid = true;
}

MIface* SdoUpdateInd::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDVarGet>(aType));
    else res = CpStateOutp::MNode_getLif(aType);
    return res;
}

void SdoUpdateInd::MDVarGet_doDump(int aLevel, int aIdt, ostream& aOs) const
{
}
#endif


