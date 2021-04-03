
#include <thread>

#include "des.h"
#include "data.h"

const string KCont_Provided = "Provided";
const string KCont_Required = "Required";


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


const string State::KCont_Value = "Value";


State::State(const string& aName, MEnv* aEnv): Vertu(aName, aEnv), mPdata(NULL), mCdata(NULL)
{
    if (aName.empty()) mName = Type();
    mPdata = new BdVar(this);
    mCdata = new BdVar(this);
    MNode* cp = Provider()->createNode(CpStateInp::Type(), "Inp", mEnv);
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
    else if (res = checkLifs<MDVarGet>(aType, dynamic_cast<MDVarGet*>(mCdata)));
    else if (res = checkLif<MDVarSet>(aType));
    else res = Vertu::MNode_getLif(aType);
    return res;
}

void State::setActivated()
{
    // Propagate activation to owner
    MDesObserver* obs = Owner()->lIf(obs);
    if (obs) {
	obs->onActivated(this);
    }
}

void State::update()
{
    MUpdatable* upd = mPdata;
    if (upd) {
	try {
	    if (upd->update()) {
	//	setUpdated(); // already set in HOnDataChanged
	    }
	} catch (std::exception e) {
	    Logger()->Write(EErr, this, "Unspecified error on update");
	}
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
	    MDesInpObserver* obs = dynamic_cast<MDesInpObserver*>(prov->iface());
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
	    NotifyInpsUpdated();
	    if (IsLogeventUpdate()) {
		string new_value;
		mCdata->ToString(new_value);
		Logger()->Write(EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
	    }
	}
    }
}

void State::setUpdated()
{
    MDesObserver* obs = Owner()->lIf(obs);
    if (obs)
	obs->onUpdated(this);
}

void State::onInpUpdated()
{
    setActivated();
}

MDVarGet* State::HGetInp(const void* aRmt)
{
    MDVarGet* res = NULL;
    if (aRmt == mPdata) {
	MNode* inpn = getNode("Inp");
	MUnit* inpu = inpn->lIf(inpu);
	MIfProv* difp = inpu->defaultIfProv(MDVarGet::Type());
	MIfProv* ifp = difp->first();
	if (ifp) {
	    res = dynamic_cast<MDVarGet*>(ifp->iface());
	} else {
	    Log(TLog(EErr, this) + "Cannot get input");
	}
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

MIface* State::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else res = Unit::MOwned_getLif(aType);
    return res;
}

MIface* State::doMOwnerGetLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType)); // ??
    else if(res = checkLif<MUnit>(aType));  // IFR from inputs
    else res = Vertu::doMOwnerGetLif(aType);
    return res;
}

MContent* State::getCont(int aIdx)
{
    return const_cast<MContent*>(const_cast<const State*>(this)->getCont(aIdx));
}

const MContent* State::getCont(int aIdx) const
{
    const MContent* res = nullptr;
    if (aIdx == 0) res = &mValue;
    else if (aIdx == 1) res = Node::getCont(aIdx - 1);
    return res;
}

bool State::getContent(const GUri& aCuri, string& aRes) const
{
    bool res = true;
    string name = aCuri;
    if (name == KCont_Value)
	res = mValue.getData(aRes);
    else
	res = Vertu::getContent(aCuri, aRes);
    return res;
}

bool State::setContent(const GUri& aCuri, const string& aData)
{
    bool res = true;
    string name = aCuri;
    if (name == KCont_Value)
	res = mValue.setData(aData);
    else
	res = Vertu::setContent(aCuri, aData);
    return res;
}

void State::onContentChanged(const MContent* aCont)
{
    if (aCont->contName() == KCont_Value) {
	string data;
	bool res = aCont->getData(data);
	if (res) {
	    res = mPdata->FromString(data);
	    res = res && mCdata->FromString(data);
	    if (mPdata->IsValid() && mCdata->IsValid()) {
		if (res) {
		    NotifyInpsUpdated();
		}
	    }  else {
		Log(TLog(EErr, this) + "Error on applying content [" + aCont->contName() + "]");
		res = mPdata->FromString(data);
	    }
	} else {
	    Log(TLog(EErr, this) + "Cannot get content data [" + aCont->contName() + "]");
	}
    }
}

bool State::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    bool res = true;
    if (aName == provName()) {
	MIface* ifr = mCdata->MDVar_getLif(aName.c_str());
	if (ifr && !aReq->binded()->provided()->findIface(ifr)) {
	    IfrLeaf* lf = new IfrLeaf(this, ifr);
	    aReq->connect(lf);
	    res = true;
	}
    } else {
	Vertu::resolveIfc(aName, aReq);
    }
    return res;
}

MIface* State::MVert_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MConnPoint>(aType));
    else res = Vertu::MVert_getLif(aType);
    return res;
}

bool State::isCompatible(MVert* aPair, bool aExt)
{
    bool res = false;
    bool ext = aExt;
    MVert* cp = aPair;
    // Checking if the pair is Extender
    if (aPair != this) {
	MVert* ecp = cp->getExtd(); 
	if (ecp) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp) {
	    // Check roles conformance
	    string prov = provName();
	    string req = reqName();
	    MConnPoint* mcp = cp->lIf(mcp);
	    if (mcp) {
		string pprov = mcp->provName();
		string preq = mcp->reqName();
		if (ext) {
		    res = prov == pprov && req == preq;
		} else {
		    res = prov == preq && req == pprov;
		}
	    }
	}
    } else {
	res = aExt;
    }
    return res;
}

string State::GetDvarUid(const MDVar* aComp) const
{
    string name =  (aComp == mPdata) ? "Pdata" : "Cdata";
    return getUid(name, MDVar::Type()); 
}




/// DES

Des::Des(const string &aName, MEnv* aEnv): Syst(aName, aEnv)
{
    if (aName.empty()) mName = Type();
}

MIface* Des::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MDesObserver>(aType));
    else res = Syst::MNode_getLif(aType);
    return res;
}

void Des::update()
{
    while (!mActive.empty()) {
	auto comp = mActive.front();
	try {
	    comp->update();
	} catch (std::exception e) {
	    Log(TLog(EErr, this) + "Error on update [" + comp->Uid() + "]");
	}
	mActive.pop_front();
    }
}

void Des::confirm()
{
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
    }
}

void Des::onActivated(MDesSyncable* aComp)
{
    if (mActive.empty()) { // Notify owner
	MDesObserver* obs = Owner()->lIf(obs);
	if (obs) obs->onActivated(this);
    }
    mActive.push_back(aComp);
}

void Des::onUpdated(MDesSyncable* aComp)
{
    if (mUpdated.empty()) { // Notify owner
	MDesObserver* obs = Owner()->lIf(obs);
	if (obs) obs->onUpdated(this);
    }
    mUpdated.push_back(aComp);
}

void Des::onOwnedAttached(MOwned* aOwned)
{
    MDesSyncable* os = aOwned->lIf(os);
    if (os) {
	mActive.push_back(os);
    }
}

MIface* Des::MOwned_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType));
    else res = Syst::MOwned_getLif(aType);
    return res;
}

MIface* Des::doMOwnerGetLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesObserver>(aType)); // Notifying from owned 
    else res = Syst::doMOwnerGetLif(aType);
    return res;
}



//// DesLauncher


const string KCont_TopDesUri = "Path";


DesLauncher::DesLauncher(const string& aName, MEnv* aEnv): Des(aName, aEnv), mStop(false)
{
    if (aName.empty()) mName = Type();
}

bool DesLauncher::Run(int aCount)
{
    bool res = true;
    int cnt = 0;
    while (!mStop && (aCount == 0 || cnt++ < aCount)) {
	if (!mActive.empty()) {
	    Log(TLog(EErr, this) + ">>> Update [" + to_string(cnt) + "]");
	    update();
	    if (!mUpdated.empty()) {
		Log(TLog(EErr, this) + ">>> Confirm [" + to_string(cnt) + "]");
		confirm();
	    }
	} else {
	    OnIdle();
	}
    }
    return res;
}


bool DesLauncher::Stop()
{
    mStop = true;
    return true;
}

void DesLauncher::OnIdle()
{
    this_thread::sleep_for(std::chrono::milliseconds(1));
}

MIface* DesLauncher::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MLauncher>(aType));
    else res = Des::MOwned_getLif(aType);
    return res;
}




