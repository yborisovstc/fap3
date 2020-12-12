#ifndef __FAP3_MOWNING_H
#define __FAP3_MOWNING_H

#include "miface.h"


/** @brief Native net owner interface
 * */
class MOwner : public MIface
{
    public:
	constexpr static const char* mType = "MOwner";
	// From MIface
	virtual string Uid() const override { return MOwner_Uid();}
	virtual string MOwner_Uid() const = 0;
};


/** @brief Native net owned interface
 * */
class MOwned : public MIface
{
    public:
	constexpr static const char* mType = "MOwned";
	// From MIface
	virtual string Uid() const override { return MOwned_Uid();}
	virtual string MOwned_Uid() const = 0;
	// Local
	virtual string ownedId() const = 0;
	virtual void deleteOwned() = 0;
};



#endif //  __FAP3_MOWNING_H
