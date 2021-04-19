
#include "node.h"
#include "chromo.h"

bool Node::NCpOwned::isOwner(const MOwner* aOwner) const
{
    bool res = false;
    if (aOwner == at()) {
	res = true;
    } else if (at() && at()->bindedOwned()) {
	res = at()->bindedOwned()->isOwner(aOwner);
    }
    return res;
}



Node::Node(const string &aName, MEnv* aEnv): mName(aName.empty() ? Type() : aName), mEnv(aEnv)
{
} 

Node::~Node()
{
}

MIface* Node::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MNode>(aType));
    else if (res = checkLif<MContentOwner>(aType));
    return res;
}

void Node::MNode_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, aOs); aOs << "Name: " << mName << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	for (int i = 0; i < mCpOwner.pcount(); i++) {
	    const MOwned* comp = mCpOwner.pairAt(i);
	    const MNode* compn = comp->lIf(compn);
	    Ifu::offset(aIdt, aOs); aOs << "- "  << compn->name() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		compn->doDump(Ifu::EDM_Comps | Ifu::EDM_Recursive, aIdt + Ifu::KDumpIndent, aOs);
	    }
	}
    }
}


const MNode* Node::getComp(const string& aId) const
{
    const MOwned* ores = mCpOwner.at(aId);
    const MNode* res = ores ? ores->lIf(res) : nullptr;
    return res;
}

const MNode* Node::getNode(const GUri& aUri) const
{
    const MNode* res = nullptr;
    if (aUri.isAbsolute()) {
	//Owner()->getNode(aUri, this);
	if (Owner()) {
	    res = Owner()->getNode(aUri, this); 
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
	    res = mEnv->provider()->getNode(aUri.at(0));
	}
    }
    return res;
}

void Node::deleteOwned()
{
    for (auto item : mCpOwner.mPairs) {
	item.second->deleteOwned();
    }
}

void Node::getUri(GUri& aUri, MNode* aBase) const
{
    if (mEnv && mEnv->provider() && mEnv->provider()->isProvided(this)) {
	// Provided
	aUri.appendElem(mName);
    } else if (aBase != this) {
	// From native hier
	const MOwner* owner = Owner();
	if (owner) {
	    owner->getUri(aUri, aBase);
	} else {
	    aUri.appendElem(string());
	}
	aUri.appendElem(mName);
    }
}

MOwner* Node::Owner()
{
    return mCpOwned.at();
}

const MOwner* Node::Owner() const
{
    return mCpOwned.at();
}

void Node::updateNs(TNs& aNs, const ChromoNode& aCnode)
{
    if (aCnode.AttrExists(ENa_NS)) {
	string ns = aCnode.Attr(ENa_NS);
	MNode* nsu = getNode(ns, aNs);
	if (nsu == NULL) {
	    Log(TLog(EErr, this) + "Cannot find namespace [" + ns + "]");
	} else {
	    aNs.clear(); // Override namespace by explicitly stated one
	    aNs.push_back(nsu);
	}
    }
}


MNode* Node::getNode(const string& aName, const TNs& aNs)
{
    assert(!aName.empty());
    MNode *res = nullptr;
    GUri uri(aName);
    // Resolving name in current context/native first
    res = getNode(uri);
    if (!res/* && uri.isName()*/) {
	// Then in namespaces
	// Applied namespaces priority approach, ref I_NRC
	for (auto nsit = aNs.rbegin(); nsit != aNs.rend() && !res; nsit++) {
	    auto ns = *nsit;
	    if (ns == this) continue;
	    res = ns->getNode(uri);
	}
    }
    return res;
}

void Node::mutSegment(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    bool res = true;
    TNs root_ns; // Don't propagate context from upper layer, ref ds_cli_sno_s3
    updateNs(root_ns, aMut);

    //TODO !! This handling of target isn't needed. This is done in mutate.
    MNode* targ = this; // Base target
    string starg;
    if (aMut.AttrExists(ENa_Targ) /* && (aMut.Type() != ENt_Node) */) {
	starg = aMut.Attr(ENa_Targ);
	targ = getNode(starg, root_ns);
	if (!targ) {
	    targ = getNode(starg, root_ns);
	    Log(TLog(EErr, this) + "Cannot find target node [" + starg + "]");
	    res = false;
	}
    }
    for (ChromoNode::Const_Iterator rit = aMut.Begin(); rit != aMut.End() && res; rit++)
    {
	ChromoNode rno = (*rit);
	//MutCtx mctx(aUpdOnly ? targ : aCtx.mNode, root_ns, aCtx.mParent);
	MutCtx mctx(this, root_ns);
	targ->mutate(rno, false, mctx);
    }
}

void Node::mutate(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx, bool aTreatAsChromo)
{
    bool res = true;
    TNs root_ns = aCtx.mNs;
    updateNs(root_ns, aMut);

    ChromoNode rno = aMut;
    Logger()->SetContextMutId(rno.LineId());
    // Get target node by analysis of mut-target and mut-node, ref ds_chr2_cctx_tn_umt
    MNode* targ = this;
    bool exs_targ = rno.AttrExists(ENa_Targ);
    bool exs_mnode = rno.AttrExists(ENa_MutNode);
    string starg, smnode;
    // Set namespace to mut node
    if (aMut.AttrExists(ENa_NS)) {
	if (!rno.AttrExists(ENa_NS)) {
	    rno.SetAttr(ENa_NS, aMut.Attr(ENa_NS));
	} else {
	    // TODO Support multiple namespaces, ref ds_chr2_ns_insmns 
	    // Currently the only last namespaces is active
	    //	assert(false);
	}
    }
    if (exs_targ) {
	starg = rno.Attr(ENa_Targ);
	targ = targ->getNode(starg, root_ns);
	if (!targ) {
	    Logger()->Write(EErr, this, "Cannot find target node [%s]", starg.c_str());
	    res = false;
	}
    }
    if (res && exs_mnode) {
	// Transform DHC mutation to OSM mutation
	// Transform ENa_Targ: enlarge to ENa_MutNode
	smnode = rno.Attr(ENa_MutNode);
	targ = targ->getNode(smnode, root_ns);
	if (!targ) {
	    Logger()->Write(EErr, this, "Cannot find mut node [%s]", smnode.c_str());
	    res = false;
	}
    }
    if (res) {
	if (targ != this) {
	    // Targeted mutation
	    rno.RmAttr(ENa_Targ);
	    if (rno.AttrExists(ENa_MutNode)) {
		rno.RmAttr(ENa_MutNode);
	    }
	    // Correcting target if the mut is component related
	    TNodeType mtype = rno.Type();
	    if (mtype == ENt_Change || mtype == ENt_Rm) {
		// Mutation is for component only, find the comp mutable owner
		/*
		   aoftarg = getMowner(targ);
		   eftarg = aoftarg->lIf(eftarg);
		   if (targ != aoftarg) {
		   string newTargUri = targ->getUriS(aoftarg);
		   rno.SetAttr(ENa_Targ, newTargUri);
		   }
		   */
	    }
	    // Redirect the mut to target: no run-time to keep the mut in internal nodes
	    // Propagate till target owning comp if run-time to keep hidden all muts from parent
	    MutCtx mctx(aUpdOnly ?targ : aCtx.mNode, root_ns);
	    targ->mutate(rno, false, mctx);
	} else  {
	    // Local mutation
	    rno.RmAttr(ENa_Targ);
	    MutCtx mctx(aCtx.mNode, root_ns);
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Seg || aTreatAsChromo) {
		mutSegment(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Node) {
		MNode* mres = mutAddElem(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Change) {
		//ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode, mctx);
	    } else if (rnotype == ENt_Cont) {
		mutContent(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Move) {
		//MoveNode(rno, aRunTime, aTrialMode, mctx);
	    } else if (rnotype == ENt_Import) {
		mutImport(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Rm) {
		mutRemove(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Conn) {
		mutConnect(rno, aUpdOnly, mctx);
	    } else if (rnotype == ENt_Note) {
		// Comment, just accept
		/*
		   iChromo->Root().AddChild(rno);
		   NotifyNodeMutated(rno, mctx);
		   */
	    } else {
		//DoSpecificMut(rno, aRunTime, aTrialMode, mctx);
	    }
	    Logger()->SetContextMutId();
	}
    }
}

bool Node::attachOwned(MNode* aOwned)
{
    bool res = owner()->connect(aOwned->owned());
    if (res) {
	onOwnedAttached(aOwned->owned());
    }
    return res;
}

MNode* Node::createHeir(const string& aName)
{
    MNode* uheir = nullptr;
    if (Provider()->isProvided(this)) {
	uheir = Provider()->createNode(name(), aName, mEnv);
    } else {
	Log(TLog(EInfo, this) + "The parent [" + aName + "] is not of provided");
    }
    return uheir;
}

void Node::setCtx(MOwner* aContext)
{
    assert(!mContext && aContext || !aContext);
    mContext = aContext;
}

MNode* Node::getNode(const GUri& aUri, const MNode* aOwned) const
{
    MNode* res = nullptr;
    Node* self = const_cast<Node*>(this);
    if (aUri.isAbsolute()) {
	if (Owner()) {
	    res = Owner()->getNode(aUri, aOwned); 
	} else {
	    // Root
	    if (aUri.size() > 2) {
		res = self->getComp(aUri.at(2));
		for (int i = 3; i < aUri.size() && res; i++) {
		    res = res->getComp(aUri.at(i));
		}
	    }
	}
	if (res) {
	    // Blocking the result if it is not owned by requestor
	    if (!res->owned()->isOwner(aOwned->owner())) {
		res = nullptr;
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
    assert(!aMut.AttrExists(ENa_Comp));
    string snode = aMut.Attr(ENa_Targ);
    string sparent = aMut.Attr(ENa_Parent);
    string sname = aMut.Name();
    TNs ns = aCtx.mNs;
    updateNs(ns, aMut);
    bool mutadded = false;
    bool res = false;
    Log(TLog(EInfo, this) + "Adding element [" + sname + "]");

    assert(!sname.empty());
    MNode* uelem = NULL;
    MNode* node = snode.empty() ? this: getNode(snode, ns);
    if (node) {
	// Obtain parent first
	MNode *parent = NULL;
	// Check if the parent is specified
	if (!sparent.empty()) {
	    // Check the parent scheme
	    GUri prnturi(sparent);
	    // Resolving parent ref basing on target, ref ds_umt_rtnsu_rbs
	    TNs pns = ns; // Parent namaspace
	    getModules(pns);
	    parent = node->getNode(prnturi, pns);
	    if (!parent) {
		// Probably external node not imported yet - ask env for resolving uri
		/*!!
		  GUri pruri(prnturi);
		  MImportMgr* impmgr = iEnv->ImpsMgr();
		  parent = impmgr->OnUriNotResolved(node, pruri);
		  */
	    }
	    if (parent) {
		if (node == this) {
		    // Create heir from the parent
		    uelem = parent->createHeir(sname);
		    if (uelem) {
			node->attachOwned(uelem);
			notifyNodeMutated(aMut, aCtx);
			mutadded = true;
			// Mutate object ignoring run-time option. This is required to keep nodes chromo even for inherited nodes
			// To avoid this inherited nodes chromo being attached we just don't attach inherited nodes chromo
			if (aMut.Count() > 0) {
			    TNs ns(aCtx.mNs);
			    //ns.push_back(uelem);
			    MutCtx ctx(aCtx.mNode, ns);
			    uelem->mutate(aMut, false, aUpdOnly ? MutCtx(uelem, ns) : ctx, true);
			}
			mutAddElemOnCreated(uelem, parent);
		    } else {
			Log(TLog(EErr, this) + "Adding node [" + sname + "] failed");
		    }
		} else  {
		    Log(TLog(EErr, this) + "Adding element [" + sname + "] in node {" + snode + "] - disabled");
		}
	    } else {
		Logger()->Write(EErr, this, "Creating [%s] - parent [%s] not found", sname.c_str(), sparent.c_str());
	    }
	}
    } else  {
	Logger()->Write(EErr, this, "Creating elem [%s] in node [%s] - cannot find node", sname.c_str(), snode.c_str());
    }
    if (!aUpdOnly && !mutadded) {
	notifyNodeMutated(aMut, aCtx);
    }
    return uelem;
}

void Node::mutRemove(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string snode, sname;
    MNode* targ = this;
    if (aMut.AttrExists(ENa_Targ)) {
	snode = aMut.Attr(ENa_Targ);
	targ = getNode(snode);
    }
    sname = aMut.Attr(ENa_Id);
    if (targ) {
	MOwned* owned = owner()->at(sname);
	owner()->detach(owner()->at(sname));
	owned->deleteOwned();
	//node->SetRemoved(aRunTime);
	Log(TLog(EInfo, this) + "Removed node [" + sname + "]");
    } else {
	Log(TLog(EErr, this) + "Removing node [" + sname + "] - target node found");
    }
    if (!aUpdOnly) {
	notifyNodeMutated(aMut, aCtx);
    }
}

void Node::mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string snode = aMut.Attr(ENa_MutNode);
    string sdata = aMut.Attr(ENa_MutVal);
    bool res = setContent(snode, sdata);
    if (!res) {
	Log(TLog(EErr, this) + "Failed changing content of [" + snode + "]");
    }
    if (!aUpdOnly) {
	notifyNodeMutated(aMut, aCtx);
    }
}

void Node::mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    string sp = aMut.Attr(ENa_P);
    string sq = aMut.Attr(ENa_Q);
    Log(TLog(EErr, this) + "Connecting [" + sp + "] to [" + sq + "] - not supported");
    if (!aUpdOnly) {
	notifyNodeMutated(aMut, aCtx);
    }
}

void Node::mutImport(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    auto* impmgr = mEnv->ImpsMgr();
    assert(impmgr);
    string srcs = aMut.Attr(ENa_Id);
    bool res = impmgr->Import(srcs);
    if (!res) {
	Log(TLog(EErr, this) + "Importing [" + srcs + "] failed");
    }
    if (!aUpdOnly) {
	notifyNodeMutated(aMut, aCtx);
    }
}


void Node::notifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx)
{
    if (Owner() && aCtx.mNode && aCtx.mNode != this) {
	Owner()->onOwnedMutated(owned(), aMut, aCtx);
    }
}

void Node::onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx)
{
    // Node is not inheritable, so nothing to do.
    if (Owner() && aCtx.mNode && aCtx.mNode != this) {
	// Propagate to owner
	Owner()->onOwnedMutated(aOwned, aMut, aCtx);
    }
}

void Node::onOwnedAttached(MOwned* aOwned)
{
}

MIface* Node::MContentOwner_getLif(const char *aType) 
{
    MIface* res = nullptr;
    if (res = checkLif<MContentOwner>(aType));
    return res;
}

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
    int cnt = owner()->pcount();
    for (int i = 0; i < cnt; i++) {
	const MOwned* owned = owner()->pairAt(i);
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
	MOwned* owned = owner()->pairAt(i);
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
	const MOwned* owned = owner()->pairAt(i);
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

bool Node::setContent(const GUri& aCuri, const string& aData)
{
    bool res = false;
    MNode* node = getNode(aCuri);
    MContent* cnode = node ? node->lIf(cnode) : nullptr;
    if (cnode) {
	res = cnode->setData(aData);
    }
    return res;

}

MIface* Node::MOwned_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MNode>(aType));
    else if (res = checkLif<MContent>(aType));
    return res;
}


const MContentOwner* Node::cntOw() const
{
    return dynamic_cast<const MContentOwner*>(this);
}

MContentOwner* Node::cntOw()
{
    return dynamic_cast<MContentOwner*>(this);
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
