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
	virtual MContNode2* node() override { return nullptr;}
	virtual const MContNode2* node() const override { return nullptr;}
	virtual void MCont2_dump(int aIdt) const;
    private:
	string mData;
	TCp mCp = TCp(this);
};


/** @brief Content node
 * */
class ContNode2 : public NTnip<MCont2, MContOwner2>, public MCont2, public MContNode2, public MContOwner2 
{
    public:
	ContNode2(const string& aId): NTnip<MCont2, MContOwner2>(this, this), mId(aId) {}
	virtual ~ContNode2();
	// From MCont2
	virtual string MCont2_Uid() const override { return string();}
	virtual bool getData(string& aData) const override;
	virtual bool setData(const string& aData) override;
	virtual MContNode2* node() override { return this;}
	virtual const MContNode2* node() const override { return this;}
	virtual void MCont2_dump(int aIdt) const override;
	// From MContNode
	virtual string MContNode2_Uid() const override { return string();}
	virtual bool addCont(const CUri& aUri, bool aLeaf = false) override;
	virtual int size() const override { return mOwnerCp.count();}
	virtual MCont2* at(int aIdx) const override;
	virtual MCont2* at(const string& aName) const override;
	virtual MCont2* getContent(const CUri& aUri) const override;
	// From MContOwner2
	virtual string MContOwner2_Uid() const override { return MContOwner2::Type();}
	virtual bool onContChanged(MCont2* aCnt) override;
	// From NTnip
	virtual bool getId(string& aId) const override { aId = mId; return true;}
    private:
	bool addCont(int aIdx, const CUri& aUri, bool aLeaf = false);
    private:
	string mId;
};

/** @brief Content leaf
 * */
class ContLeaf2 : public NCpOip<MCont2, MContOwner2>, public MCont2
{
    public:
	ContLeaf2(const string& aId): NCpOip<MCont2, MContOwner2>(this), mId(aId) {}
	virtual ~ContLeaf2();
	// From MCont2
	virtual string MCont2_Uid() const override { return string();}
	virtual bool getData(string& aData) const override { aData = mData; return true;}
	virtual bool setData(const string& aData) override { mData = aData; return true;}
	virtual MContNode2* node() override { return nullptr;}
	virtual const MContNode2* node() const override { return nullptr;}
	virtual bool getId(string& aId) const override { aId = mId; return true;}
	virtual void MCont2_dump(int aIdt) const override;
    private:
	string mId;
	string mData;
};

/** @brief Content utilities
 * */
class Contu
{
    public:
	static bool isComplexContent(const string& aVal);
};




#endif
