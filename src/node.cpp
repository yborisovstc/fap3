
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
    return res;
}

void Node::MNode_doDump(int aLevel, int aIdt, ostream& aOs) const
{
    if (aLevel & EDM_Base) {
	aOs << "Name: " << mName << endl;
    }
    if (aLevel & EDM_Comps) {
	for (int i = 0; i < mCpOwner.pcount(); i++) {
	    const MOwned* comp = mCpOwner.pairAt(i);
	    const MNode* compn = comp->lIf(compn);
	    aOs << "--" << i << ": " << compn->name() << endl;
	    if (aLevel & EDM_Recursive) {
		compn->doDump(EDM_Comps | EDM_Recursive, Ifu::KDumpIndent, aOs);
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
    } else {
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
    updateNs(root_ns, aMut);

    MNode* btarg = this; // Base target
    string sbtarg;
    if (aMut.AttrExists(ENa_Targ)) {
	sbtarg = aMut.Attr(ENa_Targ);
	btarg = getNode(sbtarg, root_ns);
	if (!btarg) {
	    Logger()->Write(EErr, this, "Cannot find base target node [%s]", sbtarg.c_str());
	    res = false;
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
