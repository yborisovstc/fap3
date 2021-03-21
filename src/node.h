
#ifndef __FAP3_NODE_H
#define __FAP3_NODE_H

#include "mnode.h"
#include "mowning.h"
#include "nconn.h"
#include "mecont.h"
#include <cstring>


using namespace std;

template<class T> MIface* checkLifs(const char* aType, MIface* aSelf) { return (strcmp(aType, T::Type()) == 0) ? dynamic_cast<T*>(aSelf) : nullptr;}

/** @brief Native hierarchy node agent
 *  Supports native hier tree and content (DCE)
 * TODO YB Consider one NCP for owner-owned tree. Ref NTnip for example.
 * */
class Node : public MNode, public MContentOwner
{
    public:
	// NNode dump masks
	enum TDumpMask {
	    EDM_Base = 0x01,
	    EDM_Comps = 0x02,
	    EDM_Recursive = 0x04,
	};
	// MConentOwner dump masks
	enum TCoDumpMask {
	    ECODM_Recursive = 0x01,
	};


    public:
	class NCpOwned;

	/** @brief Owner connection point, one-to-many
	 * */
	class NCpOwner : public NCpOmi2<MOwner, MOwned> {
	    friend class Node;
	    public:
	    NCpOwner(Node* aHost): NCpOmi2<MOwner, MOwned>(), mHost(aHost) {}
	    // From MOwner
	    virtual string MOwner_Uid() const {return mHost->getUid<MOwner>();}
	    // Local
	    virtual MOwned* bindedOwned() override { return mHost->owned();}
	    virtual const MOwned* bindedOwned() const  { return mHost->owned();}
	    // TODO Consider form specific Owned iface instead of getting whole MNode
		MNode* munitAt(const string& aId) {
		    MNode* res = nullptr;
		    MOwned* owd = at(aId);
		    res = owd->lIf(res);
		    return res;
		}
		virtual void getUri(GUri& aUri, MNode* aBase = nullptr) const override {
		    mHost->getUri(aUri, aBase);
		}
		virtual MNode* getNode(const GUri& aUri, const MNode* aOwned) const override { return mHost->getNode(aUri, aOwned);}
		virtual void onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx) override {
		    return mHost->onOwnedMutated(aOwned, aMut, aCtx);}
	    private:
		Node* mHost;
	};
	/** @brief Owned connection point, one-to-one
	 * */
	class NCpOwned : public NCpOi2<MOwned, MOwner> {
	    public:
		NCpOwned(Node* aHost): NCpOi2<MOwned, MOwner>(), mHost(aHost) {}
		// From MOwned
		virtual string MOwned_Uid() const {return mHost->getUid<MOwned>();}
		virtual MIface* MOwned_getLif(const char *aType) override {
		    MIface* res = nullptr;
		    if (res = checkLifs<MOwned>(aType, this));
		    else if (res = mHost->checkLif<MNode>(aType));
		    else if (res = mHost->checkLif<MContent>(aType));
		    return res;
		}
		virtual string ownedId() const override { return mHost->name();}
		virtual bool isOwner(const MOwner* aOwner) const override;
		virtual void deleteOwned() override { delete mHost;}
		// From MNcp
		virtual bool getId(string& aId) const override { aId = mHost->name(); return true;}
	    private:
		Node* mHost;
	};


    public:
	static const char* Type() { return "Node";}
	Node(const string &aName, MEnv* aEnv);
	virtual ~Node();
	// From MNode.MIface
	virtual string MNode_Uid() const override { return getUid<MNode>();}
	virtual MIface* MNode_getLif(const char *aType) override;
	virtual void MNode_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MNode
	virtual string name() const override { return mName;}
	virtual const MNode* getComp(const string& aId) const override;
	virtual MNode* getComp(const string& aId) override { return const_cast<MNode*>(const_cast<const Node*>(this)->getComp(aId));}
	virtual const MNode* getNode(const GUri& aUri) const override;
	virtual MNode* getNode(const GUri& aUri) override { return const_cast<MNode*>(const_cast<const Node*>(this)->getNode(aUri));}
	virtual MNode* getNode(const string& aName, const TNs& aNs) override;
	virtual void getUri(GUri& aUri, MNode* aBase = NULL) const override;
	virtual void setCtx(MNode* aContext) override;
	virtual void mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo = false) override;
	virtual MNode* createHeir(const string& aName, MNode* aContext) override;
	virtual bool attachOwned(MNode* aOwned) override;
	virtual TOwnerCp* owner() override { return &mCpOwner;}
	virtual const TOwnerCp* owner() const override { return &mCpOwner;}
	virtual TOwnedCp* owned() override { return &mCpOwned;}
	virtual const TOwnedCp* owned() const override { return &mCpOwned;}
	// From MContentOwner
	virtual string MContentOwner_Uid() const override { return getUid<MContentOwner>();}
	virtual MIface* MContentOwner_getLif(const char *aType) override;
	virtual void MContentOwner_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual int contCount() const override;
	virtual MContent* getCont(int aIdx) override;
	virtual const MContent* getCont(int aIdx) const override;
	virtual bool getContent(const GUri& aCuri, string& aRes) const override;
	virtual bool setContent(const GUri& aCuri, const string& aData) override;

    protected:
	template<class T> MIface* checkLif(const char* aType) { return (strcmp(aType, T::Type()) == 0) ? dynamic_cast<T*>(this) : nullptr;}
	void deleteOwned();
	MOwner* Owner();
	const MOwner* Owner() const;
	template<class T> string getUid() const {return getUriS() + Ifu::KUidSep + T::Type();}
	inline MLogRec* Logger() const {return mEnv ? mEnv->Logger(): nullptr; }
	inline void Log(const TLog& aRec) const { Logger()->Write(aRec);};
	inline MProvider* Provider() const {return mEnv ? mEnv->provider(): nullptr; }
	void updateNs(TNs& aNs, const ChromoNode& aCnode);
	MNode* getNode(const GUri& aUri, const MNode* aOwned) const;
	bool isOwned(const MNode* aComp) const;
	// Mutations
	virtual MNode* mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	virtual void mutSegment(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx);
	virtual void mutRemove(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	virtual void mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	void notifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx);
	virtual void onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx);
    protected:
	MEnv* mEnv = nullptr;
	string mName;
	MNode* mContext = nullptr;
    public:
	NCpOwner mCpOwner = NCpOwner(this);
	NCpOwned mCpOwned = NCpOwned(this);
};

#endif //  __FAP3_NODE_H
