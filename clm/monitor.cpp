#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <fstream>


#include <mprov.h>
#include <mchromo.h>
#include <melem.h>
#include <mlog.h>

#include "monitor.h"

using namespace std;

const string Monitor::KDefPrompt = ">";

const string K_Chr2Ext = "chs";

/** Generator of input handlers factory registry item */
template<typename T> pair<string, Monitor::TIhFact*> Item() {
    return pair<string, Monitor::TIhFact*>
	(T::command(), T::create);
}

Monitor::Monitor(): mEnv(nullptr), mPrompt(KDefPrompt),
    mIdleCyclesLimit(0)
{

}

Monitor::~Monitor()
{
}

void Monitor::setSpecName(const string& aFileName)
{
    assert(mSpecName.empty());
    mSpecName = aFileName;
}

void Monitor::SetIdleCyclesLimit(int aLimit)
{
    mIdleCyclesLimit = aLimit;
}

void Monitor::initEnv(bool aVerbose)
{
    if (mEnv != nullptr) {
	delete mEnv;
	mEnv = nullptr;
    }
    if (mLogName.empty()) {
	// Assuming log name is default
	mLogName = mSpecName + ".log";
    }
    if (mSpecName.empty() && aVerbose) {
	cout << "Error: chromo filename is not specified" << endl;
    }
    mEnv = new Env(mSpecName, mLogName);
    for (auto path : mModPaths) {
	mEnv->ImpsMgr()->AddImportsPaths(path);
    }
    for (auto evar : mEVars) {
	// TODO Updata env to return error from SetEVar
	mEnv->setEVar(evar.first, evar.second);
    }
}


void Monitor::runModel()
{
    assert(mEnv != nullptr);
    mEnv->constructSystem();
    if (mEnv->Root() != NULL) {
	bool res = mEnv->RunSystem(0, mIdleCyclesLimit);
	if (!res) {
	    mEnv->Logger()->Write(EErr, NULL, "Monitor: Failed running the model");
	}
    }
}

bool Monitor::setProfPath(const string& aPath)
{
    bool res = true;
    /*
    FILE* fp = fopen(aPath.c_str(), "w+");
    if (fp) {
	mProfName = aPath;
    } else {
	res = false;
    }
    */
    mProfName = aPath;
    return res;
}

bool Monitor::setModulePath(const string& aPath)
{
    bool res = true;
    mModPaths.push_back(aPath);
    return res;
}

void Monitor::setOutSpecName(const string& aFileName)
{
    mCSpecName = aFileName;
}

bool Monitor::setLogFile(const string& aPath)
{
    bool res = true;
    mLogName = aPath;
    return res;
}

bool Monitor::saveProfilerData()
{
    // Save profiler data to file
    bool res = true;
    //res = mEnv->profiler()->SaveToFile(mProfName);
    return res;
}

bool Monitor::run()
{
    bool res = true;
    bool end = false;
    do {
	cout << mPrompt;
	// TODO to redesign using streambuf
	const int bufsize = 256;
	char buf[bufsize];
	cin.getline(buf, bufsize);
	string input(buf); 

	if (!input.empty()) {
	    if (input == "exit") {
		end = true;
	    } else {
		size_t pos = input.find_first_of(' ');
		string command = input.substr(0, pos);
		string args = input.substr(pos + 1);
		// Create handler
		unique_ptr<InputHandler> handler(createHandler(command));
		if (handler.get() == nullptr) {
		    cout << "Error: unknown command [" << command << "]";
		} else {
		    handler->handle(args);
		}
	    }
	}
    } while (!end);
    return res;
}

bool Monitor::copyFile(const string& aSrcFname, const string& aDstFname)
{
    return false;
}

bool Monitor::convertSpec()
{
    bool res = true;
    initEnv();
    string chromo_fext = mSpecName.substr(mSpecName.find_last_of(".") + 1);
    MProvider* prov = mEnv->provider();
    MChromo* chr = prov->createChromo(chromo_fext);
    chr->SetFromFile(mSpecName);
    if (chr->IsError()) {
	cout << "Pos: " << chr->Error().mPos << " -- " << chr->Error().mText << endl;
    }
    string ochromo_fext = mCSpecName.substr(mCSpecName.find_last_of(".") + 1);
    MChromo* ochr = prov->createChromo(ochromo_fext);
    ochr->Convert(*chr);
    ochr->Save(mCSpecName);
 
    return res;
}

void storeStdinToFile(const string& aFname)
{
    ofstream os(aFname, ofstream::out);
    if (os) {
	streambuf* ibuf = cin.rdbuf();
	streambuf* obuf = os.rdbuf();
	while (ibuf->sgetc() != EOF )
	{
	    char ch = ibuf->sbumpc();
	    obuf->sputc(ch);
	}
	os.close();
    }
}

void outputFileToStdout(const string& aFname)
{
    ifstream is(aFname, ifstream::out);
    if (is) {
	streambuf* ibuf = is.rdbuf();
	streambuf* obuf = cout.rdbuf();
	while (ibuf->sgetc() != EOF )
	{
	    char ch = ibuf->sbumpc();
	    obuf->sputc(ch);
	}
	is.close();
    }
}

bool Monitor::formatSpec()
{
    bool res = false;
    string sname(mSpecName);
    if (mSpecName.empty()) {
	sname = "/tmp/.fap2_fmt_tmp.chs";
	storeStdinToFile(sname);
    }
    initEnv(false);
    MProvider* prov = mEnv->provider();
    MChromo* chr = prov->createChromo(K_Chr2Ext);
    chr->SetFromFile(sname);
    if (chr->IsError()) {
	cout << "Pos: " << chr->Error().mPos << " -- " << chr->Error().mText << endl;
    }
    MChromo* ochr = prov->createChromo(K_Chr2Ext);
    ochr->Convert(*chr);
    ochr->Save(sname);
    outputFileToStdout(sname);
    res = true;
    return res;
}

InputHandler* Monitor::createHandler(const string& aCmd)
{
    InputHandler* res = nullptr;
    if (mIhReg.count(aCmd) > 0) {
	TIhFact* fact = mIhReg.at(aCmd);
	res = fact(*this);
    }
    return res;
}

bool Monitor::saveModel(const string& aPath)
{
    bool res = false;
    MNode* uroot = mEnv->Root();
    MElem* root = uroot ? uroot->lIf(root) : nullptr;
    if (root) {
	// Check getting chromo 
	root->Chromos().Save(aPath);
    }
    return res;
}

bool Monitor::addEVar(const string& aName, const string& aValue)
{
    auto res = mEVars.insert(TEVarElem(aName, aValue));
    return res.second;
}




/** Handler of 'run' command
 * Creates environment  and runs model with given spec
 */
class IhRun: public InputHandler
{
    public:
	static const string command() { return "run";}
	static InputHandler* create(Monitor& aHost) { return new IhRun(aHost);}

	IhRun(Monitor& aHost): InputHandler(aHost) {}
	virtual bool handle(const string& aArgs) override {
	    // Init env and run
	    mHost.initEnv();
	    mHost.runModel();
	    bool sdres = mHost.saveProfilerData();
	    if (!sdres) {
		cout << "Error on saving profile data to file";
	    }
	    return true;
	}
};

/** Handler of 'save' command
 * Saves models spec (OSM)
 */
class IhSave: public InputHandler
{
    public:
	static const string command() { return "save";}
	static InputHandler* create(Monitor& aHost) { return new IhSave(aHost);}
	IhSave(Monitor& aHost): InputHandler(aHost) {}
	virtual bool handle(const string& aArgs) override {
	    bool res = mHost.saveModel(aArgs);
	    return res;
	}
};


const Monitor::TIhReg Monitor::mIhReg = {
    {IhRun::command(), IhRun::create},
    Item<IhSave>()
};


