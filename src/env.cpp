
#include <dirent.h>
#include <sys/stat.h>

#include "env.h"
#include "log.h"
#include "node.h"
#include "mchromo.h"
#include "chromo.h"
#include "elem.h"


static const string KLogFileName = "faplog.txt";
static const string KModulesName = "Modules";


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
	    string rname = spec->Root().Name();
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
	MNode* comp = aNode->getNode(aMut.Name());
	if (!comp) {
	    // Node doesn't exist yet, to mutate
	    // Using external mut instead of nodes mut to avoid mut update on recursive import
	    MChromo* mut = mHost.provider()->createChromo();
	    mut->Init(ENt_Node);
	    mut->Root().AddChild(aMut);
	    aNode->mutate(mut->Root(), true, MutCtx(aNode), true);
	    delete mut;
	} else {
	    mHost.Logger()->Write(EErr, NULL, "Importing [%s]: module already exists", aMut.Name().c_str());
	}
    }
}



///// Env


Env::Env(const string& aSpecFile, const string& aLogFileName): mRoot(NULL), mSpecFile(aSpecFile),/*mLogger(NULL),*/
    mChromo(NULL) /*, mEnPerfTrace(false), mEnIfTrace(false), mExtIfProv(NULL), mIfResolver(NULL), mProf(NULL)*/
{
    mLogger = new Logrec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    mProvider = new Factory(string(), this);
    mImpMgr = new ImportsMgr(*this);
    /*
    iProvider->LoadPlugins();
    srand(time(NULL));
    iChMgr = new ChromoMgr(*this);
    string chromo_fext = iSpecFile.substr(iSpecFile.find_last_of(".") + 1);
    iChMgr->SetChromoRslArgs(chromo_fext);
    mIfResolver = new IfcResolver(*this);    
    mProf = new GProfiler(this, KPInitData);
    */
    // Profilers events
    /*
    mPfid_Start_Constr = Profiler()->RegisterEvent(TPEvent("Start construction"));
    mPfid_Root_Created = Profiler()->RegisterEvent(TPEvent("Root created"));
    mPfid_Root_Created_From_Start = Profiler()->RegisterEvent(TPEvent("Root created from start", mPfid_Start_Constr));
    mPfid_End_Constr = Profiler()->RegisterEvent(TPEvent("End construction"));
    */
}

Env::Env(const string& aSpec, const string& aLogFileName, bool aOpt): mRoot(NULL), /*mLogger(NULL),*/
    mChromo(NULL)/*, mEnPerfTrace(false), mEnIfTrace(false), mExtIfProv(NULL), mIfResolver(NULL), mProf(NULL)*/
{
    /*
    mLogger = new GLogRec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    //mLogger = new GLogRec("Logger", aName + ".log");
    iProvider = new GFactory(string(), this);
    iProvider->LoadPlugins();
    iSpec= aSpec;
    srand(time(NULL));
    iChMgr = new ChromoMgr(*this);
    string chromo_fext = iSpecFile.substr(iSpecFile.find_last_of(".") + 1);
    iChMgr->SetChromoRslArgs(chromo_fext);
    iImpMgr = new ImportsMgr(*this);
    mIfResolver = new IfcResolver(*this);    
    mProf = new GProfiler(this, KPInitData);
    */
    // Profilers events
    /*
    mPfid_Start_Constr = Profiler()->RegisterEvent(TPEvent("Start construction"));
    mPfid_Root_Created = Profiler()->RegisterEvent(TPEvent("Root created"));
    mPfid_Root_Created_From_Start = Profiler()->RegisterEvent(TPEvent("Root created from start", mPfid_Start_Constr));
    mPfid_End_Constr = Profiler()->RegisterEvent(TPEvent("End construction"));
    */
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
    /*
    delete mIfResolver;
    delete mProf;
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
	if (!mSpecFile.empty()) {
	    mChromo->SetFromFile(mSpecFile);
	} else {
	    mChromo->SetFromSpec(mSpec);
	}
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
	    MNode* parent = mProvider->getNode(sparent);
	    mRoot = mProvider->createNode(sparent, root.Name(), this);
	    /**/
	    MElem* eroot = mRoot ? mRoot->lIf(eroot) : nullptr;
	    if (eroot != NULL) {
		//Pclock(PEvents::Env_Root_Created, mRoot);
		stringstream ss;
		struct timeval tp;
		/*
		gettimeofday(&tp, NULL);
		long int beg_us = tp.tv_sec * 1000000 + tp.tv_usec;
		*/
		Logger()->Write(EInfo, mRoot, "Started of creating system, spec [%s]", mSpecFile.c_str());
		//MutCtx mc(mRoot, mRoot);
		MutCtx mc(mRoot);
		mRoot->mutate(root, false, mc, true);
		/*
		Pclock(PEvents::Env_End_Constr, mRoot);
		gettimeofday(&tp, NULL);
		long int fin_us = tp.tv_sec * 1000000 + tp.tv_usec;
		ss << (fin_us - beg_us);
		TInt cpc = mRoot->GetCapacity();
		TInt cpi = iImpMgr->GetImportsContainer() ? iImpMgr->GetImportsContainer()->GetCapacity() : 0;
		Logger()->Write(EInfo, mRoot, "Completed of creating system, nodes: %d, imported: %d, time, us: %s", cpc,  cpi, ss.str().c_str());
		*/
		Logger()->Write(EInfo, mRoot, "Completed of creating system");
		// Set launcher

		for (int i = 0; i < mRoot->owner()->pcount(); i++) {
		    MOwned* comp = mRoot->owner()->pairAt(i);
		    MLauncher* desl = comp ? comp->lIf(desl) : nullptr;
		    if (desl) {
			mLauncher = desl; break;
		    }
		}
		/*
		mLauncher = dynamic_cast<MLauncher*>(mRoot->MUnit::GetSIfi(MLauncher::Type()));
		if (mLauncher == NULL) {
		    Logger()->Write(EErr, NULL, "Cannot find launcher");
		}
		*/
	    } else {
		Logger()->WriteFormat("Env: cannot create root elem");
	    }
	    //Pdur(PEvents::Dur_Env_Constr);
	}
    }
}

MLogRec* Env::Logger() { return mLogger;}

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

bool Env::RunSystem(int aCount)
{
    bool res = false;
    if (mLauncher) {
	res = mLauncher->Run(aCount);
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
