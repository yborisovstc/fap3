

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


void Elem::mutate(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx, bool aTreatAsChromo)
{
    Node::mutate(aMut, aUpdOnly, aCtx, aTreatAsChromo);
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

MNode* Elem::mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    if (!aUpdOnly) {
	// Copy just top node, not recursivelly, ref ds_daa_chrc_va
	mChromo->Root().AddChild(aMut, true, false);
    }
    MNode* elem = Node::mutAddElem(aMut, aUpdOnly, aCtx);
    return elem;
}

void Elem::mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    if (!aUpdOnly) {
	mChromo->Root().AddChild(aMut, true, false);
    }
    Node::mutContent(aMut, aUpdOnly, aCtx);
}

#if 0
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
#endif

void Elem::onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx)
{
    ChromoNode anode = mChromo->Root().AddChild(aMut);
    const MNode* onode = aOwned->lIf(onode);
    GUri nuri;
    onode->getUri(nuri, this);
    if (anode.AttrExists(ENa_Targ)) {
	string starg = anode.Attr(ENa_Targ);
	nuri.append(starg);
    }
    anode.SetAttr(ENa_Targ, nuri);

    Node::onOwnedMutated(aOwned, aMut, aCtx);
}

