#ifndef __FAP3_CONT2_H
#define __FAP3_CONT2_H

#include "mcontent2.h"
#include "nconn.h"


/** @brief Content leaf
 * */
class Cont2 : public MCont2
{
    public:
	using TCp = NCpOmnp<MCont2, MContNode2>;
    public:
	Cont2() {}
	virtual ~Cont2();
	// From MCont2
	virtual string MCont2_Uid() const override { return string();}
	virtual bool getData(string& aData) const override;
	virtual bool setData(const string& aData) override;
    private:
	string mData;
	TCp mCp = TCp(this);
};


#if 0 // wrong impl. needs to be native tree node 
/** @brief Content node
 * */
class ContNode2 : public MCont2
{
    friend class Node;
    public:
	class Node : public MContNode2 {
	    public:
		using TCp = NCpOmnp<MContNode2, MCont2>;
	    public:
		Node(ContNode2* aHost): mHost(aHost) {}
		virtual ~Node();
		// From MContNode
		virtual string MContNode2_Uid() const override { return string();}
		virtual bool addCont(const string& aName, bool aLeaf = false) override;
		virtual int size() const override { return mElems.size();}
		virtual MCont2* at(int aIdx) const override;
		virtual MCont2* at(const string& aName) const override;
		virtual MCont2* getContent(const CUri& aUri) const override { return mHost->nodeGetContent(aUri);}
		virtual bool onContChanged(MCont2* aCnt) override;
		// From MContNode::MContOwner
		virtual string MContOwner2_Uid() const override { return string();}
	    private:
		ContNode2* mHost;
		map<string, MCont2*> mElems;
		TCp mCp = TCp(this);
	};
    public:
	ContNode2() {}
	Node& node() { return mNode;}
	const Node& node() const { return mNode;}
	// From MCont2
	virtual string MCont2_Uid() const override {return string();}
	virtual bool getData(string& aData) const override;
	virtual bool setData(const string& aData) override;
    protected:
	virtual MCont2* nodeGetContent(const CUri& aUri) { return nullptr;}
    private:
	Node mNode = Node(this);
};
#endif

/** @brief Content node
 * */
class ContNode2 : public NTnip<MCont2, MContNode2>
{
    friend class Owner;
    public:
    /** @brief Owner part
     * */
    class Owner : public MContNode2 {
	public:
	    Owner(ContNode2* aHost): mHost(aHost) {}
	    virtual ~Owner();
	    // From MContNode
	    virtual string MContNode2_Uid() const override { return string();}
	    virtual bool addCont(const string& aName, bool aLeaf = false) override;
	    virtual int size() const override { return mElems.size();}
	    virtual MCont2* at(int aIdx) const override;
	    virtual MCont2* at(const string& aName) const override;
	    virtual MCont2* getContent(const CUri& aUri) const override { return mHost->nodeGetContent(aUri);}
	    virtual bool onContChanged(MCont2* aCnt) override;
	    // From MContNode::MContOwner
	    virtual string MContOwner2_Uid() const override { return string();}
	private:
	    ContNode2* mHost;
	    map<string, MCont2*> mElems;
    };
    /** @brief Content leaf
     * */
    class Owned : public MCont2 {
	public:
	    Owned() {}
	    virtual ~Owned();
	    // From MCont2
	    virtual string MCont2_Uid() const override { return string();}
	    virtual bool getData(string& aData) const override { aData = mData; return true;}
	    virtual bool setData(const string& aData) override { mData = aData; return true;}
	private:
	    string mData;
    };

    public:
    ContNode2(): NTnip<MCont2, MContNode2>(&mOwned, &mOwner), mOwner(this), mOwned() {}
    protected:
    virtual MCont2* nodeGetContent(const CUri& aUri) { return nullptr;}
    private:
    Owner mOwner;
    Owned mOwned;
};



#endif
