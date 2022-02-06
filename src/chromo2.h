#ifndef __FAP3_CHROMO2_H
#define __FAP3_CHROMO2_H

#include <vector>
#include <list>

#include "miface.h"
#include "mchromo.h"
#include "chromo.h"

using namespace std;

/** @brief Chromo2 mutation base
 * */
class C2Mut
{
    public:
	C2Mut() {}
	C2Mut(const C2Mut& aSrc): mP(aSrc.mP), mQ(aSrc.mQ), mR(aSrc.mR) {}
	bool operator==(const C2Mut& b) const;
    public:
	string mP; /*!< P object */
	string mQ; /*!< Q object */
	string mR; /*!< Relation */
};

/** @brief Chromo2 context unit
 * */
#if 0
class C2MdlCtxNode
{
    public:
	C2MdlCtxNode(const string& aType, const string& aValue);
    public:
	string mType; /*!< Type of context */
	string mValue;/*!< Value of context */
    public:
	static const string KT_Target; /*!< Id of target context */
	static const string KT_Node; /*!< Id of node context */
	static const string KT_Namespace; /*!< Id of name space context */
};
#endif

/** @brief Context in Chromo2 model node: map of [type, value]
 * key is type of context
 * value is value of context
 * */
typedef map<string, string> TC2MdlCtx;
typedef pair<string, string> TC2MdlCtxElem;
typedef TC2MdlCtx::iterator TC2MdlCtxIter;
typedef TC2MdlCtx::const_iterator TC2MdlCtxCiter;

class C2MdlNode;

using TC2MdlNodes = list<C2MdlNode>;

/** @brief Chromo2 model unit
 * */
class C2MdlNode
{
    public:
	C2MdlNode();
	C2MdlNode(C2MdlNode* aOwner);
	C2MdlNode(const C2MdlNode& aSrc);
	C2MdlNode(const C2MdlNode& aSrc, C2MdlNode* aOwner);
	C2MdlNode& operator=(const C2MdlNode& aSrc);
	~C2MdlNode();
    public:
	void CloneFrom(const C2MdlNode& aSrc, bool aChromo);
	void AddContext(const string& aType, const string& aValue);
	void RmContext(TNodeAttr aAttr);
	C2MdlNode* GetNextComp(C2MdlNode* aComp);
	C2MdlNode* GetPrevComp(C2MdlNode* aComp);
	void BindTree(C2MdlNode* aOwner);
	bool ExistsContextByAttr(TNodeAttr aAttr);
	bool operator==(const C2MdlNode& b) const;
	/** @brief Adds Qnode (DMC dependency) */
	void AddQnode(const C2MdlNode& aNode);
	/** @brief Checks if the node is a parent */ 
	bool IsChildOf(const C2MdlNode* aParent) const;
	/** @brief Resets the node */
	void Reset();
    public:
	C2MdlNode* mOwner = NULL;
	TC2MdlCtx mContext;       /*!< Context */
	C2Mut mMut;               /*!< Mutation */
	TC2MdlNodes mChromo;      /*!< Chromosome */
	C2MdlNode* mQnode;        /*!< Q node mut dependency, ref ds_chr2_dmc */
	int mChromoPos;          /*!< Corresponding position in chromo */
};

/** @brief Chromo2 model unit with mut
 * */
class C2MdlNodeMut: public C2MdlNode
{
    public:
	C2MdlNodeMut();
    protected:
	C2Mut mMut;                    /*!< Mutation */
};

class ChromoMdlX;

using TC2MdlNodesIter = TC2MdlNodes::iterator;
using TC2MdlNodesCiter = TC2MdlNodes::const_iterator;
using TC2MdlNodesRIter = TC2MdlNodes::reverse_iterator;

/** @brief Chromo2 model
 * */
class Chromo2Mdl: public MChromoMdl
{
    public:
	using TLexPosElem = pair<string, int>;
	using TLexPos = vector<TLexPosElem>;
	using TLex = vector<string>;
    public:
	static inline const char *Type() { return "ChromoMdlX";}; 
    public:
	Chromo2Mdl();
	Chromo2Mdl(const Chromo2Mdl& aSrc);
	virtual ~Chromo2Mdl();
	Chromo2Mdl& operator=(const Chromo2Mdl& aSrc);
	THandle Hroot() { return &mRoot;}
	// From MChromoMdl
	virtual TNodeType GetType(const THandle& aHandle);
	virtual THandle Root(const THandle& aHandle);
	virtual THandle Parent(const THandle& aHandle);
	virtual THandle Next(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle Prev(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetFirstChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetLastChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual string GetAttr(const THandle& aHandle, TNodeAttr aAttr) const;
	virtual bool AttrExists(const THandle& aHandle, TNodeAttr aAttr) const ;
	virtual THandle AddChild(const THandle& aParent, TNodeType aType);
	virtual THandle AddChild(const THandle& aParent, const THandle& aHandle, bool aCopy = true, bool aRecursively = true);
	virtual THandle AddNext(const THandle& aPrev, const THandle& aHandle, bool aCopy = true);
	virtual THandle AddNext(const THandle& aPrev, TNodeType aNode);
	virtual THandle AddPrev(const THandle& aNext, const THandle& aHandle, bool aCopy = true);
	virtual bool IsChildOf(const THandle& aNode, const THandle& aParent);
	// TODO Deattach doesn't work here!
	virtual void RmChild(const THandle& aParent, const THandle& aChild, bool aDeattachOnly = false);
	virtual void Rm(const THandle& aHandle);
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, const string& aVal);
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, int aVal);
	virtual void RmAttr(const THandle& aNode, TNodeAttr aType);
	virtual void Dump(const THandle& aNode);
	virtual void DumpBackTree(const THandle& aNode);
	virtual void DumpToLog(const THandle& aNode, MLogRec* aLogRec);
	virtual bool ToString(const THandle& aNode, string& aString) const;
	virtual void Save(const string& aFileName, int aIndent = 0) const;
	virtual THandle Find(const THandle& aHandle, const string& aUri);
	virtual int GetOrder(const THandle& aHandle, bool aTree = false) const;
	virtual void DeOrder(const THandle& aHandle);
	virtual int GetLineId(const THandle& aHandle) const;
	virtual void TransfTl(const THandle& aHandle, const THandle& aSrc) override;
    public:
	bool CheckTree(const C2MdlNode& aNode) const;
	int GetAttrInt(void *aHandle, const char *aName);
	THandle SetFromFile(const string& aFileName);
	THandle Set(const string& aUri);
	THandle SetFromSpec(const string& aSpec);
	THandle Set(const THandle& aHandle);
	virtual THandle Init(TNodeType aRootType);
	void Reset();
	const CError& Error() const { return mErr;};
	bool operator==(const Chromo2Mdl& b) const;
    protected:
	/** Recursive descent parser: parse name
	 * */
	void rdp_model_spec(istream& aIs, streampos aStart, C2MdlNode& aMnode);
	void rdp_name(istream& aIs, string& aRes);
	void rdp_spname_ns(istream& aIs, string& aRes);
	void rdp_spname_self(istream& aIs, string& aRes);
	void rdp_string(istream& aIs, string& aRes);
	void rdp_uri(istream& aIs, string& aRes);
	void rdp_mut_create_chromo(istream& aIs, C2MdlNode& aMnode);
	void rdp_mut_create(istream& aIs, C2MdlNode& aMnode);
	void rdp_mut_content(istream& aIs, C2MdlNode& aMnode);
	void rdp_mut_comment(istream& aIs, C2MdlNode& aMnode);
	bool rdp_mut_connect(istream& aIs, C2MdlNode& aMnode);
	void rdp_mut_import(istream& aIs, C2MdlNode& aMnode);
	void rdp_mut_remove(istream& aIs, C2MdlNode& aMnode);
	void rdp_sep(istream& aIs);
	bool rdp_segment(istream& aIs, C2MdlNode& aMnode);
	void rdp_segment_target(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg);
	bool rdp_segment_namespace(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg);
	bool rdp_chromo_node(istream& aIs, C2MdlNode& aMnode);
	bool rdp_mut(istream& aIs, C2MdlNode& aMnode);
	bool rdp_ctx_mutation(istream& aIs, C2MdlNode& aMnode);
	bool rdp_ctx_mut_create_chromo(istream& aIs, C2MdlNode& aMnode);
	void rdp_context(istream& aIs, C2MdlNode& aMnode);
	void rdp_context_target(istream& aIs, C2MdlNode& aMnode);
	void rdp_context_target_ext(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg);
	void rdp_context_namespace(istream& aIs, C2MdlNode& aMnode);
	/** @brief RDP pareser of context_namespace_ext
	 * @param aDepNode  segment to keep dependencies
	 * */
	void rdp_context_namespace_ext(istream& aIs, C2MdlNode& aMnode, C2MdlNode& aDepSeg);
	
    protected:
	/** @brief Sets error */
	void SetErr(istream& aIs, const string& aDescr);
	/** @brief Resets error */
	void ResetErr() { mErr = CError();}
	/** @brief Checks error */
	bool IsError() const;
	/** @brief Writes textual representation of node to stream
	 * @param[in] aNode  chromo node to output
	 * @param[in] aOs  output stream
	 * @param[in] aLevel  level of node in chromo hierarchy
	 * @param[in] aIndent  output identation
	 * */
	static void OutputNode(const C2MdlNode& aNode, ostream& aOs, int aLevel, int aIndent);
	/** @brief Parses lexems from stream */
	void GetLexs(istream& aIs, streampos aBeg, streampos aEnd, vector<string>& aLexs);
	/** @brief Parses lexems from stream, positions are stored */
	void GetLexsPos(istream& aIs, streampos aBeg, streampos aEnd, TLexPos& aLexs);
	static string GetContextByAttr(const C2MdlNode& aNode, TNodeAttr aAttr);
	/** @brief Gets indicator that lexeme is mutation operation */
	bool IsLexDmcMutop(const string& aLex) const;
    protected:
	/** @brief Dumps content of input stream fragment
	 * */
	static void DumpIsFrag(istream& aIs, streampos aStart, streampos aEnd);
	/** @brief Dumps model node
	 * */
	void DumpMnode(const C2MdlNode& aNode, int aLevel) const;
	streampos spos(istream& aIs) { return aIs.tellg();}
    protected:
	C2MdlNode mRoot;
	CError mErr;           //!< Error data
};

/** @brief Chromo2
 * */
class Chromo2: public MChromo
{
    public:
	Chromo2();
	Chromo2(const Chromo2& aSrc);
	Chromo2(const string& aFileName);
	virtual ~Chromo2();
    public:
	Chromo2& operator=(const Chromo2& aSrc);
	// From MChromo
	virtual ChromoNode& Root();
	virtual const ChromoNode& Root() const;
	virtual void SetFromFile(const string& aFileName);
	virtual bool Set(const string& aUri);
	virtual bool SetFromSpec(const string& aSpec);
	virtual bool GetSpec(string& aSpec);
	virtual void Set(const ChromoNode& aRoot);
	virtual void Convert(const MChromo& aSrc);
	virtual void TransformLn(const MChromo& aSrc) override;
	virtual void Init(TNodeType aRootType);
	virtual void Reset();
	virtual void Save(const string& aFileName, int aIndent = 0) const;
	virtual ChromoNode CreateNode(const THandle& aHandle);
	virtual void ReduceToSelection(const ChromoNode& aSelNode);
	virtual bool IsError() const;
	virtual const CError& Error() const;
	bool operator==(const Chromo2& b) const;
    protected:
	void ConvertNode(ChromoNode& aDst, const ChromoNode& aSrc);
	void TransfTlNode(ChromoNode& aDst, const ChromoNode& aSrc, bool aTarg);
    protected:
	Chromo2Mdl mMdl;
	ChromoNode mRootNode;
};

#endif
