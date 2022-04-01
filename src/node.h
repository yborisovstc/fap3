
#ifndef __FAP3_NODE_H
#define __FAP3_NODE_H

#include "mnode.h"
#include "mowning.h"
#include "mobserv.h"
#include "nconn.h"
#include "mecont.h"
#include <cstring>


using namespace std;

template<class T> MIface* checkLifs(const char* aType, MIface* aSelf) { return (strcmp(aType, T::Type()) == 0) ? dynamic_cast<T*>(aSelf) : nullptr;}

/** @brief Native hierarchy node agent
 *  Supports native hier tree and content (DCE)
 * */
class Node : public MNode, public MContentOwner, public MObservable, public MOwner, public MOwned
{
    public:
	class NOwningNode : public NTnip<MOwned, MOwner> {
	    public:
		NOwningNode(MOwned* aProvPx, MOwner* aReqPx): NTnip<MOwned, MOwner>(aProvPx, aReqPx) {}
		// Note: we need to implement virt destructor here because clean-up method disconnectAll()
		// is virtual and uses other virt method. It virt destructor is not impl here the lower
		// level virt destr will be called and the that context virt methods (often stubs) will be called
		virtual ~NOwningNode() { disconnectAll();}
		virtual bool getId(string& aId) const override { aId = provided()->ownedId(); return true;}
		virtual typename TCnode::TPair* cnodeBinded() override { return nullptr;} // To disable navigating to top
	};
    public:
	using TOwtNode = NOwningNode;                    /*!< Ownership tree node */ 
	using TObsCp = NCpOmnp<MObservable, MObserver>;  /*!< Observable connpoint */
    public:
	static const char* Type() { return "Node";}
	Node(const string &aType, const string &aName, MEnv* aEnv);
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
	virtual MNode* getNodeS(const char* aUri)  override;
	virtual void getUri(GUri& aUri, MNode* aBase = NULL) const override;
	virtual void setCtx(MOwner* aContext) override;
	virtual void mutate(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx, bool aTreatAsChromo = false, bool aLocal = false) override;
	virtual MNode* createHeir(const string& aName) override;
	virtual bool attachHeir(MNode* aHeir) override { return false;}
	virtual bool attachOwned(MNode* aOwned) override;
	virtual TOwnerCp* owner() override { return mOnode.binded();}
	virtual const TOwnerCp* owner() const override { return const_cast<const TOwnerCp*>(const_cast<Node*>(this)->mOnode.binded());}
	virtual TOwnedCp* owned() override { return &mOnode;}
	virtual const TOwnedCp* owned() const override { return &mOnode;}
	virtual const MContentOwner* cntOw() const override;
	virtual MContentOwner* cntOw() override;
	// From MContentOwner
	virtual string MContentOwner_Uid() const override { return getUid<MContentOwner>();}
	virtual MIface* MContentOwner_getLif(const char *aType) override;
	virtual void MContentOwner_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual int contCount() const override;
	virtual MContent* getCont(int aIdx) override;
	virtual const MContent* getCont(int aIdx) const override;
	virtual const MContent* getCont(const GUri& aUri) const override;
	virtual bool getContent(const GUri& aCuri, string& aRes) const override;
	virtual bool setContent(const GUri& aCuri, const string& aData) override;
	virtual void onContentChanged(const MContent* aCont) override;
	// From MObservable
	virtual string MObservable_Uid() const override { return getUid<MObservable>();}
	virtual MIface* MObservable_getLif(const char *aType) override;
	virtual bool addObserver(MObserver::TCp* aObs) override;
	virtual bool rmObserver(MObserver::TCp* aObs) override;
	// From MOwner
	virtual string MOwner_Uid() const {return getUid<MOwner>();}
	virtual MIface* MOwner_getLif(const char *aType) override;
	virtual void ownerGetUri(GUri& aUri, MNode* aBase = nullptr) const override { getUri(aUri, aBase);}
	virtual MNode* ownerGetNode(const GUri& aUri, const MNode* aReq) const override;
	virtual void onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx) override;
	virtual void onOwnedAttached(MOwned* aOwned) override;
	virtual void onOwnedDetached(MOwned* aOwned) override;
	virtual void getModules(vector<MNode*>& aModules) override;
	virtual MNode* getParent(const GUri& aUri) override;
	// From MOwned
	virtual string MOwned_Uid() const {return getUid<MOwned>();}
	virtual MIface* MOwned_getLif(const char *aType) override;
	virtual string ownedId() const override { return name();}
	virtual void deleteOwned() override { delete this;}
	virtual void onOwnerAttached() override {}
    protected:
	template<class T> MIface* checkLif(const char* aType) { return (strcmp(aType, T::Type()) == 0) ? dynamic_cast<T*>(this) : nullptr;}
	MOwner* Owner();
	const MOwner* Owner() const;
	template<class T> string getUid() const {return getUriS() + Ifu::KUidSep + T::Type();}
	string getUid(const string& aCompName, const string& aIfName) const {return getUriS() + Ifu::KUidSepIc + aCompName + Ifu::KUidSep + aIfName;}
	inline MLogRec* Logger() const {return mEnv ? mEnv->Logger(): nullptr; }
	inline void Log(const TLog& aRec) const { if (aRec.Ctg() <= mLogLevel)  Logger()->Write(aRec);}
	inline MProvider* Provider() const {return mEnv ? mEnv->provider(): nullptr; }
	void updateNs(TNs& aNs, const ChromoNode& aCnode);
	bool isOwned(const MNode* aComp) const;
	MNode* addComp(const string& aType, const string& aName);
	void notifyChanged();
	inline bool isLogLevel(int aLevel) const { return mLogLevel >= aLevel;}
	MContent* createContent(const GUri& aUri);
	int parseLogLevel(const string& aData);
	// Mutations
	virtual MNode* mutAddElem(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	virtual void mutSegment(const ChromoNode& aMut, bool aChange /*EFalse*/, const MutCtx& aCtx);
	virtual void mutRemove(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	virtual void mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	virtual void mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	virtual void mutDisconnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	virtual void mutImport(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx);
	void notifyNodeMutated(const ChromoNode& aMut, const MutCtx& aCtx);
    protected:
	MEnv* mEnv = nullptr;
	string mName;
	MOwner* mContext = nullptr;
	TObsCp mOcp;                      /*!< Observable CP */
	TOwtNode mOnode;                  /*!< Ownership node */
	int mLogLevel;                /*!< Logging level */
	bool mExplorable;                /*!< Exploring is enabled, ref ds_dcs_aes_acp */
	bool mControllable;             /*!< Control is enabled, ref ds_dcs_aes_acp */
};

#endif //  __FAP3_NODE_H
