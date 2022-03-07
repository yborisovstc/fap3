
#include "provdef.h"
/*
#include "vert.h"
#include "prop.h"
#include "syst.h"
#include "incaps.h"
#include "data.h"
#include "func.h"
#include "des.h"
#include "module.h"
#include "imports.h"
#include "mnt.h"
*/
#include "node.h"
#include "unit.h"
#include "elem.h"
#include "content.h"
#include "chromo2.h"
#include "syst.h"
#include "des.h"
#include "dest.h"
#include "desio.h"
#include "desadp.h"
#include "dessdc.h"
#include "mnt.h"
#include "link.h"

// TODO [YB] To import from build variable
const string KModulesPath = "/usr/share/grayb/modules/";

/** @brief Chromo arguments */
const string KChromRarg_Xml = "xml";
const string KChromRarg_Chs = "chs";

/** Native agents factory registry */
const ProvDef::TFReg ProvDef::mReg ( {
	Item<Node>(), Item<Unit>(), Item<Elem>(), Item<Content>(), Item<Vertu>(), Item<Vert>(), Item<Link>(),
	Item<Syst>(), Item<ConnPointu>(), Item<CpMnodeOutp>(), Item<CpMnodeInp>(),
	Item<Extd>(), Item<State>(), Item<Des>(), Item<TrAddVar>(), Item<CpStateInp>(),  Item<CpStateOutp>(),
	Item<CpStateMnodeInp>(),  Item<CpStateMnodeOutp>(),
	Item<DesLauncher>(), Item<Sout>(), Item<ADes>(), Item<Socket>(),
	Item<TrMaxVar>(), Item<TrSwitchBool>(), Item<TrCmpVar>(), Item<TrApndVar>(),
	Item<TrAndVar>(), Item<TrNegVar>(), Item<TrUri>(),
	Item<TrMutNode>(), Item<TrMutConn>(), Item<TrMutDisconn>(), Item<TrMutCont>(),
	Item<TrSizeVar>(), Item<TrAtVar>(),
	Item<TrTuple>(), Item<TrChr>(), Item<TrChrc>(),
	Item<AMnodeAdp>(), Item<AMelemAdp>(), Item<AMntp>(),
	/* SDCs */
	Item<ASdcMut>(), Item<ASdcComp>(), Item<ASdcConn>(), Item<ASdcInsert>(),
	});



ProvDef::ProvDef(const string& aName, MEnv* aEnv): ProvBase(aName, aEnv)
{
}

ProvDef::~ProvDef()
{
}

void ProvDef::getNodesInfo(vector<string>& aInfo)
{
    for (auto elem : mReg) {
	aInfo.push_back(elem.first);
    }
}

const string& ProvDef::modulesPath() const
{
    return KModulesPath;
}

MChromo* ProvDef::createChromo(const string& aRargs)
{
    MChromo* res = NULL;
    if (aRargs == KChromRarg_Chs) {
	res = new Chromo2();
    } else if (aRargs.empty()) {
	// Default chromo type
	res = new Chromo2();
    }
    return res;
}
