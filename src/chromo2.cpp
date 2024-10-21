
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>

#include "chromo2.h"


static constexpr bool _CHR2_DEBUG = false;

#define dbg if (!_CHR2_DEBUG) {} else std::cout << "CHR2 " << __func__  << ", "

/** @brief Chromo grammar terminals
 * */
const char KT_MutSeparator = ';';
const char KT_ChromoStart = '{';
const char KT_ChromoEnd = '}';
const char KT_NsSegStart = '(';
const char KT_NsSegEnd = ')';
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
const char KT_Target = '<';
const char KT_Namespace = '@';
const char KT_Space = ' ';
const char KT_SpecialNameNil = '_';

const string KTS_ChromoStart = string(1, KT_ChromoStart);
const string KTS_ChromoEnd = string(1, KT_ChromoEnd);
const string KTS_NsSegStart = string(1, KT_NsSegStart);
const string KTS_NsSegEnd = string(1, KT_NsSegEnd);
const string KTS_Space = string(1, KT_Space);
const string KTS_Target = string(1, KT_Target);
const string KTS_Namespace = string(1, KT_Namespace);

/** @brief Mutation symbols */
const string KMS_Add = ":";
const string KMS_Cont = "=";
const string KMS_Note = "#";
const string KMS_Rename = "|";
const string KMS_Remove = "!";
const string KMS_Import = "+";
const string KMS_Conn = "~";
const string KMS_Disconn = "!~";

/** @brief Anonymous ID prefix */
const string K_Aid_Prefix = "ID__";

/** @brief Key symbols */
const string KKeySymbols = {
    KT_MutSeparator,
    KT_ChromoStart,
    KT_ChromoEnd,
    KT_TextDelim,
    KT_Escape,
    KT_MutAdd,
    KT_MutConn,
    KT_MutComment,
    KT_MutImport,
    KT_MutContent,
    KT_MutRemove,
    KT_EOL,
    KT_Target,
    KT_Namespace,
};

/** @brief Mutation symbols */
const string KMutSymbols = {
    KT_MutAdd,
    KT_MutConn,
    KT_MutComment,
    KT_MutImport,
    KT_MutContent,
    KT_MutRemove,
};

/** @brief Keywolrd  of mutation allowing empty P */
const string KMutSymbolsEp = {
    KT_MutAdd,
    KT_MutComment,
    KT_MutImport,
    KT_MutContent,
};

/** @brief Keywolrd  of mutation requesting non-empty P */
const string KMutSymbolsRp = {
    KT_MutAdd,
    KT_MutConn,
    KT_MutContent,
    KT_MutRemove,
};




/** @brief Separators **/
const string KSep = " \t\n\r";
/** @brief Standard control symbols **/
const string K_Ctrls = {KT_TextDelim};

/** @brief Default chromo indentation */
const int K_Indent = 4;



/** @brief Default P part */
//static const string KT_Default = "$";
static const string KT_Default = "";

static const string KR_CREATE = ":";

/** @brief Recursive Descent Parser (RDP) errors */
static const string RDPE_FailedReadingSpec = "Error on reading chromo";
static const string RDPE_WrongContentUri = "Wrong content Uri/name";
static const string RDPE_WrongContentValue = "Wrong content value";
static const string RDPE_MissingMutSep = "Missing mut separator";
static const string RDPE_Unspecified = "Unspecified error";
static const string RDPE_WrongCommentString = "Wrong comment string";
static const string RDPE_MissingSepBeforeMut = "Missing separator before mutation symbol";
static const string RDPE_WrongMutLeftPart = "Wrong mutation left part";
static const string RDPE_UnrecognizedChrNode = "Unrecognized segment node";

/** @brief Segment offset when node output */
const int KA_OutOffset = 3;

using namespace std;

/** @brief Mapping for context: attr to ctx type
 * */
const map<TNodeAttr, string> KM_CtxAttToType = {
    {ENa_Targ, KTS_Target},
    {ENa_NS, KTS_Namespace}
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


bool C2Mut::operator==(const C2Mut& b) const
{
    return mP == b.mP && mQ == b.mQ && mR == b.mR;
}



C2MdlNode::C2MdlNode(): mOwner(NULL), mQnode(NULL), mChromoPos(-1)
{
}

C2MdlNode::C2MdlNode(C2MdlNode* aOwner): mOwner(aOwner), mQnode(NULL), mChromoPos(-1)
{
}

C2MdlNode::C2MdlNode(const C2MdlNode& aSrc): mOwner(aSrc.mOwner), mCtxType(aSrc.mCtxType), mContext(aSrc.mContext), mMut(aSrc.mMut), mChromo(aSrc.mChromo),
    mQnode(NULL), mChromoPos(aSrc.mChromoPos)
{
}

C2MdlNode::C2MdlNode(const C2MdlNode& aSrc, C2MdlNode* aOwner): mOwner(aOwner), mCtxType(aSrc.mCtxType), mContext(aSrc.mContext), mMut(aSrc.mMut),
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

C2MdlNode& C2MdlNode::operator=(const C2MdlNode& aSrc)
{
    mCtxType = aSrc.mCtxType;
    mContext = aSrc.mContext;
    mMut = aSrc.mMut;
    //mChromo = aSrc.mChromo;
    mChromo.clear();
    for (auto sitem : aSrc.mChromo) {
	C2MdlNode item(this);
	mChromo.push_back(item);
	mChromo.back() = sitem;
    }
    mQnode = nullptr;
    return *this;
}

C2MdlNode::~C2MdlNode()
{
    if (mQnode) {
	delete mQnode;
	mQnode = nullptr;
    }
}

void C2MdlNode::CloneFrom(const C2MdlNode& aSrc, bool aChromo)
{
    mCtxType = aSrc.mCtxType;
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
    mCtxType = aType;
    mContext = aValue;
}

void C2MdlNode::RmContext(TNodeAttr aAttr)
{
    assert(KM_CtxAttToType.count(aAttr) > 0);
    string crel = KM_CtxAttToType.at(aAttr);
    if (crel == mCtxType) {
	mCtxType.clear();
    }
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
	res = mCtxType == rel;
    }
    return res;
}

void C2MdlNode::AddQnode(const C2MdlNode& aNode)
{
    mQnode = new C2MdlNode(aNode);
}

bool C2MdlNode::IsChildOf(const C2MdlNode* aParent) const
{
    const C2MdlNode* parent = mOwner;
    while (parent && parent != aParent) {
	parent = parent->mOwner;
    }
    return (parent == aParent);
}

void C2MdlNode::Reset()
{
    if (mQnode) {
	delete mQnode;
	mQnode = nullptr;
    }
    mContext.clear();
    mMut = C2Mut();
    mChromo.clear();
    mChromoPos = -1;
}

bool C2MdlNode::operator==(const C2MdlNode& b) const
{
    bool res = (mMut == b.mMut);
    if (res) {
	res = (mChromo == b.mChromo);
    }
    if (res) {
	res = (!mQnode && !b.mQnode) || (mQnode && b.mQnode && mQnode == b.mQnode);
    }
    return res;
}



// **** Chromo2Mdl *****


Chromo2Mdl::Chromo2Mdl()
{
}

Chromo2Mdl::Chromo2Mdl(const Chromo2Mdl& aSrc): mRoot(aSrc.mRoot)
{
}

Chromo2Mdl& Chromo2Mdl::operator=(const Chromo2Mdl& aSrc)
{
    mRoot = aSrc.mRoot;
    return *this;
}

Chromo2Mdl::~Chromo2Mdl()
{
}

void Chromo2Mdl::ResetErr()
{
    mErr = CError();
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
    return K_Aid_Prefix + aParent + string("_") + to_string(aPos);
}

TNodeType Chromo2Mdl::GetType(const THandle& aHandle) {
    TNodeType res = ENt_Unknown;
    C2MdlNode* node = aHandle.Data(node);
    string rel = node->mMut.mR;
    if (rel.empty()) {
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

THandle Chromo2Mdl::Next(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    assert(node->mOwner != NULL);
    C2MdlNode* next = node->mOwner->GetNextComp(node);
    return next;
}

THandle Chromo2Mdl::Prev(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode* prev = node->mOwner->GetPrevComp(node);
    return prev;
}

THandle Chromo2Mdl::GetFirstChild(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    C2MdlNode *comp = NULL;
    if (node->mChromo.size() > 0) {
       comp = &(node->mChromo.front());
    }
    return comp;
}

THandle Chromo2Mdl::GetLastChild(const THandle& aHandle)
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
	if (aNode.mCtxType == rel) {
	    res = aNode.mContext;
	}
    }
    return res;
}

#if 0
string Chromo2Mdl::GetContextByAttr(const C2MdlNode& aNode, TNodeAttr aAttr)
{
    string res;
    if (KM_CtxAttToType.count(aAttr) > 0) {
	string rel = KM_CtxAttToType.at(aAttr);
	if (aNode.mCtxType == rel) {
	    if (aNode.mContext.empty()) {
		// Implicit context
		if (aNode.mMut.mR == KMS_Add) {
		    res = aNode.mMut.mP;
		} else {
		    assert(false);
		}
	    } else {
		// Explicit context
		res = aNode.mContext;
	    }
	}
    }
    return res;
}
#endif

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
    } else if (aAttr == ENa_MutNode) {
	assert(rel == KMS_Add || rel == KMS_Remove || rel == KMS_Cont || rel == KMS_Conn || rel == KMS_Disconn || rel.empty());
	if (rel == KMS_Add || rel.empty()) {
	    res = GetContextByAttr(*node, aAttr);
	} else if (rel == KMS_Cont) {
	    res = node->mMut.mP;
            /*
	} else if (rel == KMS_Remove) {
	    res = node->mMut.mQ;
            */
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
	res = (rel == KMS_Add) || rel == KMS_Cont || (rel == KMS_Import) || (rel == KMS_Remove);
    } else if (aAttr == ENa_Parent) {
	res = (rel == KMS_Add);
    } else if (aAttr == ENa_Targ || aAttr == ENa_NS) {
	res = node->ExistsContextByAttr(aAttr);
    } else if (aAttr == ENa_MutVal) {
	res = (rel == KMS_Cont || rel == KMS_Note || rel == KMS_Rename);
    } else if (aAttr == ENa_MutNode) {
        res = ((rel == KMS_Add) || (rel == KMS_Cont) || rel.empty()) && node->ExistsContextByAttr(ENa_MutNode);
    } else if (aAttr == ENa_P) {
	    res = ((rel == KMS_Conn || rel == KMS_Disconn) && !node->mMut.mP.empty());
    } else if (aAttr == ENa_Q) {
	    res = ((rel == KMS_Conn || rel == KMS_Disconn) && !node->mMut.mQ.empty());
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

bool Chromo2Mdl::IsChildOf(const THandle& aNode, const THandle& aParent)
{
    C2MdlNode* parent = aParent.Data(parent);
    C2MdlNode* node = aNode.Data(parent);
    return node->IsChildOf(parent);
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
	assert (rel == KMS_Add || rel == KMS_Remove || rel == KMS_Cont || rel == KMS_Import);
	if (rel == KMS_Add || rel == KMS_Cont) {
	    node->mMut.mP = aVal;
	} else if (rel == KMS_Remove) {
	    node->mMut.mQ = aVal;
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
	node->mCtxType = ctxrel;
	node->mContext = aVal;
    } else if (aType == ENa_MutNode) {
	assert (rel == KMS_Remove || rel == KMS_Conn || rel == KMS_Disconn);
	if (rel == KMS_Conn || rel == KMS_Disconn) {
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
    } else if (aType == ENa_NS) {
	string ctxrel = GetCtxRel(aType);
	assert (!ctxrel.empty());
	node->mCtxType = ctxrel;
	node->mContext = aVal;
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
	if (crel == node->mCtxType) {
	    node->mContext.clear();
	    node->mCtxType.clear();
	}
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
    ostringstream os;
    OutputNode(mRoot, os, 0, K_Indent);
    aString = os.str();
    return true;
}

void Chromo2Mdl::Save(const string& aFileName, int aIndent) const
{
    ofstream os(aFileName, ofstream::out);
    OutputNode(mRoot, os, 0, aIndent == 0 ? K_Indent : aIndent);
    os.flush();
    os.close();
}

int Chromo2Mdl::GetLineId(const THandle& aHandle) const
{
    C2MdlNode* node = aHandle.Data(node);
    return node->mChromoPos;
}

bool Chromo2Mdl::getName(const string& aFileName, string& aName)
{
    bool res = false;
    ifstream is(aFileName, std::ifstream::in);
    if ( (is.rdstate() & ifstream::failbit ) == 0 ) {
	Reset();
	is.seekg(0, is.beg);
	res = rdp_mut_create_chromo_name(is, aName);
    } else {
	SetErr(RDPE_FailedReadingSpec);
    }
    return res;
}

// TODO to support pure "segment" chromo, ref DS_ISS_005 
THandle Chromo2Mdl::SetFromFile(const string& aFileName)
{
    ifstream is(aFileName, std::ifstream::in);
    if ( (is.rdstate() & ifstream::failbit ) == 0 ) {
	Reset();
	is.seekg(0, is.beg);
	streampos beg = is.tellg();
	is.seekg(0, is.end);
	streampos end = is.tellg();
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
	SetErr(RDPE_FailedReadingSpec);
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
    rdp_model_spec(is, beg, mRoot);
    mRoot.BindTree(NULL);
    //DumpMnode(mRoot, 0);
    return &mRoot;
}

THandle Chromo2Mdl::Set(const THandle& aHandle)
{
    C2MdlNode* node = aHandle.Data(node);
    if (node) {
	mRoot.CloneFrom(*node, true);
	mRoot.BindTree(NULL);
    } else {
	mRoot = C2MdlNode();
    }
    return &mRoot;
}

THandle Chromo2Mdl::Init(TNodeType aRootType)
{
    mRoot.mMut.mR = GetMutRel(aRootType);
    return &mRoot;
}

void Chromo2Mdl::Reset()
{
    mRoot.Reset();
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

string EscapeCtrls(const string& aData, const string& aCtrls);

/** @brief Groups lexeme
 * */
string GroupLexeme(const string& aLex, bool aGroup)
{
    string res;
    //bool sep = ContainsSep(aLex);
    if (aGroup) {
	res = KT_TextDelim;
    }
    res += EscapeCtrls(aLex, "\"");
    if (aGroup) {
	res += KT_TextDelim;
    }
    return res;
}

bool Chromo2Mdl::IsAnonymousId(const string& aId)
{
    bool res;
    res = (aId.compare(0, K_Aid_Prefix.size(), K_Aid_Prefix)) == 0;
    return res;
}

void Chromo2Mdl::OutputNode(const C2MdlNode& aNode, ostream& aOs, int aLevel, int aIndent, bool aIndFl)
{
    bool cnt = false;
    bool cnt_ext = false;
    if (!aNode.mCtxType.empty()) {
	if (!aNode.mContext.empty()) {
	    if (aIndFl) { Offset(aLevel, aIndent, aOs); }
            if (aNode.mCtxType == KTS_Namespace) {
                // For namespace we use specific segment so no need to output ctx symbol
                aOs << aNode.mContext << KTS_Space; cnt = true;
            } else {
                aOs << aNode.mContext << KTS_Space << aNode.mCtxType << KTS_Space; cnt = true;
            }
	} else {
	    cnt_ext = true;
	}
    }

    bool mut = false;
    int cnum = aNode.mChromo.size();
    if (!aNode.mMut.mR.empty()) {
	if (!cnt) { if (aIndFl) Offset(aLevel, aIndent, aOs); }
	bool qstring = (aNode.mMut.mR == KMS_Cont) || (aNode.mMut.mR == KMS_Note);
	string mutp = (aNode.mMut.mP.empty() || IsAnonymousId(aNode.mMut.mP)) ? string() : (aNode.mMut.mP + KTS_Space);
	aOs << mutp << aNode.mMut.mR << KTS_Space << GroupLexeme(aNode.mMut.mQ, qstring);
	if (cnum == 0) {
	    //aOs << ";";
	    //aOs << endl;
	}
	//aOs << endl;
	mut = true;
    }

    if (cnt_ext) {
        if (aNode.mCtxType != KTS_Namespace) {
            // For namespace we use specific segment so no need to output ctx symbol
            aOs << KTS_Space << aNode.mCtxType << KTS_Space;
        }
    }

    if (cnum > 0) {
	if (aNode.mMut.mR == KMS_Conn && aNode.mMut.mQ.empty() && aNode.mChromo.size() == 1) {
	    // Conn mut dependent node
	    const C2MdlNode& node = *(aNode.mChromo.begin());
	    OutputNode(node, aOs, aLevel, aIndent, false);
	} else {
	    if (!cnt && !cnt_ext && !mut) { Offset(aLevel, aIndent, aOs);
	    } else  { aOs << KTS_Space; }
            if (aNode.mCtxType == KTS_Namespace) {
                aOs << KTS_NsSegStart << endl;
            } else {
                aOs << KTS_ChromoStart << endl;
            }
	    for (TC2MdlNodesCiter it = aNode.mChromo.begin(); it != aNode.mChromo.end(); it++) {
		const C2MdlNode& node = *it;
		OutputNode(node, aOs, aLevel + 1, aIndent);
		aOs << endl;
	    }
	    Offset(aLevel, aIndent, aOs);
            if (aNode.mCtxType == KTS_Namespace) {
                aOs << KTS_NsSegEnd;
            } else {
                aOs << KTS_ChromoEnd;
            }
	}
    }

    if (aNode.mMut.mR.empty() && cnum == 0) {
	Offset(aLevel, aIndent, aOs); aOs << KTS_ChromoStart << KTS_Space << KTS_ChromoEnd << endl;
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
    cout << endl;
    cout.flush();
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

void Chromo2Mdl::SetErr(const string& aDescr)
{
    streampos pos = 0;
    mErr.Set(pos, aDescr);
}


// === Recursive descent parser ==

void Chromo2Mdl::rdpBacktrack(istream& aIs, streampos aPos)
{
    aIs.seekg(aPos, aIs.beg); // Backtrack
}

bool Chromo2Mdl::rdp_name(istream& aIs, string& aRes)
{
    bool res = true;
    char c = aIs.get();
    aRes.clear();
    if (isalpha(c)) {
	do {
	    aRes.push_back(c);
	    c = aIs.get();
	} while (isalnum(c) || c == '_');
	aIs.seekg(-1, aIs.cur);
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_word_sep(istream& aIs)
{
    bool res = false;
    streampos pos = aIs.tellg(); // Debug
    char c = aIs.get();
    if (!IsSep(c)) {
	do {
	    c = aIs.get();
	} while (!IsSep(c) && c != EOF);
    }
    if (IsSep(c) && c != EOF) {
	do {
	    c = aIs.get();
	} while (IsSep(c) && c != EOF);
	if (c != EOF) {
	    aIs.seekg(-1, aIs.cur);
	    res = true;
	}
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_word(istream& aIs)
{
    bool res = false;
    char c = aIs.get();
    while (!IsSep(c) && !IsSymbOf(c, KKeySymbols) && c != EOF) {
	c = aIs.get();
    }
    if (c != EOF) {
	aIs.seekg(-1, aIs.cur);
    }
    return true;
}

bool Chromo2Mdl::rdp_spname_ns(istream& aIs, string& aRes)
{
    bool res = true;
    char c1 = aIs.get(), c2 = aIs.get();
    aRes.clear();
    if (c1 == '_' && c2 == '@') {
	aRes.push_back(c1);
	aRes.push_back(c2);
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_spname_nil(istream& aIs, string& aRes)
{
    bool res = false;
    char c1 = aIs.get();
    aRes.clear();
    if (c1 == KT_SpecialNameNil) {
	aRes.push_back(c1);
	res = true;
    }
    return res;
}


bool Chromo2Mdl::rdp_spname_self(istream& aIs, string& aRes)
{
    bool res = true;
    char c1 = aIs.get(), c2 = aIs.get();
    aRes.clear();
    if (c1 == '_' && c2 == '$') {
	aRes.push_back(c1);
	aRes.push_back(c2);
    } else {
	res = false;
    }
    return res;
}


bool Chromo2Mdl::rdp_string(istream& aIs, string& aRes)
{
    bool res = true;
    char c = aIs.get();
    bool esc = false;
    if (c == KT_TextDelim ) {
	c = aIs.get();
	while ((c != KT_TextDelim || c == KT_TextDelim && esc)  && c != KT_EOL) {
	    if (c == KT_Escape) {
		esc = true;
	    } else {
		aRes.push_back(c);
		esc = false;
	    }
	    c = aIs.get();
	}
	if (c != KT_TextDelim) {
	    res = false;
	}
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_uri(istream& aIs, string& aRes)
{
    bool res = true;
    aRes.clear();
    string elem;
    streampos pos = aIs.tellg();
    res = rdp_spname_self(aIs, elem);
    if (res) {
	aRes.append(elem);
    } else {
	rdpBacktrack(aIs, pos);
	res = rdp_name(aIs, elem);
	aRes.append(elem);
	if (res) {
	    char c = aIs.get();
	    while (c == KT_UriSep && res) {
		aRes.push_back(c);
		res = rdp_name(aIs, elem);
		if (res) {
		    aRes.append(elem);
		    c = aIs.get();
		}
	    };
	    aIs.seekg(-1, aIs.cur);
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_mut(istream& aIs, C2MdlNode& aMnode)
{
    bool res = false;
    streampos pos = aIs.tellg();
    res = rdp_mut_comment(aIs, aMnode);
    if (!res) {
	rdpBacktrack(aIs, pos); // Backtrack
	res = rdp_mut_create(aIs, aMnode);
	if (!res) {
	    rdpBacktrack(aIs, pos); // Backtrack
	    res = rdp_mut_content(aIs, aMnode);
	    if (!res) {
		rdpBacktrack(aIs, pos); // Backtrack
		res = rdp_mut_connect(aIs, aMnode);
		if (!res) {
		    rdpBacktrack(aIs, pos); // Backtrack
		    res = rdp_mut_remove(aIs, aMnode);
		    if (!res) {
			rdpBacktrack(aIs, pos); // Backtrack
			res = rdp_mut_import(aIs, aMnode);
			if (!res) {
			    rdpBacktrack(aIs, pos); // Backtrack
			    res = rdp_mut_disconnect(aIs, aMnode);
			    if (!res) {
				rdpBacktrack(aIs, pos); // Backtrack
				res = rdp_mut_content_err_p(aIs, aMnode);
				if (!res) {
				    rdpBacktrack(aIs, pos); // Backtrack
				    res = rdp_mut_content_err_q(aIs, aMnode);
				    if (!res) {
					rdpBacktrack(aIs, pos); // Backtrack
					res = rdp_mut_comment_err(aIs);
					if (!res) {
					    rdpBacktrack(aIs, pos); // Backtrack
					    res = rdp_mut_err_sep_before(aIs);
					    if (!res) {
						rdpBacktrack(aIs, pos); // Backtrack
						rdp_mut_err_p(aIs);
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    if (res) {
	pos = aIs.tellg(); // Syntax modification [';'], ref fap3 ds_cli_pi_ms
	char c = aIs.get();
	if (c == KT_MutSeparator) {
	} else {
	    //res = false;
	    /* Syntax modification [';'], ref fap3 ds_cli_pi_ms
	    */
	    res = true;
	    rdpBacktrack(aIs, pos);
	    /*
	    C2MdlNode* owner = aMnode.mOwner;
	    aMnode = C2MdlNode(); // Reset the node, to clean translated data
	    aMnode.mOwner = owner;
	    aMnode.mChromoPos = pos;
	    */
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_chromo_node(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    dbg << "pos: " << pos << endl;
    C2MdlNode node;
    // Set owner by advance to support dependent mut (DMC)
    node.mOwner = &aMnode;
    res = rdp_segment_target(aIs, node, aMnode);
    dbg << "segment_target: " << res << endl;
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_segment_namespace(aIs, node, aMnode);
        dbg << "segment_namespace: " << res << endl;
	if (!res) {
	    rdpBacktrack(aIs, pos);
	    res = rdp_ctx_mut_create_chromo(aIs, node);
            dbg << "ctx_mut_create_chromo: " << res << endl;
	    if (!res) {
		rdpBacktrack(aIs, pos);
		res = rdp_segment(aIs, node);
                dbg << "segment: " << res << endl;
		if (!res) {
		    rdpBacktrack(aIs, pos);
		    res = rdp_ctx_mutation(aIs, node);
                    dbg << "ctx_mutation: " << res << endl;
		    if (res) {
			aMnode.mChromo.push_back(node);
		    } else {
                        dbg << "err -- unrec segment node, pos: " << pos << endl;
                        SetErr(aIs, RDPE_UnrecognizedChrNode);
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
    } else {
	aMnode.mChromo.push_back(node);
    }
    return res;
}

bool Chromo2Mdl::rdp_segment(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg(); // Debug
    char c = aIs.get();
    if (c == KT_ChromoStart) {
	res = rdp_sep(aIs);
	if (res) {
	    pos = aIs.tellg();
	    c = aIs.get();
	    if (c != KT_ChromoEnd) {
		aIs.seekg(pos, aIs.beg); // Backtrack
		res = rdp_chromo_node(aIs, aMnode);
		if (res) {
		    do {
			res = rdp_sep(aIs);
			if (res) {
			    c = aIs.get();
			    if (c == KT_ChromoEnd) {
			    } else {
				aIs.seekg(-1, aIs.cur);
				res = rdp_chromo_node(aIs, aMnode);
			    }
			}
		    } while (res && c != KT_ChromoEnd);
		}
	    }
	}
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_ns_segment(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg(); // Debug
    dbg << "pos: " << pos << endl;
    char c = aIs.get();
    if (c == KT_NsSegStart) {
	res = rdp_sep(aIs);
	if (res) {
	    pos = aIs.tellg();
	    c = aIs.get();
	    if (c != KT_NsSegEnd) {
		aIs.seekg(pos, aIs.beg); // Backtrack
		res = rdp_chromo_node(aIs, aMnode);
		if (res) {
		    do {
			res = rdp_sep(aIs);
			if (res) {
			    c = aIs.get();
			    if (c == KT_NsSegEnd) {
			    } else {
				aIs.seekg(-1, aIs.cur);
				res = rdp_chromo_node(aIs, aMnode);
			    }
			}
		    } while (res && c != KT_NsSegEnd);
		}
	    }
	}
    } else {
	res = false;
    }
    return res;
}


bool Chromo2Mdl::rdp_segment_target(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    bool res = true;
    res = rdp_context_target_ext(aIs, aMnode, aDepSeg);
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    res = rdp_segment(aIs, aMnode);
	}
    }
    return res;
}


bool Chromo2Mdl::rdp_segment_namespace(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    bool res = true;
    streampos pos = aIs.tellg();
    dbg << "pos: " << pos << endl;
    res = rdp_context_namespace_ext(aIs, aMnode, aDepSeg);
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    res = rdp_segment(aIs, aMnode);
	}
    }
    if (!res) {
	rdpBacktrack(aIs, pos);
        string nsuri;
        res = rdp_context_namespace_ext_obj(aIs, aMnode, aDepSeg, nsuri);
        if (res) {
            res = rdp_sep(aIs);
            if (res) {
                res = rdp_ns_segment(aIs, aMnode);
                if (res) {
                    aMnode.AddContext(KTS_Namespace, nsuri);
                    dbg << "ns uri: " << nsuri << endl;
                }
            }
        }
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_create_chromo(istream& aIs, C2MdlNode& aMnode)
{
    bool res = rdp_mut_create(aIs, aMnode);
    if (res) {
        res = rdp_sep(aIs);
        if (res) {
            res = rdp_segment(aIs, aMnode);
        }
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_create_chromo_name(istream& aIs, string& aName)
{
    bool res = false;
    string name;
    streampos pos = aIs.tellg();
    dbg << "pos: " << pos << endl;
    res = rdp_name(aIs, name);
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    char c = aIs.get();
	    if (c == KT_MutAdd) {
		res = rdp_sep(aIs);
		if (res) {
		    string parent;
		    res = rdp_uri(aIs, parent);
		    if (res) {
			aName = name;
		    }
		}
	    } else {
		res = false;
	    }
	}
    }
    return res;

}

bool Chromo2Mdl::rdp_mut_create(istream& aIs, C2MdlNode& aMnode)
{
    bool res = false;
    string name;
    bool anon = false;
    streampos pos = aIs.tellg();
    dbg << "pos: " << pos << endl;
    res = rdp_name(aIs, name);
    if (!res) {
	// Anonymous creation
	anon = true;
	rdpBacktrack(aIs, pos);
	res = true;
    } else {
	res = rdp_sep(aIs);
    }
    if (res) {
	char c = aIs.get();
	if (c == KT_MutAdd) {
	    res = rdp_sep(aIs);
	    if (res) {
		string parent;
		res = rdp_uri(aIs, parent);
		if (res) {
		    if (anon) {
			streampos pos = aIs.tellg();
			name = GenerateName(parent, pos);
		    }
		    aMnode.mMut.mP = name;
		    aMnode.mMut.mR = string(1, KT_MutAdd);
		    aMnode.mMut.mQ = parent;
		    aMnode.mChromoPos = pos;
                    dbg << "Mut.P: " << aMnode.mMut.mP << ", Mut.R: " << aMnode.mMut.mR << ", Mut.Q: " << aMnode.mMut.mQ << endl;
		}
	    }
	} else {
	    res = false;
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_disconnect(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    string name1, name2;
    res = rdp_uri(aIs, name1);
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    char c1 = aIs.get(), c2 = aIs.get();
	    if (c1 == KMS_Disconn.at(0) && c2 == KMS_Disconn.at(1)) {
		res = rdp_sep(aIs);
		if (res) {
		    res = rdp_uri(aIs, name2);
		    if (res) {
			aMnode.mMut.mP = name1;
			aMnode.mMut.mR = KMS_Disconn;
			aMnode.mMut.mQ = name2;
			aMnode.mChromoPos = pos;
		    }
		}
	    } else {
		res = false;
	    }
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_connect(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    string name;
    res = rdp_uri(aIs, name);
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    char c = aIs.get();
	    if (c == KT_MutConn) {
		res = rdp_sep(aIs);
		if (res) {
		    // Fork uri vs mut_create vs segment_target
		    streampos pos = aIs.tellg();
		    C2MdlNode node;
		    // Set owner by advance to support dependent mut (DMC)
		    node.mOwner = &aMnode;
		    res = rdp_segment_target(aIs, node, *aMnode.mOwner);
		    if (!res) {
			rdpBacktrack(aIs, pos);
			res = rdp_segment_namespace(aIs, node, *aMnode.mOwner);
			if (!res) {
			    rdpBacktrack(aIs, pos);
			    res = rdp_mut_create_chromo(aIs, node);
			    if (!res) {
				rdpBacktrack(aIs, pos);
				res = rdp_mut_create(aIs, node);
				if (!res) {
				    rdpBacktrack(aIs, pos);
				    string parent;
				    res = rdp_uri(aIs, parent);
				    if (res) {
					aMnode.mMut.mP = name;
					aMnode.mMut.mR = string(1, KT_MutConn);
					aMnode.mMut.mQ = parent;
					aMnode.mChromoPos = pos;
				    }
				} else {
				    aMnode.mChromo.push_back(node); // Q depenency
				    aMnode.mMut.mP = name;
				    aMnode.mMut.mR = string(1, KT_MutConn);
				    aMnode.mChromoPos = pos;
				}
			    } else {
				aMnode.mChromo.push_back(node); // Q depenency
				aMnode.mMut.mP = name;
				aMnode.mMut.mR = string(1, KT_MutConn);
				aMnode.mChromoPos = pos;
			    }
			} else {
			    aMnode.mChromo.push_back(node); // Q depenency
			    aMnode.mMut.mP = name;
			    aMnode.mMut.mR = string(1, KT_MutConn);
			    aMnode.mChromoPos = pos;
			}
		    } else {
			aMnode.mChromo.push_back(node); // Q depenency
			aMnode.mMut.mP = name;
			aMnode.mMut.mR = string(1, KT_MutConn);
			aMnode.mChromoPos = pos;
		    }
		}
	    } else {
		res = false;
	    }
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_content(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    char c = aIs.get();
    if (c == KT_MutContent) {
	res = rdp_sep(aIs);
	if (res) {
	    string value;
	    res = rdp_string(aIs, value);
	    if (res) {
		aMnode.mMut.mR = string(1, KT_MutContent);
		aMnode.mMut.mQ = value;
		aMnode.mChromoPos = pos;
	    }
	}
    } else {
	rdpBacktrack(aIs, pos);
	string uri;
	res = rdp_uri(aIs, uri);
	if (res) {
	    res = rdp_sep(aIs);
	    if (res) {
		char c = aIs.get();
		if (c == KT_MutContent) {
		    res = rdp_sep(aIs);
		    if (res) {
			string value;
			res = rdp_string(aIs, value);
			if (res) {
			    aMnode.mMut.mP = uri;
			    aMnode.mMut.mR = string(1, KT_MutContent);
			    aMnode.mMut.mQ = value;
			    aMnode.mChromoPos = pos;
			}
		    }
		} else {
		    res = false;
		}
	    }
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_content_err_p(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    string uri;
    res = rdp_uri(aIs, uri);
    if (!res) {
	res = rdp_word_sep(aIs);
	if (res) {
	    char c = aIs.get();
	    if (c == KT_MutContent) {
		res = rdp_sep(aIs);
		if (res) {
		    string value;
		    res = rdp_string(aIs, value);
		    // Ignore content value
		    res = true;
		    SetErr(aIs, RDPE_WrongContentUri);
		}
	    } else {
		res = false;
	    }
	}
    } else {
	// ?? res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_content_err_q(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    char c = aIs.get();
    if (c == KT_MutContent) {
	res = rdp_sep(aIs);
	if (res) {
	    string value;
	    res = rdp_string(aIs, value);
	    if (!res) {
		SetErr(aIs, RDPE_WrongContentValue);
		res = true;
	    }
	}
    } else {
	aIs.seekg(pos, aIs.beg); // Backtrack
	string uri;
	res = rdp_uri(aIs, uri);
	if (res) {
	    res = rdp_sep(aIs);
	    if (res) {
		char c = aIs.get();
		if (c == KT_MutContent) {
		    res = rdp_sep(aIs);
		    if (res) {
			string value;
			res = rdp_string(aIs, value);
			if (!res) {
			    SetErr(aIs, RDPE_WrongContentValue);
			    res = true;
			}
		    }
		} else {
		    res = false;
		}
	    }
	}
    }
    return res;
}



bool Chromo2Mdl::rdp_mut_comment(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    dbg << "pos: " << pos << endl;
    char c = aIs.get();
    if (c == KT_MutComment) {
	res = rdp_sep(aIs);
	if (res) {
	    string text;
	    res = rdp_string(aIs, text);
	    if (res) {
		aMnode.mMut.mR = string(1, KT_MutComment);
		aMnode.mMut.mQ = text;
		streampos pos = aIs.tellg();
		aMnode.mChromoPos = pos;
                dbg << "Mut.Q: " << text << endl;
	    }
	}
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_err_sep_before(istream& aIs)
{
    bool res = true;
    streampos pos = aIs.tellg();
    string p;
    res = rdp_uri(aIs, p);
    if (res) {
	char c = aIs.get();
	if (IsSymbOf(c, KMutSymbols)) {
	    SetErr(aIs, RDPE_MissingSepBeforeMut);
	} else {
	    res = false;
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_mut_err_p(istream& aIs)
{
    bool res = true;
    streampos pos = aIs.tellg();
    char c = aIs.get();
    if (!IsSymbOf(c, KMutSymbolsEp)) {
	rdpBacktrack(aIs, pos);
	string p;
	res = rdp_uri(aIs, p);
	if (!res) {
	    rdp_word_sep(aIs);
	    char c = aIs.get();
	    if (IsSymbOf(c, KMutSymbolsRp)) {
		SetErr(aIs, RDPE_WrongMutLeftPart);
		res = true;
	    }
	} else {
	    res = rdp_sep(aIs);
	    if (res) {
		res = false;
	    } else {
		rdp_word_sep(aIs);
		char c = aIs.get();
		if (IsSymbOf(c, KMutSymbolsRp)) {
		    SetErr(aIs, RDPE_WrongMutLeftPart);
		    res = true;
		} else {
		    res = false;
		}
	    }
	}
    } else {
	res = false;
    }
    return res;
}


bool Chromo2Mdl::rdp_mut_comment_err(istream& aIs)
{
    bool res = true;
    char c = aIs.get();
    if (c == KT_MutComment) {
	res = rdp_sep(aIs);
	if (res) {
	    string text;
	    res = rdp_string(aIs, text);
	    if (!res) {
		SetErr(aIs, RDPE_WrongCommentString);
		res = true;
	    }
	}
    } else {
	res = false;
    }
    return res;
}


bool Chromo2Mdl::rdp_mut_import(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    char c = aIs.get();
    if (c == KT_MutImport) {
	res = rdp_sep(aIs);
	if (res) {
	    string name;
	    res = rdp_name(aIs, name);
	    if (res) {
		aMnode.mMut.mR = string(1, KT_MutImport);
		aMnode.mMut.mQ = name;
		streampos pos = aIs.tellg();
		aMnode.mChromoPos = pos;
	    }
	}
    } else {
	res = false;
    }
    return res;
}



bool Chromo2Mdl::rdp_mut_remove(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    char c = aIs.get();
    if (c == KT_MutRemove) {
	res = rdp_sep(aIs);
	if (res) {
	    string object;
	    res = rdp_name(aIs, object);
	    if (res) {
		aMnode.mMut.mR = string(1, KT_MutRemove);
		aMnode.mMut.mQ = object;
		streampos pos = aIs.tellg();
		aMnode.mChromoPos = pos;
	    }
	}
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::rdp_ctx_mutation(istream& aIs, C2MdlNode& aMnode)
{
    bool res = false;
    streampos pos = aIs.tellg();
    res = rdp_context(aIs, aMnode);
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_mut(aIs, aMnode);
    } else {
	res = rdp_sep(aIs);
	if (res) {
	    res = rdp_mut(aIs, aMnode);
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_ctx_mut_create_chromo(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    res = rdp_context(aIs, aMnode);
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_mut_create_chromo(aIs, aMnode);
    } else {
	res = rdp_sep(aIs);
	if (res) {
	    res = rdp_mut_create_chromo(aIs, aMnode);
	}
    }
    return res;
}


bool Chromo2Mdl::rdp_context_target(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    string uri;
    streampos pos = aIs.tellg();
    res = rdp_uri(aIs, uri);
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_spname_ns(aIs, uri);
    }
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_spname_nil(aIs, uri);
    }
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    char c = aIs.get();
	    if (c == KT_Target) {
		aMnode.AddContext(KTS_Target, uri);
	    } else {
		res = false;
	    }
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_context_target_ext(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    bool res = true;
    streampos pos = aIs.tellg();
    res = rdp_mut_create(aIs, aMnode);
    string uri;
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_uri(aIs, uri);
	if (!res) {
	    rdpBacktrack(aIs, pos);
	    res = rdp_spname_ns(aIs, uri);
	}
	if (!res) {
	    rdpBacktrack(aIs, pos);
	    res = rdp_spname_nil(aIs, uri);
	}
    }
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    char c = aIs.get();
	    if (c == KT_Target) {
		aMnode.AddContext(KTS_Target, uri);
	    } else {
		res = false;
	    }
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_context_namespace_ext_obj(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg, string& aNsUri)
{
    bool res = true;
    streampos pos = aIs.tellg();
    res = rdp_mut_create(aIs, aMnode);
    string uri;
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_uri(aIs, uri);
        if (res) {
            aNsUri = uri;
        }
    }
    return res;
}

bool Chromo2Mdl::rdp_context_namespace_ext(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg)
{
    bool res = true;
    streampos pos = aIs.tellg();
    res = rdp_mut_create(aIs, aMnode);
    string uri;
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_uri(aIs, uri);
    }
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    char c = aIs.get();
	    if (c == KT_Namespace) {
		aMnode.AddContext(KTS_Namespace, uri);
	    } else {
		res = false;
	    }
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_context_namespace(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    string uri;
    res = rdp_uri(aIs, uri);
    if (res) {
	res = rdp_sep(aIs);
	if (res) {
	    char c = aIs.get();
	    if (c == KT_Namespace) {
		aMnode.AddContext(KTS_Namespace, uri);
	    } else {
		res = false;
	    }
	}
    }
    return res;
}

// TODO Do we need this (context_target, context_namespace) branch?
bool Chromo2Mdl::rdp_context(istream& aIs, C2MdlNode& aMnode)
{
    bool res = true;
    streampos pos = aIs.tellg();
    res = rdp_context_target(aIs, aMnode);
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_context_namespace(aIs, aMnode);
    } else {
	pos = aIs.tellg();
	res = rdp_sep(aIs);
	if (res) {
	    res = rdp_context_namespace(aIs, aMnode);
	    if (!res) {
		rdpBacktrack(aIs, pos);
		res = true;
	    }
	}
    }
    return res;
}

bool Chromo2Mdl::rdp_model_spec(istream& aIs, streampos aStart, C2MdlNode& aMnode)
{
    aIs.seekg(aStart, aIs.beg);
    streampos pos = aIs.tellg();
    bool res = rdp_segment(aIs, aMnode);
    if (!res) {
	rdpBacktrack(aIs, pos);
	res = rdp_mut_create_chromo(aIs, aMnode);
    }
    if (!res && !IsError()) {
	SetErr(aIs, RDPE_Unspecified);
    }
    return res;
}

bool Chromo2Mdl::rdp_sep(istream& aIs)
{
    bool res = false;
    streampos pos = aIs.tellg(); // Debug
    char c = aIs.get();
    if (IsSep(c)) {
	do {
	    c = aIs.get();
	} while (IsSep(c));
	aIs.seekg(-1, aIs.cur);
	res = true;
    } else {
	res = false;
    }
    return res;
}

bool Chromo2Mdl::operator==(const Chromo2Mdl& b) const
{
    bool res = !IsError() && !b.IsError();
    if (res) {
	res = (mRoot == b.mRoot);
    }
    return res;
}







Chromo2::Chromo2(): mRootNode(&mMdl, mMdl.Hroot())
{
}

Chromo2::Chromo2(const Chromo2& aSrc): mMdl(aSrc.mMdl)
				       //, mRootNode(const_cast<Chromo2Mdl*>(&aSrc.mMdl), const_cast<Chromo2Mdl&>(aSrc.mMdl).Hroot())
{
    mRootNode = ChromoNode(const_cast<Chromo2Mdl*>(&aSrc.mMdl), const_cast<Chromo2Mdl&>(aSrc.mMdl).Hroot());
    //Set(aSrc.Root());
}

Chromo2& Chromo2::operator=(const Chromo2& aSrc)
{
    mMdl = aSrc.mMdl;
    mRootNode = ChromoNode(&mMdl, mMdl.Hroot());
    return *this;
}

void Chromo2::SetFromFile(const string& aFileName)
{
    dbg << "====================================== file name: " << aFileName << endl;
    THandle root = mMdl.SetFromFile(aFileName);
    C2MdlNode* nroot = root.Data(nroot);
    mRootNode = ChromoNode(&mMdl, root);
}

bool Chromo2::getName(const string& aFileName, string& aName)
{
    return mMdl.getName(aFileName, aName);
}

bool Chromo2::Set(const string& aUri)
{
    bool res = false;
    THandle root = mMdl.Set(aUri);
    if (root != NULL) {
	mRootNode = ChromoNode(&mMdl, root);
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
	mRootNode = ChromoNode(&mMdl, root);
	res = true;
    }
    return res;

}

void Chromo2::Set(const ChromoNode& aRoot)
{
    THandle root = mMdl.Set(aRoot.Handle());
    mRootNode = ChromoNode(&mMdl, root);
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
    mRootNode = ChromoNode(&mMdl, root);
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
    return ChromoNode(&mMdl, aHandle);
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

#if 0
    if (aSrc.Type() == ENt_Conn && !aSrc.AttrExists(ENa_Q)) {
	if (aSrc.Count() == 1) {
	    // Dependent node
	    const ChromoNode& scomp = *aSrc.Begin();
	} else {
	    // TODO handle the error - add error record to abstract chromo model?
	}
    } else {
	// Chromo
	for (ChromoNode::Const_Iterator it = aSrc.Begin(); it != aSrc.End(); it++) {
	    const ChromoNode& scomp = *it;
	    ChromoNode dcomp = aDst.AddChild(scomp.Type());
	    ConvertNode(dcomp, scomp);
	}
    }
#endif
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

bool Chromo2::operator==(const Chromo2& b) const
{
    return mMdl == b.mMdl;
}

