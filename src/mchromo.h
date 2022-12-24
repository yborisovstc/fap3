#ifndef __FAP3_MCHROMO_H
#define __FAP3_MCHROMO_H

#include <assert.h>
#include <array>
#include <sstream>

class MLogRec;

using namespace std;

/** @brief Chromo node type
 * */
enum TNodeType
{
    ENt_Unknown = 0,
    ENt_None = 1,
    ENt_Node = 2,
    ENt_Move = 3, 	// Mutation - move node
    ENt_Rm = 4,   	// Mutation - removal
    ENt_Change = 5, 	// Change node attribute
    ENt_Cont = 6, 	// Change node content
    ENt_Import = 8, 	// Importing node
    ENt_Seg = 9, 	// Segment of chromo
    ENt_Note = 10, 	// Note
    ENt_Conn = 11, 	// Connection, system custom mutation
    ENt_Disconn = 12, 	// Connection, system custom mutation
};

/** @brief Chromo node attributes
 * */
enum TNodeAttr
{
    ENa_Unknown = 0,
    ENa_Id = 1,
    ENa_Parent = 2,
    ENa_MutNode = 10,
    ENa_MutAttr = 11,
    ENa_MutVal = 12,
    ENa_Inactive = 13, // Sign of mutations being inactive, optimized out for instance
    ENa_Targ = 14, // Target node of mutation, is used in OSM mode, ref ds_mut_osm
    ENa_NS = 16,    //!< Name space
    ENa_P = 17, //!< 1st argument of mutation 
    ENa_Q = 18, //!< 2nd argument of mutation 
};


/** @brief Chromo error
 * */
class CError {
    public:
	CError(): mPos(0) {};
	CError(streampos aPos, const string& aDescr): mPos(aPos), mText(aDescr) {};
    public:
	void Set(streampos aPos, const string& aDescr) {mPos = aPos, mText = aDescr; };
	bool IsSet() const { return mPos != 0;}
    public:
	streampos mPos;     //!< Position of last error
	string mText;       //!< Descrition of last error 
};

/** @brief Handle of Chromo model node
 * */
template<int L> class TTHandle
{
    public:
	TTHandle() {mData.fill(0);}
	TTHandle(const TTHandle& aSrc) { for (unsigned int i = 0; i < mData.size(); i++) mData[i] = aSrc.mData[i]; }
	template<class T> TTHandle(const T& aData) { assert(sizeof(T) <= L); T* pd = reinterpret_cast<T*>(mData.data()); *pd = aData;}
	template<class T> T Data(const T& aData) const { assert(sizeof(T) <= L); const T* pd = reinterpret_cast<const T*>(mData.data()); return *pd;}
	template<class T> T Data() const { assert(sizeof(T) <= L); const T* pd = reinterpret_cast<const T*>(mData.data()); return *pd;}
	bool operator ==(const TTHandle& aB) const {return mData == aB.mData;}
	bool operator !=(const TTHandle& aB) const {return !(*this == aB);}
	bool IsNull() const { for (auto it = mData.begin(); it != mData.end(); it++) if (*it) return false; return true;}
    public:
	array<uint8_t, L> mData;
};

using THandle = TTHandle<8>;

// Interface of chromo model
class MChromoMdl
{
    public:
	virtual TNodeType GetType(const THandle& aHandle) = 0;
	virtual THandle Root(const THandle& aHandle) = 0;
	virtual THandle Parent(const THandle& aHandle) = 0;
	virtual THandle Next(const THandle& aHandle) = 0;
	virtual THandle Prev(const THandle& aHandle) = 0;
	virtual THandle GetFirstChild(const THandle& aHandle) = 0;
	virtual THandle GetLastChild(const THandle& aHandle) = 0;
	virtual string GetAttr(const THandle& aHandle, TNodeAttr aAttr) const = 0;
	virtual bool AttrExists(const THandle& aHandle, TNodeAttr aAttr) const  = 0;
	virtual THandle AddChild(const THandle& aParent, TNodeType aType) = 0;
	virtual THandle AddChild(const THandle& aParent, const THandle& aHandle, bool aCopy = true, bool aRecursively = true) = 0;
	virtual bool IsChildOf(const THandle& aNode, const THandle& aParent) = 0;
	virtual void RmChild(const THandle& aParent, const THandle& aChild, bool aDeattachOnly = false) = 0;
	virtual void Rm(const THandle& aHandle) = 0;
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, const string& aVal) = 0;
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, int aVal) = 0;
	virtual void RmAttr(const THandle& aNode, TNodeAttr aType) = 0;
	//virtual void SetAttr(THandle aNode, TNodeAttr aType, TNodeType aVal) = 0;
	//virtual void SetAttr(THandle aNode, TNodeAttr aType, TNodeAttr aVal) = 0;
	virtual void Dump(const THandle& aNode) = 0;
	virtual void DumpBackTree(const THandle& aNode) = 0;
	virtual void DumpToLog(const THandle& aNode, MLogRec* aLogRec) = 0;
	virtual bool ToString(const THandle& aNode, string& aString) const = 0;
	/** @brief Saves chromo to file
	 * @param[in] aFileName  Output file name
	 * @param[in] aIndent    Indentation of chromo level, 0 - default indent
	 * */
	virtual void Save(const string& aFileName, int aIndent = 0) const = 0;
	virtual int GetLineId(const THandle& aHandle) const = 0;
	virtual void TransfTl(const THandle& aHandle, const THandle& aSrc) = 0;
};

class ChromoNode;

// Interface of chromosome
class MChromo
{
    public:
	virtual ~MChromo() {};
	virtual ChromoNode& Root() = 0;
	virtual const ChromoNode& Root() const= 0;
	virtual void SetFromFile(const string& aFileName) = 0;
	virtual bool Set(const string& aUri) = 0;
	virtual bool SetFromSpec(const string& aSpec) = 0;
	virtual bool GetSpec(string& aSpec) = 0;
	virtual void Set(const ChromoNode& aRoot) = 0;
	/** @brief Converts from another chromo, probably not same type */
	virtual void Convert(const MChromo& aSrc) = 0;
	/** @brief Transform linearized chromo to tree-formed */
	virtual void TransformLn(const MChromo& aSrc) = 0;
	virtual void Init(TNodeType aRootType) = 0;
	virtual void Reset() = 0;
	/** @brief Saves chromo to file
	 * @param[in] aFileName  Output file name
	 * @param[in] aIndent    Indentation of chromo level, 0 - default indent
	 * */
	virtual void Save(const string& aFileName, int aIndent = 0) const = 0;
	virtual ChromoNode CreateNode(const THandle& aHandle) = 0;
	virtual bool IsError() const { return false;}
	virtual const CError& Error() const  = 0;
};

#endif
