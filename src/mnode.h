
#ifndef __FAP3_NNODE_H
#define __FAP3_NNODE_H


#include "miface.h"
#include "guri.h"
#include "menv.h"
#include "mowning.h"
#include "mobserv.h"
#include "nconn.h"


/** @brief Name spaces
 * */
class TNs: public vector<MNode*>
{
    public:
	TNs() {}
	TNs(MNode* aElem) { push_back(aElem);}
};



/** @brief Mutation context
 * */
class MutCtx
{
    public:
	MutCtx(const MutCtx& aSrc): mNode(aSrc.mNode), mNs(aSrc.mNs) {}
	MutCtx(const MNode* aNode, const TNs& aNs): mNode(aNode), mNs(aNs) {}
	MutCtx(const MNode* aNode): mNode(aNode) {}
	MutCtx(): mNode(nullptr) {}
    public:
	const MNode* mNode; //!< Node
	TNs mNs;     //!< Name spaces
};


class MContentOwner;

/** @brief Native heirarchy node interface
 * Supports owner-owning relations, is changeable (non-persistenly mutable)
 *
 * */
//TODO YB Does it make sense to add lower layer for changeable
class MNode: public MIface
{
    public:
	//!using TOwnerCp = NCpOmi2<MOwner, MOwned>;
	using TOwnerCp = MNcpp<MOwner, MOwned>;
	using TOwnedCp = MNcpp<MOwned, MOwner>;

    public:
	// From MIface
	static const char* Type() { return "MNode";}
	virtual ~MNode() {} // TODO to consider policy of system destruction
	virtual string Uid() const override { return MNode_Uid();}
	virtual string MNode_Uid() const = 0;
	virtual MIface* getLif(const char *aType) override { return MNode_getLif(aType);}
	virtual MIface* MNode_getLif(const char *aType) = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MNode_doDump(aLevel, aIdt, std::cout);}
	virtual void MNode_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual string name() const = 0;
	virtual string parentName() const = 0;
	virtual const MNode* getComp(const string& aId) const = 0;
	virtual MNode* getComp(const string& aId) = 0;
	virtual const MNode* getNode(const GUri& aUri) const = 0;
	virtual MNode* getNode(const GUri& aUri) = 0;
	virtual MNode* getNodeS(const char* aUri) = 0;
	virtual MNode* getNode(const string& aName, const TNs& aNs) = 0;
	virtual void getUri(GUri& aUri, MNode* aBase = NULL) const = 0;
	string getUriS(MNode* aBase = NULL) const { GUri uri; getUri(uri, aBase); return uri.toString();}
	/** @brief Mutates the agent
	 * @param aChange  to not save the mut in chromo but do change only
	 * @param aCtx  mutation context
	 * */
	virtual void setCtx(MOwner* aContext) = 0;
	/** @brief Apply mutation
	 * @param aTreatAsChromo  apply the mut just as chromo ignoring mut part, workaround, ref ds_adcm_pamcc
	 * */
	// TODO YB to provide proper solution for ds_adcm_pamcc
	/** @brief Mutate node
	 * @param aTreatAsChromo the indication that the only segment needs to be handled
	 * @param  aLocal  the sign of local mut. This means that target attr needs to be ignored
	 * */
	virtual void mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo = false, bool aLocal = false) = 0;
	virtual MNode* createHeir(const string& aName) = 0;
	virtual bool attachHeir(MNode* aHeir) = 0;
	virtual bool attachOwned(MNode* aOwned) = 0;
	virtual TOwnerCp* owner() = 0;
	virtual const TOwnerCp* owner() const = 0;
	virtual TOwnedCp* owned() = 0;
	virtual const TOwnedCp* owned() const = 0;
	virtual const MContentOwner* cntOw() const = 0;
	virtual MContentOwner* cntOw() = 0;
};



#endif // __FAP3_NNODE_H
