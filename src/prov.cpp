#include "prov.h"
#include "elem.h"
#include "chromo.h"
#include "chromo2.h"
#include "mnode.h"

const string KModPath;

Provider::Provider(const string& aName, MEnv* aEnv): mName(aName), mEnv(aEnv)
{
}

Provider::~Provider() 
{
    while (!mReg.empty()) {
	MNode* elem = mReg.begin()->second;
	delete elem;
	mReg.erase(mReg.begin());
    }
}

void Provider::setEnv(MEnv* aEnv)
{
    assert(mEnv == NULL || aEnv == NULL);
    mEnv = aEnv;
}

MNode* Provider::createNode(const string& aType, const string& aName, MEnv* aEnv)
{
    return NULL;
}

MNode* Provider::provGetNode(const string& aUri)
{
    return NULL;
}

bool Provider::isProvided(const MNode* aElem) const
{
    bool res = false;
    PFL_DUR_STAT_START(PEvents::EDurStat_Tmp3);
    for (TReg::const_iterator it = mReg.begin(); it != mReg.end() && !res; it++) {
	res = aElem == it->second;
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_Tmp3);
    return res;
}

MChromo* Provider::createChromo(const string& aRargs)
{
    return NULL;
}

void Provider::getNodesInfo(vector<string>& aInfo)
{
}

const string& Provider::modulesPath() const
{
    return KModPath;
}

MIface* Provider::MProvider_getLif(const char *aType)
{
    MIface* res = nullptr;
    return res;
}

void Provider::MProvider_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, aOs); aOs << "Name: " << mName << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	Ifu::offset(aIdt, aOs); aOs << "Register: " << endl;
        for (auto it = mReg.cbegin(); it != mReg.cend(); it++) {
	    auto* elem = it->second;
	    Ifu::offset(aIdt, aOs); aOs << "- " << elem->name() << endl;
        }
    }
}





ProvBase::ProvBase(const string& aName, MEnv* aEnv): Provider(aName, aEnv)
{
}

ProvBase::~ProvBase()
{
}

MNode* ProvBase::createNode(const string& aType, const string& aName, MEnv* aEnv)
{
    MNode* res = NULL;
    res = CreateAgent(aType, aName, mEnv);
    /*YB!!
    if (res) {
	MNode* parent = provGetNode(aType);
	if (parent) {
	    MElem* eparent = parent->lIf(eparent);
	    if (eparent) {
		//YB!!eparent->AppendChild(res);
	    }
	}
    }
    */
    return res;
}

MNode* ProvBase::provGetNode(const string& aUri)
{
    MNode* res = NULL;
    auto it = mReg.find(aUri);
    if (it != mReg.end()) {
        res = it->second;
    } else { 
	res = CreateAgent(aUri, string(), mEnv);
	if (res) {
	    mReg.insert(TRegVal(aUri, res));
	}
    }
    return res;
}

MChromo* ProvBase::createChromo()
{
    return new Chromo2();
}

DtBase* ProvBase::createData(const string& aType)
{
    return CreateData(aType);
}

MNode* ProvBase::CreateAgent(const string& aType, const string& aName, MEnv* aEnv) const
{
    PFL_DUR_STAT_START(PEvents::EDurStat_PvdCNode);
    MNode* res = NULL;
    auto it = FReg().find(aType);
    if (it != FReg().end()) {
        res = it->second(aName, aEnv);
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_PvdCNode);
    return res;
}

DtBase* ProvBase::CreateData(const string& aType) const
{
    DtBase* res = NULL;
    auto it = FDtReg().find(aType);
    if (it != FDtReg().end()) {
	TDtFact* fact = it->second;
	res = fact();
    }
    return res;
}
