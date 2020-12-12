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
	virtual bool connect(TPair* aPair) = 0;
	virtual bool disconnect(TPair* aPair) = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool deattach(TPair* aPair) = 0;
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
	virtual TPif* provided() = 0;
	virtual bool connect(TPair* aPair) = 0;
	virtual bool disconnect(TPair* aPair) = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool deattach(TPair* aPair) = 0;
	virtual bool isConnected(TPair* aPair) const = 0;
	virtual bool getId(string& aId) const = 0;
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
	virtual bool attach(TPair* aPair) override;
	virtual bool deattach(TPair* aPair) override;
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
    assert(aPair && aPair->getId(pid) && !aPair->isConnected(this) && !isConnected(aPair));
    mPairs.insert(TPairsElem(pid, aPair));
    return true;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::deattach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && aPair->isConnected(this) && isConnected(aPair));
    mPairs.erase(pid);
    return true;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::connect(TPair* aPair)
{
    bool res = aPair->attach(this);
    if (res) {
       	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::disconnect(TPair* aPair)
{
    bool res = aPair->deattach(this);
    if (res) {
       res = deattach(aPair);
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
	virtual bool attach(TPair* aPair) override;
	virtual bool deattach(TPair* aPair) override;
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
    assert(aPair && !mPair && !aPair->isConnected(this));
    mPair = aPair;
    return true;
}

template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::deattach(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this));
    mPair = nullptr;
    return true;
}

template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::connect(TPair* aPair)
{
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return true;
}

template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::disconnect(TPair* aPair)
{
    bool res = aPair->deattach(this);
    if (res) {
	res = deattach(aPair);
    }
    return true;
}


template <class TPif, class TRif>
bool NCpOi2<TPif, TRif>::isConnected(TPair* aPair) const
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
	virtual bool connect(TPair* aPair) override ;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool attach(TPair* aPair) override;
	virtual bool deattach(TPair* aPair) override;
	virtual bool isConnected(TPair* aPair) const override;
	virtual bool getId(string& aId) const override { return false;}
    protected:
	TPairs mPairs;
	TPif* mPx;
};

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::attach(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && mPairs.count(aPair) == 0);
    mPairs.insert(aPair);
    return true;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::deattach(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && mPairs.count(aPair) == 1);
    mPairs.erase(aPair);
    return true;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::connect(TPair* aPair)
{
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::disconnect(TPair* aPair)
{
    bool res = aPair->deattach(this);
    if (res) {
	res = deattach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmnp<TPif, TRif>::isConnected(TPair* aPair) const
{
    return mPairs.count(aPair) == 1;
}


/** @brief Native net tree node
 * */
template <class TPif, class TRif>
class NTn : public MNcp<TPif, TRif>
{
    protected:
	
};





/** @brief Native connection point, one-to-many, selecting pair by Id
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * */
template <class TPif, class TRif>
class NCpOmi3
{
    public:
	using TPair= typename MNcp<TPif, TRif>::TPair;
	using TPairs = map<string, TPair*>;
	using TPairsElem = pair<string, TPair*>;
    public:
	NCpOmi2() {}
	virtual bool attach(TPair* aPair) override;
	virtual bool deattach(TPair* aPair) override;
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
    assert(aPair && aPair->getId(pid) && !aPair->isConnected(this) && !isConnected(aPair));
    mPairs.insert(TPairsElem(pid, aPair));
    return true;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::deattach(TPair* aPair)
{
    string pid;
    assert(aPair && aPair->getId(pid) && aPair->isConnected(this) && isConnected(aPair));
    mPairs.erase(pid);
    return true;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::connect(TPair* aPair)
{
    bool res = aPair->attach(this);
    if (res) {
       	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool NCpOmi2<TPif, TRif>::disconnect(TPair* aPair)
{
    bool res = aPair->deattach(this);
    if (res) {
       res = deattach(aPair);
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




#endif //  __FAP3_NCONN_H
