#ifndef __FAP3_UNIT_H
#define __FAP3_UNIT_H

#include <string>
#include <map>
#include <list>

#include "munit.h"
#include "node.h"

#include "ifr.h"

using namespace std;

	
/** @brief Native hier unit - note supporting IRM
 * */
class Unit : public Node, public MUnit, public MIfProvOwner
{
    public:
	static const char* Type() { return "Unit";}
	Unit(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Unit();
	// From MNode
	virtual MIface* MNode_getLif(const char *aType) override;
	virtual MIface* MOwned_getLif(const char *aType);
	virtual string parentName() const { return Type(); }
	// From MUnit
	virtual string MUnit_Uid() const override {  return getUid<MUnit>();}
	virtual MIface* MUnit_getLif(const char *aType) override;
	virtual void MUnit_doDump(int aLevel, int aIdt, std::ostream& aOs) const override;
	virtual MIfProv* defaultIfProv(const string& aName) override;
	virtual void resolveIface(const string& aName, MIfReq::TIfReqCp* aReq) override;
	// From MIfProvOwner
	virtual string MIfProvOwner_Uid() const override { return getUid<MIfProvOwner>();}
	virtual MIface* MIfProvOwner_getLif(const char *aType) override;
	virtual void resolveIfc(const string& aName, MIfReq::TIfReqCp* aReq) override;
	virtual void onIfpDisconnected(MIfProv* aProv) override;
	virtual void onIfpInvalidated(MIfProv* aProv) override;
    protected:
	virtual IfrNode* createIfProv(const string& aName, MIfReq::TIfReqCp* aReq) const;
	void invalidateIrm();
	void addIfpLeaf(MIface* aIfc, MIfReq::TIfReqCp* aReq);
	void addIfpLeafs(MIfProv::TIfaces* aIfcs, MIfReq::TIfReqCp* aReq);
	// From Node.MOwner
	virtual MIface* MOwner_getLif(const char *aType) override;
	virtual void onOwnedAttached(MOwned* aOwned) override;
	bool isRequestor(MIfReq::TIfReqCp* aReq, MNode* aOwner) const;
    protected:
	map<string, IfrNode*> mLocalIrn; /*!< Local IFR node */
	list<IfrNode*> mIrns;  /*! IFR nodes */
};
	

#endif // __FAP3_UNIT_H

