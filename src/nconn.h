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


/** @brief Native connection point interface
 * @tparam  TPif  provided interface
 * @tparam  TRif  required interface
 * */
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
};

/** @brief Native connection point interface, provided iface proxy variant
 * @tparam  TPif  provided interface
 * @tparam  TRif  required interface
 * */
template <class TPif, class TRif>
class MNcpp
{
    public:
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
};

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
	virtual TPif* provided() override { return this;}
	virtual const TPif* provided() const  { return this;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
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
	NCpOi2() {}
	virtual TPif* provided() override { return this;}
	virtual const TPif* provided() const override { return this;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool connect(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
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
    public:
	TPair* next(TPair* aPair) const {
	    TPair* res = nullptr;
	    auto it = mPairs.find(aPair);
	    if (it != mPairs.end())  res = *(++it);
	    return res;
	};
	TPair* first() const {
	    TPair* res = nullptr;
	    auto it = mPairs.begin();
	    if (it != mPairs.end())  res = *it;
	    return res;
	}
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
	using TPair= typename MNcpp<TPif, TRif>::TPair;
	using TPairs = set<TPair*>;
    public:
	NCpOnp(TPif* aPx): mPx(aPx) {}
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool connect(TPair* aPair) override ;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
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
    assert(aPair && !aPair->isConnected(this) && !isConnected(mPair) == 0);
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOnp<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && isConnected(mPair) == 0);
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


/** @brief Native net tree node, not identified sub-nodes
 * */
template <class TProv, class TReq>
class NTnnp : public NCpOnp<TProv, TReq>
{
    public:
	using TNodeIf = MNcpp<TProv, TReq>; /*!< Node (pole) iface type */
	using TCnodeIf = MNcpp<TReq, TProv>; /*!< Complement node (pole) type */
	using TCnode = NCpOmnp<TReq, TProv>; /*!< Complement node (pole) type */
    public:
	NTnnp(TProv* aProvPx, TReq* aReqPx): NCpOnp<TProv, TReq>(aProvPx), mCnode(aReqPx) {}
	TCnode& cnode() { return mCnode;}
	const TCnode& cnode() const { return mCnode;}
    protected:
	TCnode mCnode;
};








#endif //  __FAP3_NCONN_H
