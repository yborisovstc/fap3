
#include "env.h"
#include "log.h"
#include "node.h"
#include "mchromo.h"
#include "chromo.h"
#include "elem.h"


static const string KLogFileName = "faplog.txt";

Env::Env(const string& aSpecFile, const string& aLogFileName): mRoot(NULL), mSpecFile(aSpecFile),/*mLogger(NULL),*/
    mChromo(NULL)/*, mEnPerfTrace(EFalse), mEnIfTrace(EFalse), mExtIfProv(NULL), mIfResolver(NULL), mProf(NULL)*/
{
    mLogger = new Logrec(aLogFileName.empty() ? KLogFileName : aLogFileName);
    mProvider = new Factory(string(), this);
    /*
    iProvider->LoadPlugins();
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

Env::Env(const string& aSpec, const string& aLogFileName, bool aOpt): mRoot(NULL), /*mLogger(NULL),*/
    mChromo(NULL)/*, mEnPerfTrace(EFalse), mEnIfTrace(EFalse), mExtIfProv(NULL), mIfResolver(NULL), mProf(NULL)*/
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
    delete mImpMgr;
    */
    delete mProvider;
    delete mLogger;
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
		MutCtx mc(mRoot, mRoot);
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


