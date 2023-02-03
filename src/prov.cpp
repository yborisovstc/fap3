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
    for (TReg::const_iterator it = mReg.begin(); it != mReg.end() && !res; it++) {
	res = aElem == it->second;
    }
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
    if (res) {
	MNode* parent = provGetNode(aType);
	if (parent) {
	    MElem* eparent = parent->lIf(eparent);
	    if (eparent) {
		//YB!!eparent->AppendChild(res);
	    }
	}
    }
    return res;
}

MNode* ProvBase::provGetNode(const string& aUri)
{
    MNode* res = NULL;
    if (mReg.count(aUri) > 0) {
	res = mReg.at(aUri);
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
    MNode* res = NULL;
    if (FReg().count(aType) > 0) {
	TFact* fact = FReg().at(aType);
	res = fact(aName, aEnv);
    }
    return res;
}

DtBase* ProvBase::CreateData(const string& aType) const
{
    DtBase* res = NULL;
    if (FDtReg().count(aType) > 0) {
	TDtFact* fact = FDtReg().at(aType);
	res = fact();
    }
    return res;
}
