
#ifndef __FAP3_MPROF_H
#define __FAP3_MPROF_H

#include <string>

/** @brief Profiler interface
 * */


/** @brief Profilers event
 * */
// TODO YB This is more perf indicator variant than event, Pind is more indicators collector than
// indicator itself. To consider redesign
class PEvent
{
public:
    using TId = int; //!< Event identificator type
public:
    static const TId KUndefEid = -1; //!< Undefined event id
    PEvent(TId aId = KUndefEid, const std::string& aDescr = std::string()): mId(aId), mDescr(aDescr) {}
public:
    TId mId = KUndefEid; //!< Event Id
    std::string mDescr; //!< Event description
};

/** @brief Profilers record, base
 * */
class PRec
{
    public:
	PRec(): mEventId(PEvent::KUndefEid) {}
	PRec(PEvent::TId aId): mEventId(aId) {}
	void setEventId(PEvent::TId aId) { mEventId = aId;}
    public:
	PEvent::TId mEventId; //!< Event Id
    public:
	static const char KFieldSep = '\t';
};

/** @brief Performance indicator interface.
 * Inherited class should contain static int constant KTypeId of type TTid with unique value
 */
class MPind
{
public:
    using TClock = struct timespec; //!< Type of clock value
    using TTid = int;               //!< Id of indicator type */
public:
    /** @brief Return type ID */
    virtual TTid tid() const = 0;
    virtual std::string getDescription() = 0;
    virtual int getBufLen() const = 0;
    virtual int getBufLenLimit() const = 0;
    /** Returns file suffix */
    virtual std::string getFileSuf() const = 0;
    /** Obtains clock resolution in nanoseconds */
    virtual bool getClockResolution(TClock& aRes) const =  0;
    virtual std::string recToString(int aRecNum) const = 0;
    virtual std::string toString() const = 0;
    virtual bool saveToFile(const std::string& aPath) = 0;
    virtual void dump() const = 0;
    virtual int getId() const = 0;
};

using TPItemId = int; //!< ID type

/** @brief Profiler interface
 * */
class MProfiler
{
    public:
	/** @brief Logs entry to code block */
	virtual void enable() = 0;
	/** @brief Saves the metrics collected */
	virtual void saveMetrics() = 0;
	/** @brief Gets Pinds count */
	virtual int pindsCount() const = 0;
	/** @brief Gets pointer to indicator by Id */
	virtual MPind* getPind(int aId) = 0;
	template<typename T> T* getPind(T* aArg) { return reinterpret_cast<T*>(getPind(aArg->KTypeId));}
	template<typename T> T& getPind() { return reinterpret_cast<T&>(*getPind(T::KTypeId));}
    public:
	// Profiler helpers
	virtual void DurStatStart(TPItemId aId) = 0;
	virtual void DurStatRec(TPItemId aId) = 0;
};


#endif
