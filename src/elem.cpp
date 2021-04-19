

#include "elem.h"
#include "chromo.h"



Elem::Elem(const string &aName, MEnv* aEnv): Unit(aName, aEnv), mInode(this, this)
{
    if (aName.empty()) mName = Type();
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
    else if (res = checkLif<MParent>(aType));
    else res = Unit::MNode_getLif(aType);
    return res;
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


void Elem::setParent(const string& aParent)
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

void Elem::mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo)
{
    if (!aChange && !aTreatAsChromo) {
	ChromoNode mut = mChromo->Root().AddChild(aMut, true, true);
    }
    Unit::mutate(aMut, aChange, aCtx, aTreatAsChromo);
}

/*
   void Elem::mutSegment(const ChromoNode& aMut, bool aChange, const MutCtx& aCtx)
   {
   }
*/

MNode* Elem::mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    MNode* elem = Node::mutAddElem(aMut, aUpdOnly, aCtx);
    return elem;
}

void Elem::mutAddElemOnCreated(MNode* aCreated, MNode* aParent)
{
    // Attach created elem to parent
    MElem* heire = aCreated->lIf(heire);
    if (heire) {
	MParent* parent = aParent->lIf(parent);
	if (parent) {
	    parent->attachChild(heire->asChild());
	}
    }
}

void Elem::mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    if (!aUpdOnly) {
	mChromo->Root().AddChild(aMut, true, false);
    }
    Node::mutContent(aMut, aUpdOnly, aCtx);
}

void Elem::onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx)
{
    //auto it = mChromo->Root().Find(aCtx.mParent);
    //if (it == const_cast<const ChromoNode&>(mChromo->Root()).End()) {
    if (aCtx.mNode && aCtx.mNode != this) {
	ChromoNode anode = mChromo->Root().AddChild(aMut, true, true);
	const MNode* onode = aOwned->lIf(onode);
	assert(!anode.AttrExists(ENa_Targ));
	GUri nuri;
	onode->getUri(nuri, this);
	anode.SetAttr(ENa_Targ, nuri);
	// Adding namespace
	if (!aCtx.mNs.empty()) {
	    MNode* ns = aCtx.mNs.at(0);
	    GUri nsuri;
	    ns->getUri(nsuri, const_cast<MNode*>(onode));
	    anode.SetAttr(ENa_NS, nsuri);
	}
	Node::onOwnedMutated(aOwned, aMut, aCtx);
    }
}

MNode* Elem::createHeir(const string& aName)
{
    MNode* heir = nullptr;
    if (Provider()->isProvided(this)) {
	heir = Provider()->createNode(name(), aName, mEnv);
    } else {
	assert(parent());
	heir = parent()->createHeirPrnt(aName);
	if (heir) {
	    // Mutate bare child with original parent chromo, updating only to have clean heir's chromo
	    heir->setCtx(nullptr);
	    heir->setCtx(Owner());
	    heir->mutate(mChromo->Root(), false, MutCtx(), true);
	} else {
	    Log(TLog(EErr, this) + "Failed creating heir [" + aName + "]");
	}
    }
    return heir;
}

MIface* Elem::MParent_getLif(const char *aType)
{
    MIface* res = nullptr;
    return res;
}

void Elem::MParent_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    Elem* self = const_cast<Elem*>(this);
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, aOs); aOs << "Name: " << name() << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	auto pair = self->mInode.binded()->firstPair();
	while (pair) {
	    const MChild* child = pair->provided();
	    Ifu::offset(aIdt, aOs); aOs << "- " << child->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		child->doDump(Ifu::EDM_Comps | Ifu::EDM_Recursive, aIdt + Ifu::KDumpIndent, aOs);
	    }
	    pair = self->mInode.binded()->nextPair(pair);
	}
    }
}

void Elem::onChildDeleting(MChild* aChild)
{
}

bool Elem::onChildRenaming(MChild* aChild, const string& aNewName)
{
    bool res = false;
    return res;
}

MIface* Elem::MChild_getLif(const char *aType)
{
    MIface* res = nullptr;
    return res;
}

void Elem::MChild_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    Elem* self = const_cast<Elem*>(this);
    if (aLevel & Ifu::EDM_Base) {
	Ifu::offset(aIdt, aOs); aOs << "Name: " << name() << endl;
    }
    if (aLevel & Ifu::EDM_Comps) {
	auto pair = self->mInode.binded()->firstPair();
	while (pair) {
	    const MChild* child = pair->provided();
	    Ifu::offset(aIdt, aOs); aOs << "- " << child->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		child->doDump(Ifu::EDM_Comps | Ifu::EDM_Recursive, aIdt + Ifu::KDumpIndent, aOs);
	    }
	    pair = self->mInode.binded()->nextPair(pair);
	}
    }
}

void Elem::onParentDeleting(MParent* aParent)
{
}

MNode* Elem::createHeirPrnt(const string& aName)
{
    return createHeir(aName);
}

MParent* Elem::parent()
{
    return mInode.firstPair()->provided();
}

bool Elem::attachChild(MChild* aChild)
{
    return mInode.binded()->connect(aChild->cP());
}

MChild* Elem::asChild()
{
    return mInode.provided();
}

MParent* Elem::asParent()
{
    return mInode.binded()->provided();
}

MChild::TCp* Elem::cP()
{
    return &mInode;
}
