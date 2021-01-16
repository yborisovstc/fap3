#ifndef __FAP3_UNIT_H
#define __FAP3_UNIT_H

#include <string>
#include <map>
#include <list>
#include <cstring>

#include "nconn.h"
#include "munit.h"
#include "mowning.h"
#include "mcontent.h"

#include "cont.h"
#include "cont2.h"
#include "ifr.h"
#include "ifu.h"

using namespace std;

template<class T> MIface* checkLifs(const char* aType, MIface* aSelf) { return (strcmp(aType, T::Type()) == 0) ? dynamic_cast<T*>(aSelf) : nullptr;}
	
class Unit : public MUnit, public MIfProvOwner
{
    public:
	class NCpOwned;

	/** @brief Owner connection point, one-to-many
	 * */
	class NCpOwner : public NCpOmi2<MOwner, MOwned> {
	    friend class Unit;
	    public:
		NCpOwner(Unit* aHost): NCpOmi2<MOwner, MOwned>(), mHost(aHost) {}
		// From MOwner
		virtual string MOwner_Uid() const {return mType;}
		// Local
		// TODO Consider form specific Owned iface instead of getting whole MUnit
		MUnit* munitAt(const string& aId) {
		    MUnit* res = nullptr;
		    MOwned* owd = at(aId);
		    res = owd->lIf(res);
		    return res;
		}

	    private:
		Unit* mHost;
	};
	/** @brief Owned connection point, one-to-one
	 * */
	class NCpOwned : public NCpOi2<MOwned, MOwner> {
	    public:
		NCpOwned(Unit* aHost): NCpOi2<MOwned, MOwner>(), mHost(aHost) {}
		// From MOwned
		virtual string MOwned_Uid() const {return mType;}
		virtual MIface* MOwned_getLif(const char *aType) override {
		    MIface* res = nullptr;
		    if (res = checkLifs<MOwned>(aType, this));
		    if (res = mHost->checkLif<MUnit>(aType));
		    return res;
		}
		virtual string MOwned_() const {return mType;}
		virtual string ownedId() const override { return mHost->name();}
		virtual void deleteOwned() override { delete mHost;}
		// From MNcp
		virtual bool getId(string& aId) const override { aId = mHost->name(); return true;}
	    private:
		Unit* mHost;
	};

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
		virtual void MCont2_dump(int aIdt) const {
		    mDefalutContent.dump(++aIdt);
		    ContNode2::MCont2_dump(++aIdt);
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
	constexpr static const char* mType = "Unit";
	Unit(const string &aName, MEnv* aEnv);
	virtual ~Unit();
	// From MUnit
	virtual string MUnit_Uid() const override { return mName;}
	virtual MIface* MUnit_getLif(const char *aType) override;
	virtual string name() const override { return mName;}
	virtual MUnit* getComp(const string& aId) override;
	virtual MUnit* getNode(const GUri& aUri) override;
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
	void deleteOwned();
	template<class T> MIface* checkLif(const char* aType) { return (strcmp(aType, T::Type()) == 0) ? dynamic_cast<T*>(this) : nullptr;}
	virtual IfrNode* createIfProv(const string& aName, TIfReqCp* aReq) const;
	void invalidateIrm();
    public:
	NCpOwner mCpOwner = NCpOwner(this);
	NCpOwned mCpOwned = NCpOwned(this);
    protected:
	string mName;
	MEnv* mEnv;
	RootContent mContent = RootContent(this);
	map<string, IfrNode*> mLocalIrn; /*!< Local IFR node */
	list<IfrNode*> mIrns;  /*! IFR nodes */
	//list<MIfProv*> mIrns;  /*! IFR nodes */
};

#endif // __FAP3_UNIT_H

