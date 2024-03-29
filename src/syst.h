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
class ConnPointu: public Vertu, public MConnPoint, public Cnt::Host
{
    public:
	static const char* Type() { return "ConnPointu";}
	ConnPointu(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~ConnPointu() {}
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MVert
	virtual MIface *MVert_getLif(const char *aType) override;
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	// From MConnPoint
	virtual string MConnPoint_Uid() const { return getUid<MConnPoint>();}
	virtual string provName() const override;
	virtual string reqName() const override;
	// From MIfProvOwner
	virtual MIface* MIfProvOwner_getLif(const char *aType) override;
	// From Node.MContentOwner
	virtual int contCount() const override { return 2;}
	virtual MContent* getCont(int aIdx) override;
	virtual const MContent* getCont(int aIdx) const override;
	virtual bool getContent(const GUri& aCuri, string& aRes) const override;
	virtual bool setContent(const GUri& aCuri, const string& aData) override;
	// From Cnt.Host
	virtual string getCntUid(const string& aName, const string& aIfName) const override { return getUid(aName, aIfName);}
	virtual MContentOwner* cntOwner() override { return this;}
    protected:
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From Vertu
	//virtual void onConnected() override;
	//virtual void onDisconnected() override;

    protected:
	Cnt mReq = Cnt(*this, KReqName);
	Cnt mProv = Cnt(*this, KProvName);
	static string KReqName;
	static string KProvName;
};

/** @brief Extender, monolitic, multicontent, unit. Redirects request for iface to internal CP of extention.
*/
class Extd: public Vertu
{
    public:
	static const char* Type() { return "Extd";};
	Extd(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MVert
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual MVert* getExtd() override;
	virtual TDir getDir() const override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
    public:
	static const string KUriInt;  /*!< Internal connpoint */
};


/** @brief Socket, monolitic.
*/
class Socket: public Vert, public MSocket
{
    public:
	static const char* Type() { return "Socket";};
	Socket(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MVert
	virtual bool isCompatible(MVert* aPair, bool aExt) override;
	virtual MVert* getExtd() override;
	virtual TDir getDir() const override;
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MSocket
	virtual string MSocket_Uid() const override  { return getUid<MSocket>();}
	virtual int PinsCount() const override;
	virtual MNode* GetPin(int aInd) override;
	virtual MNode* GetPin(MIfReq::TIfReqCp* aReq) override;
};



/** @brief System, inheritable
 * Isn't connectable itlelf but can represent itself via connpoints
 * Can contain connectables and connect them
 * */
class Syst : public Elem, public MAhost, public MActr, public MSyst
{
    public:
	using TAgtCp = NCpOmnp<MAhost, MAgent>;
    public:
	static const char* Type() { return "Syst";}
	Syst(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Syst();
	// From Node
	virtual void mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	virtual void mutDisconnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	virtual MIface* MNode_getLif(const char *aType) override;
	virtual MIface* MOwner_getLif(const char *aType) override;
	// From MActr
	virtual string MActr_Uid() const override {return getUid<MActr>();}
	virtual bool attachAgent(MAgent::TCp* aAgt) override;
	virtual bool detachAgent(MAgent::TCp* aAgt) override;
	// From MAhost
	virtual string MAhost_Uid() const override {return getUid<MAhost>();}
	virtual MIface* MAhost_getLif(const char *aType) override;
	// From MSyst
	virtual string MSyst_Uid() const override {return getUid<MSyst>();};
	virtual const TEdges& connections() const { return mEdges; }
    protected:
	// From Unit.MIfProvOwner
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// Local
	bool isComp(MIfProvOwner* aNode) const;
    protected:
	TAgtCp mAgtCp;  /*!< Agents connpoint */
	TEdges mEdges;  /*!< Edges */
};


/** @brief Connection point - access to MNode
 * Just ConnPointu with pre-configured prepared/required
 * */
class CpMnodeInp: public ConnPointu
{
    public:
	static const char* Type() { return "CpMnodeInp";};
	CpMnodeInp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};

/** @brief Connection point - access to MNode
 * Just ConnPointu with pre-configured prepared/required
 * */
class CpMnodeOutp: public ConnPointu
{
    public:
	static const char* Type() { return "CpMnodeOutp";};
	CpMnodeOutp(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
};





#endif
