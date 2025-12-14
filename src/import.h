#ifndef __FAP3_IMPORT_H
#define __FAP3_IMPORT_H

#include <string>
#include <map>
#include <list>

#include "node.h"

#include "ifr.h"

using namespace std;

	
/** @brief Import - note used as hidden node to keep imported modules
 * Prevents propagating modules mutations to root
 * */
class Import : public Node
{
    public:
	inline static constexpr std::string_view idStr() { return "Import"sv;}
    public:
	Import(const string &aType, const string &aName, MEnv* aEnv);
	virtual ~Import();
	// From MNode
	virtual void onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx) override;
};
	

#endif // __FAP3_IMPORT_H

