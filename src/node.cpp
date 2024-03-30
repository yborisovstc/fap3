
#include <list>

#include "node.h"
#include "chromo.h"



static const string K_Cont_Debug_LogLevel = "Debug.LogLevel";
static const string K_Cont_LogLevel = "LogLevel";
static const string K_Cont_OwdLogLevel = "OwdLogLevel";
static const string K_Cont_Explorable = "Explorable"; // Value "n", "y"
static const string K_Cont_Controllable = "Controllable"; // Value "n", "y"
static const string K_Cont_Val_Yes = "y";
static const string K_Cont_Val_No = "n";
static const string K_ContentType = "Content";

/** @brief Content "Logging parameters"
 * The content data format is:
 * */ 
static const char K_LogLevelSep= '.';
static const string K_LogLevel_Err= "Err";
static const string K_LogLevel_Warn= "Warn";
static const string K_LogLevel_Info= "Info";
static const string K_LogLevel_Dbg= "Dbg";
static const string K_LogLevel_Dbg2= "Dbg2";

// TODO to find proper place for it
static const string K_SpName_Ns= "_@";
static const string K_SpName_Nil= "_";

// Owned LogLevel
int Node::K_Ll_Mask = 0xff;
int Node::K_Oll_Shift = 8;
int Node::K_SOll_Shift = 16;
int Node::K_Ll_Shift = 0;

Node::Node(const string &aType, const string &aName, MEnv* aEnv): mName(aName.empty() ? aType : aName), mEnv(aEnv), mOcp(this), mOnode(this, this),
    mLogLevel(EInfo), mExplorable(false), mControllable(false)
{
} 

Node::~Node()
{
    // Removing the owneds
    /*
    auto owdCp = owner()->firstPair();
    while (owdCp) {
	owner()->disconnect(owdCp);
	owdCp->provided()->deleteOwned();
	owdCp = owner()->firstPair();
    }
    */
    auto bg = owner()->pairsBegin();
    while (bg != owner()->pairsEnd()) {
	auto* owdCp = *bg;
	owner()->disconnect(owdCp);
	owdCp->provided()->deleteOwned();
	bg = owner()->pairsBegin();
    }
    // Disconnect all observers
    mOcp.disconnectAll();
}

#ifdef ENABLE_IFC
MIface* Node::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif2<MNode>(aType, mMNodePtr));
    else if (res = checkLif2<MOwned>(aType, mMOwnedPtr));
    else if (res = checkLif2<MOwner>(aType, mMOwnerPtr));
    else if (res = checkLif2<MContentOwner>(aType, mMContentOwnerPtr));
    else if (res = checkLif2<MObservable>(aType, mMObservablePtr));
    return res;
}
#else
MIface* Node::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MNode>(aType));
    else if (res = checkLif<MOwned>(aType));
    else if (res = checkLif<MOwner>(aType));
    else if (res = checkLif<MContentOwner>(aType));
    else if (res = checkLif<MObservable>(aType));
    return res;
}
#endif


MIface* Node::MObservable_getLif(const char *aType)
{
    MIface* res = nullptr;
    return res;
}

void Node::MObservable_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, cout); cout << "Name: " << mName << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	Ifu::offset(aIdt, cout); cout << "Observers: " << endl;
	for (auto it = mOcp.pairsCBegin(); it != mOcp.pairsCEnd(); it++) {
	    auto* obs = *it;
	    Ifu::offset(aIdt, cout); cout << "- " << obs->provided()->Uid() << endl;
	}
    }
}

void Node::MNode_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, cout); cout << "Name: " << mName << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	for (int i = 0; i < owner()->pcount(); i++) {
	    if (owner()->pairAt(i)) {
		const MOwned* comp = owner()->pairAt(i)->provided();
		const MNode* compn = comp->lIf(compn);
		if (compn) {
		    Ifu::offset(aIdt, cout);
		    cout << "- "  << compn->name() << endl;
		    if (aLevel & Ifu::EDM_Recursive) {
			compn->doDump(Ifu::EDM_Comps | Ifu::EDM_Recursive, aIdt + Ifu::KDumpIndent, cout);
		    }
		} else {
		    cout << "=== ERROR on getting component [" << i << "] MNode" << endl;
		}
	    } else {
		cout << "=== ERROR on getting comp [" << i << "]" << endl;
	    }
	}
    }
}

const MNode* Node::getComp(const string& aId) const
{
    auto ownerCp = owner()->pairAt(aId);
    const MOwned* ores = ownerCp ? ownerCp->provided() : nullptr;
    const MNode* res = ores ? ores->lIf(res) : nullptr;
    return res;
}

const MNode* Node::getNode(const GUri& aUri) const
{
    const MNode* res = nullptr;
    if (aUri.isValid()) {
	if (aUri.size() == 0 || aUri.size() == 1 && aUri.at(0) == GUri::K_Self) {
	    res = this;
	} else {
	    if (aUri.isAbsolute()) {
		if (Owner()) {
		    res = Owner()->ownerGetNode(aUri, this);
		} else {
		    // Root
		    if (aUri.size() > 2) {
			res = getComp(aUri.at(2));
			for (int i = 3; i < aUri.size() && res; i++) {
			    res = res->getComp(aUri.at(i));
			}
		    }
		}
	    } else {
		res = getComp(aUri.at(0));
		for (int i = 1; i < aUri.size() && res; i++) {
		    res = res->getComp(aUri.at(i));
		}
	    }
	    if (!res && aUri.size() == 1) {
		// Try native
		if (mEnv && mEnv->provider()) {
		    res = mEnv->provider()->provGetNode(aUri.at(0));
		}
	    }
	}
    } else {
	Log(EErr, TLog(this) + "Invalid URI [" + aUri + "]");
    }
    return res;
}

void Node::getUri(GUri& aUri, MNode* aBase) const
{
    aUri.clear();
    if (mEnv && mEnv->provider() && mEnv->provider()->isProvided(this)) {
	// Provided
	aUri.appendElem(mName);
    } else if (aBase != this) {
	// From native hier
	const MOwner* owner = Owner();
	if (owner) {
	    owner->ownerGetUri(aUri, aBase);
	} else {
	    aUri.appendElem(string());
	}
	aUri.appendElem(mName);
    }
}

MOwner* Node::Owner()
{
    return (mOnode.pcount() > 0) ? (*mOnode.pairsBegin())->provided() : nullptr;
}

const MOwner* Node::Owner() const
{
    auto onode = const_cast<TOwtNode*>(&mOnode);
    return (mOnode.pcount() > 0) ? (*onode->pairsBegin())->provided() : nullptr;
}

void Node::updateNs(TNs& aNs, const ChromoNode& aCnode)
{
    if (aCnode.AttrExists(ENa_NS)) {
	string ns = aCnode.Attr(ENa_NS);
	// Handle only NS for explicit Ctx, ref ds_cli_ddmc_ues_ics
	if (!ns.empty()) {
	    MNode* nsu = getNode(ns, aNs);
	    if (nsu == NULL) {
		Log(EErr, TLog(this) + "Cannot find namespace [" + ns + "]");
	    } else {
		aNs.clear(); // Override namespace by explicitly stated one
		aNs.push_back(nsu);
	    }
	}
    }
}


MNode* Node::getNode(const string& aName, const TNs& aNs)
{
    MNode *res = nullptr;
    MNode *resns = nullptr;
    GUri uri(aName);
    res = getNode(uri);
    for (auto nsit = aNs.rbegin(); nsit != aNs.rend() && !resns; nsit++) {
	auto ns = *nsit;
	if (ns == this) continue;
	resns = ns->getNode(uri);
    }
    // Resolving name in current context/native first
    if (!res) {
	// Then in namespaces
	// Applied namespaces priority approach, ref I_NRC
	res = resns;
    } else if (resns && res != this) {
	Log(EErr, TLog(this) + "Same node [" + uri.toString() + "] is resolved both in current ctx and namespace");
    }
    return res;
}

void Node::mutSegment(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    bool res = true;
    //!!TNs root_ns; // Don't propagate context from upper layer, ref ds_cli_sno_s3
    TNs root_ns = aCtx.mNs; 
    updateNs(root_ns, aMut);

    for (ChromoNode::Const_Iterator rit = aMut.Begin(); rit != aMut.End() && res; rit++)
    {
	ChromoNode rno = (*rit);
	MutCtx mctx(this, root_ns);
	try {
	    mutate(rno, aUpdOnly, mctx);
	} catch (std::exception e) {
	    Logger()->Write(EErr, this, "Unspecified error on mutation");
	}
    }
}

void Node::mutate(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx, bool aTreatAsChromo, bool aLocal)
{
    bool res = true;
    TNs root_ns = aCtx.mNs;
    updateNs(root_ns, aMut);
    bool targ_nil = false;

    ChromoNode rno = aMut;
    Logger()->SetContextMutId(rno.LineId());
    // Get target node by analysis of mut-target and mut-node, ref ds_chr2_cctx_tn_umt
    PFL_DUR_STAT_START(PEvents::EDurStat_MutNtf);
    notifyNodeMutated(aMut, aCtx);
    PFL_DUR_STAT_REC(PEvents::EDurStat_MutNtf);
    MNode* targ = this;
    bool exs_targ = rno.AttrExists(ENa_Targ);
    bool exs_mnode = rno.AttrExists(ENa_MutNode);
    string starg, smnode;
    if (!aTreatAsChromo && exs_targ && !aLocal) {
	starg = rno.Attr(ENa_Targ);
	if (starg == K_SpName_Ns) {
	    if (root_ns.size()) {
		targ = root_ns.back();
	    } else {
		Logger()->Write(EErr, this, "No namespace defined");
	    }
	} else if (starg == K_SpName_Nil) {
	    targ_nil = true;
	} else {
	    targ = targ->getNode(starg, root_ns);
	}
	if (!targ) {
	    Logger()->Write(EErr, this, "Cannot find target node [%s]", starg.c_str());
	    res = false;
	}
    }
    if (res) {
	if (targ != this || targ_nil) {
	    // Targeted mutation
	    if (!targ_nil) {
		// Redirect the mut to target: no run-time to keep the mut in internal nodes
		// Propagate till target owning comp if run-time to keep hidden all muts from parent
		MutCtx mctx(aUpdOnly ? targ : aCtx.mNode, root_ns);
		targ->mutate(rno, aUpdOnly, mctx, aTreatAsChromo, true);
	    }
	} else  {
	    // Local mutation
	    MutCtx mctx(aCtx.mNode, root_ns);
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Seg || aTreatAsChromo) {
		mutSegment(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Node) {
		MNode* mres = mutAddElem(rno, aUpdOnly, mctx);
		if (rno.Count()) {
		    TNs root_ns;
		    assert(!(exs_targ && rno.AttrExists(ENa_NS)));
		    if (exs_targ) {
			targ = mres;
		    } else if (rno.AttrExists(ENa_NS) && rno.Attr(ENa_NS).empty()) {
			if (rno.Attr(ENa_NS).empty()) {
			    root_ns.push_back(mres);
			    targ = this;
			} else {
			}
		    } else {
			targ = mres;
		    }
		    MutCtx mctx(this, root_ns);
		    if (targ) {
			targ->mutate(rno, aUpdOnly, mctx, true);
		    }
		}
	    } else if (rnotype == ENt_Change) {
		//ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode, mctx);
	    } else if (rnotype == ENt_Cont) {
		PFL_DUR_STAT_START(PEvents::EDurStat_MutCont);
		mutContent(rno, aUpdOnly, mctx);
		PFL_DUR_STAT_REC(PEvents::EDurStat_MutCont);
	    } else if (rnotype == ENt_Move) {
		//MoveNode(rno, aRunTime, aTrialMode, mctx);
	    } else if (rnotype == ENt_Import) {
		mutImport(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Rm) {
		//PFL_DUR_STAT_START(PEvents::EDurStat_MutRm);
		mutRemove(rno, aUpdOnly, mctx);
		//PFL_DUR_STAT_REC(PEvents::EDurStat_MutRm);
	    } else if (rnotype == ENt_Conn) {
		PFL_DUR_STAT_START(PEvents::EDurStat_MutConn);
		mutConnect(rno, aUpdOnly, mctx);
		PFL_DUR_STAT_REC(PEvents::EDurStat_MutConn);
	    } else if (rnotype == ENt_Disconn) {
		//PFL_DUR_STAT_START(PEvents::EDurStat_MutDsc);
		mutDisconnect(rno, aUpdOnly, mctx);
		//PFL_DUR_STAT_REC(PEvents::EDurStat_MutDsc);
	    } else if (rnotype == ENt_Note) {
		// Comment, just accept
	    } else {
		Logger()->Write(EErr, this, "Unknown mutation [%d]", rnotype);
	    }
	    Logger()->SetContextMutId();
	}
    }
}

bool Node::attachOwned(MNode* aOwned)
{
    bool res = owner()->connect(aOwned->owned());
    if (res) {
	aOwned->owned()->provided()->onOwnerAttached();
	onOwnedAttached(aOwned->owned()->provided());
	// Set owned log level
	/*
	MOwned* owd = aOwned->lIf(owd);
	assert(owd);
	auto oll = getOwdLogLevel();
	auto soll = getSOwdLogLevel();
	auto ll = (oll != ELcUndef) ? oll : soll;
	if (ll != ELcUndef) {
	    owd->setLogLevel(ll);
	}
	*/
    } else {
	Log(EErr, TLog(this) + "Attaching owned: already exists [" + aOwned->name() + "]");
    }
    return res;
}

MNode* Node::createHeir(const string& aName)
{
    PFL_DUR_STAT_START(PEvents::EDurStat_MutCrn);
    MNode* uheir = nullptr;
    if (Provider()->isProvided(this)) {
	uheir = Provider()->createNode(name(), aName, mEnv);
    } else {
	Log(EInfo, TLog(this) + "The parent of [" + aName + "] is not of provided");
    }
    PFL_DUR_STAT_REC(PEvents::EDurStat_MutCrn);
    return uheir;
}

void Node::setCtx(MOwner* aContext)
{
    assert(!mContext && aContext || !aContext);
    mContext = aContext;
}

MNode* Node::ownerGetNode(const GUri& aUri, const MNode* aReq) const
{
    MNode* res = nullptr;
    Node* self = const_cast<Node*>(this);
    if (aUri.isAbsolute()) {
       // Standard access, just absolute URI
       if (Owner()) {
           res = Owner()->ownerGetNode(aUri, aReq);
       } else {
           // Root
           if (aUri.size() > 2) {
               res = self->getComp(aUri.at(2));
	       bool foundReq = res == aReq;
               for (int i = 3; i < aUri.size() && res; i++) {
                   res = res->getComp(aUri.at(i));
		   foundReq |= (res == aReq);
               }
	       res = foundReq ? res : nullptr;
           }
       }
    }
    return res;
}


MNode* Node::getNodeS(const char* aUri)
{
    return getNode(string(aUri));
}

MNode* Node::mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string sparent = aMut.Attr(ENa_Parent);
    string sname = aMut.Attr(ENa_Id);
    /*
    TNs ns = aCtx.mNs;
    updateNs(ns, aMut);
    */
    bool res = false;
    //Log(EDbg2, TLog(this, aMut) + "Adding element [" + sname + "]");

    assert(!sname.empty());
    MNode* uelem = NULL;
    // Obtain parent first
    MNode *parent = NULL;
    // Check if the parent is specified
    if (!sparent.empty()) {
	// Check the parent scheme
	GUri prnturi(sparent);
	parent = getParent(prnturi);
	if (parent) {
	    // Create heir from the parent
	    uelem = parent->createHeir(sname);
	    if (uelem) {
		PFL_DUR_STAT_START(PEvents::EDurStat_MutAtt);
		attachOwned(uelem);
		parent->attachHeir(uelem);
		PFL_DUR_STAT_REC(PEvents::EDurStat_MutAtt);
	    } else {
		Log(EErr, TLog(this) + "Adding node [" + sname + "] failed");
	    }
	} else {
	    Logger()->Write(EErr, this, "Creating [%s] - parent [%s] not found", sname.c_str(), sparent.c_str());
	    parent = getParent(prnturi);
	}
    } else {
	Log(EErr, TLog(this) + "Missing parent name");
    }
    return uelem;
}

void Node::mutRemove(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string snode, sname;
    sname = aMut.Attr(ENa_Id);
    if (owner()->pairAt(sname)) {
	MOwned* owned = owner()->pairAt(sname)->provided();
	onOwnedDetached(owned); // TODO to rename to onOwnedTobeDetached
	owned->onOwnerDetached();
	bool res = owner()->disconnect(owner()->pairAt(sname));
	if (res) {
	    owned->deleteOwned();
	    Log(EInfo, TLog(this) + "Removed node [" + sname + "]");
	} else {
	    Log(EErr, TLog(this) + "Failed detached owned [" + sname + "]");
	}
    } else {
	Log(EErr, TLog(this) + "Failed removing [" + sname + "] - not found");
    }
}

void Node::mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string snode = aMut.Attr(ENa_MutNode);
    string sdata = aMut.Attr(ENa_MutVal);
    bool res = setContent(snode, sdata);
}

void Node::mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string sp = aMut.Attr(ENa_P);
    string sq = aMut.Attr(ENa_Q);
    Log(EErr, TLog(this) + "Connecting [" + sp + "] to [" + sq + "] - not supported");
}

void Node::mutDisconnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string sp = aMut.Attr(ENa_P);
    string sq = aMut.Attr(ENa_Q);
    Log(EErr, TLog(this) + "Disconnecting [" + sp + "] from [" + sq + "] - not supported");
}

void Node::mutImport(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    auto* impmgr = mEnv->ImpsMgr();
    assert(impmgr);
    string srcs = aMut.Attr(ENa_Id);
    bool res = impmgr->Import(srcs);
    if (!res) {
	Log(EErr, TLog(this) + "Importing [" + srcs + "] failed");
    }
}


void Node::notifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx)
{
    if (Owner() /* && aCtx.mNode */ && aCtx.mNode != this) {
	Owner()->onOwnedMutated(owned()->provided(), aMut, aCtx);
    }
}

void Node::onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx)
{
    // Node is not inheritable, so nothing to do.
    if (Owner() /* && aCtx.mNode */ && aCtx.mNode != this) {
	// Propagate to owner
	Owner()->onOwnedMutated(aOwned, aMut, aCtx);
    }
}

void Node::onOwnedAttached(MOwned* aOwned)
{
    // Notify the observers
    // Cache observers first to avoid iterating broked due to observers change
    list<MObserver*> cache;
    for (auto it = mOcp.pairsBegin(); it != mOcp.pairsEnd(); it++) {
	auto* pair = *it;
        cache.push_back(pair->provided());
    }
    for (auto obs : cache) {
        obs->onObsOwnedAttached(this, aOwned);
    }
}

void Node::onOwnedDetached(MOwned* aOwned)
{
    for (auto it = mOcp.pairsBegin(); it != mOcp.pairsEnd(); it++) {
        (*it)->provided()->onObsOwnedDetached(this, aOwned);
    }
}

#ifdef ENABLE_IFC
MIface* Node::MContentOwner_getLif(const char *aType) 
{
    MIface* res = nullptr;
    if (res = checkLif2<MContentOwner>(aType, mMContentOwnerPtr));
    return res;
}
#else
MIface* Node::MContentOwner_getLif(const char *aType) 
{
    MIface* res = nullptr;
    if (res = checkLif<MContentOwner>(aType));
    return res;
}
#endif



void Node::MContentOwner_doDump(int aLevel, int aIdt, ostream& aOs) const 
{
    for (int i = 0; i < contCount(); i++) {
	const MContent* cont = getCont(i);
	string data;
	bool res = cont->getData(data);
	Ifu::offset(aIdt, aOs); aOs << "- "  << cont->contName() << ": " << (res ? data : "nil") << endl;
	if (aLevel & Ifu::EDM_Recursive) {
	    const MContentOwner* cowner = cont->lIf(cowner);
	    if (cowner) {
		cowner->doDump(aLevel, aIdt + Ifu::KDumpIndent, aOs);
	    }
	}
    }
}

int Node::contCount() const 
{
    int res = 0;
    for (auto it = owner()->pairsCBegin(); it != owner()->pairsCEnd(); it++) {
	const MOwned* owned = (*it)->provided();
	const MContent* cont = owned->lIf(cont);
	if (cont) res++;
    }
    return res;
}

MContent* Node::getCont(int aIdx) 
{
    MContent* res = nullptr;
    int cnt = owner()->pcount();
    int ci = -1;
    for (int i = 0; i < cnt && ci != aIdx; i++) {
	MOwned* owned = owner()->pairAt(i)->provided();
	res = owned->lIf(res);
	if (res) ci++;
    }
    return res;
}

const MContent* Node::getCont(int aIdx) const
{
    const MContent* res = nullptr;
    int cnt = owner()->pcount();
    int ci = -1;
    for (int i = 0; i < cnt && ci != aIdx; i++) {
	const MOwned* owned = owner()->pairAt(i)->provided();
	res = owned->lIf(res);
	if (res) ci++;
    }
    return res;
}

bool Node::getContent(const GUri& aCuri, string& aRes) const
{
    bool res = false;
    const MNode* node = getNode(aCuri);
    const MContent* cnode = node ? node->lIf(cnode) : nullptr;
    if (cnode) {
	res = cnode->getData(aRes);
    }
    return res;
}

MContent* Node::createContent(const GUri& aUri)
{
    MContent* res = nullptr;
    MNode* cntn = this;
    bool pres = true;
    for (int i = 0; i < aUri.size() && cntn && pres; i++) {
	string cname = aUri.elems().at(i);
	MNode* nextcont = cntn->getComp(cname);
	if (!nextcont) {
	    nextcont = Provider()->createNode(K_ContentType, cname, mEnv);
	    pres = cntn->attachOwned(nextcont);
	}
	cntn = nextcont;
    }
    res = (pres && cntn) ? cntn->lIf(res) : nullptr;
    return res;
}

bool Node::setContent(const GUri& aCuri, const string& aData)
{
    bool res = false;
    MNode* node = getNode(aCuri);
    MContent* cnode = node ? node->lIf(cnode) : nullptr;
    if (!cnode) {
	cnode = createContent(aCuri);
    }
    if (cnode) {
	res = cnode->setData(aData);
    }
    if (!res) {
	Log(EErr, TLog(this) + "Setting [" + aCuri.toString() + "] to [" + aData + "]: content doesn't exist");
    }
    return res;

}

const MContent* Node::getCont(const GUri& aUri) const
{
    const MContent* res = nullptr;
    const MNode* node = getNode(aUri);
    res = node ? node->lIf(res) : nullptr;
    return res;
}

#ifdef ENABLE_IFC
MIface* Node::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif2<MNode>(aType, mMNodePtr));
    return res;
}
#else
MIface* Node::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MNode>(aType));
    else if (res = checkLif<MContent>(aType));
    return res;
}
#endif



bool Node::isOwner(const MOwner* mOwned) const
{
    assert(false);
    return false;
}


const MContentOwner* Node::cntOw() const
{
    return dynamic_cast<const MContentOwner*>(this);
}

MContentOwner* Node::cntOw()
{
    return dynamic_cast<MContentOwner*>(this);
}

bool Node::addObserver(MObserver::TCp* aObs)
{
    bool res = mOcp.connect(aObs);
    return res;
}

bool Node::rmObserver(MObserver::TCp* aObs)
{
    bool res = mOcp.disconnect(aObs);
    return res;
}

void Node::getModules(vector<MNode*>& aModules)
{
    MNode* mdl = getNode(mEnv->modulesUri());
    if (mdl) {
	aModules.push_back(mdl);
    }
    MOwner* owner = mContext ? mContext : Owner();
    if (owner) {
	owner->getModules(aModules);
    }
}

MNode* Node::getParent(const GUri& aUri)
{
    MNode* res = getNode(aUri);
    if (!res) {
	MOwner* owner = mContext ? mContext : Owner();
	if (owner) {
	    //res =owner->getParent(aUri);
	    res = mContext ?  mContext->getParent(aUri) : nullptr;
	    if (!res) {
		res = Owner() ?  Owner()->getParent(aUri) : nullptr;
	    }
	} else{
	    MNode* mdl = getNode(mEnv->modulesUri());
	    res = mdl ? mdl->getNode(aUri) : nullptr;
	}
    }
    return res;
}

bool Node::isOwned(const MOwned* mOwned) const
{
    bool res = false;
    const MNode*  owndn = mOwned->lIf(owndn);
    res = owner()->isConnected(const_cast<MNode*>(owndn)->owned());
    return res;
}

#ifdef ENABLE_IFC
MIface* Node::MOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    // TODO Vulnerabilty, consider to configure the access
    if (res = checkLif2<MObservable>(aType, mMObservablePtr));
    // TODO to introduce specific iface for explorable, ref ds_dcs_aes_acp
    else if (res = mExplorable ? checkLif2<MNode>(aType, mMNodePtr) : nullptr);
    else if (res = mControllable ? checkLif2<MNode>(aType, mMNodePtr) : nullptr);
    else if (res = checkLif2<MContentOwner>(aType, mMContentOwnerPtr));
    return res;
}
#else
MIface* Node::MOwner_getLif(const char *aType)
{
    MIface* res = nullptr;
    // TODO Vulnerabilty, consider to configure the access
    if (res = checkLif<MObservable>(aType));
    // TODO to introduce specific iface for explorable, ref ds_dcs_aes_acp
    else if (res = mExplorable ? checkLif<MNode>(aType) : nullptr);
    else if (res = mControllable ? checkLif<MNode>(aType) : nullptr);
    else if (res = checkLif<MContentOwner>(aType));
    return res;
}
#endif


bool Node::isNodeOwned(const MNode* aNode) const
{
    bool res = false;
    for (auto it = owner()->pairsCBegin(); it != owner()->pairsCEnd() && !res; it++) {
	auto* comp = (*it)->provided();
	const MNode* compn = comp->lIf(compn);
	res = (aNode == compn);
    }
    return res;
}

void Node::notifyChanged()
{
    // Notify observers
    for (auto it = mOcp.pairsBegin(); it != mOcp.pairsEnd(); it++) {
	(*it)->provided()->onObsChanged(this);
    }
}

int Node::parseLogLevel(const string& aData)
{
    int res = -1;
    int extl = -1;
    string::size_type pos = aData.find_first_of(K_LogLevelSep);
    bool isExt = (pos != string::npos);
    string name = aData.substr(0, pos);
    string ext = aData.substr(pos + 1);
    if (name == K_LogLevel_Err) res = EErr;
    else if (name == K_LogLevel_Warn) res = EWarn;
    else if (name == K_LogLevel_Info) res = EInfo;
    else if (name == K_LogLevel_Dbg) res = EDbg;
    else if (name == K_LogLevel_Dbg2) res = EDbg2;
    if (res != -1) {
	if (isExt) {
	    try {
		extl = stoi(ext);
	    } catch (std::exception& e) { }
	    if (extl != -1) res += extl;
	    else {
		Log(EErr, TLog(this) + "Wrong log level extension [" + ext + "]");
	    }
	}
    } else {
	Log(EErr, TLog(this) + "Unknown log level [" + name + "]");
    }
    return res;
}

void Node::onContentChanged(const MContent* aCont)
{
    bool res = true;
    string data;
    // Cache logging level
    if (aCont->contName() == K_Cont_LogLevel) {
	if (res = aCont->getData(data)) {
	    auto ll = parseLogLevel(data);
	    setLocLogLevel(ll);
	}
    } else if (aCont->contName() == K_Cont_OwdLogLevel) {
	if (res = aCont->getData(data)) {
	    auto oll = parseLogLevel(data);
	    setSOwdLogLevel(oll);
	    // Notify owners of log level updated
	    for (int i = 0; i < owner()->pcount(); i++) {
		auto owd = owner()->pairAt(0);
		owd->provided()->setLogLevel(oll);
	    }
	}

    } else if (aCont->contName() == K_Cont_Explorable) {
	if (res = aCont->getData(data)) {
	    mExplorable = (data == K_Cont_Val_Yes);
	}
    } else if (aCont->contName() == K_Cont_Controllable) {
	if (res = aCont->getData(data)) {
	    mControllable = (data == K_Cont_Val_Yes);
	}
    }

    // Notify observers
    for (auto it = mOcp.pairsBegin(); it != mOcp.pairsEnd(); it++) {
        (*it)->provided()->onObsContentChanged(this, aCont);
    }
}

void Node::setOwdLogLevel(int aLevel)
{
    mLogLevel &= ~(K_Ll_Mask << K_Oll_Shift);
    mLogLevel |= ((aLevel & K_Ll_Mask) << K_Oll_Shift);
}

void Node::setSOwdLogLevel(int aLevel)
{
    mLogLevel &= ~(K_Ll_Mask << K_SOll_Shift);
    mLogLevel |= ((aLevel & K_Ll_Mask) << K_SOll_Shift);
}

void Node::setLocLogLevel(int aLevel)
{
    mLogLevel &= ~K_Ll_Mask;
    mLogLevel |= (aLevel & K_Ll_Mask);
}


bool Node::setLogLevel(int aLevel)
{
    setOwdLogLevel(aLevel);
    // Propagate to owned
    for (int i = 0; i < owner()->pcount(); i++) {
	auto owd = owner()->pairAt(i);
	owd->provided()->setLogLevel(aLevel);
    }
    return true;
}


//!! not tested
MNode* Node::addComp(const string& aType, const string& aName)
{
    MNode* node = Provider()->createNode(aType, aName, mEnv);
    assert(node);
    bool res = attachOwned(node);
    assert(res);
    return node;
}
