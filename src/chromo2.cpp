
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "chromo2.h"

/** @brief Chromo grammar terminals
 * */
const string KT_Target = "<";
const string KT_Node = "-";
const string KT_Namespace = "@";
const string KT_ContextSmb = KT_Target + KT_Namespace;

const char KT_MutSeparator = ';';
const char KT_ChromoStart = '{';
const char KT_ChromoEnd = '}';
const char KT_TextDelim = '"';
const char KT_Escape = '\\';
const char KT_MutAdd = ':';
const char KT_MutConn = '~';
const char KT_MutComment = '#';
const char KT_MutImport = '+';
const char KT_MutContent = '=';
const char KT_MutRemove = '!';
const char KT_UriSep = '.';
const char KT_EOL = '\n';
const char KTC_Target = '<';
const char KTC_Namespace = '@';

/** @brief Mutation symbols */
const string KMS_Add = ":";
const string KMS_Cont = "=";
const string KMS_Note = "#";
const string KMS_Rename = "|";
const string KMS_Remove = "!";
const string KMS_Import = "+";
const string KMS_Conn = "~";
const string KMS_Disconn = "!~";

static const vector<string> KDmcMutops = {KMS_Conn};

/** @brief Separators **/
const string KSep = " \t\n\r";
/** @brief Standard control symbols **/
const string K_Ctrls = {KT_TextDelim};

/** @brief Default chromo indentation */
const int K_Indent = 4;



/** @brief Default P part */
static const string KT_Default = "$";

static const string KR_CREATE = ":";

/** @brief Chromo spec parsing errors
 * */

static const string KPE_ErrOnReadingSpec = "Error on reading chromo";
static const string KPE_UnexpChrEnd = "Unexpected end of chromo";
static const string KPE_WrongMutLexNum = "Wrong number of mutation lexems";
static const string KPE_WrongCtxType = "Wrong context type";
static const string KPE_MissingEndOfGroup = "Missin end of group";
static const string KPE_UnknownMutation = "Unknown mutation";
static const string KPE_WrongCombMutType = "Wrong type of combined mutation";
static const string KPE_TextNotClosed = "Text block is not closed";
static const string KPE_WrongDepMut = "Wrong depenent mutation, shall be 'node'";
static const string KPE_MissingSegContext = "Missing context of the segment";
static const string KPE_WrongNumOfCtxLex = "Wrong number of context lexemd";
static const string KPE_SmallLexNumInDmcMut = "Too small number of lex in DMC mut";


/** @brief Recursive Descent Parser (RDP) errors */
static const string RDPE_WrongName = "Wrong name";
static const string RDPE_WrongUriElem = "Wrong uri elem";
static const string RDPE_SepMissing = "Separator is missing";
static const string RDPE_MissingMutSmb = "Missing mutation symbol";
static const string RDPE_MissingCtxSmb = "Missing context symbol";
static const string RDPE_MissingMutSmbAdd = "Missing symbol of Add mutation";
static const string RDPE_MissingUriSep = "Missing URI separator";
static const string RDPE_MissingChromo = "Missing chromo";
static const string RDPE_MissingMutSep = "Missing mut separator";
static const string RDPE_MissingTextDelim = "Missing text delimiter";
static const string RDPE_WrongCtxType = "Incorrect context type";

/** @brief Segment offset when node output */
const int KA_OutOffset = 3;

using namespace std;

/** @brief Mapping for context: attr to ctx type
 * */
const map<TNodeAttr, string> KM_CtxAttToType = {
    {ENa_Targ, KT_Target},
    {ENa_MutNode, KT_Node},
    {ENa_NS, KT_Namespace}
};

/** @brief Mapping for mut: node type to mut rel
 * */
const map<TNodeType, string> KM_MutTypeToRel = {
    {ENt_Node, KMS_Add},
    {ENt_Note, KMS_Note},
    {ENt_Change, KMS_Rename},
    {ENt_Rm, KMS_Remove},
    {ENt_Import, KMS_Import},
    {ENt_Cont, KMS_Cont},
    {ENt_Conn, KMS_Conn},
    {ENt_Disconn, KMS_Disconn}
};

string GetMutRel(TNodeType aType)
{
    return KM_MutTypeToRel.count(aType) == 0 ? string() : KM_MutTypeToRel.at(aType);
}

string GetCtxRel(TNodeAttr aAttr)
{
    return KM_CtxAttToType.count(aAttr) == 0 ? string() : KM_CtxAttToType.at(aAttr);
}

bool IsMutSmb(const string& aSmb)
{
    bool res = false;
    for (auto it : KM_MutTypeToRel) {
	if (it.second == aSmb) {
	    res = true; break;
	}
    }
    return res;
}

/** @brief Checks if given symbol is separator
 * */
bool IsSep(char aSmb)
{
    return  (KSep.find(aSmb) != string::npos);
}

/** @brief Checks if given symbol belongs to the set
 * */
bool IsSymbOf(char aSmb, const string& aSet)
{
    return  (aSet.find(aSmb) != string::npos);
}





static const map<TNodeType, string> KNtToR = {
    {ENt_Node, KR_CREATE}
};

/*
C2MdlCtxNode::C2MdlCtxNode(const string& aType, const string& aValue): mType(aType), mValue(aValue)
{
}
*/



C2MdlNode::C2MdlNode(): mOwner(NULL), mQnode(NULL), mChromoPos(-1)
{
}

C2MdlNode::C2MdlNode(C2MdlNode* aOwner): mOwner(aOwner), mQnode(NULL), mChromoPos(-1)
{
}

C2MdlNode::C2MdlNode(const C2MdlNode& aSrc): mOwner(aSrc.mOwner), mContext(aSrc.mContext), mMut(aSrc.mMut), mChromo(aSrc.mChromo),
    mQnode(NULL), mChromoPos(aSrc.mChromoPos)
{
}

C2MdlNode::C2MdlNode(const C2MdlNode& aSrc, C2MdlNode* aOwner): mOwner(aOwner), mContext(aSrc.mContext), mMut(aSrc.mMut),
    mQnode(NULL), mChromoPos(aSrc.mChromoPos)
{
    if (aSrc.mQnode) {
	mQnode = new C2MdlNode(aSrc.mQnode, this);
    }
    for (TC2MdlNodesCiter it = aSrc.mChromo.begin(); it != aSrc.mChromo.end(); it++) {
	const C2MdlNode& cn = *it;
	mChromo.push_back(C2MdlNode(cn, this));
    }
}

C2MdlNode::~C2MdlNode()
{
    /*
    if (mQnode) {
	delete mQnode;
	mQnode = NULL;
    }
    */
}

void C2MdlNode::CloneFrom(const C2MdlNode& aSrc, bool aChromo)
{
    mContext = aSrc.mContext;
    mMut = aSrc.mMut;
    mChromoPos = aSrc.mChromoPos;
    if (aChromo) {
	for (TC2MdlNodesCiter it = aSrc.mChromo.begin(); it != aSrc.mChromo.end(); it++) {
	    const C2MdlNode& cn = *it;
	    mChromo.push_back(C2MdlNode(cn, this));
	}
    }
}

void C2MdlNode::BindTree(C2MdlNode* aOwner)
{
    mOwner = aOwner;
    for (TC2MdlNodesIter it = mChromo.begin(); it != mChromo.end(); it++) {
	C2MdlNode& cn = *it;
	cn.BindTree(this);
    }
}

void C2MdlNode::AddContext(const string& aType, const string& aValue)
{
    mContext.insert(TC2MdlCtxElem(aType, aValue));
}

void C2MdlNode::RmContext(TNodeAttr aAttr)
{
    assert(KM_CtxAttToType.count(aAttr) > 0);
    string crel = KM_CtxAttToType.at(aAttr);
    assert(mContext.count(crel) > 0);
    mContext.erase(crel);
}

C2MdlNode* C2MdlNode::GetNextComp(C2MdlNode* aComp)
{
    C2MdlNode* res = NULL;
    for (TC2MdlNodesIter it = mChromo.begin(); it != mChromo.end(); it++) {
	C2MdlNode* node = &(*it);
	if (node == aComp) {
	    res = (++it == mChromo.end()) ? NULL: &(*it);
	    break;
	}
    }
    return res;
}

C2MdlNode* C2MdlNode::GetPrevComp(C2MdlNode* aComp)
{
    C2MdlNode* res = NULL;
    for (TC2MdlNodesRIter it = mChromo.rbegin(); it != mChromo.rend(); it++) {
	C2MdlNode* node = &(*it);
	if (node == aComp) {
	    res = (++it == mChromo.rend()) ? NULL: &(*it);
	    break;
	}
    }
    return res;
}

bool C2MdlNode::ExistsContextByAttr(TNodeAttr aAttr)
{
    bool res = false;
    if (KM_CtxAttToType.count(aAttr) > 0) {
	string rel = KM_CtxAttToType.at(aAttr);
	res = mContext.count(rel) > 0;
    }
    return res;
}

void C2MdlNode::AddQnode(const C2MdlNode& aNode)
{
    mQnode = new C2MdlNode(aNode);
}


// **** Chromo2Mdl *****


Chromo2Mdl::Chromo2Mdl()
{
}

Chromo2Mdl::~Chromo2Mdl()
{
}

streampos SeekChar(istream& aIs, streampos aBeg, streampos aEnd, char aChar)
{
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == aChar) {
	    pos = aIs.tellg();
	    break;
	}
	pos = aIs.tellg();
    }
    return pos;
}

/** @brief Finds control symbol */
streampos SeekCtrl(istream& aIs, streampos aBeg, streampos aEnd, char aChar)
{
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    bool text = false;
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == KT_TextDelim) {
	    text = !text;
	}
	if (!text && c == aChar) {
	    pos = aIs.tellg();
	    break;
	}
	pos = aIs.tellg();
    }
    return pos;
}

string GenerateName(const string& aParent, int aPos)
{
    return string("_") + aParent + to_string(aPos);
}

bool IsLexCtx(const string& aLex)
{
    return aLex == KT_Namespace || aLex == KT_Target || aLex == KT_Node;
}

TNodeType Chromo2Mdl::GetType(const THandle& aHandle) {
    TNodeType res = ENt_Unknown;
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (rel.empty()) {
	assert(!node->mChromo.empty());
	res = ENt_Seg;
    } else  if (rel == KMS_Add) {
	res = ENt_Node;
    } else if (rel == KMS_Rename) {
	// TODO To replace change mut with renaming mut
	res = ENt_Change;
    } else if (rel == KMS_Remove) {
	res = ENt_Rm;
    } else  if (rel == KMS_Note) {
	res = ENt_Note;
    } else  if (rel == KMS_Import) {
	res = ENt_Import;
    } else  if (rel == KMS_Cont) {
	res = ENt_Cont;
    } else  if (rel == KMS_Conn) {
	res = ENt_Conn;
    } else  if (rel == KMS_Disconn) {
	res = ENt_Disconn;
    }
    return res;
}

THandle Chromo2Mdl::Root(const THandle& aHandle)
{
    return &mRoot;
}

THandle Chromo2Mdl::Parent(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    return node->mOwner;
}

THandle Chromo2Mdl::Next(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    assert(node->mOwner != NULL);
    C2MdlNode* next = node->mOwner->GetNextComp(node);
    return next;
}

THandle Chromo2Mdl::Prev(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode* prev = node->mOwner->GetPrevComp(node);
    return prev;
}

THandle Chromo2Mdl::GetFirstChild(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode *comp = NULL;
    if (node->mChromo.size() > 0) {
	comp = &(node->mChromo.front());
    }
    return comp;
}

THandle Chromo2Mdl::GetLastChild(const THandle& aHandle, TNodeType aType)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode *comp = NULL;
    if (node->mChromo.size() > 0) {
	comp = &(node->mChromo.back());
    }
    return comp;
}

string Chromo2Mdl::GetContextByAttr(const C2MdlNode& aNode, TNodeAttr aAttr)
{
    string res;
    if (KM_CtxAttToType.count(aAttr) > 0) {
	string rel = KM_CtxAttToType.at(aAttr);
	if (aNode.mContext.count(rel) > 0) {
	    res = aNode.mContext.at(rel);
	}
    }
    return res;
}

string Chromo2Mdl::GetAttr(const THandle& aHandle, TNodeAttr aAttr) const
{
    C2MdlNode* node = aHandle.Data(node);
    string res;
    string rel = node->mMut.mR;
    if (aAttr == ENa_Id) {
	assert(rel == KMS_Remove || rel == KMS_Add || rel == KMS_Import || rel == KMS_Cont);
	if (rel == KMS_Add) {
	    res = node->mMut.mP;
	} else if (rel == KMS_Cont) {
	    res = node->mMut.mP;
	    if (res == KT_Default) {
		res.clear();
	    }
	} else if (rel == KMS_Import || rel == KMS_Remove) {
	    res = node->mMut.mQ;
	}
    } else if (aAttr == ENa_Parent && rel == KMS_Add) {
	res = node->mMut.mQ;
    } else if (aAttr == ENa_Targ || aAttr == ENa_NS) {
	res = GetContextByAttr(*node, aAttr);
    } else if (aAttr == ENa_MutAttr) {
	res = "id";
    } else if (aAttr == ENa_MutNode) {
	assert(rel == KMS_Add || rel == KMS_Remove || rel == KMS_Cont || rel == KMS_Conn || rel == KMS_Disconn || rel.empty());
	if (rel == KMS_Add || rel.empty()) {
	    res = GetContextByAttr(*node, aAttr);
	} else if (rel == KMS_Cont) {
	    res = node->mMut.mP;
	} else if (rel == KMS_Remove) {
	    res = node->mMut.mQ;
	} else if (rel == KMS_Conn || rel == KMS_Disconn) {
	    res = node->mMut.mP;
	} else {
	    res = node->mMut.mP;
	}
    } else if (aAttr == ENa_P) {
	assert(rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    res = node->mMut.mP;
	}
    } else if (aAttr == ENa_Q) {
	assert(rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    res = node->mMut.mQ;
	}
    } else if (aAttr == ENa_MutVal) {
	assert(rel == KMS_Cont || rel == KMS_Note || rel == KMS_Rename);
	res = node->mMut.mQ;
    }
    return res;
}

bool Chromo2Mdl::AttrExists(const THandle& aHandle, TNodeAttr aAttr) const 
{
    bool res = false;
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (aAttr == ENa_Id) {
	res = (rel == KMS_Add) || rel == KMS_Cont || (rel == KMS_Import);
    } else if (aAttr == ENa_Parent) {
	res = (rel == KMS_Add);
    } else if (aAttr == ENa_Targ || aAttr == ENa_NS) {
	res = node->ExistsContextByAttr(aAttr);
    } else if (aAttr == ENa_MutVal) {
	res = (rel == KMS_Cont || rel == KMS_Note || rel == KMS_Rename);
    } else if (aAttr == ENa_MutAttr) {
	res = (rel == KMS_Rename);
    } else if (aAttr == ENa_MutNode) {
	if (rel == KMS_Remove) {
	    res = !node->mMut.mP.empty();
	} else {
	    res = ((rel == KMS_Add) || (rel == KMS_Cont) || rel.empty()) && node->ExistsContextByAttr(ENa_MutNode);
	}
    } else if (aAttr == ENa_P) {
	    res = ((rel == KMS_Conn || rel == KMS_Disconn) && !node->mMut.mP.empty());
    } else if (aAttr == ENa_Q) {
	    res = ((rel == KMS_Conn || rel == KMS_Disconn) && !node->mMut.mQ.empty());
    } else if (aAttr == ENa_Ref) {
	// Use MutVal instead
    }
    return res;
}

THandle Chromo2Mdl::AddChild(const THandle& aParent, TNodeType aType)
{
    C2MdlNode* parent = aParent.Data(parent);
    C2MdlNode node;
    node.mMut.mR = GetMutRel(aType);
    node.mOwner = parent;
    parent->mChromo.push_back(node);
    C2MdlNode& res = parent->mChromo.back();
    return &res;
}

THandle Chromo2Mdl::AddChild(const THandle& aParent, const THandle& aHandle, bool aCopy, bool aRecursively)
{
    C2MdlNode* parent = aParent.Data(parent);
    C2MdlNode* child = aHandle.Data(child);
    C2MdlNode node;
    node.CloneFrom(*child, true);
    node.mOwner = parent;
    if (!aRecursively) {
	node.mChromo.clear();
    }
    parent->mChromo.push_back(node);
    parent->BindTree(parent->mOwner);
    C2MdlNode& res = parent->mChromo.back();
    return &res;
}

THandle Chromo2Mdl::AddNext(const THandle& aPrev, const THandle& aHandle, bool aCopy)
{
    assert(false);
}

THandle Chromo2Mdl::AddNext(const THandle& aPrev, TNodeType aNode)
{
    assert(false);
}

THandle Chromo2Mdl::AddPrev(const THandle& aNext, const THandle& aHandle, bool aCopy)
{
    assert(false);
}

void Chromo2Mdl::RmChild(const THandle& aParent, const THandle& aChild, bool aDeattachOnly)
{
    bool removed = false;
    C2MdlNode* parent = aParent.Data(parent);
    C2MdlNode* child = aChild.Data(child);
    for (TC2MdlNodesIter it = parent->mChromo.begin(); it != parent->mChromo.end(); it++) {
	C2MdlNode& cn = *it;
	if (&cn == child) {
	    parent->mChromo.erase(it);
	    child->mOwner = NULL;
	    removed = true;
	    break;
	}
    }
    assert(removed);
}

void Chromo2Mdl::Rm(const THandle& aHandle)
{
    assert(false);
}

void Chromo2Mdl::SetAttr(const THandle& aHandle, TNodeAttr aType, const string& aVal)
{
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (aType == ENa_Id) {
	assert (rel == KMS_Add || rel == KMS_Cont || rel == KMS_Import);
	if (rel == KMS_Add || rel == KMS_Cont) {
	    node->mMut.mP = aVal;
	} else if (rel == KMS_Import) {
	    node->mMut.mQ = aVal;
	    node->mMut.mP = KT_Default;
	} else if (rel == KMS_Cont) {
	    node->mMut.mP = aVal.empty() ? KT_Default : aVal;
	}
    } else if (aType == ENa_Parent) {
	assert (rel.empty() || rel == KMS_Add);
	node->mMut.mQ = aVal;
	if (rel.empty()) {
	    node->mMut.mR = KMS_Add;
	}
    } else if (aType == ENa_MutAttr) {
	assert (rel == KMS_Cont || (rel == KMS_Rename));
	// Attr for rename is not used, omit
    } else if (aType == ENa_MutVal) {
	assert (rel == KMS_Cont || rel == KMS_Note || rel == KMS_Rename);
	if (rel == KMS_Note || rel == KMS_Rename) {
	    node->mMut.mQ = aVal;
	    node->mMut.mP = KT_Default;
	} else {
	    node->mMut.mQ = aVal;
	}
    } else if (aType == ENa_Targ) {
	string ctxrel = GetCtxRel(aType);
	assert (!ctxrel.empty());
	node->mContext[ctxrel] = aVal;
    } else if (aType == ENa_MutNode) {
	assert (rel == KMS_Remove || rel == KMS_Add || rel == KMS_Cont || rel == KMS_Conn || rel == KMS_Disconn || rel.empty());
	if (rel == KMS_Add || rel == KMS_Cont || rel.empty()) {
	    node->AddContext(KT_Node, aVal);
	} else if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mP = aVal;
	} else if (rel == KMS_Remove) {
	    node->mMut.mQ = aVal;
	} else {
	    node->mMut.mP = aVal;
	}
    } else if (aType == ENa_P) {
	assert (rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mP = aVal;
	}
    } else if (aType == ENa_Q) {
	assert (rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mQ = aVal;
	}
    } else if (aType == ENa_Ref) {
	assert(rel == KMS_Cont);
	node->mMut.mQ = aVal;
	if (node->mMut.mP.empty()) {
	    node->mMut.mP = KT_Default;
	}
    } else if (aType == ENa_NS) {
	string ctxrel = GetCtxRel(aType);
	assert (!ctxrel.empty());
	node->mContext[ctxrel] = aVal;
    }
}

void Chromo2Mdl::SetAttr(const THandle& aNode, TNodeAttr aType, int aVal)
{
}

void Chromo2Mdl::RmAttr(const THandle& aHandle, TNodeAttr aType)
{
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (aType == ENa_MutNode) {
	assert (rel == KMS_Remove || rel == KMS_Add || rel == KMS_Cont || rel == KMS_Conn || rel == KMS_Disconn || rel.empty());
	if (rel == KMS_Add || rel == KMS_Cont || rel.empty()) {
	    node->RmContext(aType);
	} else if (rel == KMS_Conn || rel == KMS_Disconn) {
	    node->mMut.mP.erase();
	} else {
	    node->mMut.mP.erase();
	}
    } else if (KM_CtxAttToType.count(aType) > 0) {
	string crel = KM_CtxAttToType.at(aType);
	node->mContext.erase(crel);
    }
}

void Chromo2Mdl::Dump(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    DumpMnode(*node, 0);
}

void Chromo2Mdl::DumpBackTree(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    while (node != NULL) {
	cout << node << "  Id: " << node->mMut.mP << ", Owner: " << node->mOwner << endl;;
	node = node->mOwner;
    }
}


void Chromo2Mdl::DumpToLog(const THandle& aNode, MLogRec* aLogRec)
{
}

bool Chromo2Mdl::ToString(const THandle& aNode, string& aString) const
{
    assert(false);
}

void Chromo2Mdl::Save(const string& aFileName, int aIndent) const
{
    ofstream os(aFileName, ofstream::out);
    OutputNode(mRoot, os, 0, aIndent == 0 ? K_Indent : aIndent);
    os.flush();
    os.close();
}

THandle Chromo2Mdl::Find(const THandle& aHandle, const string& aUri)
{
    assert(false);
}

int Chromo2Mdl::GetOrder(const THandle& aHandle, bool aTree) const
{
    return 0;
}

void Chromo2Mdl::DeOrder(const THandle& aHandle)
{
    assert(false);
}

int Chromo2Mdl::GetLineId(const THandle& aHandle) const
{
    C2MdlNode* node = aHandle.Data(node);
    return node->mChromoPos;
}



int Chromo2Mdl::GetAttrInt(void *aHandle, const char *aName)
{
    assert(false);
}

THandle Chromo2Mdl::SetFromFile(const string& aFileName)
{
    ifstream is(aFileName, std::ifstream::in);
    if ( (is.rdstate() & ifstream::failbit ) == 0 ) {
	is.seekg(0, is.beg);
	streampos beg = is.tellg();
	is.seekg(0, is.end);
	streampos end = is.tellg();
	//ParseCnodeChromo(is, beg, end, mRoot, true, false);
	rdp_model_spec(is, beg, mRoot);
	mRoot.BindTree(NULL);
	// Debug
	/*
	cout << "SetFromFile: parsed chromo [" << aFileName << "]"<< endl;
	if (IsError()) {
	    cout << "Error: " << mErr.mPos << " -- " << mErr.mText << endl;
	}
	DumpMnode(mRoot, 0); //!! Debug end
	*/
    } else {
	// Error reading the file
	mErr.Set(0, KPE_ErrOnReadingSpec);
    }
    return &mRoot;
}

THandle Chromo2Mdl::Set(const string& aUri)
{
    string path;
    ChromoUtils::GetPath(aUri, path);
    return SetFromFile(path);
}

THandle Chromo2Mdl::SetFromSpec(const string& aSpec)
{
    istringstream is(aSpec);
    is.seekg(0, is.beg);
    streampos beg = is.tellg();
    is.seekg(0, is.end);
    streampos end = is.tellg();
    //ParseCnodeChromo(is, beg, end, mRoot, true, false);
    rdp_model_spec(is, beg, mRoot);
    mRoot.BindTree(NULL);
    //DumpMnode(mRoot, 0);
    return &mRoot;
}

THandle Chromo2Mdl::Set(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    mRoot.CloneFrom(*node, true);
    mRoot.BindTree(NULL);
    return &mRoot;
}

THandle Chromo2Mdl::Init(TNodeType aRootType)
{
    mRoot.mMut.mR = GetMutRel(aRootType);
    return &mRoot;
}

void Chromo2Mdl::Reset()
{
    assert(false);
}

static void DumpIs(istream& aIs, streampos aStart, streampos aEnd)
{
    streampos curpos = aIs.tellg();
    aIs.seekg(aStart, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	cout << c;
	pos = aIs.tellg();
    }
    cout << endl;
    aIs.seekg(curpos, aIs.beg);
}

bool PassThroughText(istream& aIs, streampos aEnd)
{
    bool closed = false;
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if (c == KT_TextDelim) {
	    closed = true; break;
	}
	pos = aIs.tellg();
    }
    return closed;
}

bool IsGroupNested(char aGroupMark)
{
    return false;
}

bool IsTextDelimiter(char aChar)
{
    return (aChar == KT_TextDelim);
}

bool GetText(istream& aIs, streampos aEnd, string& aLex, char aDelimiter)
{
    bool res = false;
    streampos pos = aIs.tellg();
    char prev_c = '\x00';
    while (pos != aEnd) {
	char c = aIs.get();
	if (IsTextDelimiter(c) && (prev_c != KT_Escape)) {
	    res = true;
	    break;
	} else {
	    aLex.push_back(c);
	}
	prev_c = c;
    }
    return res;
}

void Chromo2Mdl::GetLexs(istream& aIs, streampos aBeg, streampos aEnd, vector<string>& aLexs)
{
    bool res = true;
    string lexeme;
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if ((c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
	    if (!lexeme.empty()) {
		aLexs.push_back(lexeme);
		lexeme.clear();
	    }
	} else if (IsTextDelimiter(c)) {
	    res = GetText(aIs, aEnd, lexeme, KT_TextDelim);
	    pos = aIs.tellg();
	    if (res) {
		aLexs.push_back(lexeme);
		lexeme.clear();
	    } else {
		mErr.Set(pos, KPE_MissingEndOfGroup);
		break;
	    }
	} else {
	    lexeme.push_back(c);
	}
	pos = aIs.tellg();
    }
    if (!lexeme.empty()) {
	aLexs.push_back(lexeme);
    }
}

void Chromo2Mdl::GetLexsPos(istream& aIs, streampos aBeg, streampos aEnd, TLexPos& aLexs)
{
    bool res = true;
    string lexeme;
    aIs.seekg(aBeg, aIs.beg);
    streampos pos = aIs.tellg();
    while (pos != aEnd) {
	char c = aIs.get();
	if ((c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
	    if (!lexeme.empty()) {
		// Set pos before the lexem start
		aLexs.push_back(TLexPosElem(lexeme, pos - (streampos) lexeme.size()));
		lexeme.clear();
	    }
	} else if (IsTextDelimiter(c)) {
	    pos = aIs.tellg();
	    res = GetText(aIs, aEnd, lexeme, KT_TextDelim);
	    if (res) {
		aLexs.push_back(TLexPosElem(lexeme, pos));
		lexeme.clear();
	    } else {
		mErr.Set(pos, KPE_MissingEndOfGroup);
		break;
	    }
	} else {
	    lexeme.push_back(c);
	}
	pos = aIs.tellg();
    }
    if (!lexeme.empty()) {
	aLexs.push_back(TLexPosElem(lexeme, pos));
    }
}

bool Chromo2Mdl::IsLexDmcMutop(const string& aLex) const
{
    bool res = false;
    for (auto lex : KDmcMutops) {
	if (aLex == lex) {
	    res = true; break;
	}
    }
    return res;
}

void Chromo2Mdl::DumpIsFrag(istream& aIs, streampos aStart, streampos aEnd)
{
    streampos curpos = aIs.tellg();
    streampos pos = aStart;
    aIs.seekg(pos, aIs.beg);
    while (pos != aEnd) {
	char c = aIs.get();
	cout << c;
	pos = aIs.tellg();
    };
    cout << endl;
    aIs.seekg(curpos, aIs.beg);
}

void Offset(int aLevel, int aIndent, ostream& aOs)
{
    for (int i = 0; i < aLevel*aIndent; i++)  aOs << " ";
}

/** @brief Checks if lexeme contains separators
 * */
bool ContainsSep(const string& aLex)
{
    bool res = false;
    for (auto c : aLex) {
	if (KSep.find(c) != string::npos) {
	    res = true; break;
	}
    }
    return res;
}

/** @brief Groups lexeme
 * */
string GroupLexeme(const string& aLex, bool aGroup)
{
    string res;
    //bool sep = ContainsSep(aLex);
    if (aGroup) {
	res = KT_TextDelim;
    }
    res += aLex;
    if (aGroup) {
	res += KT_TextDelim;
    }
    return res;
}

void Chromo2Mdl::OutputNode(const C2MdlNode& aNode, ostream& aOs, int aLevel, int aIndent)
{
    bool cnt = false;
    if (!aNode.mContext.empty()) {
	Offset(aLevel, aIndent, aOs); 
	for (TC2MdlCtxCiter it = aNode.mContext.begin(); it != aNode.mContext.end(); it++) {
	    aOs << it->second << " " << it->first << " "; cnt = true;
	}
    }

    bool mut = false;
    int cnum = aNode.mChromo.size();
    if (!aNode.mMut.mR.empty()) {
	if (!cnt) Offset(aLevel, aIndent, aOs);
	bool qstring = (aNode.mMut.mR == KMS_Cont) || (aNode.mMut.mR == KMS_Note);
	if (aNode.mMut.mP.empty()) aOs  << aNode.mMut.mR << " " << GroupLexeme(aNode.mMut.mQ, qstring);
	else aOs << aNode.mMut.mP << " " << aNode.mMut.mR << " " << GroupLexeme(aNode.mMut.mQ, qstring);
	if (cnum == 0) {
	    aOs << ";";
	}
	aOs << endl;
	mut = true;
    }

    if (cnum > 0) {
	if (!cnt || mut) Offset(aLevel, aIndent, aOs); aOs << "{" << endl;
	for (TC2MdlNodesCiter it = aNode.mChromo.begin(); it != aNode.mChromo.end(); it++) {
	    const C2MdlNode& node = *it;
	    OutputNode(node, aOs, aLevel + 1, aIndent);
	}
	Offset(aLevel, aIndent, aOs); aOs << "}" << endl;
    }
}

void Chromo2Mdl::DumpMnode(const C2MdlNode& aNode, int aLevel) const
{
    bool cnt = false;
    if (aNode.mOwner == NULL) {
	bool treeok = CheckTree(aNode);
	cout << "Tree integrity: " << (treeok ? "OK" : "NOK") << endl;
    }
    OutputNode(aNode, cout, aLevel, K_Indent);
}

bool Chromo2Mdl::CheckTree(const C2MdlNode& aNode) const
{
    bool res = true;
    for (TC2MdlNodesCiter it = aNode.mChromo.begin(); it != aNode.mChromo.end() && res; it++) {
	const C2MdlNode& comp = *it;
	res = comp.mOwner == &aNode;
	res = res && CheckTree(comp);
    }
    return res;
}

bool Chromo2Mdl::IsError() const
{
    return !mErr.mText.empty();
}

void Chromo2Mdl::TransfTl(const THandle& aHandle, const THandle& aSrc)
{
    C2MdlNode* node = aHandle.Data(node);
    
}

void Chromo2Mdl::SetErr(istream& aIs, const string& aDescr)
{
    streampos pos = aIs.tellg();
    mErr.Set(pos, aDescr);
}


// === Recursive descent parser ==

void Chromo2Mdl::rdp_name(istream& aIs, string& aRes)
{
    char c = aIs.get();
    aRes.clear();
    if (isalpha(c) || (c == '_')) {
	do {
	    aRes.push_back(c);
	    c = aIs.get();
	} while (isalnum(c) || c == '_');
	aIs.seekg(-1, aIs.cur);
    } else {
	SetErr(aIs, RDPE_WrongName);
    }
}

void Chromo2Mdl::rdp_string(istream& aIs, string& aRes)
{
    char c = aIs.get();
    if (c == KT_TextDelim ) {
	c = aIs.get();
	while (c != KT_TextDelim && c != KT_EOL) {
	    aRes.push_back(c);
	    c = aIs.get();
	}
	if (c != KT_TextDelim) {
	    SetErr(aIs, RDPE_MissingTextDelim);
	}
    } else {
	SetErr(aIs, RDPE_MissingTextDelim);
    }
}

void Chromo2Mdl::rdp_uri(istream& aIs, string& aRes)
{
    aRes.clear();
    string elem;
    streampos pos = aIs.tellg(); // debug
    rdp_name(aIs, elem);
    aRes.append(elem);
    if (!IsError()) {
	char c = aIs.get();
	while (c == KT_UriSep && !IsError()) {
	    aRes.push_back(c);
	    rdp_name(aIs, elem);
	    if (!IsError()) {
		aRes.append(elem);
		c = aIs.get();
	    }
	};
	aIs.seekg(-1, aIs.cur);
    } else {
	SetErr(aIs, RDPE_WrongUriElem);
    }
}

void Chromo2Mdl::rdp_mut(istream& aIs, C2MdlNode& aMnode)
{
    streampos pos = aIs.tellg();
    rdp_mut_comment(aIs, aMnode);
    if (IsError()) {
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
	rdp_mut_create(aIs, aMnode);
	if (IsError()) {
	    aIs.seekg(pos, aIs.beg); // Backtrack
	    ResetErr();
	    rdp_mut_content(aIs, aMnode);
	    if (IsError()) {
		aIs.seekg(pos, aIs.beg); // Backtrack
		ResetErr();
		rdp_mut_connect(aIs, aMnode);
		if (IsError()) {
		    aIs.seekg(pos, aIs.beg); // Backtrack
		    ResetErr();
		    rdp_mut_remove(aIs, aMnode);
		    if (IsError()) {
			aIs.seekg(pos, aIs.beg); // Backtrack
			ResetErr();
			rdp_mut_import(aIs, aMnode);
		    }
		}
	    }
	}
    }
    if (!IsError()) {
	char c = aIs.get();
	if (c == KT_MutSeparator) {
	} else {
	    SetErr(aIs, RDPE_MissingMutSep);
	    C2MdlNode* owner = aMnode.mOwner;
	    aMnode = C2MdlNode(); // Reset the node, to clean translated data
	    aMnode.mOwner = owner;
	}
    }
}

void Chromo2Mdl::rdp_chromo_node(istream& aIs, C2MdlNode& aMnode)
{
    streampos pos = aIs.tellg();
    C2MdlNode node;
    // Set owner by advance to support dependent mut (DMC)
    node.mOwner = &aMnode;
    rdp_ctx_mutation(aIs, node);
    if (IsError()) {
	aIs.seekg(pos, aIs.beg); // Backtrace
	ResetErr();
	rdp_segment_target(aIs, node, aMnode);
	if (IsError()) {
	    aIs.seekg(pos, aIs.beg); // Backtrack
	    ResetErr();
	    rdp_segment_namespace(aIs, node, aMnode);
	    if (IsError()) {
		aIs.seekg(pos, aIs.beg); // Backtrack
		ResetErr();
		rdp_mut_create_chromo(aIs, node);
		if (!IsError()) {
		    aMnode.mChromo.push_back(node);
		}
	    } else {
		aMnode.mChromo.push_back(node);
	    }
	} else {
	    aMnode.mChromo.push_back(node);
	}
    } else {
	aMnode.mChromo.push_back(node);
    }
}

void Chromo2Mdl::rdp_segment(istream& aIs, C2MdlNode& aMnode)
{
    streampos pos = aIs.tellg(); // Debug
    char c = aIs.get();
    if (c == KT_ChromoStart) {
	rdp_sep(aIs);
	if (!IsError()) {
	    rdp_chromo_node(aIs, aMnode);
	    if (!IsError()) {
		do {
		    rdp_sep(aIs);
		    if (!IsError()) {
			c = aIs.get();
			if (c == KT_ChromoEnd) {
			} else {
			    aIs.seekg(-1, aIs.cur);
			    rdp_chromo_node(aIs, aMnode);
			    if (!IsError()) {
			    }
			}
		    }
		} while (!IsError() && c != KT_ChromoEnd);
	    }
	}
    } else {
	SetErr(aIs, RDPE_MissingChromo);
    }
}

#if 0
void Chromo2Mdl::rdp_segment_target(istream& aIs, C2MdlNode& aMnode)
{
    string target;
    rdp_uri(aIs, target);
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    char c = aIs.get();
	    if (c == KTC_Target) {
		rdp_sep(aIs);
		if (!IsError()) {
		    aMnode.AddContext(string(1, KTC_Target), target);
		    rdp_segment(aIs, aMnode);
		}
	    } else {
		SetErr(aIs, RDPE_WrongCtxType);
	    }
	}
    }
}
#endif

void Chromo2Mdl::rdp_segment_target(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    rdp_context_target_ext(aIs, aMnode, aDepSeg);
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    rdp_segment(aIs, aMnode);
	}
    }
}


void Chromo2Mdl::rdp_segment_namespace(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    rdp_context_namespace_ext(aIs, aMnode, aDepSeg);
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    rdp_segment(aIs, aMnode);
	}
    }
}


void Chromo2Mdl::rdp_mut_create_chromo(istream& aIs, C2MdlNode& aMnode)
{
    rdp_mut_create(aIs, aMnode);
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    rdp_segment(aIs, aMnode);
	}
    }
}

void Chromo2Mdl::rdp_mut_create(istream& aIs, C2MdlNode& aMnode)
{
    string name;
    bool anon = false;
    streampos pos = aIs.tellg();
    rdp_name(aIs, name);
    if (IsError()) {
	// Anonymous creation
	anon = true;
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
    } else {
	rdp_sep(aIs);
    }
    if (!IsError()) {
	char c = aIs.get();
	if (c == KT_MutAdd) {
	    rdp_sep(aIs);
	    if (!IsError()) {
		string parent;
		rdp_uri(aIs, parent);
		if (!IsError()) {
		    if (anon) {
			streampos pos = aIs.tellg();
			name = GenerateName(parent, pos);
		    }
		    aMnode.mMut.mP = name;
		    aMnode.mMut.mR = string(1, KT_MutAdd);
		    aMnode.mMut.mQ = parent;
		}
	    }
	} else {
	    SetErr(aIs, RDPE_MissingMutSmbAdd);
	}
    }
}

void Chromo2Mdl::rdp_mut_connect(istream& aIs, C2MdlNode& aMnode)
{
    string name;
    rdp_uri(aIs, name);
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    char c = aIs.get();
	    if (c == KT_MutConn) {
		rdp_sep(aIs);
		if (!IsError()) {
		    // Fork uri vs mut_create vs segment_target
		    streampos pos = aIs.tellg();
		    C2MdlNode node;
		    // Set owner by advance to support dependent mut (DMC)
		    node.mOwner = &aMnode;
		    rdp_segment_target(aIs, node, *aMnode.mOwner);
		    //SetErr(aIs, RDPE_MissingMutSmb);
		    if (IsError()) {
			aIs.seekg(pos, aIs.beg); // Backtrack
			ResetErr();
			rdp_segment_namespace(aIs, node, *aMnode.mOwner);
			if (IsError()) {
			    aIs.seekg(pos, aIs.beg); // Backtrack
			    ResetErr();
			    rdp_mut_create_chromo(aIs, node);
			    if (IsError()) {
				aIs.seekg(pos, aIs.beg); // Backtrack
				ResetErr();
				rdp_mut_create(aIs, node);
				if (IsError()) {
				    aIs.seekg(pos, aIs.beg); // Backtrack
				    ResetErr();
				    string parent;
				    rdp_uri(aIs, parent);
				    if (!IsError()) {
					aMnode.mMut.mP = name;
					aMnode.mMut.mR = string(1, KT_MutConn);
					aMnode.mMut.mQ = parent;
				    }
				} else {
				    aMnode.mOwner->mChromo.push_back(node);
				    aMnode.mMut.mP = name;
				    aMnode.mMut.mR = string(1, KT_MutConn);
				    aMnode.mMut.mQ = node.mMut.mP;
				}
			    } else {
				aMnode.mOwner->mChromo.push_back(node);
				aMnode.mMut.mP = name;
				aMnode.mMut.mR = string(1, KT_MutConn);
				aMnode.mMut.mQ = node.mMut.mP;
			    }
			} else {
			    aMnode.mOwner->mChromo.push_back(node);
			    aMnode.mMut.mP = name;
			    aMnode.mMut.mR = string(1, KT_MutConn);
			    aMnode.mMut.mQ = node.mContext.at(KT_Namespace);
			}
		    } else {
			aMnode.mOwner->mChromo.push_back(node);
			aMnode.mMut.mP = name;
			aMnode.mMut.mR = string(1, KT_MutConn);
			aMnode.mMut.mQ = node.mContext.at(KT_Target);
		    }
		} else {
		    SetErr(aIs, RDPE_MissingMutSmb);
		}
	    }
	}
    }
}

void Chromo2Mdl::rdp_mut_content(istream& aIs, C2MdlNode& aMnode)
{
    streampos pos = aIs.tellg();
    char c = aIs.get();
    if (c == KT_MutContent) {
	rdp_sep(aIs);
	if (!IsError()) {
	    string value;
	    rdp_string(aIs, value);
	    if (!IsError()) {
		aMnode.mMut.mR = string(1, KT_MutContent);
		aMnode.mMut.mQ = value;
	    }
	}
    } else {
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
	string uri;
	rdp_uri(aIs, uri);
	if (!IsError()) {
	    rdp_sep(aIs);
	    if (!IsError()) {
		char c = aIs.get();
		if (c == KT_MutContent) {
		    rdp_sep(aIs);
		    if (!IsError()) {
			string value;
			rdp_string(aIs, value);
			if (!IsError()) {
			    aMnode.mMut.mP = uri;
			    aMnode.mMut.mR = string(1, KT_MutContent);
			    aMnode.mMut.mQ = value;
			}
		    }
		} else {
		    SetErr(aIs, RDPE_MissingMutSmb);
		}
	    }
	}
    }
}

void Chromo2Mdl::rdp_mut_comment(istream& aIs, C2MdlNode& aMnode)
{
    char c = aIs.get();
    if (c == KT_MutComment) {
	rdp_sep(aIs);
	if (!IsError()) {
	    string text;
	    rdp_string(aIs, text);
	    if (!IsError()) {
		aMnode.mMut.mR = string(1, KT_MutComment);
		aMnode.mMut.mQ = text;
	    }
	}
    } else {
	SetErr(aIs, RDPE_MissingMutSmb);
    }
}

void Chromo2Mdl::rdp_mut_import(istream& aIs, C2MdlNode& aMnode)
{
    char c = aIs.get();
    if (c == KT_MutImport) {
	rdp_sep(aIs);
	if (!IsError()) {
	    string name;
	    rdp_name(aIs, name);
	    if (!IsError()) {
		aMnode.mMut.mR = string(1, KT_MutImport);
		aMnode.mMut.mQ = name;
	    }
	}
    } else {
	SetErr(aIs, RDPE_MissingMutSmb);
    }
}



void Chromo2Mdl::rdp_mut_remove(istream& aIs, C2MdlNode& aMnode)
{
    char c = aIs.get();
    if (c == KT_MutRemove) {
	rdp_sep(aIs);
	if (!IsError()) {
	    string object;
	    rdp_name(aIs, object);
	    if (!IsError()) {
		aMnode.mMut.mR = string(1, KT_MutRemove);
		aMnode.mMut.mQ = object;
	    }
	}
    } else {
	SetErr(aIs, RDPE_MissingMutSmb);
    }
}

void Chromo2Mdl::rdp_ctx_mutation(istream& aIs, C2MdlNode& aMnode)
{
    streampos pos = aIs.tellg();
    rdp_context(aIs, aMnode);
    if (IsError()) {
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
	rdp_mut(aIs, aMnode);
    } else {
	rdp_sep(aIs);
	if (!IsError()) {
	    rdp_mut(aIs, aMnode);
	}
    }
}

void Chromo2Mdl::rdp_context_target(istream& aIs, C2MdlNode& aMnode)
{
    string uri;
    rdp_uri(aIs, uri);
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    char c = aIs.get();
	    if (c == KTC_Target) {
		aMnode.AddContext(KT_Target, uri);
	    } else {
		SetErr(aIs, RDPE_MissingCtxSmb);
	    }
	}
    }
}

#if 0
void Chromo2Mdl::rdp_context_target_ext(istream& aIs, C2MdlNode& aMnode)
{
    string uri;
    streampos pos = aIs.tellg();
    rdp_uri(aIs, uri);
    if (IsError()) {
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
    }
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    char c = aIs.get();
	    if (c == KTC_Target) {
		aMnode.AddContext(KT_Target, uri);
	    } else {
		SetErr(aIs, RDPE_MissingCtxSmb);
	    }
	}
    }
}
#endif

void Chromo2Mdl::rdp_context_target_ext(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    streampos pos = aIs.tellg();
    C2MdlNode node;
    bool ismut = false;
    rdp_mut_create(aIs, node);
    string uri;
    if (IsError()) {
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
	rdp_uri(aIs, uri);
    } else {
	ismut = true;
    }
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    char c = aIs.get();
	    if (c == KTC_Target) {
		if (ismut) {
		    aDepSeg.mChromo.push_back(node);
		    uri = node.mMut.mP;
		}
		aMnode.AddContext(KT_Target, uri);
	    } else {
		SetErr(aIs, RDPE_MissingCtxSmb);
	    }
	}
    }
}


void Chromo2Mdl::rdp_context_namespace_ext(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    streampos pos = aIs.tellg();
    C2MdlNode node;
    bool ismut = false;
    rdp_mut_create(aIs, node);
    string uri;
    if (IsError()) {
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
	rdp_uri(aIs, uri);
    } else {
	ismut = true;
    }
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    char c = aIs.get();
	    if (c == KTC_Namespace) {
		if (ismut) {
		    aDepSeg.mChromo.push_back(node);
		    uri = node.mMut.mP;
		}
		aMnode.AddContext(KT_Namespace, uri);
	    } else {
		SetErr(aIs, RDPE_MissingCtxSmb);
	    }
	}
    }
}

void Chromo2Mdl::rdp_context_namespace(istream& aIs, C2MdlNode& aMnode)
{
    string uri;
    rdp_uri(aIs, uri);
    if (!IsError()) {
	rdp_sep(aIs);
	if (!IsError()) {
	    char c = aIs.get();
	    if (c == KTC_Namespace) {
		aMnode.AddContext(KT_Namespace, uri);
	    } else {
		SetErr(aIs, RDPE_MissingCtxSmb);
	    }
	}
    }
}

void Chromo2Mdl::rdp_context(istream& aIs, C2MdlNode& aMnode)
{
    streampos pos = aIs.tellg();
    rdp_context_target(aIs, aMnode);
    if (IsError()) {
	aIs.seekg(pos, aIs.beg); // Backtrack
	ResetErr();
	rdp_context_namespace(aIs, aMnode);
    } else {
	pos = aIs.tellg();
	rdp_sep(aIs);
	if (!IsError()) {
	    rdp_context_namespace(aIs, aMnode);
	    if (IsError()) {
		aIs.seekg(pos, aIs.beg); // Backtrack
		ResetErr();
	    }
	}
    }
}

void Chromo2Mdl::rdp_model_spec(istream& aIs, streampos aStart, C2MdlNode& aMnode)
{
    aIs.seekg(aStart, aIs.beg);
    rdp_mut_create_chromo(aIs, aMnode);
}

void Chromo2Mdl::rdp_sep(istream& aIs)
{
    streampos pos = aIs.tellg(); // Debug
    char c = aIs.get();
    if (IsSep(c)) {
	do {
	    c = aIs.get();
	} while (IsSep(c));
	aIs.seekg(-1, aIs.cur);
    } else {
	streampos pos = aIs.tellg();
	mErr.Set(pos, RDPE_SepMissing);
    }
}






Chromo2::Chromo2(): mRootNode(mMdl, THandle())
{
}

Chromo2::Chromo2(const Chromo2& aSrc)
{
    Set(aSrc.Root());
}

void Chromo2::SetFromFile(const string& aFileName)
{
    THandle root = mMdl.SetFromFile(aFileName);
    C2MdlNode* nroot = root.Data(nroot);
    mRootNode = ChromoNode(mMdl, root);
}

bool Chromo2::Set(const string& aUri)
{
    bool res = false;
    THandle root = mMdl.Set(aUri);
    if (root != NULL) {
	mRootNode = ChromoNode(mMdl, root);
	res = true;
    }
    return res;
}

bool Chromo2::SetFromSpec(const string& aSpec)
{
    bool res = false;
    if (!mRootNode.IsNil()) {
	// Clear old mutation // TODO to re-design
	ChromoNode::Iterator mit = mRootNode.Begin();
	while (mit != mRootNode.End()) {
	    ChromoNode node = *mit;
	    mRootNode.RmChild(node);
	    mit = mRootNode.Begin();
	}
    }
    THandle root = mMdl.SetFromSpec(aSpec);
    if (root != NULL) {
	mRootNode = ChromoNode(mMdl, root);
	res = true;
    }
    return res;

}

void Chromo2::Set(const ChromoNode& aRoot)
{
    THandle root = mMdl.Set(aRoot.Handle());
    mRootNode = ChromoNode(mMdl, root);
    //int cnt = mRootNode.Count();
    //cout << cnt << endl;
}

Chromo2::~Chromo2()
{
}

ChromoNode& Chromo2::Root()
{
    return mRootNode;
}

const ChromoNode& Chromo2::Root() const
{
    return mRootNode;
}

void Chromo2::Reset()
{
    mMdl.Reset();
}

void Chromo2::Init(TNodeType aRootType)
{
    THandle root = mMdl.Init(aRootType);
    mRootNode = ChromoNode(mMdl, root);
}

void Chromo2::Save(const string& aFileName, int aIndent) const
{
    // We cannot simply save the doc (mMdl.iDoc) because it will save not only root node but
    // also adjacent nodes. So we need to create new model and add to doc only one separated root
    Chromo2Mdl mdl;
    mdl.Set(mRootNode.Handle());
    mdl.Save(aFileName, aIndent);
}

ChromoNode Chromo2::CreateNode(const THandle& aHandle)
{
    return ChromoNode(mMdl, aHandle);
}

void Chromo2::ReduceToSelection(const ChromoNode& aSelNode)
{
    ChromoNode sel = aSelNode;
    ChromoNode prnt = *sel.Parent();
    while (prnt != ChromoNode()) {
	prnt.ReduceToSelection(sel);
	sel = prnt;
	prnt = *sel.Parent();
    }
}

bool Chromo2::GetSpec(string& aSpec)
{
    return mMdl.ToString(mRootNode.Handle(), aSpec);
}

bool Chromo2::IsError() const
{
    return !mMdl.Error().mText.empty();
}

const CError& Chromo2::Error() const
{
    return mMdl.Error();
}

void Chromo2::Convert(const MChromo& aSrc)
{
    ChromoNode sroot = aSrc.Root();
    Init(sroot.Type());
    ConvertNode(mRootNode, sroot);
}

/** @brief Escape control symbols */
string EscapeCtrls(const string& aData, const string& aCtrls)
{
    string res;
    for (auto c : aData) {
	if (aCtrls.find(c) != string::npos || c == KT_Escape) {
	    res.push_back(KT_Escape);
	}
	res.push_back(c);
    }
    return res;
}

void ConvertAttr(ChromoNode& aDst, const ChromoNode& aSrc, TNodeAttr aAttr)
{
    if (aSrc.AttrExists(aAttr)) {
	string escSrc = EscapeCtrls(aSrc.Attr(aAttr), K_Ctrls);
	aDst.SetAttr(aAttr, escSrc);
    }
}

void Chromo2::ConvertNode(ChromoNode& aDst, const ChromoNode& aSrc)
{
    ConvertAttr(aDst, aSrc, ENa_Id);
    ConvertAttr(aDst, aSrc, ENa_Parent);
    ConvertAttr(aDst, aSrc, ENa_Targ);
    ConvertAttr(aDst, aSrc, ENa_MutNode);
    ConvertAttr(aDst, aSrc, ENa_MutVal);
    ConvertAttr(aDst, aSrc, ENa_Ref);
    ConvertAttr(aDst, aSrc, ENa_NS);
    ConvertAttr(aDst, aSrc, ENa_P);
    ConvertAttr(aDst, aSrc, ENa_Q);
    if (aSrc.Type() == ENt_Cont && (!aSrc.AttrExists(ENa_Id) || aSrc.Attr(ENa_Id).empty())) {
	// Default content
	aDst.SetAttr(ENa_Id, KT_Default);
    }
    for (ChromoNode::Const_Iterator it = aSrc.Begin(); it != aSrc.End(); it++) {
	const ChromoNode& scomp = *it;
	ChromoNode dcomp = aDst.AddChild(scomp.Type());
	ConvertNode(dcomp, scomp);
    }
}

void Chromo2::TransformLn(const MChromo& aSrc)
{
    ChromoNode sroot = aSrc.Root();
    Init(sroot.Type());
    //mRootNode.TransfTl(sroot);
    TransfTlNode(mRootNode, sroot, true);
}

bool IsTarg(const ChromoNode& aNode, const ChromoNode& aTarg, ChromoNode& aBase)
{
    bool res = false;
    if (aTarg.Type() == ENt_Node) {
	string ntarg = aNode.Attr(ENa_Targ);
	GUri nuri(ntarg);
	GUri turi;
	aTarg.GetUri(turi, aBase);
	res = (nuri == turi);
    }
    return res;
}

/** @brief Checks if adjacent node is same segment (has same targ) as given node
 * @parem  aNode  given node
 * @parem  aAdj   adjacent node
 * @parem  aBase   owner of given node
 *
 * */
bool IsSeg(const ChromoNode& aNode, const ChromoNode& aAdj, ChromoNode& aBase)
{
    bool res = false;
    if (aNode.AttrExists(ENa_Targ)) {
	string nnode = aNode.Attr(ENa_Targ);
	GUri nuri(nnode);
	GUri adjUri;
	aAdj.GetTarget(adjUri, aBase);
	res = (nuri == adjUri);
    }
    return res;
}

/** @brief Checks if adjacent node is owning segment (owns targ) as given node
 * @parem  aNode  given node
 * @parem  aAdj   adjacent node
 * @parem  aBase   owner of given node
 *
 * */
bool IsOwningSeg(const ChromoNode& aNode, const ChromoNode& aAdj, ChromoNode& aBase, GUri& aTail)
{
    bool res = false;
    if (aNode.AttrExists(ENa_Targ)) {
	string nnode = aNode.Attr(ENa_Targ);
	GUri nuri(nnode);
	GUri adjUri;
	aAdj.GetTarget(adjUri, aBase);
	res = (!adjUri.elems().empty() && adjUri <= nuri);
	if (res) {
	    nuri.tail(adjUri, aTail);
	}
    }
    return res;
}



/** @brief Gets adjacent node mutation corresponding to the given node target
 * @param  aNode  given node
 * @param  aTarg  adjacent node - candidate for the corresponding node
 * @param  aBase  base node, owner of aNode
 * */
ChromoNode getAdjNode(const ChromoNode& aNode, ChromoNode& aTarg, ChromoNode& aBase)
{
    ChromoNode res(aTarg.Mdl(), THandle());
    if (IsTarg(aNode, aTarg, aBase)) {
	res = aTarg;
    } else if (aTarg.Count() > 0) {
	ChromoNode targ = *(aTarg.Rbegin());
	res = getAdjNode(aNode, targ, aBase);
    }
    return res;
}

/** @brief Gets the chromo adjacent segment */
ChromoNode getAdjSeg(const ChromoNode& aNode, ChromoNode& aTarg, ChromoNode& aBase)
{
    ChromoNode res(aTarg.Mdl(), THandle());
    if (IsSeg(aNode, aTarg, aBase)) {
	res = aTarg;
    } else if (aTarg.Count() > 0) {
	ChromoNode targ = *(aTarg.Rbegin());
	res = getAdjSeg(aNode, targ, aBase);
    }
    return res;
}

/** @brief Gets the chromo adjacent owning segment */
ChromoNode getAdjOwnSeg(const ChromoNode& aNode, ChromoNode& aTarg, ChromoNode& aBase, GUri& aTail)
{
    ChromoNode res(aTarg.Mdl(), THandle());
    if (IsOwningSeg(aNode, aTarg, aBase, aTail)) {
	res = aTarg;
    } else if (aTarg.Count() > 0) {
	ChromoNode targ = *(aTarg.Rbegin());
	ChromoNode cres = getAdjOwnSeg(aNode, targ, aBase, aTail);
	if (!cres.IsNil()) {
	    res = cres;
	}
    }
    return res;
}


// TODO To optimize. Seems the cases  with getAdjSeg and getAdjOwnSeg can be combined
void Chromo2::TransfTlNode(ChromoNode& aDst, const ChromoNode& aSrc, bool aTarg)
{
    ConvertAttr(aDst, aSrc, ENa_Id);
    ConvertAttr(aDst, aSrc, ENa_Parent);
    if (aTarg) ConvertAttr(aDst, aSrc, ENa_Targ);
    ConvertAttr(aDst, aSrc, ENa_MutVal);
    ConvertAttr(aDst, aSrc, ENa_Ref);
    ConvertAttr(aDst, aSrc, ENa_NS);
    ConvertAttr(aDst, aSrc, ENa_P);
    ConvertAttr(aDst, aSrc, ENa_Q);
    auto it = aSrc.Begin();
    if (it != aSrc.End()) {
	auto scomp = *it;
	ChromoNode dcomp = aDst.AddChild(scomp.Type());
	TransfTlNode(dcomp, scomp, true);
	ChromoNode sprev = dcomp; // Previous comp
	it++;
	for (; it != aSrc.End(); it++) {
	    scomp = *it;
	    // Try to find corresponding node in adjacent mutation and add cur mut to it
	    ChromoNode targ = getAdjNode(scomp, sprev, aDst);
	    if (!targ.IsNil()) {
		dcomp = targ.AddChild(scomp.Type());
		TransfTlNode(dcomp, scomp, false);
	    } else {
		// Try to find corresponging segment in adjacent mutation and add cur mut to it
		targ = getAdjSeg(scomp, sprev, aDst);
		if (!targ.IsNil()) {
		    if (targ.Type() == ENt_Seg) {
			dcomp = targ.AddChild(scomp.Type());
			TransfTlNode(dcomp, scomp, false);
		    } else {
			// Target is just simple mut. Adding segment.
			ChromoNode targOwner = *targ.Parent(); 
			//targOwner.RmChild(targ, true);
			ChromoNode seg = targOwner.AddChild(ENt_Seg);
			seg.SetAttr(ENa_Targ, targ.Attr(ENa_Targ));
			ChromoNode newTarg = seg.AddChild(targ, true, true);
			newTarg.RmAttr(ENa_Targ);
			targOwner.RmChild(targ);
			dcomp = seg.AddChild(scomp.Type());
			TransfTlNode(dcomp, scomp, false);
			sprev = seg;
		    }
		} else {
		    GUri shiftUri;
		    targ = getAdjOwnSeg(scomp, sprev, aDst, shiftUri);
		    if (!targ.IsNil()) {
			if (targ.Type() == ENt_Seg) {
			    dcomp = targ.AddChild(scomp.Type());
			    TransfTlNode(dcomp, scomp, false);
			    dcomp.SetAttr(ENa_Targ, shiftUri);
			} else {
			    // Target is just simple mut. Adding segment.
			    ChromoNode targOwner = *targ.Parent(); 
			    ChromoNode seg = targOwner.AddChild(ENt_Seg);
			    seg.SetAttr(ENa_Targ, targ.Attr(ENa_Targ));
			    ChromoNode newTarg = seg.AddChild(targ, true, true);
			    newTarg.RmAttr(ENa_Targ);
			    targOwner.RmChild(targ);
			    dcomp = seg.AddChild(scomp.Type());
			    TransfTlNode(dcomp, scomp, false);
			    dcomp.SetAttr(ENa_Targ, shiftUri);
			    sprev = seg;
			}
		    } else {
			dcomp = aDst.AddChild(scomp.Type());
			TransfTlNode(dcomp, scomp, true);
			sprev = dcomp;
		    }
		}
	    }
	}
    }
}


