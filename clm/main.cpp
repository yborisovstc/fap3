
#include <string>
#include <iostream>

//#include "gperftools/profiler.h"

#include "monitor.h"

/* \brief FAP2 command line monitor
 *
 */

using namespace std;

static bool mAutoRun = false;

static bool mConvert = false; //!< Flag operation of converting spec */
static bool mFormat = false; //!< Flag operation of formatting spec */

const string help = "\nFAP2 Command line monitor\n\n\
Usage:\n\
fapm [options] [file]\n\
Options:\n\
-h - show help\n\
-c - convert chromo spec\n\
-f - format chromo spec\n\
-l <file> - log file\n\
-p <file> - profiler file\n\
-m <path> - path to module files\n\
-a        - auto-run\n\
-s <file> - spec file\n\
-o <file> - converted spec file\n\
-e <name,value> - environment variable\n\
\n";


bool parseEVar(const string& aEVar, pair<string, string>& aRes)
{
    bool ret = false;
    size_t pos = aEVar.find(',');
    if (pos != string::npos) {
	aRes.first = aEVar.substr(0, pos);
	aRes.second = aEVar.substr(pos + 1);
	ret = true;
    }
    return ret;
}

int main(int argc, char* argv[])
{
    int res = 0;

    Monitor mnt;
    if (argc > 1) {
	for (int i = 1; i < argc; i++) {
	    string arg(argv[i]);
	    //cout << arg << endl;
	    if (arg == "-h") {
		cout << help;
		res = -1;
	    } else if (arg.compare("-l") == 0) {
		// Log file
		string path(argv[++i]);
		bool spres = mnt.setLogFile(path);
		if (!spres) {
		    cout << "Cannot open log file";
		}
	    } else if (arg.compare("-p") == 0) {
		// Profile file
		string path(argv[++i]);
		bool spres = mnt.setProfPath(path);
		if (!spres) {
		    cout << "Cannot open profiler file";
		}
	    } else if (arg.compare("-m") == 0) {
		// Path to modules
		string path(argv[++i]);
		bool spres = mnt.setModulePath(path);
		if (!spres) {
		    cout << "Cannot find module path";
		}
	    } else if (arg.compare("-s") == 0) {
		// Spec file
		string path(argv[++i]);
		mnt.setSpecName(path);
	    } else if (arg.compare("-o") == 0) {
		// Converted Spec file
		string path(argv[++i]);
		mnt.setOutSpecName(path);
	    } else if (arg.compare("-a") == 0) {
		// Autorun
		mAutoRun = true;
	    } else if (arg.compare("-c") == 0) {
		// Convert
		mConvert = true;
	    } else if (arg.compare("-f") == 0) {
		// Format
		mFormat = true;
	    } else if (arg.compare("-e") == 0) {
		// Environment variable
		string sevar(argv[++i]);
		pair<string, string> evar;
		bool pres = parseEVar(sevar, evar);
		if (pres) {
		    bool ores = mnt.addEVar(evar.first, evar.second);
		    if (!ores) {
			cout << "Error: env var [" << evar.first << "] already exists" << endl;
		    }
		} else {
		    cout << "Error: incorrect env var spec [" << sevar << "]" << endl;
		    res = -1;
		}
	    } else {
		cout << "Error: unknown argument [" << arg << "]" << endl;
		res = -1;
	    }
	}
	if (res == 0) {
	    if (mConvert) {
		bool sdres = mnt.convertSpec();
		if (!sdres) {
		    cout << "Error on converting chromo spec" << endl;
		}
	    } else if (mFormat) {
		bool sdres = mnt.formatSpec();
		if (!sdres) {
		    cout << "Error on formatting chromo spec" << endl;
		}
	    } else if (mAutoRun) {
		// Auto-run mode: run model and exit
		// Init env and run
		mnt.initEnv();
		mnt.runModel();
		bool sdres = mnt.saveProfilerData();
		if (!sdres) {
		    cout << "Error on saving profile data to file" << endl;
		}
	    } else {
		// Run monitor user input handling loop
		mnt.run();
	    }
	}
    } else {
	cout << help;
    }
    return res;
}
