#ifndef __FAP3_NCONN_H
#define __FAP3_NCONN_H

#include <map>
#include <set>

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
	virtual bool attach(TPair* aPair) = 0;
	virtual bool detach(TPair* aPair) = 0;
	virtual bool isConnected(TPair* aPair) const = 0;
	virtual bool getId(string& aId) const = 0;
	virtual void dump(int aIdt) const {}
	/** @brief Gets binded connpoint, in tree node for instance, ref ds_nn_tree_bc */
	virtual TPair* binded() = 0;
	virtual TPair* firstPair() = 0;
	virtual TPair* nextPair(TPair* aPair) = 0;
	virtual TPair* firstLeaf() = 0;
	/** @brief gets first leaf of binded
	 * Allows to override leafs tree-travarsal behavour in treish topology
	 * Is required for IRM invalidation solution#3, ref ds_irm_ut_inv_cno
	 * */
	virtual TSelf* firstLeafB() = 0;
	virtual TPair* nextLeaf(TPair* aLeaf) = 0;
	/** @brief Gets next leaf from the leaf */
	virtual TSelf* nextLeaf() = 0;
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


#if 0
/** @brief Binded Cp for tree node, ref ds_nn_tree_bc
 * */
template <class TPif, class TRif>
class MNcpb: public MNcpp<TPif, TRif>
{
};
#endif



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


#if 0
/** @brief Tree node, ref ds_nn_tree_si
 * */
template <class TProv, class TReq>
class MNtn: public MNcpp<TProv, TReq> /* Down */, public MNcppTn<TReq, TProv> /* Up */
{
    public:
	// Common
	virtual ~MNtn() {}
    public:
	// Down
	using TPair = typename MNcpp<TProv, TReq>::TPair;
	virtual TProv* provided() = 0;
	virtual const TProv* provided() const = 0;
	virtual bool connect(TPair* aPair) = 0;
	virtual bool disconnect(TPair* aPair) = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool detach(TPair* aPair) = 0;
	virtual bool isConnected(TPair* aPair) const = 0;
	virtual bool getId(string& aId) const = 0;
    public:
	// Up
	using TDPair = typename MNcpp<TReq, TProv>::TPair;
	virtual TProv* tn_provided() = 0;
	virtual const TProv* tn_provided() const = 0;
	virtual bool connect(TDPair* aPair) = 0;
	virtual bool disconnect(TDPair* aPair) = 0;
	virtual bool attach(TDPair* aPair) = 0;
	virtual bool detach(TDPair* aPair) = 0;
	virtual bool isConnected(TDPair* aPair) const = 0;
	virtual bool tn_getId(string& aId) const = 0;
};
#endif

#if 0
/** @brief Native connection point interface, generalized
 * Binds MIfaces instead of particular interfaces, so
 * requires extra ops of iface resolution, that causes overhead
 * */
class MNc
{
    public:
	using TPair = MNc;
	virtual MIface* provided() = 0;
	virtual bool connect(TPair* aPair) = 0;
	virtual bool disconnect(TPair* aPair) = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool detach(TPair* aPair) = 0;
	virtual bool isConnected(TPair* aPair) const = 0;
	virtual bool getId(string& aId) const = 0;
};
#endif



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
	virtual int pcount() const { return mPairs.size(); }
	virtual const TPair* pairAt(int aInd) const { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return it->second; return nullptr; }
	virtual TPair* pairAt(int aInd) { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return it->second; return nullptr; }
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
	using TSelf= typename MNcpp<TPif, TRif>::TSelf;
	using TPair= typename MNcpp<TPif, TRif>::TPair;
	using TPairs = map<string, TPair*>;
	using TPairsElem = pair<string, TPair*>;
    public:
	NCpOmip(TPif* aPx): mPx(aPx) {}
	virtual ~NCpOmip();
	// From MNcpp
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
	virtual void dump(int aIdt) const override {
	    for (auto item : mPairs) {
		item.second->dump(aIdt);
	    }
	}
	virtual TPair* binded() override { return nullptr;}
	virtual TPair* firstPair() { return  nullptr;}
	virtual TPair* nextPair(TPair* aPair) { return nullptr;}
	virtual TPair* firstLeaf() override { return nullptr;}
	virtual TSelf* firstLeafB() override { return nullptr;}
	virtual TPair* nextLeaf(TPair* aLeaf) override { return nullptr;}
	virtual TSelf* nextLeaf() override { return nullptr;}
    public:
	// Local
	int count() const { return mPairs.size();}
	TPair* at(const string& aId) const { return mPairs.count(aId) > 0 ? mPairs.at(aId) : nullptr;}
    protected:
	TPairs mPairs;
	TPif* mPx;
};

template <class TPif, class TRif>
NCpOmip<TPif, TRif>::~NCpOmip()
{
    while (!mPairs.empty()) {
	auto it = mPairs.begin();
	TPair* pair = it->second;
	disconnect(pair);
	delete pair;
    }
}

template <class TPif, class TRif>
bool NCpOmip<TPif, TRif>::attach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && !isConnected(aPair));
    mPairs.insert(TPairsElem(pid, aPair));
    return true;
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



/** @brief Native connection point, one-to-one, supports Id, proxied
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * */
template <class TPif, class TRif>
class NCpOip : public MNcpp<TPif, TRif>
{
    public:
	using TSelf = typename MNcpp<TPif, TRif>::TSelf;
	using TPair = typename MNcpp<TPif, TRif>::TPair;
    public:
	NCpOip(TPif* aPx): mPair(nullptr), mPx(aPx) {}
	virtual ~NCpOip() { delete mPx;}
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
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
	using TSelf= typename MNcpp<TPif, TRif>::TSelf;
	using TPair= typename MNcpp<TPif, TRif>::TPair;
	using TPairs = set<TPair*>;
    public:
	NCpOmnp(TPif* aPx): mPx(aPx) {}
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool connect(TPair* aPair) override ;
	virtual bool disconnect(TPair* aPair) override;
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
	    if (np) {
		res = np->binded() ? np->firstLeafB() : np;
	    } else if (binded()) {
		res = binded()->nextLeaf();
	    }
	    return res;
	}
	virtual TSelf* nextLeaf() override { return nullptr;}
	// Local
	virtual int pcount() const { return mPairs.size(); }
	virtual const TPair* pairAt(int aInd) const { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return *it; return nullptr; }
	virtual TPair* pairAt(int aInd) { for (auto it = mPairs.begin(); it != mPairs.end(); it++) if (aInd-- == 0) return *it; return nullptr; }
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
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool connect(TPair* aPair) override ;
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
class NTnip : public NCpOip<TProv, TReq>
{
    public:
	using TNode = NCpOmip<TReq, TProv>;
    public:
	NTnip(TProv* aProvPx, TReq* aReqPx): NCpOip<TProv, TReq>(aProvPx), mOwnerCp(aReqPx) {}
	NCpOmip<TReq, TProv>& ownerCp() { return mOwnerCp;}
	const NCpOmip<TReq, TProv>& ownerCp() const { return mOwnerCp;}
    protected:
	NCpOmip<TReq, TProv> mOwnerCp;
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
	// Local
	/** @brief Gets first item in tree traversal */
	TPair* firstTreeItem() {
	    auto fp = mCnode.firstPair();
	    auto res = fp;
	    while (fp) {
		auto bnd = fp->binded();
		fp = fp->firstPair();
	    }
	}
	TPair* nextTreeItem(TPair* aPair);
    protected:
	Cnode mCnode;
};








#endif //  __FAP3_NCONN_H
