

#include "elem.h"
#include "chromo.h"



Elem::Elem(const string &aType, const string &aName, MEnv* aEnv): Unit(aType, aName, aEnv), mInode(this, this)
{
    mChromo = mEnv->provider()->createChromo();
    mChromo->Init(ENt_Node);
    setCrAttr(aType, aName);
}

Elem::~Elem()
{
    //assert(parent());
    // Notify parent
    if (parent()) {
	parent()->onChildDeleting(this);
    }
    // Notify childs and disconnect
    auto cn = mInode.binded()->firstPair();
    while (cn) {
	cn->provided()->onParentDeleting(this);
	cn = mInode.binded()->nextPair(cn);
    }
    mInode.binded()->disconnectAll();
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

#if 0 // V1
void Elem::mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo, bool aLocal)
{
    bool isChild = aMut.IsChildOf(mChromo->Root());
    if (!aChange /* && !aTreatAsChromo*/ && !isChild) {
	notifyNodeMutated(aMut, aCtx);
	if (aTreatAsChromo) {
	    for (ChromoNode::Const_Iterator rit = aMut.Begin(); rit != aMut.End(); rit++)
	    {
		ChromoNode rno = (*rit);
		ChromoNode mut = mChromo->Root().AddChild(rno, true, true);
	    }
	    Unit::mutate(mChromo->Root(), aChange, aCtx, aTreatAsChromo, aLocal);
	} else {
	    ChromoNode mut = mChromo->Root().AddChild(aMut, true, true);
	    if (aLocal) {
		mut.RmAttr(ENa_Targ);
	    }
	    Unit::mutate(mut, aChange, aCtx, aTreatAsChromo, aLocal);
	}
    } else {
	Unit::mutate(aMut, aChange, aCtx, aTreatAsChromo, aLocal);
    }
}
#endif

void Elem::mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo, bool aLocal)
{
    bool isChild = aMut.IsChildOf(mChromo->Root());
    if (!aChange /* && !aTreatAsChromo*/ && !isChild) {
	notifyNodeMutated(aMut, aCtx);
	if (aTreatAsChromo) {
	    for (ChromoNode::Const_Iterator rit = aMut.Begin(); rit != aMut.End(); rit++)
	    {
		ChromoNode rno = (*rit);
		ChromoNode mut = mChromo->Root().AddChild(rno, true, true);
	    }
	    Unit::mutate(mChromo->Root(), aChange, aCtx, aTreatAsChromo, aLocal);
	} else {
	    ChromoNode mut = mChromo->Root().AddChild(aMut, true, true);
	    if (aLocal) {
		mut.RmAttr(ENa_Targ);
	    }
	    Unit::mutate(mut, aChange, aCtx, aTreatAsChromo, aLocal);
	}
    } else {
	Unit::mutate(aMut, aChange, aCtx, aTreatAsChromo, aLocal);
    }
}

MNode* Elem::mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx)
{
    MNode* elem = Node::mutAddElem(aMut, aUpdOnly, aCtx);
    return elem;
}

bool Elem::attachHeir(MNode* aHeir)
{
    bool res = false;
    MElem* heire = aHeir->lIf(heire);
    if (heire) {
	res = attachChild(heire->asChild());
    }
    return res;
}

void Elem::onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx)
{
    //auto it = mChromo->Root().Find(aCtx.mParent);
    //if (it == const_cast<const ChromoNode&>(mChromo->Root()).End()) {
    bool isChild = aMut.IsChildOf(mChromo->Root());
    if (!isChild /* && aCtx.mNode != this*/) {
	ChromoNode anode = mChromo->Root().AddChild(aMut, true, true);
	const MNode* onode = aOwned->lIf(onode);
	GUri nuri;
	onode->getUri(nuri, this);
	anode.SetAttr(ENa_Targ, nuri);
	// Adding namespace
	if (!aCtx.mNs.empty()) {
	    MNode* ns = aCtx.mNs.at(0);
	    if (ns != this) {
		GUri nsuri;
		ns->getUri(nsuri, const_cast<MNode*>(onode));
		anode.SetAttr(ENa_NS, nsuri);
	    }
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
	    heir->mutate(mChromo->Root(), true, MutCtx(), true);
	    // Mutated with parent's own chromo - so panent's name is the type now.
	    // TODO Seems wrong, why name, should be URI.
	    MElem* heire = heir->lIf(heire);
	    if (heire) {
		heire->Chromos().Root().SetAttr(ENa_Parent, name());
	    }
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
    detachChild(aChild);
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
    auto fp = mInode.firstPair();
    return fp ? fp->provided() : nullptr;
}

bool Elem::attachChild(MChild* aChild)
{
    return mInode.binded()->connect(aChild->cP());
}

bool Elem::detachChild(MChild* aChild)
{
    return mInode.binded()->disconnect(aChild->cP());
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
