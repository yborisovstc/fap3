#ifndef __FAP3_SYST_H
#define __FAP3_SYST_H

#include "menv.h"
#include "msyst.h"

#include "vert.h"
#include "elem.h"

/** @brief Connection point, non-inheritable
 * Don't use Nodes content owner but uses custom conext
 * Has only 2 contents: Provided and Required
 * */
class ConnPointu: public Vertu, public MConnPoint
{
    friend class CpIfrNode;

    public:
    /** @brief Custom content */
    class Cnt : public MContent {
	public:
	    Cnt(ConnPointu& aHost, const string& aName): mHost(aHost), mName(aName) {}
	    virtual ~Cnt();
	    // From MContent
	    virtual string MContent_Uid() const override { return mHost.getUid<MContent>();}
	    virtual MIface* MContent_getLif(const char *aType) override { return nullptr;}
	    virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	    virtual string contName() const override { return mName;}
	    virtual bool getData(string& aData) const override { aData = mData; return true;}
	    virtual bool setData(const string& aData) override { mData = aData; return true;}
	private:
	    const string& mName;
	    string mData;
	    ConnPointu& mHost;
    };

    public:
    static const char* Type() { return "ConnPointu";}
    ConnPointu(const string &aName, MEnv* aEnv);
    virtual ~ConnPointu() {}
    // From MNode
    virtual MIface* MNode_getLif(const char *aType) override;
    // From MVert
    virtual MIface *MVert_getLif(const char *aType) override;
    // From MVert::MCIface
    virtual bool isCompatible(MCIface* aPair) const override;
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

    protected:
    // From Unit
    virtual IfrNode* createIfProv(const string& aName, TIfReqCp* aReq) const override;
    // From Vertu
    virtual void onConnected() override;
    virtual void onDisconnected() override;

    protected:
    Cnt mReq = Cnt(*this, KReqName);
    Cnt mProv = Cnt(*this, KProvName);
    static string KReqName;
    static string KProvName;
};


/** @brief System, inheritable
 * Isn't connectable itlelf but can represent itself via connpoints
 * Can contain connectables and connect them
 * */
class Syst : public Elem
{
    public:
	static const char* Type() { return "Syst";}
	Syst(const string &aName, MEnv* aEnv);
	virtual ~Syst();
	// From Node
	virtual void mutConnect(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	virtual MIface* doMOwnerGetLif(const char *aType) override;
};




#endif
