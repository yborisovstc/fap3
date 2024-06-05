
#include <dirent.h>
#include <sys/stat.h>

#include "env.h"
#include "log.h"
#include "node.h"
#include "mchromo.h"
#include "chromo.h"
#include "elem.h"
#include "prof.h"
#include "prof_ids.h"


static const string KLogFileName = "faplog.txt";
static const string KModulesName = "Modules";

/** @brief Init data for profiler duration indicator */
const PindCluster<PindDur>::Idata KPindDurIdata = {
    "dur",
    {
	{PEvents::EDur_LaunchActive, "IFC_LNCH_ACTIVE"},
	{PEvents::EDur_Construct, "IFC_CONSTRUCT"},
	{PEvents::EDur_Tst1, "IFC_TEST1"},
	{PEvents::EDur_EnvSetChromo, "IFC_ENV_SCHR"},
    }
};

/** @brief Init data for profiler duration indicator */
const PindCluster<PindDurStat>::Idata KPindDurStatIdata = {
    "durstat",
    {
	{PEvents::EDurStat_Trans, "IFC_TRANS", 1000000, false},
	{PEvents::EDurStat_PvdCNode, "IFC_PVD_CRNODE", 1000000, false},
	{PEvents::EDurStat_UInvldIrm, "IFC_U_INV", 1000000, false},
	{PEvents::EDurStat_LaunchRun, "IFC_LNCH_RUN", 1000000, false},
	{PEvents::EDurStat_LaunchUpdate, "IFC_LNCH_UPD", 1000000, false},
	{PEvents::EDurStat_LaunchConfirm, "IFC_LNCH_CONF", 1000000, false},
	{PEvents::EDurStat_DesRfInpObs, "IFC_RF_IOBS", 1000000, false},
	{PEvents::EDurStat_DesAsUpd, "IFC_DESAS_UPD", 1000000, false},
	{PEvents::EDurStat_StConfirm, "IFC_ST_CONF", 1000000, false},
	{PEvents::EDurStat_StUpdate, "IFC_ST_UPD", 1000000, false},
	{PEvents::EDurStat_ASdcConfirm, "IFC_ASDC_CONF", 1000000, false},
	{PEvents::EDurStat_ASdcUpdate, "IFC_ASDC_UPD", 1000000, false},
	{PEvents::EDurStat_ASdcConfState, "IFC_ASDC_CONFST", 1000000, false},
	{PEvents::EDurStat_ASdcCtlCmp, "ASDC_CTLCMP", 1000000, false},
	{PEvents::EDurStat_ASdcCtlConn, "ASDC_CTLCONN", 1000000, false},
	{PEvents::EDurStat_ASdcCtlMut, "ASDC_CTLMUT", 1000000, false},
	{PEvents::EDurStat_Clbr, "IFC_DST_CLBR", 100000000, false},
	{PEvents::EDurStat_Tmp, "IFC_DST_TMP", 10000000, false},
	{PEvents::EDurStat_Tmp2, "IFC_DST_TMP2", 1000000, false},
	{PEvents::EDurStat_Tmp3, "IFC_DST_TMP3", 10000000, false},
	{PEvents::EDurStat_MutCrn, "IFC_MUT_CRN", 1000000, false},
	{PEvents::EDurStat_MutConn, "IFC_MUT_CONN", 1000000, false},
	{PEvents::EDurStat_MutDsc, "IFC_MUT_DISC", 1000000, false},
	{PEvents::EDurStat_MutCont, "IFC_MUT_CONT", 1000000, false},
	{PEvents::EDurStat_MutRm, "IFC_MUT_RMV", 1000000, false},
	{PEvents::EDurStat_MutAtt, "IFC_MUT_ATT", 1000000, false},
	{PEvents::EDurStat_MutNtf, "IFC_MUT_NTF", 1000000, false},
	{PEvents::EDurStat_MutCad, "IFC_MUT_CAD", 1000000, false},
	{PEvents::EDurStat_Tst1, "IFC_TST_1", 1000000, false},
    }
};



///// ImportsMgr

const string ImportsMgr::KDefImportPath = "/usr/share/fap3/modules";
const string ImportsMgr::KImportsContainerUri = "Modules";

ImportsMgr::ImportsMgr(Env& aHost): mHost(aHost)
{
    AddImportsPaths(KDefImportPath);
}

ImportsMgr::~ImportsMgr()
{
}

void ImportsMgr::AddImportModulesInfo(const string& aPath)
{
    const string& dirpath = aPath;
    DIR* dp;
    struct dirent *dirp;
    struct stat filestat;
    dp = opendir(dirpath.c_str());
    if (dp) {
	while ((dirp = readdir(dp))) {
	    string fname(dirp->d_name);
	    string filepath = dirpath + "/" + fname;
	    // If the file is a directory (or is in some way invalid) we'll skip it
	    if (stat( filepath.c_str(), &filestat )) continue;
	    if (S_ISDIR( filestat.st_mode ))         continue;
	    string spec_ext("chs");
	    if(fname.substr(fname.find_last_of(".") + 1) != spec_ext) continue;
	    // Get chromo root as the module name
	    MChromo *spec = mHost.provider()->createChromo();
	    assert(spec);
	    spec->SetFromFile(filepath);
	    if (spec->IsError()) {
		const CError& cerr = spec->Error();
		mHost.Logger()->Write(EErr, NULL, "Module [%s] error [pos %d]: %s", filepath.c_str(), cerr.mPos.operator streamoff(), cerr.mText.c_str());
	    }
	    string rname = spec->Root().Attr(ENa_Id);
	    mModsPaths.insert(pair<string, string>(rname, filepath));
	    delete spec;
	}
	closedir(dp);
    } else {
	mHost.Logger()->Write(EErr, NULL, "Collecting modules, cannot open imports dir [%s]", dirpath.c_str());
    }
}

void ImportsMgr::GetModulesNames(vector<string>& aModules) const
{
    for (map<string, string>::const_iterator it = mModsPaths.begin(); it != mModsPaths.end(); it++) {
	aModules.push_back(it->first);
    }
}

void ImportsMgr::ResetImportsPaths()
{
    mImportsPaths.clear();
    mModsPaths.clear();
}

void ImportsMgr::AddImportsPaths(const string& aPaths)
{
    mImportsPaths.push_back(aPaths);
    AddImportModulesInfo(aPaths);
}

string ImportsMgr::GetModulePath(const string& aModName) const
{
    string res;
    if (mModsPaths.count(aModName) != 0) {
	res = mModsPaths.at(aModName);
    }
    return res;
}

MNode* ImportsMgr::GetImportsContainer() const
{
    return mHost.Root()->getNode(KImportsContainerUri);
}

bool ImportsMgr::Import(const string& aUri)
{
    bool res = false;
    GUri moduri(aUri);
    assert(moduri.isName());
    string modname = moduri.at(0);
    string modpath = GetModulePath(modname);
    mHost.Logger()->Write(EInfo, NULL, "Started importing node: [%s]", aUri.c_str());
    if (!modpath.empty()) {
	MNode* icontr = GetImportsContainer();
	assert(icontr);
	MChromo* chromo = mHost.provider()->createChromo();
	bool res1 = chromo->Set(modpath);
	ImportToNode(icontr, chromo->Root());
	MNode* node = icontr->getNode(moduri);
	if (node) {
	    mHost.Logger()->Write(EInfo, NULL, "Imported node: [%s]", aUri.c_str());
	    res = true;
	} else {
	    mHost.Logger()->Write(EErr, NULL, "Importing node: failed [%s]", aUri.c_str());
	}
	delete chromo;
    } else {
	mHost.Logger()->Write(EErr, NULL, "Importing [%s]: cannot find module", aUri.c_str());
    }
    return res;
}

void ImportsMgr::ImportToNode(MNode* aNode, const ChromoNode& aMut)
{
    if (aMut.Type() == ENt_Node) {
	MNode* comp = aNode->getNode(aMut.Attr(ENa_Id));
	if (!comp) {
	    // Node doesn't exist yet, to mutate
	    aNode->mutate(aMut, false, MutCtx(aNode), false);
	} else {
	    // mHost.Logger()->Write(EErr, NULL, "Importing [%s]: module already exists", aMut.Name().c_str());
	}
    }
}



///// Env


Env::Env(const string& aSpecFile, const string& aLogFileName): mRoot(NULL), mSpecFile(aSpecFile), mLogger(nullptr),
    mChromo(NULL), mProf(nullptr) /*, mEnPerfTrace(false), mEnIfTrace(false), mExtIfProv(NULL), mIfResolver(NULL)*/
{
    mLogger = new Logrec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    mProvider = new Factory(string(), this);
    mImpMgr = new ImportsMgr(*this);
    mProvider->LoadPlugins();
#ifdef PROFILING_ENABLED
    mProf = new DProf<EPiid_NUM>(this, aSpecFile);
    // Profiler indicators
    mProf->addPind<PindCluster<PindDur>>(KPindDurIdata);
    mProf->addPind<PindCluster<PindDurStat>>(KPindDurStatIdata);
#endif
}

Env::~Env()
{
    //delete mExtIfProv;
    //Logger()->Write(EInfo, mRoot, "Starting deleting system");
    if (mRoot) { delete mRoot; mRoot = nullptr; }
    //Logger()->Write(EInfo, NULL, "Finished deleting system");
    /*
    delete iChMgr;
    */
    delete mProvider;
    delete mLogger;
    delete mImpMgr;
    delete mProf;
    /*
    delete mIfResolver;
    */
}

void Env::constructSystem()
{
    // Create root system
    MChromo* mChromo = mProvider->createChromo();
    if (mSpecFile.empty() && mSpec.empty()) {
	// TODO to add provider method createRoot. Env shouldn't know of model types.
	mRoot = mProvider->createNode(Elem::Type(), "Root", this);
    } else {
	PROF_DUR_START(mProf, PROF_DUR, PEvents::EDur_EnvSetChromo);
	if (!mSpecFile.empty()) {
	    mChromo->SetFromFile(mSpecFile);
	} else {
	    mChromo->SetFromSpec(mSpec);
	}
	PROF_DUR_REC(mProf, PROF_DUR, PEvents::EDur_EnvSetChromo);
	if (mChromo->IsError()) {
	    const CError& cerr = mChromo->Error();
	    if (!mSpecFile.empty()) {
		Logger()->Write(EErr, NULL, "Chromo [%s] error [pos %d]: %s", mSpecFile.c_str(), cerr.mPos.operator streamoff(), cerr.mText.c_str());
	    } else {
		Logger()->Write(EErr, NULL, "Chromo error [pos %d]: %s", cerr.mPos.operator streamoff(), cerr.mText.c_str());
	    }
	} else {
	    /*
	    Pdur(PEvents::Dur_Profiler_Dur_Start);
	    Pdur(PEvents::Dur_Profiler_Dur);
	    Pclock(PEvents::Env_Start_Constr, mRoot);
	    Pdur(PEvents::Dur_Env_Constr_Start);
	    */
	    const ChromoNode& root = mChromo->Root();

	    /**/
	    string sparent = root.Attr(ENa_Parent);
	    MNode* parent = mProvider->provGetNode(sparent);
	    mRoot = mProvider->createNode(sparent, root.Attr(ENa_Id), this);
	    /*
	    mRoot = mProvider->createNode("Elem", "ROOT", this);
	    */
	    /**/
	    MElem* eroot = mRoot ? mRoot->lIf(eroot) : nullptr;
	    if (eroot != NULL) {
		Logger()->Write(EInfo, mRoot, "Started of creating system, spec [%s], loaded: %s", mSpecFile.c_str(), PROF_FIELD(mProf, PROF_DUR, PEvents::EDur_EnvSetChromo, PIndFId::EInd_VAL).c_str());
		PROF_DUR_START(mProf, PROF_DUR, PEvents::EDur_Construct);
		MutCtx mc(mRoot);
		// Adding Modules node for imports
		MChromo* chr = mProvider->createChromo();
		bool res1 = chr->SetFromSpec("Modules : Import");
		mRoot->mutate(chr->Root(), true, mc);
		mRoot->mutate(root, false, mc, true);
		// Set launcher
		mLauncher = mRoot->lIf(mLauncher);
		if (!mLauncher) for (int i = 0; i < mRoot->owner()->pcount(); i++) {
		    MOwned* comp = mRoot->owner()->pairAt(i)->provided();
		    MLauncher* desl = comp ? comp->lIf(desl) : nullptr;
		    if (desl) {
			mLauncher = desl; break;
		    }
		}
		PROF_DUR_REC(mProf, PROF_DUR, PEvents::EDur_Construct);
                auto ncnt = mRoot->owner()->bpcount(true);
		Logger()->Write(EInfo, mRoot, "Completed of creating system, nodes: %d, time: %s", ncnt,
                        PROF_FIELD(mProf, PROF_DUR, PEvents::EDur_Construct, PIndFId::EInd_VAL).c_str());
	    } else {
		Logger()->WriteFormat("Env: cannot create root elem");
	    }
	}
    }
}

MLogRec* Env::Logger()
{
    return mLogger;
}

MNode* Env::Root() const
{ 
    return mRoot; 
}

bool Env::addProvider(MProvider* aProv)
{
    return mProvider->AddProvider(aProv);
}

void Env::removeProvider(MProvider* aProv)
{
    mProvider->RemoveProvider(aProv);
}

bool Env::RunSystem(int aCount, int aIdleCount)
{
    bool res = false;
    if (mLauncher) {
	res = mLauncher->Run(aCount, aIdleCount);
    } else {
	Logger()->WriteFormat("Env: Failed running system - launcher isn't found");
    }
    return res;
}

bool Env::StopSystem()
{
    return mLauncher ? mLauncher->Stop() : false;
}

string Env::modulesUri() const
{
    return KModulesName;
}


bool Env::setEVar(const string& aName, const string& aValue)
{
    mEVars.insert(pair<string, string>(aName, aValue));
    return true;
}

bool Env::getEVar(const string& aName, string& aValue) const
{
    bool res = false;
    if (mEVars.count(aName) > 0) {
	aValue = mEVars.at(aName);
	res = true;
    }
    return res;

}
