

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
    for (auto it = mInode.binded()->pairsBegin(); it != mInode.binded()->pairsEnd(); it++) {
	auto cn = *it;
	cn->provided()->onParentDeleting(this);
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

string Elem::parentName() const
{
    string res;
    GUri uri;
    const MParent* prnt = parent();
    if (prnt) {
        prnt->getUriPrnt(uri);
        if (uri.isValid()) {
            res = uri.tailn(1);
        }
    }
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

void Elem::mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo, bool aLocal)
{
    bool isChild = aMut.IsChildOf(mChromo->Root());
    if (!aChange && !aTreatAsChromo && !isChild) {
	// TODO OPT bad perf on mut adding to target chromo (ref ut_elem_mutperf_1 UT)"
	PFL_DUR_STAT_START(PEvents::EDurStat_MutCad);
        ChromoNode mut = mChromo->Root().AddChild(aMut, true, true);
        if (aLocal) {
            mut.RmAttr(ENa_Targ);
        }
	PFL_DUR_STAT_REC(PEvents::EDurStat_MutCad);
        Unit::mutate(mut, aChange, aCtx, aTreatAsChromo, aLocal);
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
    if (aCtx.mNode != this) {
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
	    LOGN(EErr, "Failed creating heir [" + aName + "]");
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
	for (auto it = self->mInode.binded()->pairsBegin(); it != self->mInode.binded()->pairsEnd(); it++) {
	    auto* pair = *it;
	    const MChild* child = pair->provided();
	    Ifu::offset(aIdt, aOs); aOs << "- " << child->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		child->doDump(Ifu::EDM_Comps | Ifu::EDM_Recursive, aIdt + Ifu::KDumpIndent, aOs);
	    }
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
	for (auto it = self->mInode.binded()->pairsBegin(); it != self->mInode.binded()->pairsEnd(); it++) {
	    auto* pair = *it;
	    const MChild* child = pair->provided();
	    Ifu::offset(aIdt, aOs); aOs << "- " << child->Uid() << endl;
	    if (aLevel & Ifu::EDM_Recursive) {
		child->doDump(Ifu::EDM_Comps | Ifu::EDM_Recursive, aIdt + Ifu::KDumpIndent, aOs);
	    }
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
    //auto fp = mInode.firstPair();
    auto* fp = *mInode.pairsBegin();
    return fp ? fp->provided() : nullptr;
}

const MParent* Elem::parent() const
{
    //auto fp = const_cast<TInhTreeNode&>(mInode).firstPair();
    auto* fp = *const_cast<TInhTreeNode&>(mInode).pairsBegin();
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

void Elem::getUriPrnt(GUri& aUri) const
{
    getUri(aUri);
}
