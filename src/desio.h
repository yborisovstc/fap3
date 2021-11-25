
#ifndef __FAP3_DESIO_H
#define __FAP3_DESIO_H

#include "des.h"

/** @brief Output
 * */
class Sout : public State
{
    public:
	static const char* Type() { return "Sout";};
	Sout(const string &aType, const string& aName = string(), MEnv* aEnv = NULL);
	// From State.MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
};


#endif
