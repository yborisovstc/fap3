
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

// TODO [YB] To import from build variable
const string KModulesPath = "/usr/share/grayb/modules/";

/** @brief Chromo arguments */
const string KChromRarg_Xml = "xml";
const string KChromRarg_Chs = "chs";

/** Native agents factory registry */
const ProvDef::TFReg ProvDef::mReg ( {
	Item<Node>(), Item<Unit>(), Item<Elem>(), Item<Content>(), Item<Vertu>(), Item<Syst>(), Item<ConnPointu>(),
/*
	Item<Vertu>(), Item<Vertp>(), Item<Syst>(), Item<Systp>(), Item<Vert>(),
	Item<ACapsule>(), Item<Edge>(), Item<Aedge>(), Item<AMod>(), Item<AImports>(),
	Item<Prop>(), Item<Description>(), Item<ExtenderAgent>(), Item<ExtenderAgentInp>(), Item<ExtenderAgentOut>(),
	Item<AExtender>(), Item<ASocket>(), Item<ASocketInp>(), Item<ASocketOut>(), Item<ASocketMc>(),
	Item<Incaps>(), Item<DataBase>(), Item<DVar>(), Item<FuncBase>(), Item<ATrBase>(),
	Item<AFunc>(), Item<StateAgent>(), Item<ADes>(), Item<DInt>(), Item<DVar>(),
	Item<DNInt>(), Item<AFunInt>(), Item<AFunVar>(), Item<AFAddVar>(), Item<AFMplVar>(),
	Item<AFMplncVar>(), Item<AFMplinvVar>(), Item<AFCastVar>(), Item<AFCpsMtrdVar>(), Item<AFCpsVectVar>(),
	Item<AFDivVar>(), Item<AFBcmpVar>(), Item<AFCmpVar>(), Item<AFAtVar>(), Item<AFBoolNegVar>(),
	Item<AFSwitchVar>(), Item<AIncInt>(), Item<AFunIntRes>(), Item<AAddInt>(), Item<AFuncInt>(),
	Item<AFAddInt>(), Item<AFSubInt>(), Item<AFConvInt>(), Item<AFLimInt>(), Item<AFDivInt>(),
	Item<AFGTInt>(), Item<AFBoolToInt>(), Item<ATrInt>(), Item<ATrVar>(), Item<ATrInt>(),
	Item<ATrSubInt>(), Item<ATrMplInt>(), Item<ATrDivInt>(), Item<ATrIncInt>(), Item<ATrAddVar>(),
	Item<ATrMplVar>(), Item<ATrDivVar>(), Item<ATrCpsVectVar>(), Item<ATrSwitchVar>(), Item<ATrAtVar>(),
	Item<ATrBcmpVar>(), Item<ConnPointBase>(), Item<ConnPointBaseInp>(), Item<ConnPointBaseOut>(), Item<ConnPointMc>(),
	Item<AExtd>(), Item<AExtdInp>(), Item<AExtdOut>(), Item<ASocketMcm>(), Item<ASocketInpMcm>(), Item<ASocketOutMcm>(),
	Item<ConnPointMcu>(), Item<AExtdu>(), Item<AExtduInp>(), Item<AExtduOut>(), Item<AState>(), Item<AStatec>(),
	Item<CpStatecInp>(), Item<CpStatecOutp>(), Item<ATrcAddVar>(), Item<ATrcMplVar>(), Item<ATrcAndVar>(), Item<ATrcNegVar>(),
	Item<ATrcMaxVar>(), Item<ATrcApndVar>(), Item<ATrcNtosVar>(), Item<AMunitAdp>(), Item<AMelemAdp>(), Item<ATrcMutNode>(),
	Item<ADesLauncher>(), Item<ATrcSwitchBool>(), Item<ATrcCmpVar>(), Item<ATrcAtVar>(), Item<ATrcSizeVar>(), Item<ATrcMinVar>(),
	Item<AMntp>(), Item<ATrcMutConn>(), Item<ATrcUri>()
	*/
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
