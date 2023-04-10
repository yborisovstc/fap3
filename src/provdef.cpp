
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
#include "import.h"
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
#include "dessdo.h"
#include "dessp.h"
#include "desconst.h"
#include "mnt.h"
#include "link.h"

// TODO [YB] To import from build variable
const string KModulesPath = "/usr/share/grayb/modules/";

/** @brief Chromo arguments */
const string KChromRarg_Xml = "xml";
const string KChromRarg_Chs = "chs";

/** Native agents factory registry */
const ProvDef::TFReg ProvDef::mReg ( {
	Item<Node>(), Item<Unit>(), Item<Import>(), Item<Elem>(), Item<Content>(), Item<Vertu>(), Item<Vert>(), Item<Link>(),
	Item<Syst>(), Item<ConnPointu>(), Item<CpMnodeOutp>(), Item<CpMnodeInp>(),
	Item<Extd>(), Item<State>(), Item<Const>(), Item<Des>(), Item<TrAddVar>(), Item<TrAdd2Var>(), Item<TrSub2Var>(),
	Item<CpStateInp>(), Item<CpStateOutp>(), Item<ExtdStateInp>(), Item<ExtdStateOutp>(), Item<ExtdStateMnodeOutp>(),
	Item<CpStateMnodeInp>(),  Item<CpStateMnodeOutp>(),
	Item<DesLauncher>(), Item<Sout>(), Item<ADes>(), Item<Socket>(),
	Item<TrMinVar>(), Item<TrMaxVar>(), Item<TrSwitchBool>(), Item<TrSwitchBool2>(), Item<TrCmpVar>(), Item<TrApndVar>(),
	Item<TrAndVar>(), Item<TrOrVar>(), Item<TrNegVar>(), Item<TrToUriVar>(), Item<TrTostrVar>(),
	Item<TrMutNode>(), Item<TrMutConn>(), Item<TrMutDisconn>(), Item<TrMutCont>(), Item<TrHash>(), Item<TrPair>(),
	Item<TrSizeVar>(), Item<TrAtVar>(), Item<TrAtgVar>(), Item<TrMplVar>(), Item<TrDivVar>(), Item<TrSvldVar>(), Item<TrInpSel>(), Item<TrInpCnt>(),
	Item<TrTuple>(), Item<TrTupleSel>(), Item<TrChr>(), Item<TrChrc>(), Item<TrIsValid>(), Item<TrTailVar>(), Item<TrHeadVar>(), Item<TrTailnVar>(),
	/* Des adapters */
	Item<AMnodeAdp>(), Item<AMelemAdp>(), Item<AMntp>(), Item<DAdp>(),
	/* SDCs */
	Item<ASdcMut>(), Item<ASdcComp>(), Item<ASdcRm>(), Item<ASdcConn>(), Item<ASdcDisconn>(), Item<ASdcInsert2>(), Item<ASdcInsertN>(),
	Item<ASdcExtract>(), 
	/* SDOs */
        Item<SdoName>(), Item<SdoUri>(), Item<SdoParent>(), Item<SdoComp>(), Item<SdoConn>(), Item<SdoCompsCount>(), Item<SdoCompsNames>(), 
	Item<SdoPairsCount>(), Item<SdoPair>(), Item<SdoCompOwner>(), Item<SdoCompComp>(), Item<SdoPairs>(), Item<SdoTPairs>(),
        Item<SdoTcPair>(), Item<SdoEdges>(),
	/* DES context */
	Item<DesCtxSpl>(), Item<DesCtxCsm>(), 
	/* DES active subs */
	Item<DesAs>(), 
	/* DES service point */
	Item<DesSpe>(), Item<DesSp>(), Item<DesSpt>(), Item<ADesSpc>(), 
	/* DES constants */
	Item<SI_0>(), Item<SI_1>(), Item<SB_False>(), Item<SB_True>(),
	});

/** Data factory registry */
const ProvDef::TDtFReg ProvDef::mDtReg ( {
	DItem<DGuri>(), 
	DItem<Sdata<int>>(), DItem<Sdata<bool>>(), DItem<Sdata<string>>(),
	DItem<Pair<string>>(), DItem<Pair<Sdata<int>>>(), DItem<Pair<Sdata<string>>>(), DItem<Pair<DGuri>>(),
	DItem<Vector<string>>(), DItem<Vector<DGuri>>(), DItem<Vector<Pair<DGuri>>>(), DItem<Vector<Pair<string>>>(), 
	DItem<NTuple>(), 
	DItem<DChr2>(), DItem<DMut>()
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
