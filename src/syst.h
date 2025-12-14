#ifndef __FAP3_SYST_H
#define __FAP3_SYST_H

#include "menv.h"
#include "msyst.h"

#include "vert.h"
#include "elem.h"
#include "content.h"

/** @brief Connection point, non-inheritable
 * Don't use Nodes content owner but uses custom conext
 * Has only 2 contents: Provided and Required
 * */
// TODO Needs connpoint based on Vert (not Vertu) to support inheritance
class ConnPointu: public Vertu, public MConnPoint
{
    public:
	inline static constexpr std::string_view idStr() { return "ConnPointu"sv;}
    public:
	ConnPointu(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~ConnPointu() {}
	// From MNode
	MIface* MNode_getLif(TIdHash aTid) override;
	// From MVert
	MIface *MVert_getLif(TIdHash aTid) override;
	bool isCompatible(MVert* aPair, bool aExt) override;
	// From MConnPoint
	string MConnPoint_Uid() const { return getUid<MConnPoint>();}
	// From MIfProvOwner
	MIface* MIfProvOwner_getLif(TIdHash aTid) override;
    protected:
	// From Unit.MIfProvOwner
	virtual void resolveIfc(TIdHash aIfid, MIfReq::TIfReqCp* aReq) override;
	// From Vertu
	virtual void onConnected() override;
	virtual void onDisconnected() override;
    protected:
        MConnPoint* mMConnPointPtr = nullptr;
};

/** @brief Extender, monolitic, multicontent, unit. Redirects request for iface to internal CP of extention.
*/
class Extd: public Vertu, public Cnt::Host
{
    public:
	inline static constexpr std::string_view idStr() { return "Extd"sv;}
    public:
	Extd(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual string parentName() const override { return string(idStr()); }
	// From MVert
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual MVert* getExtd() override;
	virtual TDir getDir() const override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
	// From Cnt.Host
	virtual string getCntUid(const string& aName, const string& aIfName) const override { return getUid(aName, aIfName);}
	virtual MContentOwner* cntOwner() override { return this;}
    public:
	static const string KUriInt;  /*!< Internal connpoint */
	Cnt mDir = Cnt(*this, KContDir);
	static const string KContDir;
};

/** @brief Extender, chromoable, monolitic, multicontent, unit. Redirects request for iface to internal CP of extention.
*/
class Extde: public Vert
{
    public:
	inline static constexpr std::string_view idStr() { return "Extde"sv;}
    public:
	Extde(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual string parentName() const override { return string(idStr()); }
	// From MVert
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual MVert* getExtd() override;
	virtual TDir getDir() const override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
    public:
	static const string KUriInt;  /*!< Internal connpoint */
};



/** @brief Socket, monolitic.
*/
class Socket: public Vert, public MSocket
{
    public:
	inline static constexpr std::string_view idStr() { return "Socket"sv;}
    public:
	Socket(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(TIdHash aTid) override;
	virtual string parentName() const override { return string(idStr()); }
	// From MVert
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual MVert* getExtd() override;
	virtual TDir getDir() const override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
	// From MSocket
	virtual string MSocket_Uid() const override  { return getUid<MSocket>();}
	virtual int PinsCount() const override;
	virtual MNode* GetPin(int aInd) override;
	virtual MNode* GetPin(MIfReq::TIfReqCp* aReq) override;
    protected:
	MSocket* mMSocketPtr = nullptr;
};



/** @brief System, inheritable
 * Isn't connectable itlelf but can represent itself via connpoints
 * Can contain connectables and connect them
 * */
class Syst : public Elem, public MAhost, public MActr, public MSyst
{
    public:
	inline static constexpr std::string_view idStr() { return "Syst"sv;}
    public:
	using TAgtCp = NCpOmnp<MAhost, MAgent>;
    public:
	static const char* Type() { return "Syst";}
	static vector<GUri> getParentsUri();
	Syst(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Syst();
	// From Node
	virtual void mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	virtual void mutDisconnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	virtual MIface* MNode_getLif(TIdHash aTid) override;
	virtual MIface* MOwner_getLif(TIdHash aTid) override;
	vector<GUri> parentsUri() const override { return getParentsUri(); }
	// From MActr
	virtual string MActr_Uid() const override {return getUid<MActr>();}
	// TODO Do we really need it? System can observe owned attach and then do attach/detach agent
	virtual bool attachAgent(MAgent::TCp* aAgt) override;
	virtual bool detachAgent(MAgent::TCp* aAgt) override;
	// From MAhost
	virtual string MAhost_Uid() const override {return getUid<MAhost>();}
	virtual MIface* MAhost_getLif(TIdHash aTid) override;
	// From MSyst
	virtual string MSyst_Uid() const override {return getUid<MSyst>();};
	virtual const TEdges& connections() const { return mEdges; }
    protected:
	// From Unit.MIfProvOwner
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
	// Local
	bool isComp(MIfProvOwner* aNode) const;
    protected:
	TAgtCp mAgtCp;  /*!< Agents connpoint */
	TEdges mEdges;  /*!< Edges */
	MAhost* mMAhostPtr = nullptr;
	MActr* mMActrPtr = nullptr;
	MSyst* mMSystPtr = nullptr;
};


/** @brief Connection point - access to MNode
 * Just ConnPointu with pre-configured prepared/required
 * */
class CpMnodeInp: public ConnPointu
{
    public:
	inline static constexpr std::string_view idStr() { return "CpMnodeInp"sv;}
    public:
	CpMnodeInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
        // From MConnPoint
        TIdHash idProvided() const override;
        TIdHash idRequired() const override;
};

/** @brief Connection point - access to MNode
 * Just ConnPointu with pre-configured prepared/required
 * */
class CpMnodeOutp: public ConnPointu
{
    public:
	inline static constexpr std::string_view idStr() { return "CpMnodeOutp"sv;}
    public:
	CpMnodeOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
        // From MConnPoint
        TIdHash idProvided() const override;
        TIdHash idRequired() const override;
};

/** @brief Agent base
 * */
class AgtBase: public Unit, public MAgent
{
    public:
	using TAgtCp = NCpOnp<MAgent, MAhost>;  /*!< Agent conn point */
	using TObserverCp = NCpOmnp<MObserver, MObservable>;
    public:
	AgtBase(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	virtual ~AgtBase();
	// From Base
	virtual MIface* MNode_getLif(TIdHash aTid) override;
	// From MAgent
	virtual string MAgent_Uid() const override {return getUid<MAgent>();}
	virtual MIface* MAgent_getLif(TIdHash aTid) override;
	// From Node.MOwned
	virtual void onOwnerAttached() override;
    protected:
	MNode* ahostNode();
    protected:
	TAgtCp mAgtCp;                   /*!< Agent connpoint */
	MAgent* mMAgentPtr = nullptr;
};

#endif
