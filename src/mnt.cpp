
#include "mnt.h"


// Content: env variable kept mounted model path
const string KCntEnvVar = "EnvVar";


AMntp::AMntp(const string &aType, const string &aName, MEnv* aEnv): Node(aType, aName, aEnv), mMdlEnv(nullptr)
{
} 


AMntp::~AMntp() 
{
    if (mMdlEnv) {
	delete mMdlEnv;
	mMdlEnv = nullptr;
    }
}

void AMntp::onContentChanged(const MContent* aCont)
{
    if (aCont->contName() == KCntEnvVar) {
	string envvid;
	aCont->getData(envvid);
	string mpath;
	bool eres = mEnv->getEVar(envvid, mpath);
	if (eres) {
	    Logger()->Write(EInfo, this, "Model path from evar: %s", mpath.c_str());
	    bool cres = CreateModel(mpath);
	    if (cres) {
		cres = attachOwned(mMdlEnv->Root());
		if (!cres) {
		    Logger()->Write(EErr, this, "Cannot attach the model [%s]", mpath.c_str());
		}
	    } else {
		Logger()->Write(EErr, this, "Failed creating the model [%s]", mpath.c_str());
	    }
	} else {
	    Logger()->Write(EErr, this, "Cannot find env variable [%s]", envvid.c_str());
	}

    }
    Node::onContentChanged(aCont);
}

bool AMntp::CreateModel(const string& aSpecPath)
{
    bool res = true;
    if (mMdlEnv) {
	delete mMdlEnv;
	mMdlEnv = nullptr;
    }
    // Create model
    mMdlEnv = new Env(aSpecPath, aSpecPath + ".log");
    assert(mEnv);
    mMdlEnv->constructSystem();
    if (!mMdlEnv->Root()) {
	Logger()->Write(EErr, this, "Cannot create model [%s]", aSpecPath.c_str());
	res = false;
    }
    return res;
}



