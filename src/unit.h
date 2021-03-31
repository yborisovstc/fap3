#ifndef __FAP3_UNIT_H
#define __FAP3_UNIT_H

#include <string>
#include <map>
#include <list>

#include "nconn.h"
#include "munit.h"
#include "mcontent.h"
#include "node.h"

#include "cont.h"
#include "cont2.h"
#include "ifr.h"
#include "ifu.h"

using namespace std;

	
class Unit : public Node, public MUnit, public MIfProvOwner
{
    public:
	static const char* Type() { return "Unit";}
	Unit(const string &aName, MEnv* aEnv);
	virtual ~Unit();
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	// From MUnit
	virtual string MUnit_Uid() const override {  return getUid<MUnit>();}
	virtual MIface* MUnit_getLif(const char *aType) override;
	virtual MIfProv* defaultIfProv(const string& aName) override;
	virtual bool resolveIface(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MIfProvOwner
	virtual string MIfProvOwner_Uid() const override { return getUid<MIfProvOwner>();}
	virtual MIface* MIfProvOwner_getLif(const char *aType) override;
	virtual bool resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	virtual void onIfpDisconnected(MIfProv* aProv) override;
    protected:
	virtual IfrNode* createIfProv(const string& aName, MIfReq::TIfReqCp* aReq) const;
	void invalidateIrm();
	void addIfpLeaf(MIface* aIfc, MIfReq::TIfReqCp* aReq);
	template<class T> void getIfs(MNode* aObj, vector<T*>& aRes);
    protected:
	map<string, IfrNode*> mLocalIrn; /*!< Local IFR node */
	list<IfrNode*> mIrns;  /*! IFR nodes */
};

template<class T> void Unit::getIfs(MNode* aObj, vector<T*>& aRes)
{
    MUnit* obju = aObj->lIf(obju);
    MIfProv* ifp = obju ? obju->defaultIfProv(T::Type()) : nullptr;
    MIfProv* maprov = ifp ? ifp->first() : nullptr;
    while (maprov) {
	T* res = dynamic_cast<T*>(maprov->iface());
	aRes.push_back(res);
	maprov = maprov->next();
    }
}

#endif // __FAP3_UNIT_H

