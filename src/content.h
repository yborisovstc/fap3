
#ifndef __FAP3_CONTENT_H
#define __FAP3_CONTENT_H

#include "mecont.h"
#include "node.h"


/** @brief Content node
 * */
class Content : public Node, public MContent
{
    public:
	static const char* Type() { return "Content";}
	Content(const string &aName, MEnv* aEnv);
	virtual ~Content();
	// From Node.MNode.MIface
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MCont.MIface
	virtual string MContent_Uid() const override { return getUid<MContent>();}
	virtual MIface* MContent_getLif(const char *aType) override;
	virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	// From MContent
	virtual string contName() const override { return name();}
	virtual bool getData(string& aData) const override;
	virtual bool setData(const string& aData) override;
	// From Node
	virtual void mutContent(const ChromoNode& aMut, bool aUpdOnly, const MutCtx& aCtx) override;
	// From Node.MContentOwner
	virtual void onContentChanged(const MContent* aCont) override;
    protected:
	void notifyContentChanged(const MContent* aCont);
    protected:
	string mData;
	bool mValid = false;
};


/** @brief Custom content */
class Cnt : public MContent {
    public:
	class Host
	{
	    public:
		virtual string getCntUid(const string& aName, const string& aIfName) const = 0;
		virtual MContentOwner* cntOwner() = 0;
	};
    public:
	Cnt(Host& aHost, const string& aName): mHost(aHost), mName(aName) {}
	virtual ~Cnt() {}
	// From MContent
	virtual string MContent_Uid() const override { return mHost.getCntUid(mName, MContent::Type());}
	virtual MIface* MContent_getLif(const char *aType) override { return nullptr;}
	virtual void MContent_doDump(int aLevel, int aIdt, ostream& aOs) const override;
	virtual string contName() const override { return mName;}
	virtual bool getData(string& aData) const override { aData = mData; return true;}
	virtual bool setData(const string& aData) override;
    private:
	const string& mName;
	string mData;
	Host& mHost;
};


#endif
