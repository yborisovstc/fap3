#include "prov.h"
#include "factr.h"
#include "provdef.h"
#include "mplugin.h"
#include <dlfcn.h>
#include <dirent.h>
#include <iostream>
#include <assert.h>


// Plugins default dir
string KPluginDir = "/usr/lib/fap3/plugins/";

Factory::Factory(const string& aName, MEnv* aEnv): iName(aName), iEnv(aEnv)
{
    Provider* baseprov = new ProvDef("ProvDef", iEnv);
    assert(baseprov != NULL);
    AddProvider(baseprov);
}

Factory::~Factory()
{
    while (!iProviders.empty()) {
	MProvider* prov = iProviders.begin()->second;
	delete prov;
	iProviders.erase(iProviders.begin());
    }
}

MNode* Factory::createNode(const string& aType, const string& aName, MEnv* aEnv)
{
    MNode* res = NULL;
    for (map<string, MProvider*>::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->createNode(aType, aName, aEnv);
    }
    return res;
}

MNode* Factory::provGetNode(const string& aUri)
{
    MNode* res = NULL;
    for (TProviders::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->provGetNode(aUri);
    }
    return res;
}

bool Factory::isProvided(const MNode* aElem) const
{
    bool res = false;
    for (TProviders::const_iterator it = iProviders.begin(); it != iProviders.end() && !res; it++) {
	res = it->second->isProvided(aElem);
    }
    return res;

}

void Factory::setChromoRslArgs(const string& aRargs)
{
    assert(mChromoRargs.empty());
    mChromoRargs = aRargs;
}

void Factory::getChromoRslArgs(string& aRargs)
{
    aRargs = mChromoRargs;
}

MChromo* Factory::createChromo(const string& aRargs)
{
    MChromo* res = NULL;
    for (TProviders::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	res = it->second->createChromo(aRargs.empty() ? mChromoRargs : aRargs);
    }
    return res;
}

bool Factory::LoadPlugin(const string& aName)
{
    bool res = false;
    MProvider* prov = NULL;
    string plgpath = KPluginDir + aName;
    void* handle = dlopen(plgpath.c_str(), RTLD_NOW|RTLD_LOCAL|RTLD_DEEPBIND);
    if (handle != NULL) {
	plugin_init_func_t* init = (plugin_init_func_t*) dlsym(handle, "init");
	if (init!= NULL) {
	    prov = init(iEnv);
	    if (prov != NULL) {
		res = true;
		AddProvider(prov);
	    }
	}
	if (!res) {
	    dlclose(handle);
	}
    } else {
	char* err = dlerror();
	cout << "Failed loading plugin: " << err << endl;
    }
    return res;
}

int Factory::FilterPlgDirEntries(const struct dirent *aEntry)
{
    string name = aEntry->d_name;
    size_t ppos = name.find_first_of(".");
    string suff = name.substr(ppos + 1);
    int res = suff.compare("so"); 
    return (res == 0) ? 1 : 0;
}

void Factory::LoadPlugins()
{
    // List plugins directory
    struct dirent **entlist;
    int n = scandir(KPluginDir.c_str(), &entlist, FilterPlgDirEntries, alphasort);
    // Load plugins
    for (int cnt = 0; cnt < n; ++cnt) {
	LoadPlugin(entlist[cnt]->d_name);
    }
}

bool Factory::AddProvider(MProvider* aProv)
{
    bool res = true;
    // TODO To support name
    TProviders::const_iterator found = iProviders.find(aProv->providerName());
    if(found == iProviders.end()) {
	iProviders.insert(TProvidersElem(aProv->providerName(), aProv));
    } else {
	res = false;
    }
    return res;
}

void Factory::RemoveProvider(MProvider* aProv)
{
    assert(iProviders.count(aProv->providerName()) > 0);
    iProviders.erase(aProv->providerName());
}

void Factory::getNodesInfo(vector<string>& aInfo)
{
    for (TProviders::iterator it = iProviders.begin(); it != iProviders.end(); it++) {
	it->second->getNodesInfo(aInfo);
    }
}

const string& Factory::modulesPath() const
{
    MProvider* defprov = iProviders.at("ProvDef");
    assert(defprov != NULL);
    return defprov->modulesPath();
}

void Factory::setEnv(MEnv* aEnv)
{
    assert(iEnv == NULL || aEnv == NULL);
    iEnv = aEnv;
}

DtBase* Factory::createData(const string& aType)
{
    DtBase* res = NULL;
    for (TProviders::iterator it = iProviders.begin(); it != iProviders.end() && res == NULL; it++) {
	auto* prov = it->second;
	res = prov->createData(aType);
    }
    return res;

}

void Factory::MProvider_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    for (auto it = iProviders.cbegin(); it != iProviders.cend(); it++) {
	auto* prov = it->second;
	prov->MProvider_doDump(aLevel, aIdt, aOs);
    }
}
