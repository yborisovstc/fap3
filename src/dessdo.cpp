
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


///  SDO "Name"

SdoName::SdoName(const string &aType, const string& aName, MEnv* aEnv): Sdo<string>(aType, aName, aEnv) { }

void SdoName::DtGet(Stype& aData)
{
    if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	aData.mData = mSue->name();
	aData.mValid = true;
    }
}

///  SDO "Parent"

SdoParent::SdoParent(const string &aType, const string& aName, MEnv* aEnv): Sdo<string>(aType, aName, aEnv) { }

void SdoParent::DtGet(Stype& aData)
{
    if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	aData.mData = mSue->parentName();
	aData.mValid = true;
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
	Log(TLog(EWarn, this) + "Owner is not explorable");
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
	Log(TLog(EWarn, this) + "Owner is not explorable");
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
	Log(TLog(EWarn, this) + "Owner is not explorable");
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

///  SDO "Component owner"

SdoCompOwner::SdoCompOwner(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpCompUri(this, "Comp")
{
}

void SdoCompOwner::DtGet(Stype& aData)
{
    DGuri curi;
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
		aData.mData = ownerUri;
		aData.mValid = true;
	    } else {
		Log(TLog(EErr, this) + "Couldn't get component [" + mInpCompUri.mName + "] owner");
	    }
	} else {
	    Log(TLog(EErr, this) + "Couldn't get component [" + mInpCompUri.mName + "]");
	}
    }
}


///  SDO "Component comp"

SdoCompComp::SdoCompComp(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpCompUri(this, "Comp"), mInpCompCompUri(this, "CompComp")
{
}

void SdoCompComp::DtGet(Stype& aData)
{
    DGuri curi, ccuri;
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
		    ccomp->getUri(aData.mData, mSue);
		    aData.mValid = true;
		} else {
		    Log(TLog(EErr, this) + "Couldn't get component [" + mInpCompCompUri.mName + "] owner");
		}
	    } else {
		Log(TLog(EErr, this) + "Couldn't get component [" + mInpCompUri.mName + "]");
	    }
	}
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
	Log(TLog(EWarn, this) + "Owner is not explorable");
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

///  SDO "Vertex pairs count"

SdoPairsCount::SdoPairsCount(const string &aType, const string& aName, MEnv* aEnv): Sdo<int>(aType, aName, aEnv),
    mInpVert(this, "Vp")
{
}

void SdoPairsCount::DtGet(Stype& aData)
{
    string verts;
    bool res = mInpVert.getData(verts);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpVert.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	MNode* vertn = mSue->getNode(verts);
	if (vertn) {
	    MVert* vertv = vertn->lIf(vertv);
	    if (vertv) {
		aData.mData = vertv->pairsCount();
		aData.mValid = true;
	    }
	}
    }
}

///  SDO "Vertex pair URI"

SdoPair::SdoPair(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpTarg(this, "Vp")
{
}

void SdoPair::DtGet(Stype& aData)
{
    string verts;
    bool res = mInpTarg.getData(verts);
    if (!res) {
	Log(TLog(EErr, this) + "Failed getting input [" + mInpTarg.mName + "] data");
    } else if (!mSue)  {
	Log(TLog(EWarn, this) + "Owner is not explorable");
    } else {
	MNode* vertn = mSue->getNode(verts);
	if (vertn) {
	    MVert* vertv = vertn->lIf(vertv);
	    if (vertv) {
		if (vertv->pairsCount() == 1) {
		    MVert* pair = vertv->getPair(0);
		    // TODO Workaround used, ref ds_dcs_sdo_gmn. Create solid solution.
		    MUnit* pairu = pair->lIf(pairu);
		    MNode* pairn = pairu ? pairu->getSif(pairn) : nullptr;
		    if (pairn) {
			pairn->getUri(aData.mData, mSue);
			aData.mValid = true;
		    }
		}
	    } else {
		Log(TLog(EErr, this) + "Target [" + verts + "] is not vertex");
	    }
	}
    }
}

///  SDO "Single pair of targets comp"

SdoTcPair::SdoTcPair(const string &aType, const string& aName, MEnv* aEnv): Sdog<DGuri>(aType, aName, aEnv),
    mInpTarg(this, "Targ"), mInpTargComp(this, "TargComp")
{
}

void SdoTcPair::DtGet(Stype& aData)
{
    DGuri targUri;
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
			    aData.mData = uri;
			    aData.mValid = true;
			}
		    }
		} else {
		    Log(TLog(EErr, this) + "Target [" + targUri.mData + "] is not vertex");
		}
	    }
	}
    }
}


///  SDO "Pairs"

SdoPairs::SdoPairs(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<DGuri>>(aType, aName, aEnv) { }

void SdoPairs::DtGet(Stype& aData)
{
    if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	MVert* suev = mSue->lIf(suev);
	if (!suev) {
	    Log(TLog(EErr, this) + "Explorable isn't vertex");
	} else {
	    aData.mValid = true;
	    aData.mData.clear();
	    for (int ind = 0; ind < suev->pairsCount(); ind++) {
		MVert* pair = suev->getPair(ind);
		// TODO Workaround used, ref ds_dcs_sdo_gmn. Create solid solution.
		MUnit* pairu = pair->lIf(pairu);
		MNode* pairn = pairu ? pairu->getSif(pairn) : nullptr;
		if (!pairn) {
		    Log(TLog(EErr, this) + "Couldnt get URI for pair [" + pair->Uid() + "]");
		    aData.mValid = false;
		    break;
		} else {
		    aData.mValid = true;
		    GUri puri;
		    pairn->getUri(puri, mSue);
		    DGuri purid(puri);
		    aData.mData.push_back(purid);
		}
	    }
	}
    }
}


///  SDO "Target pairs"

SdoTPairs::SdoTPairs(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<DGuri>>(aType, aName, aEnv),
    mInpTarg(this, "Targ") {}

void SdoTPairs::DtGet(Stype& aData)
{
    DGuri turi;
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
		aData.mValid = true;
		for (int ind = 0; ind < targv->pairsCount(); ind++) {
		    MVert* pair = targv->getPair(ind);
		    // TODO Workaround used, ref ds_dcs_sdo_gmn. Create solid solution.
		    MUnit* pairu = pair->lIf(pairu);
		    MNode* pairn = pairu ? pairu->getSif(pairn) : nullptr;
		    if (!pairn) {
			Log(TLog(EErr, this) + "Couldnt get URI for pair [" + pair->Uid() + "]");
			aData.mValid = false;
			break;
		    } else {
			aData.mValid = true;
			GUri puri;
			pairn->getUri(puri, mSue);
			DGuri purid(puri);
			aData.mData.push_back(purid);
		    }
		}
	    }
	}
    }
}


///  SDO "Edges"

SdoEdges::SdoEdges(const string &aType, const string& aName, MEnv* aEnv): Sdog<Vector<Pair<DGuri>>>(aType, aName, aEnv) { }

void SdoEdges::DtGet(Stype& aData)
{
    if (!mSue)  {
	Log(TLog(EErr, this) + "Owner is not explorable");
    } else {
	MSyst* sues = mSue->lIf(sues);
	if (!sues) {
	    Log(TLog(EErr, this) + "Explorable isn't system");
	} else {
	    aData.mValid = true;
	    aData.mData.clear();
	    for (auto conn : sues->connections()) {
		MVert* p = conn.first;
		MUnit* pu = p->lIf(pu);
		MNode* pn = pu ? pu->getSif(pn) : nullptr;
		MVert* q = conn.second;
		MUnit* qu = q->lIf(qu);
		MNode* qn = qu ? qu->getSif(qn) : nullptr;
		if (!pn || !qn) {
		    Log(TLog(EErr, this) + "Couldnt get URI for vert [" + (pn ? p->Uid(): q->Uid()) + "]");
		    aData.mValid = false;
		    break;
		} else {
		    aData.mValid = true;
		    GUri puri;
		    pn->getUri(puri, mSue);
		    GUri quri;
		    qn->getUri(quri, mSue);
		    DGuri purid(puri);
		    DGuri qurid(puri);
		    Pair<DGuri> elem;
		    elem.mData.first = purid;
		    elem.mData.second = qurid;
		    aData.mData.push_back(elem);
		}
	    }
	}
    }
}



