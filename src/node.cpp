
#include "node.h"
#include "chromo.h"

void offset(int aIndent, ostream& aOs)
{
    for (int i = 0; i < aIndent; i++)  aOs << " ";
}


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
    return res;
}

void Node::MNode_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & EDM_Base) {
	offset(aIdt, aOs); aOs << "Name: " << mName << endl;
    }
    if (aLevel & EDM_Comps) {
	for (int i = 0; i < mCpOwner.pcount(); i++) {
	    const MOwned* comp = mCpOwner.pairAt(i);
	    const MNode* compn = comp->lIf(compn);
	    offset(aIdt, aOs); aOs << "- "  << compn->name() << endl;
	    if (aLevel & EDM_Recursive) {
		compn->doDump(EDM_Comps | EDM_Recursive, aIdt + Ifu::KDumpIndent, aOs);
	    }
	}
    }
}


MNode* Node::getComp(const string& aId)
{
    MOwned* ores = mCpOwner.at(aId);
    MNode* res = ores ? ores->lIf(res) : nullptr;
    return res;
}

MNode* Node::getNode(const GUri& aUri)
{
    MNode* res = nullptr;
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
    if (!res && uri.isName()) {
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

void Node::mutate(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    bool res = true;
    TNs root_ns = aCtx.mNs;
    updateNs(root_ns, aMut); //!!

    MNode* btarg = this; // Base target
    string sbtarg;
    if (aMut.AttrExists(ENa_Targ) /* && (aMut.Type() != ENt_Node) */) {
	sbtarg = aMut.Attr(ENa_Targ);
	btarg = getNode(sbtarg, root_ns);
	if (btarg == NULL) {
	    btarg = getNode(sbtarg, root_ns);
	    Logger()->Write(EErr, this, "Cannot find base target node [%s]", sbtarg.c_str());
	    res = false;
	}
    }

    for (ChromoNode::Const_Iterator rit = aMut.Begin(); rit != aMut.End() && res; rit++)
    {
	ChromoNode rno = (*rit);
	Logger()->SetContextMutId(rno.LineId());
	// Get target node by analysis of mut-target and mut-node, ref ds_chr2_cctx_tn_umt
	MNode* ftarg = btarg;
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
	    ftarg = btarg->getNode(starg, root_ns);
	    if (!ftarg) {
		Logger()->Write(EErr, this, "Cannot find target node [%s]", starg.c_str());
		continue;
	    }
	}
	if (exs_mnode) {
	    // Transform DHC mutation to OSM mutation
	    // Transform ENa_Targ: enlarge to ENa_MutNode
	    smnode = rno.Attr(ENa_MutNode);
	    ftarg = ftarg->getNode(smnode, root_ns);
	    if (!ftarg) {
		Logger()->Write(EErr, this, "Cannot find mut node [%s]", smnode.c_str());
		continue;
	    }
	}
	if (ftarg != this) {
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
		aoftarg = getMowner(ftarg);
		eftarg = aoftarg->lIf(eftarg);
		if (ftarg != aoftarg) {
		    string newTargUri = ftarg->getUriS(aoftarg);
		    rno.SetAttr(ENa_Targ, newTargUri);
		}
		*/
	    }
	    // Redirect the mut to target: no run-time to keep the mut in internal nodes
	    // Propagate till target owning comp if run-time to keep hidden all muts from parent
	    MutCtx mctx(aUpdOnly ?ftarg : aCtx.mNode, root_ns);
	    ftarg->mutate(rno, false, mctx);
	} else {
	    // Local mutation
	    rno.RmAttr(ENa_Targ);
	    MutCtx mctx(aCtx.mNode, root_ns);
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Node) {
		MNode* mres = mutAddElem(rno, aUpdOnly, mctx);
	    }
	    else if (rnotype == ENt_Seg) {
		mutate(rno, aUpdOnly, mctx);
	    }
	    else if (rnotype == ENt_Change) {
		//ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode, mctx);
	    }
	    else if (rnotype == ENt_Cont) {
		//DoMutChangeCont(rno, aRunTime, aCheckSafety, aTrialMode, mctx);
	    }
	    else if (rnotype == ENt_Move) {
		//MoveNode(rno, aRunTime, aTrialMode, mctx);
	    }
	    else if (rnotype == ENt_Import) {
		//ImportNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Rm) {
		//RmNode(rno, aRunTime, aCheckSafety, aTrialMode, mctx);
	    }
	    else if (rnotype == ENt_Note) {
		// Comment, just accept
		/*
		iChromo->Root().AddChild(rno);
		NotifyNodeMutated(rno, mctx);
		*/
	    }
	    else {
		//DoSpecificMut(rno, aRunTime, aTrialMode, mctx);
	    }
	    Logger()->SetContextMutId();
	}
    }
}

bool Node::attachOwned(MNode* aOwned)
{
    bool res = owner()->connect(aOwned->owned());
    return res;
}

MNode* Node::createHeir(const string& aName, MNode* aContext)
{
    MNode* uheir = NULL;
    if (Provider()->isProvided(this)) {
	uheir = Provider()->createNode(name(), aName, mEnv);
	uheir->setCtx(aContext);
    } else {
	Logger()->Write(EErr, this, "The parent is not provided: [%s]: -->", aName.c_str());
    }
    return uheir;
}


void Node::setCtx(MNode* aContext)
{
    assert(!mContext && aContext || !aContext);
    mContext = aContext;
}

// TODO Remove?
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
	    //if (!aOwned->owned()->isOwner(res->owner())) {
	    if (!res->owned()->isOwner(aOwned->owner())) {
		res = nullptr;
	    }
	}
    } 
    return res;
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
	    parent = node->getNode(prnturi, ns);
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
		    uelem = parent->createHeir(sname, NULL);
		    if (uelem) {
			node->attachOwned(uelem);
			notifyNodeMutated(aMut, aCtx);
			mutadded = true;
			// Mutate object ignoring run-time option. This is required to keep nodes chromo even for inherited nodes
			// To avoid this inherited nodes chromo being attached we just don't attach inherited nodes chromo
			TNs ns(aCtx.mNs);
			ns.push_back(uelem);
			MutCtx ctx(aCtx.mNode, ns);
			uelem->mutate(aMut, false, aUpdOnly ? MutCtx(uelem, ns) : ctx);
		    } else {
			Log(TLog(EErr, this) + "Adding node [" + uelem->name() + "] failed");
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
	Owner()->onOwnedMutated(owned(), aMut, aCtx);
    }
}

