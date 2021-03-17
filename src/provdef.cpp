
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
#include "chromo2.h"

// TODO [YB] To import from build variable
const string KModulesPath = "/usr/share/grayb/modules/";

/** @brief Chromo arguments */
const string KChromRarg_Xml = "xml";
const string KChromRarg_Chs = "chs";

/** Native agents factory registry */
const ProvDef::TFReg ProvDef::mReg ( {
	Item<Node>(), Item<Unit>(), Item<Elem>(),
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
    /*
    aInfo.push_back(Elem::Type());
    aInfo.push_back(Edge::Type());
    aInfo.push_back(Aedge::Type());
    aInfo.push_back(Vert::Type());
    aInfo.push_back(Vertp::Type());
    aInfo.push_back(Prop::Type());
    aInfo.push_back(Description::Type());
    aInfo.push_back(ConnPointBase::Type());
    aInfo.push_back(ConnPointMc::Type());
    aInfo.push_back(Syst::Type());
    aInfo.push_back(Incaps::Type());
    aInfo.push_back(DInt::Type());
    aInfo.push_back(DNInt::Type());
    aInfo.push_back(AIncInt::Type());
    aInfo.push_back(AAddInt::Type());
    aInfo.push_back(AFAddInt::Type());
    aInfo.push_back(AFSubInt::Type());
    aInfo.push_back(AFLimInt::Type());
    aInfo.push_back(AFDivInt::Type());
    aInfo.push_back(AFGTInt::Type());
    aInfo.push_back(AFBoolToInt::Type());
    aInfo.push_back(AFConvInt::Type());
    aInfo.push_back(AFIntToVect::Type());
    aInfo.push_back(AFunIntRes::Type());
    aInfo.push_back(ExtenderAgent::Type());
    aInfo.push_back(AExtender::Type());
    aInfo.push_back(StateAgent::Type());
    aInfo.push_back(ATrIncInt::Type());
    aInfo.push_back(ATrSubInt::Type());
    aInfo.push_back(ATrMplInt::Type());
    aInfo.push_back(ATrDivInt::Type());
    aInfo.push_back(ASocket::Type());
    aInfo.push_back(ADes::Type());
    */
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
