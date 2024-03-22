#ifndef __FAP3_NCONN_H
#define __FAP3_NCONN_H

#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <forward_list>
#include <iostream>

#include <assert.h>

using namespace std;

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
    public:
	/** @brief Iterator interface, ref ds_nn_tp_uit for design
	 * */
	class MIter : std::iterator<input_iterator_tag, TPair*> {
	    public:
		virtual ~MIter() {}
		virtual MIter* clone() { return nullptr;}
		virtual MIter& operator++() { return *this;}
		//virtual MIter operator++(int) { return MIter();}
		virtual bool operator==(const MIter& aIt) const { return false;}
		virtual bool operator!=(const MIter& aIt) const { return false;}
		virtual TPair* operator*() { return nullptr;}
	};
	/** @brief Pairs Iterator, ref ds_nn_tp_uit for design
	 * */
	class PairsIter :  std::iterator<input_iterator_tag, TPair*> {
	    public:
		PairsIter(const PairsIter& aItr) {
		    mItr = aItr.mItr->clone();
		}
		PairsIter(MIter* aItr): mItr(aItr) {}
		virtual ~PairsIter() {
		    delete mItr;
		    mItr = nullptr;
		}
		PairsIter& operator=(const PairsIter& aItr) {
		    delete mItr;
		    mItr = aItr.mItr->clone();
		    return *this;
		}
		PairsIter& operator++() { ++(*mItr); return *this; }
		PairsIter operator++(int) { PairsIter tmp(*this); operator++(); return tmp; }
		bool operator==(const PairsIter& aIt) const {
		    return (*mItr == *(aIt.mItr));
		}
		bool operator!=(const PairsIter& aIt) const {
		    return (*mItr != *(aIt.mItr));
		}
		TPair* operator*() { return *(*mItr);}
	    protected:
		MIter* mItr = nullptr;
	};
	class PairsConstIter :  std::iterator<input_iterator_tag, const TPair*> {
	    public:
		PairsConstIter(const PairsConstIter& aItr) {
		    mItr = aItr.mItr->clone();
		}
		PairsConstIter(MIter* aItr): mItr(aItr) {}
		virtual ~PairsConstIter() {
		    delete mItr;
		    mItr = nullptr;
		}
		PairsConstIter& operator=(const PairsConstIter& aItr) {
		    delete mItr;
		    mItr = aItr.mItr->clone();
		    return *this;
		}
		PairsConstIter& operator++() { ++(*mItr); return *this; }
		PairsConstIter operator++(int) { PairsConstIter tmp(*this); operator++(); return tmp; }
		bool operator==(const PairsConstIter& aIt) {
		    return (*mItr == *(aIt.mItr));
		}
		bool operator!=(const PairsConstIter& aIt) {
		    return (*mItr != *(aIt.mItr));
		}
		const TPair* operator*() { return const_cast<const TPair*>(*(*mItr));}
	    protected:
		MIter* mItr = nullptr;
	};
	/** @brief Leafs Iterator
	 * This variant considers any nodes w/o binded or zero pairs binded as the leaf
	 * It seems less effective that "true" leafs (w/o binded) 
	 * */
	struct LeafsIterator {
	    LeafsIterator(const PairsIter& aRootEnd): mRootEnd(aRootEnd) {}
	    LeafsIterator(const PairsIter& aPi, const PairsIter &aRootEnd): LeafsIterator(aRootEnd) { mPi.push_front(aPi); }
	    TPair* operator*() {
		return *(mPi.front());
	    }
	    LeafsIterator operator++(int) {
		auto res = *this;
		bool stop = false;
		bool end = false;
		do {
		    // Find nearest uncompleted node
		    auto& front = mPi.front();
		    auto plit = mPi.begin();
		    plit++;
		    if (plit != mPi.end()) {
			PairsIter plpit = *plit;
			TSelf* pl = (*plpit)->binded();
			front++;
			if (front == pl->pairsEnd()) {
			    // Current node is over, go to next one, and set to it's leaf begin
			    mPi.pop_front();
			} else {
			    // Uncompleted node found
			    stop = true;
			}
		    } else {
			// Root node reached
			front++;
			stop = true;
			if (front == mRootEnd) {
			    end = true;
			}
		    }
		} while (!stop);

		if (!end) {
		    auto& front = mPi.front();
		    TSelf* pbnd = (*front)->binded();
		    if (pbnd) {
			auto pli = pbnd->leafsBegin();
			if (pli != pbnd->leafsEnd()) {
			    mPi.splice_after(mPi.before_begin(), pli.mPi);
			}
		    }
		}
		/*
		if (!end) {
		    auto& front = mPi.front();
		    auto* cnode = *front;
		    assert(cnode);
			auto pli = cnode->leafsBegin();
			if (pli != cnode->leafsEnd()) {
			    mPi.splice_after(mPi.before_begin(), pli.mPi);
			}
		}
		*/
		return res;
	    }
	    bool operator==(const LeafsIterator& aB) {
		return  (mPi == aB.mPi);
	    }
	    bool operator!=(const LeafsIterator& aB) {
		return  (mPi != aB.mPi);
	    }
	    void __attribute__ ((noinline)) dump() {
		for (auto it = mPi.begin(); it != mPi.end(); it++) {
		    TPair* ptr = **it;
		    std::cout << std::hex << ptr << std::endl;
		}
	    }
	    std::forward_list<PairsIter> mPi;
	    PairsIter mRootEnd;
	};
#if 0
	/** @brief Leafs Iterator
	 * This variant considers only nodes w/o binded as leafs
	 * */
	struct LeafsIterator {
	    LeafsIterator(const PairsIter& aRootEnd): mRootEnd(aRootEnd) {}
	    LeafsIterator(const PairsIter& aPi, const PairsIter &aRootEnd): LeafsIterator(aRootEnd) { mPi.push_front(aPi); }
	    TPair* operator*() {
		return *(mPi.front());
	    }
	    LeafsIterator operator++(int) {
		auto res = *this;
		bool stop = false;
		bool end = false;
		do {
		    // Find nearest uncompleted node
		    auto& front = mPi.front();
		    auto plit = mPi.begin();
		    plit++;
		    if (plit != mPi.end()) {
			PairsIter plpit = *plit;
			TSelf* pl = (*plpit)->binded();
			front++;
			if (front == pl->pairsEnd()) {
			    // Current node is over, go to next one, and set to it's leaf begin
			    mPi.pop_front();
			} else {
			    // Uncompleted node found
			    stop = true;
			}
		    } else {
			// Root node reached
			front++;
			stop = true;
			if (front == mRootEnd) {
			    end = true;
			}
		    }
		} while (!stop);

		if (!end) {
		    auto& front = mPi.front();
		    TSelf* pbnd = (*front)->binded();
		    if (pbnd) {
			auto pli = pbnd->leafsBegin();
			if (pli != pbnd->leafsEnd()) {
			    mPi.splice_after(mPi.before_begin(), pli.mPi);
			}
		    }
		}

		return res;
	    }
	    bool operator==(const LeafsIterator& aB) {
		return  (mPi == aB.mPi);
	    }
	    bool operator!=(const LeafsIterator& aB) {
		return  (mPi != aB.mPi);
	    }
	    void __attribute__ ((noinline)) dump() {
		for (auto it = mPi.begin(); it != mPi.end(); it++) {
		    TPair* ptr = **it;
		    std::cout << std::hex << ptr << std::endl;
		}
	    }
	    std::forward_list<PairsIter> mPi;
	    PairsIter mRootEnd;
	};
#endif

    public:
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
	virtual TPair* pairAt(int aInd) = 0;
	virtual const TPair* pairAt(int aInd) const = 0;
	virtual TPair* pairAt(const string aId) = 0;
	virtual const TPair* pairAt(const string aId) const = 0;
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
	// Traversal
	virtual PairsIter pairsBegin() = 0;
	virtual PairsIter pairsEnd() = 0;
	//virtual LeafsIterator leafsBegin() { return LeafsIterator(pairsBegin(), pairsEnd());}
	virtual LeafsIterator leafsBegin() {
	    std::cout << "leafsBegin" << std::endl;
	    auto pit = pairsBegin();
	    LeafsIterator it(pit, pairsEnd());
	    if (pit != pairsEnd()) {
		TSelf* pbnd = (*pit)->binded();
		if (pbnd) {
		    // Node, proceed to leafs
		    auto pli = pbnd->leafsBegin();
		    if (pli != pbnd->leafsEnd()) {
			it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		    }
		}
	    }
	    return it;
	}
	virtual LeafsIterator leafsEnd() { return LeafsIterator(pairsEnd(), pairsEnd());}
	//PairsConstIterator pairsCbegin() const { return mPairs.cbegin(); }
	//PairsConstIterator pairsCend() const { return mPairs.cend(); }
};


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
	using TPairsIterator = typename TPairs::iterator;
	using TPairsConstIterator = typename TPairs::const_iterator;
    public:
	class PIter : public TSelf::MIter {
	    public:
		PIter(const TPairsIterator& aItr): mItr(aItr) {}
		PIter(const PIter& aItr): mItr(aItr.mItr) {}
		typename TSelf::MIter* clone() override { return new PIter(*this);}
		PIter& operator=(const PIter& aIt) { this->mItr = aIt.mItr; return *this; }
		PIter& operator++() override { mItr++; return *this; }
		bool operator==(const typename TSelf::MIter& aIt) const override {
		    auto ait = reinterpret_cast<const TThis::PIter&>(aIt);
		    return (mItr == ait.mItr);
		}
		bool operator!=(const typename TSelf::MIter& aIt) const override {
		    auto ait = reinterpret_cast<const TThis::PIter&>(aIt);
		    return !(mItr == ait.mItr);
		}
		TPair* operator*() override { return mItr->second;}
	    protected:
		TPairsIterator mItr;

	};
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
	/*
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
	*/
	virtual bool disconnectAll() override {
	    bool res = true;
	    auto pi = mPairs.begin();
	    while (pi != mPairs.end()) {
		res = disconnect(pi->second);
		assert(res);
		pi++;
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
	// Traversal
	virtual typename TSelf::PairsIter pairsBegin() {
	    return typename TSelf::PairsIter(new PIter(mPairs.begin()));
	}
	virtual typename TSelf::PairsIter pairsEnd()  {
	    return typename TSelf::PairsIter(new PIter(mPairs.end()));
	}
    protected:
	// Local. Traversal.
	/*
	TPairsIterator pairsBegin() { return mPairs.begin(); }
	TPairsIterator pairsEnd() { return mPairs.end(); }
	TPairsConstIterator pairsCbegin() const { return mPairs.cbegin(); }
	TPairsConstIterator pairsCend() const { return mPairs.cend(); }
	*/
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
	using TThis = NCpOmnp<TPif, TRif>;
	using TSelf = typename MNcpp<TPif, TRif>::TSelf;
	using TPair = typename MNcpp<TPif, TRif>::TPair;
	using TPairs = set<TPair*>;
	using TPairsIterator = typename TPairs::iterator;
	using TPairsConstIterator = typename TPairs::const_iterator;
    public:
	class PIter : public TSelf::MIter {
	    public:
		PIter(const TPairsIterator& aItr): mItr(aItr) {}
		PIter(const PIter& aItr): mItr(aItr.mItr) {}
		typename TSelf::MIter* clone() override { return new PIter(*this);}
		PIter& operator=(const PIter& aIt) { this->mItr = aIt.mItr; return *this; }
		PIter& operator++() override { mItr++; return *this; }
		//PIter operator++(int) { PIter tmp(*this); operator++(); return tmp; }
		bool operator==(const typename TSelf::MIter& aIt) const override {
		    auto ait = reinterpret_cast<const TThis::PIter&>(aIt);
		    return (mItr == ait.mItr);
		}
		bool operator!=(const typename TSelf::MIter& aIt) const override {
		    auto ait = reinterpret_cast<const TThis::PIter&>(aIt);
		    return (mItr != ait.mItr);
		}
		TPair* operator*() override { return *mItr;}
	    protected:
		TPairsIterator mItr;
	};
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
	    if (!res && binded()) {
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
	// Traversal
	virtual typename TSelf::PairsIter pairsBegin() {
	    //return typename TSelf::PairsIter(new PIter(mPairs.begin()));
	    auto pb = mPairs.begin();
	    auto* pi = new PIter(pb);
	    return typename TSelf::PairsIter(pi);
	}
	virtual typename TSelf::PairsIter pairsEnd()  { return typename TSelf::PairsIter(new PIter(mPairs.end()));}
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
	using TThis= NCpOnp<TPif, TRif>;
	using TPair= typename MNcpp<TPif, TRif>::TPair;
	using TPairsIterator = TPair*;
    public:
	class PIter : public TSelf::MIter {
	    public:
		PIter(const TPairsIterator& aItr): mItr(aItr) {}
		PIter& operator=(const PIter& aIt) { this->mItr = aIt.mItr; return *this; }
		PIter& operator++() override { mItr++; return *this; }
		//PIter operator++(int) override { PIter tmp(*this); operator++(); return tmp; }
		bool operator==(const typename TSelf::MIter& aIt) const override { auto ait = reinterpret_cast<const TThis::PIter&>(aIt); return (mItr == ait.mItr); }
		bool operator!=(const typename TSelf::MIter& aIt) const override { auto ait = reinterpret_cast<const TThis::PIter&>(aIt); return !(mItr == ait.mItr); }
		TPair* operator*() override { return mItr;}
	    protected:
		TPair* mItr;
	};
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
	// Traversal
	virtual typename TSelf::PairsIter pairsBegin() {
	    auto it = typename TSelf::PairsIter(new PIter(mPair));
	    return it;
	}
	virtual typename TSelf::PairsIter pairsEnd()  { return typename TSelf::PairsIter(new PIter(nullptr));}
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

#if 0

/** @brief Native connection point, one-to-many, no Id, proxy
 * ver 2 - parametrized pair
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * @tparam TPx type of provided iface proxy
 * */
template <class TPif, class TRif, template <class Pa, class Pb> class PPair>
class NCpOmnp2 : public MNcpp<TPif, TRif>
{
    public:
	using TThis = typename NCpOmnp2<TPif, TRif, PPair>::TSelf;
	using TSelf = typename MNcpp<TPif, TRif>::TSelf;
	using TPair = PPair<TRif, TPif>;
	using TPairs = set<TPair*>;
    public:
	NCpOmnp2(TPif* aPx): mPx(aPx) {}
	virtual ~NCpOmnp2() { disconnectAll(); }
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
	    if (!res && binded()) {
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
    protected:
	TPairs mPairs;
	TPif* mPx;
};

template <class TPif, class TRif, template <class Pa, class Pb> class PPair>
bool NCpOmnp2<TPif, TRif, PPair>::attach(TPair* aPair)
{
    assert(aPair && !isConnected(aPair));
    mPairs.insert(aPair);
    return true;
}

template <class TPif, class TRif, template <class Pa, class Pb> class PPair>
bool NCpOmnp2<TPif, TRif, PPair>::detach(TPair* aPair)
{
    assert(aPair && isConnected(aPair));
    mPairs.erase(aPair);
    return true;
}

template <class TPif, class TRif, template <class Pa, class Pb> class PPair>
bool NCpOmnp2<TPif, TRif, PPair>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isConnected(this) && !isConnected(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif, template <class Pa, class Pb> class PPair>
bool NCpOmnp2<TPif, TRif, PPair>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isConnected(this) && isConnected(aPair));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    assert(res);
    return res;
}

template <class TPif, class TRif, template <class Pa, class Pb> class PPair>
bool NCpOmnp2<TPif, TRif, PPair>::isConnected(TPair* aPair) const
{
    return mPairs.count(aPair) == 1;
}
#endif

#endif //  __FAP3_NCONN_H
