#ifndef __FAPM_MONITOR__
#define __FAPM_MONITOR__

#include <string>
#include <map>

#include <env.h>

using namespace std;

class InputHandler;

/**
 * \brief System movement monitor
 */
class Monitor
{
    public:
	/** Input handler factory function */
	using TIhFact = InputHandler* (Monitor& aHost);
	/** Input handlers registry type */
	using TIhReg = map<string, TIhFact*>;
	/** @brief Environment variables type: name, value */
	using TEVarElem = pair<string, string>;
	using TEVars = map<string, string>;
    public:
	Monitor();
	~Monitor();
	void setSpecName(const string& aFileName);
	/** @brief Sets converted spec file name */
	void setOutSpecName(const string& aFileName);
	bool setLogFile(const string& aPath);
	bool setProfPath(const string& aPath);
	void initEnv(bool aVerbose = true);
	void runModel();
	bool saveModel(const string& aPath);
	bool saveProfilerData();
	/** @brief Runs user interaction loop */
	bool run();
	/** @brief Converts chromo spec
	 * @return Sign of sucess
	 * */
	bool convertSpec();
	/** @brief Formats chromo spec
	 * @return Sign of sucess
	 * */
	bool formatSpec();
	/** @brief Sets module path
	 * @return Sign of sucess
	 * */
	bool setModulePath(const string& aPath);
	/** @brief Adds environment variable
	 * @return Sign of success
	 * */
	bool addEVar(const string& aName, const string& aValue);
    protected:
	/** @brief Creates input handler for given command */
	InputHandler* createHandler(const string& aCmd);
	/** @brief Copies file */
	static bool copyFile(const string& aSrcFname, const string& aDstFname);
    protected:
	string mSpecName;  //!< Input spec file name
	string mCSpecName; //!< Convrted spec file name
	string mLogName;
	/** Profiler file path */
	string mProfName;
	vector<string> mModPaths; /*!< Module paths */	
	Env* mEnv;
	string mPrompt;
	static const string KDefPrompt;
	/** Input handlers factory function registry */
	static const TIhReg mIhReg;
	TEVars mEVars; /*!< Env variables */
};

/** Input handler base */
class InputHandler
{
    public:
	/** Each handler has to include two static method 
	 * 1. 'command' returning specific command's string
	 * 2. "create" returning newly created instanse of handler
	 * static const string command();
	 * static InputHandler* create(Monitor& aHost);
	 */
	InputHandler(Monitor& aHost): mHost(aHost) {}
	virtual ~InputHandler() {}
	/** Handles command with given arguments aArgs */
	virtual bool handle(const string& aArgs) = 0;
    protected:
	Monitor& mHost;
};

#endif // __FAPM_MONITOR__
