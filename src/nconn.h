#ifndef __FAP3_NCONN_H
#define __FAP3_NCONN_H

#include <map>
#include <unordered_map>
#include <set>
#include <string>

#include <assert.h>

using namespace std;

/** @brief Native connection point, one-to-many, seleting pair by Id
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * */
template <class TPif, class TPair, class THost>
class NCpOmi : public TPif
{
    public:
	using TPairs = map<string, TPair*>;
	using TPairsElem = pair<string, TPair*>;
    public:
	NCpOmi(THost* aHost): mHost(aHost) {}
	bool connect(TPair* aPair);
	bool disconnect(TPair* aPair);
	bool isConnected(TPair* aPair) const;
    protected:
	TPairs mPairs;
	THost* mHost;
};


template <class TPif, class TPair, class THost>
bool NCpOmi<TPif, TPair, THost>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && mPairs.count(aPair->ownedId()) == 0);
    bool res = aPair->connect(this);
    if (res) {
	mPairs.insert(TPairsElem(aPair->ownedId(), aPair));
    }
    return res;
}

template <class TPif, class TPair, class THost>
bool NCpOmi<TPif, TPair, THost>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && mPairs.count(aPair->ownedId()) == 1);
    bool res = aPair->disconnect(this);
    if (res) {
	mPairs.erase(aPair->ownedId());
    }
    return res;
}

template <class TPif, class TPair, class THost>
bool NCpOmi<TPif, TPair, THost>::isConnected(TPair* aPair) const
{
    return mPairs.count(aPair->ownedId()) == 1 && mPairs.at(aPair->ownedId()) == aPair;
}



/** @brief Native connection point, one-to-one, supports Id
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * */
template <class TPif, class TPair, class THost>
class NCpOi : public TPif
{
    public:
	NCpOi(THost* aHost): mHost(aHost) {}
	bool connect(TPair* aPair);
	bool disconnect(TPair* aPair);
	bool isConnected(TPair* aPair) const;
    protected:
	TPair* mPair;
	THost* mHost;
};


template <class TPif, class TPair, class THost>
bool NCpOi<TPif, TPair, THost>::connect(TPair* aPair)
{
    assert(aPair && !mPair && !aPair->isConnected(this));
    bool res = true;
    mPair = aPair;
    return res;
}

template <class TPif, class TPair, class THost>
bool NCpOi<TPif, TPair, THost>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this));
    bool res = true;
    mPair = nullptr;
    return res;
}

/** @brief Native connection point interface, provided iface proxy variant
 * @tparam  TPif  provided interface
 * @tparam  TRif  required interface
 * */
// TODO rename to MNc and use as the base class for connpoint
template <class TPif, class TRif>
class MNcpp
{
    public:
	using TSelf = MNcpp<TPif, TRif>;
	using TPair = MNcpp<TRif, TPif>;
	virtual ~MNcpp() {}
	virtual TPif* provided() = 0;
	virtual const TPif* provided() const = 0;
	virtual bool connect(TPair* aPair) = 0;
	virtual bool disconnect(TPair* aPair) = 0;
	virtual bool disconnectAll() = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool detach(TPair* aPair) = 0;
	virtual bool isConnected(TPair* aPair) const = 0;
	virtual bool getId(string& aId) const = 0;
	virtual void dump(int aIdt) const {}
	/** @brief Gets binded connpoint, in tree node for instance, ref ds_nn_tree_bc */
	virtual TPair* binded() = 0;
	const TPair* binded() const { const_cast<const TPair*>(const_cast<TSelf*>(this)->binded());}
	virtual TPair* firstPair() = 0;
	virtual TPair* nextPair(TPair* aPair) = 0;
	/** @brief Gets pairs count */
	virtual int pcount(bool aRcr = false) const = 0;
	virtual const TPair* pairAt(int aInd) const = 0;
	virtual TPair* pairAt(const string aId) = 0;
	virtual const TPair* pairAt(const string aId) const = 0;
	virtual TPair* pairAt(int aInd) = 0;
	/** @brief gets first leaf */
	virtual TPair* firstLeaf() = 0;
	/** @brief gets first leaf of binded
	 * Allows to override leafs tree-travarsal behavour in treish topology
	 * Is required for IRM invalidation solution#3, ref ds_irm_ut_inv_cno
	 * */
	virtual TSelf* firstLeafB() = 0;
	virtual TPair* nextLeaf(TPair* aLeaf) = 0;
	/** @brief Gets next leaf from the leaf */
	virtual TSelf* nextLeaf() = 0;
	/** @brief Find owned item
	 * @param aItem  current item, if nullptr then the first item is requested
	 * @returns item predecessor if the item is found
	 * */
	virtual TSelf* findItem(TPair* aItem) = 0;
	virtual TPair* findItem(TSelf* aItem) = 0;
};


/** @brief Native connection point interface with direct inheritance of provided iface
 * @tparam  TPif  provided interface
 * @tparam  TRif  required interface
 * */
// TODO inherit from MNcpp or better not use it at all
template <class TPif, class TRif>
class MNcp: public TPif
{
    public:
	using TPair = MNcp<TRif, TPif>;
	virtual ~MNcp() {}
	virtual TPif* provided() = 0;
	virtual const TPif* provided() const = 0;
	virtual bool connect(TPair* aPair) = 0;
	virtual bool disconnect(TPair* aPair) = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool detach(TPair* aPair) = 0;
	virtual bool isConnected(TPair* aPair) const = 0;
	virtual bool getId(string& aId) const = 0;
	virtual TPair* at() = 0;
	virtual const TPair* at() const = 0;
	virtual TPair* at(const string aId) = 0;
};


/** @brief Cp stub for Tree node 
 * */
template <class TPif, class TRif>
class MNcppTn
{
    public:
	virtual TPif* provided() override { return tn_provided();}
	virtual TPif* tn_provided() = 0;
	virtual const TPif* provided() const override { return tn_provided();}
	virtual const TPif* tn_provided() const = 0;
	virtual bool getId(string& aId) const override { return tn_getId(aId);}
	virtual bool tn_getId(string& aId) const = 0;
};


/** @brief Native connection point, one-to-many, seleting pair by Id
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * */
template <class TPif, class TRif>
class NCpOmi2 : public MNcp<TPif, TRif>
{
    public:
	using TPair= typename MNcp<TPif, TRif>::TPair;
	using TPairs = map<string, TPair*>;
	using TPairsElem = pair<string, TPair*>;
    public:
	NCpOmi2() {}
	// From MNcp
	virtual TPif* provided() override { return this;}
	virtual const TPif* provided() const  { return this;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual TPair* at() override { return nullptr;}
	virtual const TPair* at() const override { return nullptr; }
	virtual const TPair* at(const string aId) const { return mPairs.count(aId) > 0 ? mPairs.at(aId) : nullptr;;}
	virtual TPair* at(const string aId) { return mPairs.count(aId) > 0 ? mPairs.at(aId) : nullptr;;}
	// Local
	virtual int pcount(bool aRcr = false) const override { return mPairs.size(); }
	virtual const TPair* pairAt(int aInd) const override { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return it->second; return nullptr; }
	virtual TPair* pairAt(int aInd) override { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return it->second; return nullptr; }
	virtual TPair* firstPair() { return mPairs.size() > 0 ? mPairs.begin()->second : nullptr;}
	virtual TPair* nextPair(TPair* aPair) {
	    string id; aPair->getId(id);
	    auto it = mPairs.find(id); it++;
	    auto res = (it != mPairs.end()) ? it->second : nullptr; 
	    return res;
	}
    protected:
	TPairs mPairs;
};

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::attach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && !isConnected(aPair));
    mPairs.insert(TPairsElem(pid, aPair));
    return true;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::detach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && isConnected(aPair));
    mPairs.erase(pid);
    return true;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && !isConnected(aPair));
    bool res = aPair->attach(this);
    if (res) {
       	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && isConnected(aPair));
    bool res = aPair->detach(this);
    if (res) {
       res = detach(aPair);
    }
    return res;
}


template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::isConnected(TPair* aPair) const
{
    string pid;
    assert(aPair && aPair->getId(pid));
    return mPairs.count(pid) == 1 && mPairs.at(pid) == aPair;
}


/** @brief Native connection point, one-to-one, supports Id
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * */
template <class TPif, class TRif>
class NCpOi2 : public MNcp<TPif, TRif>
{
    public:
	//using TPair = MNcp<TRif, TPif>;
	using TPair = typename MNcp<TPif, TRif>::TPair;
    public:
	NCpOi2(): mPair(nullptr) {}
	virtual TPif* provided() override { return this;}
	virtual const TPif* provided() const override { return this;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual TPair* at() override { return mPair;}
	virtual const TPair* at() const override { return mPair;}
	virtual TPair* at(const string aId) { return nullptr;}
    protected:
	TPair* mPair;
};


template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::attach(TPair* aPair)
{
    assert(aPair && !mPair && !isConnected(aPair));
    mPair = aPair;
    return true;
}

template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::detach(TPair* aPair)
{
    assert(aPair && isConnected(aPair));
    mPair = nullptr;
    return true;
}

template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !mPair && !aPair->isConnected(this) && !isConnected(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return true;
}

template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    return true;
}


template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::isConnected(TPair* aPair) const
{
    return mPair &&  aPair == mPair;
}


/** @brief Native connection point, one-to-many, seleting pair by Id, proxied
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * */
template <class TPif, class TRif>
class NCpOmip : public MNcpp<TPif, TRif>
{
    public:
	using TThis = NCpOmip<TPif, TRif>;
	using TSelf= typename MNcpp<TPif, TRif>::TSelf;
	using TPair= typename MNcpp<TPif, TRif>::TPair;
	using TPairs = unordered_map<string, TPair*>;
	using TPairsElem = pair<string, TPair*>;
    public:
	NCpOmip(TPif* aPx): mPx(aPx) {}
	virtual ~NCpOmip() { disconnectAll(); }
	// From MNcpp
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool disconnectAll() override {
	    bool res = true;
	    TPair* pair = firstPair();
	    while (pair) {
		res = disconnect(pair);
		assert(res);
		pair = firstPair();
	    }
	    return res;
	}
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual void dump(int aIdt) const override {
	    for (auto item : mPairs) {
		item.second->dump(aIdt);
	    }
	}
	virtual TPair* binded() override { return nullptr;}
	virtual TPair* firstPair() { return mPairs.size() > 0 ? mPairs.begin()->second : nullptr;}
	virtual TPair* nextPair(TPair* aPair) {
	    TPair* res = nullptr;
	    string pid;
	    bool ires = aPair->getId(pid);
	    if (ires) {
		auto it = mPairs.find(pid); it++;
		res = (it != mPairs.end()) ? it->second : nullptr; 
	    }
	    return res;
	}
	virtual TPair* firstLeaf() override { return nullptr;}
	virtual TSelf* firstLeafB() override { return nullptr;}
	virtual TPair* nextLeaf(TPair* aLeaf) override { return nullptr;}
	virtual TSelf* nextLeaf() override { return nullptr;}
	virtual int pcount(bool aRcr = false) const override { return mPairs.size(); }
	virtual const TPair* pairAt(int aInd) const override { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return it->second; return nullptr; }
	virtual TPair* pairAt(int aInd) override { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return it->second; return nullptr; }
	virtual TPair* pairAt(const string aId) { return mPairs.count(aId) > 0 ? mPairs.at(aId) : nullptr;}
	virtual const TPair* pairAt(const string aId) const { return const_cast<const TPair*>(const_cast<TThis*>(this)->pairAt(aId));}
	virtual TSelf* findItem(TPair* aItem) override;
	virtual TPair* findItem(TSelf* aItem) override;
    protected:
	TPairs mPairs;
	TPif* mPx;
};

template <class TPif, class TRif>
bool NCpOmip<TPif, TRif>::attach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && !isConnected(aPair));
    auto res = mPairs.insert(TPairsElem(pid, aPair));
    return res.second;
}

template <class TPif, class TRif>
bool NCpOmip<TPif, TRif>::detach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && isConnected(aPair));
    mPairs.erase(pid);
    return true;
}

template <class TPif, class TRif>
bool NCpOmip<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && !isConnected(aPair));
    bool res = aPair->attach(this);
    if (res) {
       	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmip<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && isConnected(aPair));
    bool res = aPair->detach(this);
    if (res) {
       res = detach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmip<TPif, TRif>::isConnected(TPair* aPair) const
{
    string pid;
    assert(aPair && aPair->getId(pid));
    return mPairs.count(pid) == 1 && mPairs.at(pid) == aPair;
}

template <class TPif, class TRif>
typename NCpOmip<TPif, TRif>::TSelf* NCpOmip<TPif, TRif>::findItem(TPair* aItem)
{
    TSelf* res = nullptr;
    auto pair = firstPair();
    while (pair && !res) {
	if (pair == aItem) {
	    res = this; 
	} else {
	    res = pair->binded()->findItem(aItem);
	}
	pair = nextPair(pair);
    }
    return res;
}

template <class TPif, class TRif>
typename NCpOmip<TPif, TRif>::TPair* NCpOmip<TPif, TRif>::findItem(TSelf* aItem)
{
    TPair* res = nullptr;
    auto pair = firstPair();
    while (pair && !res) {
	if (pair->binded() == aItem) {
	    res = pair; 
	} else {
	    res = pair->binded()->findItem(aItem);
	}
	pair = nextPair(pair);
    }
    return res;
}




/** @brief Native connection point, one-to-one, supports Id, proxied
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * */
// TODO seems same as NCpOnp, why is it needed? 
template <class TPif, class TRif>
class NCpOip : public MNcpp<TPif, TRif>
{
    public:
	using TSelf = typename MNcpp<TPif, TRif>::TSelf;
	using TPair = typename MNcpp<TPif, TRif>::TPair;
    public:
	NCpOip(TPif* aPx): mPair(nullptr), mPx(aPx) {}
	virtual ~NCpOip() { disconnectAll();}
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool disconnectAll() override {
	    bool res = true;
	    if (mPair) res = disconnect(mPair);
	    return res;
	}
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual void dump(int aIdt) const { mPx->dump(aIdt); }
	virtual TPair* binded() override { return nullptr;}
	virtual TPair* firstPair() { return  nullptr;}
	virtual TPair* nextPair(TPair* aPair) { return nullptr;}
	virtual TPair* firstLeaf() override { return nullptr;}
	virtual TSelf* firstLeafB() override { return nullptr;}
	virtual TPair* nextLeaf(TPair* aLeaf) override { return nullptr;}
	virtual TSelf* nextLeaf() override { return nullptr;}
    public:
	void resetPx() {mPx = nullptr;}
    protected:
	TPair* mPair;
	TPif* mPx;
};

template <class TPif, class TRif>
bool NCpOip<TPif, TRif>::attach(TPair* aPair)
{
    assert(aPair && !mPair && !isConnected(aPair));
    mPair = aPair;
    return true;
}

template <class TPif, class TRif>
bool NCpOip<TPif, TRif>::detach(TPair* aPair)
{
    assert(aPair && isConnected(aPair));
    mPair = nullptr;
    return true;
}

template <class TPif, class TRif>
bool NCpOip<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !mPair && !aPair->isConnected(this) && !isConnected(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return true;
}

template <class TPif, class TRif>
bool NCpOip<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    return true;
}

template <class TPif, class TRif>
bool NCpOip<TPif, TRif>::isConnected(TPair* aPair) const
{
    return mPair &&  aPair == mPair;
}





/** @brief Native connection point, one-to-many, no Id
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * */
template <class TPif, class TRif>
class NCpOmn : public MNcp<TPif, TRif>
{
    public:
	using TPair= typename MNcp<TPif, TRif>::TPair;
	using TPairs = set<TPair*>;
    public:
	NCpOmn() {}
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual TPair* at() override { return nullptr;}
	virtual const TPair* at() const override { return nullptr;}
	virtual TPair* at(const string aId) { return mPairs.at(aId);}
    protected:
	TPairs mPairs;
};

template <class TPif, class TRif>
bool NCpOmn<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && mPairs.count(aPair) == 0);
    bool res = aPair->connect(this);
    if (res) {
	mPairs.insert(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmn<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && mPairs.count(aPair) == 1);
    bool res = aPair->disconnect(this);
    if (res) {
	mPairs.erase(aPair->getId());
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmn<TPif, TRif>::isConnected(TPair* aPair) const
{
    return mPairs.count(aPair) == 1;
}


/** @brief Native connection point, one-to-one, doesn't supports Id
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * */
template <class TPif, class TRif>
class NCpOn : public MNcp<TPif, TRif>
{
    public:
	using TPair = typename MNcp<TPif, TRif>::TPair;
    public:
	NCpOn() {}
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual TPair* at() override { return mPair;}
	virtual const TPair* at() const override { return mPair;}
	virtual TPair* at(const string aId) { return nullptr;}
    protected:
	TPair* mPair;
};


template <class TPif, class TRif>
bool NCpOn<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !mPair && !aPair->isConnected(this));
    bool res = true;
    mPair = aPair;
    return res;
}

template <class TPif, class TRif>
bool NCpOn<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this));
    bool res = true;
    mPair = nullptr;
    return res;
}

template <class TPif, class TRif>
bool NCpOn<TPif, TRif>::isConnected(TPair* aPair) const
{
    return mPair &&  aPair == mPair;
}



/** @brief Native connection point, one-to-many, no Id, proxy
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * @tparam TPx type of provided iface proxy
 * */
template <class TPif, class TRif>
class NCpOmnp : public MNcpp<TPif, TRif>
{
    public:
	using TThis= typename NCpOmnp<TPif, TRif>::TSelf;
	using TSelf= typename MNcpp<TPif, TRif>::TSelf;
	using TPair= typename MNcpp<TPif, TRif>::TPair;
	using TPairs = set<TPair*>;
    public:
	NCpOmnp(TPif* aPx): mPx(aPx) {}
	virtual ~NCpOmnp() { disconnectAll(); }
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool connect(TPair* aPair) override ;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool disconnectAll() override {
	    bool res = true;
	    TPair* pair = firstPair();
	    while (pair) {
		res = res && disconnect(pair);
		assert(res);
		pair = firstPair();
	    }
	    return res;
	}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual TPair* binded() override { return nullptr;}
	virtual TPair* firstPair() { return mPairs.size() > 0 ? *mPairs.begin() : nullptr;}
	virtual TPair* nextPair(TPair* aPair) {
	    auto it = mPairs.find(aPair); it++;
	    auto res = (it != mPairs.end()) ? *it : nullptr; 
	    return res;
	}
	virtual TPair* firstLeaf() override {
	    TPair* res = nullptr;
	    auto pair = firstPair();
	    while (pair) {
		if (res = pair->binded() ? pair->firstLeafB() : pair) break;
		pair = nextPair(pair);
	    }
	    return res;
	}
	virtual TSelf* firstLeafB() override { return nullptr;}
	virtual TPair* nextLeaf(TPair* aLeaf) override {
	    TPair* res = nullptr;
	    auto np = nextPair(aLeaf);
	    while (np) {
		res = np->binded() ? np->firstLeafB() : np;
		if (res) break;
		np = nextPair(np);
	    }
	    if (! res && binded()) {
		res = binded()->nextLeaf();
	    }
	    return res;
	}
	virtual TSelf* nextLeaf() override { return nullptr;}
	virtual int pcount(bool aRcr = false) const override {
	    int res = mPairs.size();
	    if (aRcr) {
		for (auto it = mPairs.begin(); it != mPairs.end(); it++) {
		    auto pair = *it;
		    res += pair->pcount(aRcr);
		}
	    }
	    return res;
	}
	virtual const TPair* pairAt(int aInd) const override { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return *it; return nullptr; }
	virtual TPair* pairAt(int aInd) override { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return *it; return nullptr; }
	virtual TPair* pairAt(const string aId) { return nullptr;}
	virtual const TPair* pairAt(const string aId) const { return nullptr;}
	virtual TSelf* findItem(TPair* aItem) override { return nullptr;}
	virtual TPair* findItem(TSelf* aItem) override { return nullptr;}
    protected:
	TPairs mPairs;
	TPif* mPx;
};

    template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::attach(TPair* aPair)
{
    assert(aPair && !isConnected(aPair));
    mPairs.insert(aPair);
    return true;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::detach(TPair* aPair)
{
    assert(aPair && isConnected(aPair));
    mPairs.erase(aPair);
    return true;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && !isConnected(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && isConnected(aPair));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    assert(res);
    return res;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::isConnected(TPair* aPair) const
{
    return mPairs.count(aPair) == 1;
}


/** @brief Native connection point, one-to-one, no Id, proxy
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * @tparam TPx type of provided iface proxy
 * */
template <class TPif, class TRif>
class NCpOnp : public MNcpp<TPif, TRif>
{
    public:
	using TSelf= typename MNcpp<TPif, TRif>::TSelf;
	using TPair= typename MNcpp<TPif, TRif>::TPair;
	using TPairs = set<TPair*>;
    public:
	NCpOnp(TPif* aPx): mPair(nullptr), mPx(aPx) {}
	virtual ~NCpOnp() { disconnectAll(); }
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool connect(TPair* aPair) override ;
	virtual bool disconnectAll() override {
	    bool res = true;
	    if (mPair) res = disconnect(mPair);
	    return res;
	}
	virtual bool disconnect(TPair* aPair) override;
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual TPair* binded() override { return nullptr;}
	virtual TPair* firstPair() { return  mPair;}
	virtual TPair* nextPair(TPair* aPair) { return nullptr;}
	virtual TPair* firstLeaf() override { return nullptr;}
	virtual TSelf* firstLeafB() override { return binded() ? binded()->firstLeaf() : nullptr;}
	virtual TPair* nextLeaf(TPair* aLeaf) override { return nullptr;}
	virtual TSelf* nextLeaf() override {
	    return mPair ? mPair->nextLeaf(this) : nullptr;
	}
	virtual int pcount(bool aRcr = false) const override {return mPair ? 1 : 0;}
	virtual const TPair* pairAt(int aInd) const override { return (aInd < pcount()) ? mPair : nullptr; }
	virtual TPair* pairAt(int aInd) override { return (aInd < pcount()) ? mPair : nullptr; }
	virtual TPair* pairAt(const string aId) override { return nullptr;}
	virtual const TPair* pairAt(const string aId) const { return nullptr;}
	virtual TSelf* findItem(TPair* aItem) override {
	    TSelf* res = nullptr;
	    if (binded() == aItem) {
		res = this;
	    }
	    return res;
	}
	virtual TPair* findItem(TSelf* aItem) override {
	    TPair* res = nullptr;
	    return res;
	}
	// Local
	void resetPx() {mPx = nullptr;}
	bool disconnect() { return  mPair ? disconnect(mPair) : false; }
    protected:
	TPair* mPair;
	TPif* mPx;
};

template <class TPif, class TRif>
bool NCpOnp<TPif, TRif>::attach(TPair* aPair)
{
    assert(aPair && !mPair && !isConnected(aPair));
    mPair = aPair;
    return true;
}

template <class TPif, class TRif>
bool NCpOnp<TPif, TRif>::detach(TPair* aPair)
{
    assert(aPair && isConnected(aPair));
    mPair = nullptr;
    return true;
}

template <class TPif, class TRif>
bool NCpOnp<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && !isConnected(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOnp<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && isConnected(aPair));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOnp<TPif, TRif>::isConnected(TPair* aPair) const
{
    return mPair == aPair;
}





#if 0
/** @brief Native connection point, generiliezed iface, one-to-many, seleting pair by Id
 * @tparam TProv  type of provided interface
 * @tparam TPreq  type of required interface
 * */
template <class TProv, class TReq>
class NCOmi : public MNc
{
    public:
	using TPair= typename MNc::TPair;
	using TPairs = map<string, TPair*>;
	using TPairsElem = pair<string, TPair*>;
    public:
	NCOmi() {}
	virtual MIface* provided() override { return mPx;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
    protected:
	TPairs mPairs;
	TProv* mPx;
};

template <class TProv, class TReq>
bool NCOmi<TProv, TReq>::attach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && !aPair->isConnected(this) && !isConnected(aPair));
    mPairs.insert(TPairsElem(pid, aPair));
    return true;
}

template <class TProv, class TReq>
bool NCOmi<TProv, TReq>::detach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && aPair->isConnected(this) && isConnected(aPair));
    mPairs.erase(pid);
    return true;
}

template <class TProv, class TReq>
bool NCOmi<TProv, TReq>::connect(TPair* aPair)
{
    bool res = aPair->attach(this);
    if (res) {
       	res = attach(aPair);
    }
    return res;
}

template <class TProv, class TReq>
bool NCOmi<TProv, TReq>::disconnect(TPair* aPair)
{
    bool res = aPair->detach(this);
    if (res) {
       res = detach(aPair);
    }
    return res;
}


template <class TProv, class TReq>
bool NCOmi<TProv, TReq>::isConnected(TPair* aPair) const
{
    string pid;
    assert(aPair && aPair->getId(pid));
    return mPairs.count(pid) == 1 && mPairs.at(pid) == aPair;
}
#endif


/** @brief Native net tree node, identified sub-nodes
 * */
template <class TProv, class TReq>
class NTnip : public NCpOnp<TProv, TReq>
{
    public:
	using TScp = NCpOnp<TProv, TReq>;  /*!< Self connpoint type */
	using TPair = typename TScp::TPair;
	using TCnode = NCpOmip<TReq, TProv>; /*!< Complement node (pole) type */
    public:
	class Cnode: public TCnode {
	    public:
		Cnode(TReq* aPx, NTnip* aHost): TCnode(aPx), mHost(aHost) {}
		// From MNcpp
		virtual typename TCnode::TPair* binded() override { return mHost->cnodeBinded();}
	    private:
		NTnip* mHost;
	};
    public:
	NTnip(TProv* aProvPx, TReq* aReqPx): NCpOnp<TProv, TReq>(aProvPx), mCnode(aReqPx) {}
	// From MNcpp
	virtual typename TScp::TPair* binded() override { return &mCnode;}
	virtual bool disconnectAll() override {
	    bool res = TScp::disconnectAll();
	    res = res && mCnode.disconnectAll();
	    return res;
	}
	// Local
	virtual typename TCnode::TPair* cnodeBinded() { return this;}
    protected:
	TCnode mCnode;
};


/** @brief Native net tree node, not indexed sub-nodes
 * */
template <class TProv, class TReq>
class NTnnp : public NCpOnp<TProv, TReq>
{
    public:
	using TScp = NCpOnp<TProv, TReq>;  /*!< Self connpoint type */
	using TPair = typename TScp::TPair;
	using TNodeIf = MNcpp<TProv, TReq>; /*!< Node (pole) iface type */
	using TCnodeIf = MNcpp<TReq, TProv>; /*!< Complement node (pole) type */
	using TCnode = NCpOmnp<TReq, TProv>; /*!< Complement node (pole) type */
    public:
	class Cnode: public TCnode {
	    public:
		Cnode(TReq* aPx, NTnnp* aHost): TCnode(aPx), mHost(aHost) {}
		// From MNcpp
		virtual typename TCnode::TPair* binded() override { return mHost;}
	    private:
		NTnnp* mHost;
	};
    public:
	NTnnp(TProv* aProvPx, TReq* aReqPx): NCpOnp<TProv, TReq>(aProvPx), mCnode(aReqPx, this) {}
	// From MNcpp
	virtual typename TScp::TPair* binded() override { return &mCnode;}
	virtual bool disconnectAll() override {
	    bool res = TScp::disconnectAll();
	    res = res && mCnode.disconnectAll();
	    return res;
	}
	virtual int pcount(bool aRcr = false) const override {
	    return mCnode.pcount(aRcr);
	}
    protected:
	Cnode mCnode;
};








#endif //  __FAP3_NCONN_H
