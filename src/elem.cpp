

#include "elem.h"
#include "chromo.h"



Elem::Elem(const string &aName, MEnv* aEnv): Unit(aName, aEnv)
{
    mChromo = mEnv->provider()->createChromo();
    mChromo->Init(ENt_Node);
    setCrAttr(Type(), aName);
}

Elem::~Elem()
{
}

MIface* Elem::MElem_getLif(const char *aType)
{
    return nullptr;
}

void Elem::MElem_doDump(int aLevel, int aIdt, ostream& aOs) const
{
}

MIface* Elem::MNode_getLif(const char *aType)
{
    MIface* res = nullptr;
    if (res = checkLif<MElem>(aType));
    else Node::MNode_getLif(aType);
    return res;
}


void Elem::mutate(const ChromoNode& aMut, bool aUpdOnly /*false*/, const MutCtx& aCtx)
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
	MElem* eftarg = this; // Mutable target
	MNode* aoftarg = NULL; // Attached owner of target
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
	    eftarg = ftarg->lIf(eftarg);
	    aoftarg = ftarg;
	    if (!eftarg) {
		// Target is not mutable, redirect to mutable owner
		// TODO Should the mut be redirected to attached owner but not just mutable?
		aoftarg = getMowner(ftarg);
		eftarg = aoftarg ? aoftarg->lIf(eftarg) : nullptr;
	    }
	    if (eftarg) {
		if (ftarg != aoftarg) {
		    string newTargUri = ftarg->getUriS(aoftarg);
		    rno.SetAttr(ENa_Targ, newTargUri);
		} else {
		    rno.RmAttr(ENa_Targ);
		}
		if (rno.AttrExists(ENa_MutNode)) {
		    rno.RmAttr(ENa_MutNode);
		}
		if (!rno.AttrExists(ENa_Targ)) {
		    // Correcting target if the mut is component related
		    TNodeType mtype = rno.Type();
		    if (mtype == ENt_Change || mtype == ENt_Rm) {
			// Mutation is for component only, find the comp mutable owner
			aoftarg = getMowner(ftarg);
			eftarg = aoftarg->lIf(eftarg);
			if (ftarg != aoftarg) {
			    string newTargUri = ftarg->getUriS(aoftarg);
			    rno.SetAttr(ENa_Targ, newTargUri);
			}
		    }
		}
	    } else {
		string ftarg_uri = ftarg->getUriS(NULL);
		Logger()->Write(EErr, this, "Cannot find mutable target for [%s]", ftarg_uri.c_str());
	    }
	} else {
	    // Local mutation
	    rno.RmAttr(ENa_Targ);
	}
	if (eftarg != this) {
	    // Redirect the mut to target: no run-time to keep the mut in internal nodes
	    // Propagate till target owning comp if run-time to keep hidden all muts from parent
	    /*
	    ChromoNode madd = eftarg->AppendMutation(rno);
	    MutCtx mctx(aRunTime ? GetCompOwning(ftarg) : aCtx.mUnit, root_ns);
	    eftarg->Mutate(false, aCheckSafety, aTrialMode, mctx);
	    */
	} else {
	    // Local mutation
	    MutCtx mctx(aCtx.mNode, root_ns);
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Node) {
		MNode* mres = mutAddElem(rno, aUpdOnly, mctx);
		if (rno.Count() > 0) {
		    MElem* emres = mres->lIf(emres);
		    if (!emres) {
			// There is node chromo but node is not mutable. Mutate it from the current mutable.
			string targUri = mres->getUriS(this);
			rno.SetAttr(ENa_Targ, targUri);
			mutate(rno, false, mctx);
		    }
		}
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

// TODO Roughtly implemeted, re-implement

MNode* Elem::getMowner(MNode* aNode)
{
    /*
    assert(aNode);
    MElem* eowner = NULL;
    MNode* owner = aNode->GetMan();
    while (owner != NULL) {
	eowner = owner->GetObj(eowner);
	if (eowner != NULL) {
	    break;
	}
	owner = owner->GetMan();
    }
    return owner;
    */
    return nullptr;
}

MNode* Elem::mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
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
		    }
		    MElem* elem = uelem ? uelem->lIf(elem) : nullptr;
		    if (elem) {
			// TODO [YB] Seems to be just temporal solution. To consider using context instead.
			// Make heir based on the parent: re-parent the heir (currently it's of grandparent's parent) and clean the chromo
			ChromoNode croot = elem->Chromos().Root();
			croot.SetAttr(ENa_Parent, sparent);
			if (!snode.empty()) {
			    croot.SetAttr(ENa_MutNode, snode);
			}
			if (!aMut.Attr(ENa_NS).empty()) {
			    croot.SetAttr(ENa_NS, aMut.Attr(ENa_NS));
			}
			elem->setParent(NULL);
			MElem* eparent = parent ? parent->lIf(eparent) : nullptr;
			res = eparent  ? eparent->appendChild(uelem) : false;
			if (res) {
			    if (!aUpdOnly) {
				// Copy just top node, not recursivelly, ref ds_daa_chrc_va
				ChromoNode ech = elem->Chromos().Root();
				ChromoNode chn = mChromo->Root().AddChild(ech, true, false);
				notifyNodeMutated(ech, aCtx);
				mutadded = true;
			    } else {
				mutadded = true;
			    }
			    // Mutate object ignoring run-time option. This is required to keep nodes chromo even for inherited nodes
			    // To avoid this inherited nodes chromo being attached we just don't attach inherited nodes chromo
			    TNs ns(aCtx.mNs);
			    ns.push_back(uelem);
			    MutCtx ctx(aCtx.mNode, ns);
			    uelem->mutate(aMut, false, aUpdOnly ? MutCtx(uelem, ns) : ctx);
			}
			else {
			    Log(TLog(EErr, this) + "Adding node [" + uelem->name() + "] failed");
			    delete elem;
			}
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
	ChromoNode chn = mChromo->Root().AddChild(aMut, true, false);
	notifyNodeMutated(chn, aCtx);
    }
    return uelem;
}

MNode* Elem::createHeir(const string& aName, MNode* aContext)
{
    return nullptr;
}

void Elem::setParent(const string& aParent)
{
}

bool Elem::appendChild(MNode* aChild)
{
    bool res = false;
    return res;
}

void Elem::notifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx)
{
}

void Elem::notifyParentMutated(const TMut& aMut)
{
}

void Elem::setCrAttr(const string& aEType, const string& aName)
{
    ChromoNode croot = mChromo->Root();
    string ptype;
    if (aName.empty()) {
	// Native base agent, its name is cpp class type
	mName = aEType;
    } else {
	// Inherited native agent, its name is given, type is class extended type
	mName = aName;
	// Using short type for parent to be compatible with current version
	// Needs to consider to use full type
	ptype = aEType;
    }
    croot.SetAttr(ENa_Id, mName);
    croot.SetAttr(ENa_Parent, ptype);
}


