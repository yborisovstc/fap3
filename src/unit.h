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
	inline static constexpr std::string_view idStr() { return "Unit"sv;}
    public:
	static vector<GUri> getParentsUri();
	Unit(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Unit();
	// From MNode
	virtual MIface* MNode_getLif(TIdHash aTid) override;
	virtual MIface* MOwned_getLif(TIdHash aTid);
	virtual string parentName() const { return string(idStr()); }
	vector<GUri> parentsUri() const override { return getParentsUri(); }
	// From MUnit
	virtual string MUnit_Uid() const override {  return getUid<MUnit>();}
	virtual MIface* MUnit_getLif(TIdHash aTid) override;
	virtual void MUnit_doDump(int aLevel, int aIdt, std::ostream& aOs) const override;
	virtual MIfProv* defaultIfProv(TIdHash aTid) override;
	virtual void resolveIface(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
	// From MIfProvOwner
	virtual string MIfProvOwner_Uid() const override { return getUid<MIfProvOwner>();}
	virtual MIface* MIfProvOwner_getLif(TIdHash aTid) override;
	virtual void resolveIfc(TIdHash aTid, MIfReq::TIfReqCp* aReq) override;
	virtual void onIfpDisconnected(MIfProv* aProv) override;
	virtual void onIfpInvalidated(MIfProv* aProv) override;
    protected:
	virtual IfrNode* createIfProv(TIdHash aTid, MIfReq::TIfReqCp* aReq) const;
	void invalidateIrm();
	void invalidateIrm(TIdHash aTid);
	void addIfpLeaf(MIface* aIfc, MIfReq::TIfReqCp* aReq);
	void addIfpLeafs(MIfProv::TIfaces* aIfcs, MIfReq::TIfReqCp* aReq);
	// From Node.MOwner
	virtual MIface* MOwner_getLif(TIdHash aTid) override;
	virtual void onOwnedAttached(MOwned* aOwned) override;
	bool isRequestor(MIfReq::TIfReqCp* aReq, MNode* aOwner) const;
    protected:
	map<TIdHash, IfrNode*> mLocalIrn; /*!< Local IFR node */
	list<IfrNode*> mIrns;  /*! IFR nodes */
	MUnit* mMUnitPtr = nullptr;
	MIfProvOwner* mMIfProvOwnerPtr = nullptr;
};
	

#endif // __FAP3_UNIT_H

