
#include <thread>

#include "des.h"
#include "data.h"

const string KCont_Provided = "Provided";
const string KCont_Required = "Required";


/* Connection point - input of combined chain state AStatec */

CpStateInp::CpStateInp(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MDesInpObserver");
    res = setContent("Required", "MDVarGet");
    assert(res);
}


/* Connection point - output of combined chain state AStatec */

CpStateOutp::CpStateOutp(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MDVarGet");
    res = setContent("Required", "MDesInpObserver");
    assert(res);
}

/* Connection point - input of state */

CpStateMnodeInp::CpStateMnodeInp(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MDesInpObserver");
    res = setContent("Required", "MLink");
    assert(res);
}

/* Connection point - output of combined chain state AStatec */

CpStateMnodeOutp::CpStateMnodeOutp(const string &aType, const string& aName, MEnv* aEnv): ConnPointu(aType, aName, aEnv)
{
    bool res = setContent("Provided", "MLink");
    res = setContent("Required", "MDesInpObserver");
    assert(res);
}






/* State base agent, monolitic, using unit base organs, combined data-observer chain */


// Debug logging levels
static const int KStatecDlog_Obs = 6;  // Observers
static const int KStatecDlog_ObsIfr = 7;  // Observers ifaces routing


const string State::KCont_Value = "";


State::State(const string &aType, const string& aName, MEnv* aEnv): Vertu(aType, aName, aEnv), mPdata(NULL), mCdata(NULL), mUpdNotified(false), mActNotified(false)
{
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
    //MDesObserver* obs = Owner()->lIf(obs);
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs && !mActNotified) {
	obs->onActivated(this);
	mActNotified = true;
    }
}

void State::update()
{
    mActNotified = false;
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
    mUpdNotified = false;
    MUpdatable* upd = mCdata;
    if (upd != NULL) {
	string old_value;
	mCdata->ToString(old_value);
	if (upd->update()) {
	    NotifyInpsUpdated();
	    if (isLogLevel(EDbg)) {
		string new_value;
		mCdata->ToString(new_value);
		Logger()->Write(EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
	    }
	}
    }
}

void State::setUpdated()
{
    //MDesObserver* obs = Owner() ? Owner()->lIf(obs) : nullptr;
    MUnit* ownu = Owner()->lIf(ownu);
    MDesObserver* obs = ownu->getSif(obs);
    if (obs && !mUpdNotified) {
	obs->onUpdated(this);
	mUpdNotified = true;
    }
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
	MIfProv* ifp = difp ? difp->first() : nullptr;
	if (ifp) {
	    res = dynamic_cast<MDVarGet*>(ifp->iface());
	} else {
	    Log(TLog(EDbg, this) + "Cannot get input");
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

MIface* State::MOwner_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType)); // ??
    else if(res = checkLif<MUnit>(aType));  // IFR from inputs
    else res = Vertu::MOwner_getLif(aType);
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
    Vertu::onContentChanged(aCont);
}

void State::resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq)
{
    if (aName == provName()) {
	MIface* ifr = mCdata->MDVar_getLif(aName.c_str());
	if (ifr && !aReq->binded()->provided()->findIface(ifr)) {
	    addIfpLeaf(ifr, aReq);
	}
    } else {
	Vertu::resolveIfc(aName, aReq);
    }
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


bool State::SContValue::getData(string& aData) const
{
    mHost.mCdata->ToString(aData);
    return true;
}

bool State::SContValue::setData(const string& aData)
{
    bool res = mHost.mPdata->FromString(aData);
    res = res && mHost.mCdata->FromString(aData);
    if (mHost.mPdata->IsValid() && mHost.mCdata->IsValid()) {
	if (res) {
	    mHost.NotifyInpsUpdated();
	}
    }  else {
	mHost.Log(TLog(EErr, &mHost) + "Error on applying content [" + mName + "]");
	res = mHost.mPdata->FromString(aData);
    }
    return res;
}




/// DES

Des::Des(const string &aType, const string &aName, MEnv* aEnv): Syst(aType, aName, aEnv), mUpdNotified(false), mActNotified(false)
{
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
    mActNotified = false;
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
    mUpdNotified = false;
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
    }
}

void Des::onActivated(MDesSyncable* aComp)
{
    if (!mActNotified) { // Notify owner
	MDesObserver* obs = Owner()->lIf(obs);
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
    }
    if (aComp) {
	mActive.push_back(aComp);
    }
}

void Des::onUpdated(MDesSyncable* aComp)
{
    if (!mUpdNotified) { // Notify owner
	MDesObserver* obs = Owner() ? Owner()->lIf(obs) : nullptr;
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	}
    }
    if (aComp) {
	mUpdated.push_back(aComp);
    }
}

void Des::onOwnedAttached(MOwned* aOwned)
{
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
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

MIface* Des::MOwner_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesObserver>(aType)); // Notifying from owned 
    else res = Syst::MOwner_getLif(aType);
    return res;
}

void Des::MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Opt1) {
	Ifu::offset(aIdt, aOs); aOs << "Active:" << endl;
	for (auto item : mActive) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    } else {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
	for (auto item : mUpdated) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    }
}

void Des::MDesObserver_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    MDesSyncable_doDump(aLevel, aIdt, aOs);
    const MDesObserver* obs = Owner()->lIf(obs);
    if (obs && (aLevel & Ifu::EDM_Recursive)) {
	aIdt += 4;
	Ifu::offset(aIdt, aOs); aOs << ">> " << obs->Uid() << endl;
	obs->MDesObserver_doDump(aLevel, aIdt, aOs);
    }
}


///// ADES

ADes::ADes(const string &aType, const string &aName, MEnv* aEnv): Unit(aType, aName, aEnv), mOrCp(this), mAgtCp(this), mUpdNotified(false), mActNotified(false)
{
}

ADes::~ADes()
{
}


MIface* ADes::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MAgent>(aType));
    else if (res = checkLif<MDesObserver>(aType));
    else res = Unit::MNode_getLif(aType);
    return res;
}

MIface* ADes::MAgent_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MDesSyncable>(aType));
    else if (res = checkLif<MUnit>(aType)); // To allow client to request IFR
    else if (res = checkLif<MDesObserver>(aType));
    return res;
}

void ADes::update()
{
    mActNotified = false;
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

void ADes::confirm()
{
    mUpdNotified = false;
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
    }
}

MDesObserver* ADes::getDesObs()
{
    // Get access to owners owner via MAhost iface
    MNode* ahn = ahostNode();
    MOwner* ahno = ahn->owned()->pairAt(0) ? ahn->owned()->pairAt(0)->provided() : nullptr;
    MUnit* ahnou = ahno ? ahno->lIf(ahnou) : nullptr;
    MDesObserver* obs = ahnou ? ahnou->getSif(obs) : nullptr;
    return obs;
}

void ADes::onActivated(MDesSyncable* aComp)
{
    if (!mActNotified) { // Notify owner
	MDesObserver* obs = getDesObs();
	if (obs) {
	    obs->onActivated(this);
	    mActNotified = true;
	}
    }
    if (aComp) {
	mActive.push_back(aComp);
    }
}

void ADes::onUpdated(MDesSyncable* aComp)
{
    if (!mUpdNotified) { // Notify owner
	MDesObserver* obs = getDesObs();
	if (obs) {
	    obs->onUpdated(this);
	    mUpdNotified = true;
	}
    }
    if (aComp) {
	mUpdated.push_back(aComp);
    }
}

MIface* ADes::MOwned_getLif(const char *aType)
{
    MIface* res = NULL;
    if (res = checkLif<MDesSyncable>(aType));
    else res = Unit::MOwned_getLif(aType);
    return res;
}

MIface* ADes::MObserver_getLif(const char *aType)
{
    return nullptr;
}

void ADes::onObsOwnedAttached(MObservable* aObl, MOwned* aOwned)
{
    MUnit* osu = aOwned->lIf(osu);
    MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
    if (os) {
	onActivated(os);
    }
}

void ADes::onOwnerAttached()
{
    bool res = false;
    MObservable* obl = Owner()->lIf(obl);
    if (obl) {
	res = obl->addObserver(&mOrCp);
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
    // Activate all existing syncable components
    MNode* host = ahostNode();
    auto owdCp = host->owner()->firstPair();
    while (owdCp) {
	MUnit* osu = owdCp->provided()->lIf(osu);
	MDesSyncable* os = osu ? osu->getSif(os) : nullptr;
	if (os && os != this) {
	    onActivated(os);
	}
	owdCp = host->owner()->nextPair(owdCp);
    }
}

void ADes::MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Opt1) {
	Ifu::offset(aIdt, aOs); aOs << "Active:" << endl;
	for (auto item : mActive) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    } else {
	Ifu::offset(aIdt, aOs); aOs << "Updated:" << endl;
	for (auto item : mUpdated) {
	    Ifu::offset(aIdt, aOs); aOs << item->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		item->MDesSyncable_doDump(aLevel, aIdt + 4, aOs);
	    }
	}
    }
}

MNode* ADes::ahostNode()
{
    MAhost* ahost = mAgtCp.firstPair()->provided();
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    return hostn;
}

MNode* ADes::ahostGetNode(const GUri& aUri)
{
    MAhost* ahost = mAgtCp.firstPair()->provided();
    MNode* hostn = ahost ? ahost->lIf(hostn) : nullptr;
    MNode* res = hostn ? hostn->getNode(aUri, this) : nullptr;
    return res;
}

//// DesLauncher


const string KCont_TopDesUri = "Path";


DesLauncher::DesLauncher(const string &aType, const string& aName, MEnv* aEnv): Des(aType, aName, aEnv), mStop(false)
{
}

bool DesLauncher::Run(int aCount)
{
    bool res = true;
    int cnt = 0;
    while (!mStop && (aCount == 0 || cnt < aCount)) {
	if (!mActive.empty()) {
	    Log(TLog(EInfo, this) + ">>> Update [" + to_string(cnt) + "]");
	    update();
	    if (!mUpdated.empty()) {
		Log(TLog(EInfo, this) + ">>> Confirm [" + to_string(cnt) + "]");
		confirm();
	    }
	    cnt++;
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
    this_thread::sleep_for(std::chrono::milliseconds(100));
    //mStop = true;
}

MIface* DesLauncher::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MLauncher>(aType));
    else res = Des::MOwned_getLif(aType);
    return res;
}

MIface* DesLauncher::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MLauncher>(aType));
    else res = Des::MNode_getLif(aType);
    return res;
}
