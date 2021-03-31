
#ifndef __FAP3_MDES_H
#define __FAP3_MDES_H

#include "miface.h"

using namespace std;

class MDesSyncable;

/** @brief Components Observer. Upper layer is observer of components 
 *
 * Components notifies upper layer of status changes
 * */
class MDesObserver: public MIface
{
    public:
	static const char* Type() { return "MDesObserver";};
	// From MIface
	virtual string Uid() const override { return MDesObserver_Uid();}
	virtual string MDesObserver_Uid() const = 0;
	// Local
	/** @brief Notification that component was activated */
	virtual void onActivated(MDesSyncable* aComp) = 0;
	/** @brief Notification that component was changed */
	virtual void onUpdated(MDesSyncable* aComp) = 0;
};

/** @brief Inputs Observer
 *
 * State is notified by its inputs of inputs update
 * */
class MDesInpObserver: public MIface
{
    public:
	static const char* Type() { return "MDesInpObserver";};
	// From MIface
	virtual string Uid() const override { return MDesInpObserver_Uid();}
	virtual string MDesInpObserver_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesInpObserver_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesInpObserver_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	/** @brief Notification that input state was changed */
	virtual void onInpUpdated() = 0;
};

/** @brief Interface of DES syncable
 * */
class MDesSyncable: public MIface
{
    public:
	static const char* Type() { return "MDesSyncable";};
	// From MIface
	virtual string Uid() const override { return MDesSyncable_Uid();}
	virtual string MDesSyncable_Uid() const = 0;
	virtual void doDump(int aLevel, int aIdt, ostream& aOs) const override { return MDesSyncable_doDump(aLevel, aIdt, std::cout);}
	virtual void MDesSyncable_doDump(int aLevel, int aIdt, ostream& aOs) const = 0;
	// Local
	virtual void update() = 0;
	virtual void confirm() = 0;
	/*
	virtual bool isUpdated() const = 0;
	virtual void setUpdated() = 0;
	virtual void resetUpdated() = 0;
	virtual bool isActive() const = 0;
	virtual void setActive() = 0;
	virtual void resetActive() = 0;
	*/
};


#endif
