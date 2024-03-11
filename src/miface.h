#ifndef __FAP3_MIFACE_H
#define __FAP3_MIFACE_H

#include <string>
#include <iostream>

using namespace std;

/** @brief Interface of interface
 * Any interface has to include type static attr const char* mType
 * */
class MIface
{
    public:
	/** @brief Remote call of iface method: aRes - data of result, returns context or NULL otherwise */
	virtual MIface* Call(const string& aSpec, string& aRes) { return nullptr;}
	/** @brief Getting id unique in the scope of the env */
	virtual string Uid() const = 0;
	/** @brief Gets local interface of type aType */
	virtual MIface* getLif(const char *aType) { return nullptr;}
	template <class T> T* lIf(T* aInst) {return aInst = reinterpret_cast<T*>(getLif(aInst->Type()));}
	template <class T> const T* lIf(T* aInst) const { MIface* self = const_cast<MIface*>(this); return aInst = reinterpret_cast<T*>(self->getLif(aInst->Type()));}
	/** @brief outputs dump
	 * @param aInt  indentation level
	 * */
	inline void dump(int aLevel, int aIdt = 0) const { doDump(aLevel, aIdt, std::cout);}
	virtual void doDump(int aLevel, int aIdt, std::ostream& aOs) const {}
};

/** @brief Connection (2-ways relation) interface
 * @tparam  TPair  required iface (pair)
 * */
class MCIface : public MIface
{
    public:
	/** @brief Gets provided iface type */
	//virtual string provided() const = 0;
	/** @brief Indicates the pair is comatible */
	virtual bool isCompatible(MCIface* aPair) const = 0;
	/** @brief Connects to pair, one-way, returns success indicator */
	virtual bool connect(MCIface* aPair) = 0;
	/** @brief Disconnects to pair, one-way, returns success indicator */
	virtual bool disconnect(MCIface* aPair) = 0;
	/** @brief Connects to pair, two-way, returns success indicator */
	static bool connect(MCIface* aSelf, MCIface* aPair) { return aPair->connect(aSelf) ? aSelf->connect(aPair) : false;}
	/** @brief Disonnects pairs, two-way, returns success indicator */
	static bool disconnect(MCIface* aSelf, MCIface* aPair) { return aPair->disconnect(aSelf) ? aSelf->disconnect(aPair) : false;}
	/** @brief Gets ID, returns success indicator */
	virtual bool getId(string& aId) const = 0;
	/** @brief Indicates of connected to pair */
	virtual bool isConnected(MCIface* aPair) const = 0;
};

/** @brief Connection (2-ways relation) interface
 * @tparam  TProv  provided iface (self)
 * @tparam  TReq   required iface (pair)
 * @note Experimental
 * */
// TODO The design is a bit weird: it allows one way connection that was not intended
// ref nconn for better design
template <typename TProv, typename TReq>
class MCtIface : public MIface
{
    public:
	using TPair = MCtIface<TReq, TProv>;
    public:
	/** @brief Gets provided iface type */
	virtual string provided() const = 0;
	/** @brief Indicates the pair is comatible */
	virtual bool isCompatible(TPair* aPair) const = 0;
	/** @brief Connects to pair, one-way, returns success indicator */
	virtual bool connect(TPair* aPair) = 0;
	/** @brief Disconnects to pair, one-way, returns success indicator */
	virtual bool disconnect(TPair* aPair) = 0;
	/** @brief Connects to pair, two-way, returns success indicator */
	bool connect(MCIface* aSelf, MCIface* aPair) { return aPair->connect(aSelf) ? aSelf->connect(aPair) : false;}
	/** @brief Gets ID, returns success indicator */
	virtual bool getId(string& aId) const = 0;
	/** @brief Indicates of connected to pair */
	virtual bool isConnected(MCIface* aPair) const = 0;
};

#endif // __FAP3_MIFACE_H
