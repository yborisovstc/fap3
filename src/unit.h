#ifndef __FAP3_UNIT_H
#define __FAP3_UNIT_H

#include <string>
#include <map>

#include "nconn.h"
#include "munit.h"
#include "mowning.h"
#include "mcontent.h"

#include "cont.h"
#include "cont2.h"

using namespace std;

class Unit : public MUnit
{
    public:
	class NCpOwned;
#if 0
	/** @brief Owner connection point, one-to-many
	 * */
	class NCpOwner : public MOwner {
	    friend class Unit;
	    public:
		using TPair = NCpOwned;
		using TPairs = map<string, TPair*>;
		using TPairsElem = pair<string, TPair*>;
	    public:
		NCpOwner(Unit* aHost): mHost(aHost) {}
		bool connect(TPair* aPair);
		bool disconnect(TPair* aPair);
		bool isConnected(TPair* aPair) const;
		// From MOwner
		virtual string MOwner_Uid() const {return mType;}
	    private:
		TPairs mPairs;
		Unit* mHost;
	};
	/** @brief Owned connection point, one-to-one
	 * */
	class NCpOwned : public MOwned {
	    public:
		using TPair = NCpOwner;
	    public:
		NCpOwned(Unit* aHost): mHost(aHost) {}
		bool connect(TPair* aPair);
		bool disconnect(TPair* aPair);
		bool isConnected(TPair* aPair) const { return mPair && mPair == aPair;}
		// From MOwner
		virtual string MOwned_Uid() const {return mType;}
		virtual string ownedId() const override { return mHost->name();}
		virtual void deleteOwned() override;
	    private:
		TPair* mPair;
		Unit* mHost;
	};
#endif

#if 0
	/** @brief Owner connection point, one-to-many
	 * */
	class NCpOwner : public NCpOmi<MOwner, NCpOwned, Unit> {
	    friend class Unit;
	    public:
		NCpOwner(Unit* aHost): NCpOmi<MOwner, NCpOwned, Unit>(aHost) {}
		// From MOwner
		virtual string MOwner_Uid() const {return mType;}
	};
	/** @brief Owned connection point, one-to-one
	 * */
	class NCpOwned : public NCpOi<MOwned, NCpOwner, Unit> {
	    public:
		NCpOwned(Unit* aHost): NCpOi<MOwned, NCpOwner, Unit>(aHost) {}
		// From MOwned
		virtual string MOwned_Uid() const {return mType;}
		virtual string ownedId() const override { return mHost->name();}
		virtual void deleteOwned() override { delete mHost;}
	};
#endif

	/** @brief Owner connection point, one-to-many
	 * */
	class NCpOwner : public NCpOmi2<MOwner, MOwned> {
	    friend class Unit;
	    public:
		NCpOwner(Unit* aHost): NCpOmi2<MOwner, MOwned>(), mHost(aHost) {}
		// From MOwner
		virtual string MOwner_Uid() const {return mType;}
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
		RootContent(Unit* aHost): mHost(aHost) {}
	    protected:
		// From ContNode
		virtual MCont2* nodeGetContent(const CUri& aUri) override {
		    MCont2* res = nullptr;
		    if (aUri.size() == 0) {
			res = &mDefalutContent;
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
	virtual string name() const override { return mName;}
	virtual bool getContent(string& aData, const string& aName) const override;
	virtual bool setContent(const string& aData, const string& aName) override;
	virtual bool addContent(const string& aName, bool aLeaf = false) override;
    protected:
	void deleteOwned();
    public:
	NCpOwner mCpOwner = NCpOwner(this);
	NCpOwned mCpOwned = NCpOwned(this);
    protected:
	string mName;
	MEnv* mEnv;
	RootContent mContent = RootContent(this);
};

#endif // __FAP3_UNIT_H

