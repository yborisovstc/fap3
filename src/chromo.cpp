#include "chromo.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept> 


using namespace std;

const string TMut::KCtrls = string(TMut::KSep, TMut::KAttrSep);

map<TNodeAttr, string> KNodeAttrsNames_Init()
{
    map<TNodeAttr, string> res;
    res[ENa_Id] = "id";
    res[ENa_Parent] = "parent";
    res[ENa_Ref] = "ref";
    res[ENa_MutNode] = "node";
    res[ENa_MutAttr] = "attr";
    res[ENa_MutVal] = "val";
    res[ENa_Order] = "ord";
    res[ENa_TOrder] = "tord";
    res[ENa_Inactive] = "na";
    res[ENa_Targ] = "targ";
    res[ENa_NS] = "ns";
    res[ENa_P] = "p";
    res[ENa_Q] = "q";
    return res;
}

map<TNodeType, string> KNodeTypesNames_Init()
{
    map<TNodeType, string> res;
    res[ENt_None] = "none";
    res[ENt_Node] = "node";
    res[ENt_Move] = "move";
    res[ENt_Rm] = "rm";
    res[ENt_Change] = "change";
    res[ENt_Cont] = "cont";
    res[ENt_Import] = "import";
    res[ENt_Seg] = "seg";
    res[ENt_Conn] = "conn";
    res[ENt_Disconn] = "disconn";
    return res;
}

map<string, TNodeType> TMut::KNodeTypes_Init()
{
    map<string, TNodeType> res;
    for (map<TNodeType, string>::const_iterator it = KNodeTypesNames.begin(); it != KNodeTypesNames.end(); it++) {
	res[it->second] = it->first;
    }
    return res;
}

map<string, TNodeAttr> TMut::KNodeAttrs_Init()
{
    map<string, TNodeAttr> res;
    for (map<TNodeAttr, string>::const_iterator it = KNodeAttrsNames.begin(); it != KNodeAttrsNames.end(); it++) {
	res[it->second] = it->first;
    }
    return res;
}

map<TNodeType, string> TMut::KNodeTypesNames = KNodeTypesNames_Init();
map<TNodeAttr, string> TMut::KNodeAttrsNames = KNodeAttrsNames_Init();
map<string, TNodeType> TMut::KNodeTypes = TMut::KNodeTypes_Init();
map<string, TNodeAttr> TMut::KNodeAttrs = TMut::KNodeAttrs_Init();


const string& TMut::NodeAttrName(TNodeAttr aAttr)
{
    return KNodeAttrsNames[aAttr];
}

const string& TMut::NodeTypeName(TNodeType aType)
{
    return KNodeTypesNames[aType];
}

TNodeAttr TMut::NodeAttr(const string& aAttrName)
{
    return KNodeAttrs.count(aAttrName) > 0 ? KNodeAttrs[aAttrName] : ENa_Unknown;
}

TNodeType TMut::NodeType(const string& aTypeName)
{
    return KNodeTypes.count(aTypeName) > 0 ? KNodeTypes[aTypeName] : ENt_Unknown;
}

TMut::TMut(): mType(ENt_Unknown), mIsValid(true)
{
}

TMut::TMut(TNodeType aType): mType(aType), mIsValid(true)
{
}

TMut::TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val):
    mType(aType), mIsValid(true)
{
    mAttrs.insert(TElem(aAttr0, aAttr0Val));
}

TMut::TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val, TNodeAttr aAttr1, const string& aAttr1Val):
    mType(aType), mIsValid(true)
{
    mAttrs.insert(TElem(aAttr0, aAttr0Val));
    mAttrs.insert(TElem(aAttr1, aAttr1Val));
}

TMut::TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val, TNodeAttr aAttr1, const string& aAttr1Val,
	TNodeAttr aAttr2, const string& aAttr2Val): mType(aType), mIsValid(true)
{
    mAttrs.insert(TElem(aAttr0, aAttr0Val));
    mAttrs.insert(TElem(aAttr1, aAttr1Val));
    mAttrs.insert(TElem(aAttr2, aAttr2Val));
}

TMut::TMut(const ChromoNode& aCnode): mType(aCnode.Type()), mIsValid(true)
{
    for (map<TNodeAttr, string>::const_iterator it = KNodeAttrsNames.begin(); it != KNodeAttrsNames.end(); it++) {
	TNodeAttr attr = it->first;
	if (aCnode.AttrExists(attr)) {
	    SetAttr(attr, aCnode.Attr(attr));
	}
    }
}

bool TMut::AttrExists(TNodeAttr aId) const
{
    return mAttrs.count(aId) > 0;
}

string TMut::Attr(TNodeAttr aId) const
{
    return mAttrs.count(aId) > 0 ? mAttrs.at(aId) : string();
}

void TMut::SetAttr(TNodeAttr aAttr, const string& aAttrVal)
{
    mAttrs[aAttr] = aAttrVal;
}

void TMut::RmAttr(TNodeAttr aAttr)
{
    mAttrs.erase(aAttr);
}

TMut::TMut(const string& aSpec): TMut()
{
    FromString(aSpec);
}

void TMut::FromString(const string& aSpec)
{
    size_t type_beg = 0, type_end = 0;
    type_end = aSpec.find_first_of(KSep, type_beg); 
    string types = aSpec.substr(type_beg, (type_end == string::npos) ? string::npos : type_end - type_beg);
    mType = NodeType(types);
    if (mType == ENt_Unknown) {
	mIsValid = false;
    } else if (type_end != string::npos) {
	size_t attr_end = type_end;
	size_t attr_beg;
	do {
	    attr_beg = attr_end + 1;
	    size_t attr_mid = attr_beg;
	    // Find first non-escaped separator
	    do {
		attr_end = aSpec.find_first_of(KSep, attr_mid); 
		attr_mid = attr_end + 1;
	    } while (attr_end != string::npos && aSpec.at(attr_end - 1) == KEsc);
	    string attr = aSpec.substr(attr_beg, (attr_end == string::npos) ? string::npos : attr_end - attr_beg);
	    // Parse arg
		size_t attrtype_end = attr.find_first_of(KAttrSep); 
		if (attrtype_end != string::npos) {
		    string attrtype = attr.substr(0, attrtype_end);
		    TNodeAttr atype = NodeAttr(attrtype);
		    if (atype == ENa_Unknown) {
			mIsValid = false;
		    }
		    size_t attrval_beg = attrtype_end + 1;
		    string attrval = attr.substr(attrval_beg, string::npos);
		    SetAttr(atype, DeEscCtrls(attrval));
		} else {
		    mIsValid = false;
		}
	} while (attr_end != string::npos);
    }
}

TMut::operator string() const
{
    string res(NodeTypeName(mType));
    for (TAttrs::const_iterator it = mAttrs.begin(); it != mAttrs.end(); it++) {
	res += KSep + NodeAttrName(it->first) + KAttrSep + EscapeCtrls(it->second);
    }
    return res;
}

/*
const TMut::TElem& TMut::AttrAt(int aInd) const
{
    assert(aInd < mAttrs.size());
    return mAttrs.at(aInd);
}
*/

string TMut::EscapeCtrls(const string& aInp)
{
    string res;
    for (string::const_iterator it = aInp.begin(); it != aInp.end(); it++) {
	const char cc = *it;
	if (cc == KSep || cc == KAttrSep || cc == KEsc) {
	    res.push_back(KEsc);
	}
	res.push_back(cc);
    }
    return res;
}

string TMut::DeEscCtrls(const string& aInp)
{
    string res;
    for (string::const_iterator it = aInp.begin(); it != aInp.end(); it++) {
	const char cc = *it;
	string::const_iterator itn = it + 1;
	if (itn != aInp.end()) {
	    const char cn = *itn;
	    if (cc == KEsc && IsCtrlSymb(cn)) {
		res.push_back(cn);
		it++;
		continue;
	    }
	}
	res.push_back(cc);
    }
    return res;
}


// Chromo utilities

void ChromoUtils::GetPath(const string& aUri, string& aPath)
{
    size_t scheme_end = aUri.find_first_of(':');
    size_t hier_beg = (scheme_end != string::npos) ? scheme_end+1 : 0; 
    size_t frag_beg = aUri.find_first_of("#");
    string hier = aUri.substr(hier_beg, frag_beg - hier_beg);
    size_t pos = hier.find("//"); 
    if (pos == 0) {
	// There is authority
    }
    else {
	aPath = hier;
    }
}

void ChromoUtils::GetFrag(const string& aUri, string& aFrag)
{
    size_t frag_beg = aUri.find_first_of('#');
    if (frag_beg != string::npos) {
	aFrag = aUri.substr(frag_beg + 1);
    }
}


// Node of chromo spec

ChromoNode::ChromoNode(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle)
{
}

ChromoNode& ChromoNode::operator=(const ChromoNode& aNode)
{
    iMdl = aNode.iMdl;
    iHandle = aNode.iHandle;
    return *this;
}

bool ChromoNode::operator==(const ChromoNode& aNode)
{
    return iHandle == aNode.iHandle;
}

bool ChromoNode::operator==(const ChromoNode& aNode) const
{ 
    return iHandle == aNode.iHandle;
}

const string ChromoNode::Attr(TNodeAttr aAttr)
{
    return  iMdl->GetAttr(iHandle, aAttr);
    /*
    string res; 
    if (sattr != NULL)
	res.assign(sattr);
    free(sattr); 
    return res; 
    */
};

const string ChromoNode::Attr(TNodeAttr aAttr) const
{
    return iMdl->GetAttr(iHandle, aAttr);
    /*
    string res; 
    if (sattr != NULL)
	res.assign(sattr);
    free(sattr); 
    return res; 
    */
};

int ChromoNode::AttrInt(TNodeAttr aAttr) const 
{
    string attr = Attr(aAttr); 
    assert(!attr.empty()); 
    return atoi(attr.c_str());
};

bool ChromoNode::AttrBool(TNodeAttr aAttr) const 
{ 
    string attr = Attr(aAttr); 
    assert(!attr.empty() || attr == "yes" || attr == "no"); 
    return (attr == "yes"); 
};

string ChromoNode::GetName(const string& aTname)
{
    size_t tpos = aTname.find("%");
    return (tpos != string::npos) ? aTname.substr(tpos+1) : aTname;
}

ChromoNode::Iterator ChromoNode::Parent()
{
    THandle parent = iMdl->Parent(iHandle);
    return  (parent == THandle()) ?  End() : Iterator(ChromoNode(iMdl, parent));
}

ChromoNode::Iterator ChromoNode::Root()
{
    THandle root = iMdl->Root(iHandle);
    return  (root == THandle()) ?  End() : Iterator(ChromoNode(iMdl, root));
}

ChromoNode::Const_Iterator ChromoNode::Root() const 
{
    THandle root = iMdl->Root(iHandle);
    return  (root == THandle()) ?  End() : Const_Iterator(ChromoNode(iMdl, root));
}

ChromoNode::Const_Iterator ChromoNode::Parent() const
{
    THandle parent = iMdl->Parent(iHandle);
    return  (parent == THandle()) ?  End() : Const_Iterator(ChromoNode(iMdl, parent));
}

ChromoNode::Iterator ChromoNode::Find(TNodeType aType, const string& aName) 
{ 
    ChromoNode::Iterator res = End();
    size_t pos = aName.find("/");
    string name = aName.substr(0, pos);
    for (ChromoNode::Iterator it = Begin(); it != End(); it++) {
	if (((*it).Type() == aType) && (name.compare((*it).Name()) == 0)) {
	    res = it;  break;
	}
    }
    if ((res != End()) &&  (pos != string::npos)) {
	res = (*res).Find(aType, aName.substr(pos + 1));	
    }
    return res;
};

ChromoNode::Const_Iterator ChromoNode::Find(TNodeType aType, const string& aName) const
{ 
    ChromoNode::Const_Iterator res = End();
    for (ChromoNode::Const_Iterator it = Begin(); it != End(); it++) {
	if (((*it).Type() == aType) && (aName.compare((*it).Name()) == 0)) {
	    res = it;  break;
	}
    }
    return res;
};

ChromoNode::Iterator ChromoNode::Find(TNodeType aType, const string& aName, TNodeAttr aAttr, const string& aAttrVal)
{
    ChromoNode::Iterator res = End();
    for (ChromoNode::Iterator it = Begin(); it != End(); it++) {
	const ChromoNode& node = (*it);
	if ((node.Type() == aType)  && (aName.compare(node.Name()) == 0) && node.AttrExists(aAttr) && (aAttrVal.compare(node.Attr(aAttr)) == 0)) {
	    res = it;  break;
	}
    }
    return res;
};

ChromoNode::Const_Iterator ChromoNode::Find(const ChromoNode aNode) const
{
    auto res = End();
    for (auto it = Begin(); it != End(); it++) {
	const ChromoNode& node = (*it);
	if (node == aNode) {
	    res = it;  break;
	} else {
	    res = node.Find(aNode);
	    if (res != node.End()) {
		break;
	    }
	}
    }
    return res;
}

int ChromoNode::GetLocalSize()
{
    int res = 0;
    //for (Iterator it = Begin(); it != End(); it++, res++);
    Iterator it = Begin();
    while (it != End()) {
	it++;
	res++;
    }
    return res;
}

int ChromoNode::GetLocalSize(TNodeType aType) const
{
    int res = 0;
    for (Const_Iterator it = Begin(); it != End(); it++) {
	if ((*it).Type() == aType) {
	    res++;
	}
    }
    return res;
}

int ChromoNode::Count() const
{
    int res = 0;
    for (Const_Iterator it = Begin(); it != End(); it++, res++);
    return res;
}

ChromoNode ChromoNode::AddChild(TNodeType aType) 
{ 
    return ChromoNode(iMdl, iMdl->AddChild(iHandle, aType));
}

ChromoNode ChromoNode::AddChild(const TMut& aMut)
{
    ChromoNode mut(iMdl, iMdl->AddChild(iHandle, aMut.Type()));
    for (TMut::TAttrs::const_iterator it = aMut.Attrs().begin(); it != aMut.Attrs().end(); it++) {
	mut.SetAttr(it->first, it->second);
    }
    return mut;
}

ChromoNode ChromoNode::AddChild(const ChromoNode& aNode, bool aCopy, bool aRecursively) 
{ 
    return ChromoNode(iMdl, iMdl->AddChild(iHandle, aNode.Handle(), aCopy, aRecursively));
}

ChromoNode::Iterator ChromoNode::GetChildOwning(const ChromoNode& aNode) const
{
    ChromoNode res(aNode);
    ChromoNode prnt = *res.Parent();
    while (prnt != *this && prnt != ChromoNode(iMdl, THandle())) {
	res = prnt;
    }
    return Iterator(res);
}

void ChromoNode::ReduceToSelection(const ChromoNode& aSelNode)
{
    assert(*aSelNode.Parent() == *this);
    Iterator it = Begin();
    while (it != End() && !(*it == aSelNode)) {
	ChromoNode node = *it;
	if (node.Type() == ENt_Import) {
	    it++;
	} else {
	    RmChild(node);
	    it = Begin();
	}
    }
    Reverse_Iterator itr = Rbegin();
    while (itr != Rend() && !(*itr == aSelNode)) {
	RmChild(*itr);
	itr = Rbegin();
    }
}

void ChromoNode::GetUri(GUri& aUri, const ChromoNode& aBase) const
{
    Const_Iterator parent = Parent();
    if (parent != End()) {
	if (AttrExists(ENa_Id)) {
	    aUri.prependElem(Name());
	}
	if (AttrExists(ENa_Targ)) {
	    aUri.prepend(Attr(ENa_Targ));
	}
	ChromoNode owner = *parent;
	if (owner == aBase) { return; }
	owner.GetUri(aUri, aBase);
    } else {
	aUri.prependElem(Name());
	aUri.prependElem("");
    }
}

void ChromoNode::Activate()
{ 
    if (AttrExists(ENa_Inactive)) {
	RmAttr(ENa_Inactive);
    }
    if (Type() == ENt_Node) {
	Iterator it = Begin();
	while (it != End()) {
	    (*it).Activate();
	    it++;
	}
    }
};

ChromoNode::operator string() const
{
    string res;
    ToString(res);
    return res;
}

void ChromoNode::Dump() const { iMdl->Dump(iHandle);}

void ChromoNode::DumpBackTree() const { iMdl->DumpBackTree(iHandle);}

void ChromoNode::GetTarget(GUri& aTargUri, const ChromoNode& aBase) const
{
    if (AttrExists(ENa_Targ)) {
	GUri cur(Attr(ENa_Targ));
	aTargUri.prepend(cur);
    }
    auto owner = *Parent();
    if (owner != aBase && !owner.IsNil()) {
	owner.GetTarget(aTargUri, aBase);
    }
}
