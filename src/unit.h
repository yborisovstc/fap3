#ifndef __FAP3_UNIT_H
#define __FAP3_UNIT_H

#include <string>
#include <map>
#include <list>

#include "nconn.h"
#include "munit.h"
#include "mcontent.h"
#include "node.h"

#include "cont.h"
#include "cont2.h"
#include "ifr.h"
#include "ifu.h"

using namespace std;

	
class Unit : public Node, public MUnit, public MIfProvOwner
{
    public:
	friend class RootContent;

	/** @brief Unit root content - specialized version of content node
	 * the specific is that the content contains pre-allocated default conten
	 * with empty key (URI is .)
	 * */
	class RootContent : public ContNode2 {
	    public:
		RootContent(Unit* aHost): ContNode2(string()), mHost(aHost) {}
		virtual ~RootContent() {}
		// From MCont
		virtual void MCont2_doDump(int aLevel, int aIdt, ostream& aOs) const {
		    mDefalutContent.doDump(aLevel, ++aIdt, aOs);
		    ContNode2::MCont2_doDump(aLevel, ++aIdt, aOs);
		}
		// From ContNode
		virtual MCont2* getContent(const CUri& aUri) const override {
		    MCont2* res = nullptr;
		    if (aUri.size() == 0) {
			const MCont2* cont = &mDefalutContent;
			res = const_cast<MCont2*>(cont);
		    } else {
			res = ContNode2::getContent(aUri);
		    }
		    return res;
		}
	    private:
		Unit* mHost;
		Cont2 mDefalutContent;
	};

    public:
	static const char* Type() { return "Unit";}
	Unit(const string &aName, MEnv* aEnv);
	virtual ~Unit();
	// From MUnit
	virtual string MUnit_Uid() const override { return name();}
	virtual MIface* MUnit_getLif(const char *aType) override;
	virtual bool getContent(string& aData, const string& aName = string()) const override;
	virtual bool setContent(const string& aData, const string& aName = string()) override;
	virtual bool addContent(const string& aName, bool aLeaf = false) override;
	virtual void dumpContent() const override { dynamic_cast<const MCont2&>(mContent).dump(0);}
	virtual MIfProv* defaultIfProv(const string& aName) override;
	virtual bool resolveIface(const string& aName, TIfReqCp* aReq) override;
	// From MIfProvOwner
	virtual string MIfProvOwner_Uid() const override {return name() +  Ifu::KUidSep + MIfProvOwner::Type();}
	virtual MIface* MIfProvOwner_getLif(const char *aType) override;
	virtual void onIfpDisconnected(MIfProv* aProv) override;
    protected:
	virtual IfrNode* createIfProv(const string& aName, TIfReqCp* aReq) const;
	void invalidateIrm();
    protected:
	RootContent mContent = RootContent(this);
	map<string, IfrNode*> mLocalIrn; /*!< Local IFR node */
	list<IfrNode*> mIrns;  /*! IFR nodes */
	//list<MIfProv*> mIrns;  /*! IFR nodes */
};

#endif // __FAP3_UNIT_H

