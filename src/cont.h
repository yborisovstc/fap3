#ifndef __FAP3_CONT_H
#define __FAP3_CONt_H

#include <string>
#include <map>

#include "mcontent.h"

using namespace std;

/** @brief Content leaf
 * */
class Cont : public MCont
{
    public:
	Cont() {}
	// From MCont
	virtual string MCont_Uid() const override { return string();}
	virtual bool getContent(string& aCont) const override;
	virtual bool setContent(const string& aCont) override;
	// From MCont::MCIface
	virtual string provided() const override { return MCont::Type();}
	virtual bool isCompatible(MCIface* aPair) const override;
	virtual bool connect(MCIface* aPair) override;
	virtual bool disconnect(MCIface* aPair) override;
	virtual bool getId(string& aId) const override;
	virtual bool isConnected(MCIface* aPair) const override;
    private:
	string mData;
};


/** @brief Content node
 * */
class ContNode : public MCont
{
    friend class Node;
    public:
	class Node : public MContNode {
	    public:
		Node(ContNode* aHost): mHost(aHost) {}
		// From MContNode
		virtual string MContNode_Uid() const override { return string();}
		virtual bool addCont(const string& aName, bool aLeaf = false) override;
		virtual int size() const override { return mElems.size();}
		virtual MCont* at(int aIdx) const override;
		virtual MCont* at(const string& aName) const override;
		virtual MCont* getContent(const CUri& aUri) const override { return mHost->nodeGetContent(aUri);}
		// From MContNode::MContOwner
		virtual string MContOwner_Uid() const override { return string();}
		virtual bool onContChanged(MCont* aCnt) override;
		//  Fron MContNode::MContOwner::MCIface
		virtual string provided() const override;
		virtual bool isCompatible(MCIface* aPair) const override;
		virtual bool connect(MCIface* aPair) override;
		virtual bool disconnect(MCIface* aPair) override;
		virtual bool getId(string& aId) const override;
		virtual bool isConnected(MCIface* aPair) const override;
	    private:
		ContNode* mHost;
		map<string, MCont*> mElems;
	};
    public:
	ContNode() {}
	// From MCont
	virtual string MCont_Uid() const override {return string();}
	virtual bool getContent(string& aCont) const override;
	virtual bool setContent(const string& aCont) override;
	//  Fron MCont::MCIface
	virtual string provided() const override;
	virtual bool isCompatible(MCIface* aPair) const override;
	virtual bool connect(MCIface* aPair) override;
	virtual bool disconnect(MCIface* aPair) override;
	virtual bool getId(string& aId) const override;
	virtual bool isConnected(MCIface* aPair) const override;
    protected:
	virtual MCont* nodeGetContent(const CUri& aUri);
    private:
	Node mNode = Node(this);
};

#endif
