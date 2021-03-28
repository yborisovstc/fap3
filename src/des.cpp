
#include "des.h"
#include "data.h"



/* Connection point - input of combined chain state AStatec */

CpStateInp::CpStateInp(const string& aName, MEnv* aEnv): ConnPointu(aName, aEnv)
{
    if (aName.empty()) mName = Type();
    bool res = setContent("Provided", "MDesInpObserver");
    res = setContent("Required", "MDVarGet");
    assert(res);
}

/* Connection point - output of combined chain state AStatec */


CpStateOutp::CpStateOutp(const string& aName, MEnv* aEnv): ConnPointu(aName, aEnv)
{
    if (aName.empty()) mName = Type();
    bool res = setContent("Provided", "MDVarGet");
    res = setContent("Required", "MDesInpObserver");
    assert(res);
}




/* State base agent, monolitic, using unit base organs, combined data-observer chain */


// Debug logging levels
static const int KStatecDlog_Obs = 6;  // Observers
static const int KStatecDlog_ObsIfr = 7;  // Observers ifaces routing

State::State(const string& aName, MEnv* aEnv): Vertu(aName, aEnv), mUpdated(true), mActive(true),
    mPdata(NULL), mCdata(NULL)
{
    if (aName.empty()) mName = Type();
    mPdata = new BdVar(this);
    mCdata = new BdVar(this);
    MNode* cp = Provider()->createNode("CpStatecInp", "Inp", mEnv);
    assert(cp);
    bool res = attachOwned(cp);
    assert(res);
}

MIface* State::MNode_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MDesInpObserver>(aType));
    else if (res = checkLif<MConnPoint>(aType));
    else if (res = checkLif<MDVarGet>(aType));
    else if (res = checkLif<MDVarSet>(aType));
    else res = Vertu::MNode_getLif(aType);
    return res;
}


bool State::isActive() const
{
    return mActive;
}

void State::setActive()
{
    mActive = true;
    // Propagate activation to owner
    MDesObserver* mobs = Owner()->lIf(mobs);
    if (mobs) {
	mobs->onActivated();
    }
}

void State::resetActive()
{
    mActive = false;
}

void State::update()
{
    MUpdatable* upd = mPdata;
    if (upd) {
	try {
	    if (upd->update()) {
		setUpdated();
	    }
	} catch (std::exception e) {
	    Logger()->Write(EErr, this, "Unspecified error on update");
	}
	resetActive();
    }
}

void State::NotifyInpsUpdated()
{
    // It would be better to request MDesInpObserver interface from self. But there is the problem
    // with using this approach because self implements this iface. So accoriding to iface resolution
    // rules UpdateIfi has to try local ifaces first, so self will be selected. Solution here could be
    // not following this rule and try pairs first for MDesObserver. But we choose another solution ATM -
    // to request pairs directly.
    for (auto pair : mPairs) {
	MUnit* pe = pair->lIf(pe);
	// Don't add self to if request context to enable routing back to self
	MIfProv* ifp = pe->defaultIfProv(MDesInpObserver::Type());
	MIfProv* prov = ifp->first();
	while (prov) {
	    MDesInpObserver* obs = prov->iface()->lIf(obs);
	    obs->onInpUpdated();
	    prov = prov->next();
	}
    }
}


void State::confirm()
{
    MUpdatable* upd = mCdata;
    if (upd != NULL) {
	string old_value;
	mCdata->ToString(old_value);
	if (upd->update()) {
	    // Notify dependencies
	    NotifyInpsUpdated();
	    if (IsLogeventUpdate()) {
		string new_value;
		mCdata->ToString(new_value);
		Logger()->Write(EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
	    }
	}
	resetUpdated();
    }
}

bool State::isUpdated() const
{
    return mUpdated;
}

void State::setUpdated()
{
    mUpdated = true;
    // Propagate updation to the owner
    MUnit* downer = Owner()->lIf(downer);
    MIfProv* ifp = downer->defaultIfProv(MDesObserver::Type());
    MIfProv* prov = ifp->first();
    MDesObserver* obs = prov ? prov->iface()->lIf(obs) : nullptr;
    if (obs)
	obs->onUpdated();
}

void State::resetUpdated()
{
    mUpdated = false;
}

void State::onInpUpdated()
{
    setActive();
}

MDVarGet* State::HGetInp(const void* aRmt)
{
    MDVarGet* res = NULL;
    if (aRmt == mPdata) {
	MNode* inpn = getNode("Inp");
	MUnit* inpu = inpn->lIf(inpu);
	MIfProv* difp = inpu->defaultIfProv(MDVarGet::Type());
	MIfProv* ifp = difp->first();
	res = ifp->lIf(res);
    } else {
	res = mPdata->MDVar::lIf(res);
    }
    return res;
}

void State::HOnDataChanged(const void* aRmt)
{
    if (aRmt == mPdata) {
	setUpdated();
    }
}

string State::provName() const
{
    return MDVarGet::Type();
}

string State::reqName() const
{
    return MDesInpObserver::Type();
}

/*
bool State::IsCompatible(MUnit* aPair, bool aExt)
{
    bool res = false;
    bool ext = aExt;
    MUnit *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfi(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    // Check roles conformance
	    string ppt1prov = Provided();
	    string ppt1req = Required();
	    MConnPoint* mcp = cp->GetObj(mcp);
	    if (mcp != NULL) {
		if (ext) {
		    res = mcp->IsProvided(ppt1prov) && mcp->IsRequired(ppt1req);
		} else {
		    res = mcp->IsProvided(ppt1req) && mcp->IsRequired(ppt1prov);
		}
	    }
	}
    }
    return res;
}

MUnit* State::GetExtd()
{
    return NULL;
}

MCompatChecker::TDir State::GetDir() const
{
    return ERegular;
}
*/

    
#if 0
bool State::OnCompChanged(const MUnit* aComp, const string& aContName, bool aModif)
{
    return Vertu::OnCompChanged(aComp, aContName, aModif);
}

TEhr State::ProcessCompChanged(const MUnit* aComp, const string& aContName)
{
    // There is not still nice solution for changing state value
    // The following is used atm: change both prepared and congifmed and notify
    // obs chain and deps (i.e maximum notification). It doesn't seem optimal
    TEhr res = EEHR_Ignored;
    if (aComp == this && aContName == KContVal) {
	string val = GetContent(KContVal);
	bool sres = mPdata->FromString(val);
	sres = sres && mCdata->FromString(val);
	if (mPdata->IsValid() && mCdata->IsValid()) {
	    res = EEHR_Accepted;
	    if (sres) {
		SetActive();
		SetUpdated();
		NotifyInpsUpdated();
	    }
	}  else {
	    Logger()->Write(EErr, this, "[%s] Error on applying content [%s]", GetUri(NULL, true).c_str(), aContName.c_str());
	    sres = mPdata->FromString(val);
	    res = EEHR_Denied;
	}
    } else {
	res = Vertu::ProcessCompChanged(aComp, aContName);
    }
    return res;
}
#endif
	
bool State::IsLogeventUpdate() 
{
    string upd;
    getContent("Debug.Update", upd);
    return upd == "y";
}

string State::VarGetSIfid()
{
    return mPdata == NULL ? string() : mPdata->VarGetSIfid();
}

MIface *State::DoGetSDObj(const char *aName)
{
    return mPdata == NULL ? NULL : mPdata->DoGetSDObj(aName);
}


