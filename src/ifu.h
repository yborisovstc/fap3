#ifndef __FAP3_IFU_H
#define __FAP3_IFU_H

#include <vector>
#include <string>
#include <sstream>
#include <map>


using namespace std;

/** @brief Interface support utility
 * */
class Ifu
{
    public:
	Ifu();
	Ifu(const string& aPars);
	static string CombineIcSpec(const string& aName, const string& aSig);
	static string CombineIcSpec(const string& aName, const string& aSig, const string& aArg);
	static void AddIcSpecArg(string& aSpec, const string& aArg);
	static void AddIcSpecArg(string& aSpec, bool aArg);
	// Parsing of Iface call invocation spec
	static void ParseIcSpec(const string& aSpec, string& aName, string& aSig, vector<string>& aArgs);
	template<typename TArg> static string Pack(TArg aArg);
	template<typename TArg> static TArg Unpack(const string& aString, TArg& aArg);
	//template<typename ... TArgs> static string PackMethod(const string& aName, TArgs... aArgs);
	inline static string PackMethod(const string& aName);
	template<typename TArg1> static string PackMethod(const string& aName, TArg1 aAgr1);
	template<typename TArg1, typename TArg2> static string PackMethod(const string& aName, TArg1 aAgr1, TArg2 aArg2);
	template<typename TArg1, typename TArg2, typename TArg3> static string PackMethod(const string& aName, TArg1 aAgr1, TArg2 aArg2, TArg3 aArg3);
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	    static string PackMethod(const string& aName, TArg1 aAgr1, TArg2 aArg2, TArg3 aArg3, TArg4 aArg4);
	static bool ToBool(const string& aString);
	static string FromBool(bool aBool);
	static int ToInt(const string& aString);
	static string FromInt(int aInt);
	static void ToStringArray(const string& aString, vector<string>& aRes);
	void RegMethod(const string& aName, int aArgsNum);
	bool CheckMname(const string& aName) const;
	bool CheckMpars(const string& aName, int aArgsNum) const;
	static void ParseUid(const string& aUid, string& aOid, string& aType);
	static void CombineUid(const string& aOid, const string& aType, string& aUid);
	static bool IsSimpleIid(const string& aIid);
	static string EscCtrl(const string& aInp, char aCtrl);
	static string DeEscCtrl(const string& aInp, char aCtrl);
	static size_t FindFirstCtrl(const string& aString, const char aCtrl, size_t aPos); 
	static size_t FindFirstCtrl(const string& aString, const string& aCtrls, size_t aPos); 
	// Find first occurance of symbol out of delimiters. Delims can be simple and nested
	//	static size_t FindFirstOutOfDelims(const string& aString, const string& aSymbols, char aSimpleDelim,
	//		const string& aNestedDelims, size_t aBeg); 
	static size_t FindRightDelim(const string& aString, char LeftDelim, char RightDelim, size_t aPos);
    public:
	static char KRinvSep;
	static char KEsc;
	static string KArraySep;
	static char KUidSep;
    protected:
	// Methods parameters
	map<string, int> mMpars;
};

/*
   template<typename ... TArgs> string Ifu::PackMethod(const string& aName, TArgs ... aArgs)
   {
   string res = CombineIcSpec(aName, "1");
   AddIcSpecArg(res, Pack(aArg1));
   return res;
   }
   */

inline string Ifu::PackMethod(const string& aName)
{
    string res = CombineIcSpec(aName, "1");
    return res;
}

template<typename TArg1> string Ifu::PackMethod(const string& aName, TArg1 aArg1)
{
    string res = CombineIcSpec(aName, "1");
    AddIcSpecArg(res, Pack(aArg1));
    return res;
}

template<typename TArg1, typename TArg2> string Ifu::PackMethod(const string& aName, TArg1 aArg1, TArg2 aArg2)
{
    string res = Ifu::CombineIcSpec(aName, "1");
    AddIcSpecArg(res, Pack(aArg1));
    AddIcSpecArg(res, Pack(aArg2));
    return res;
}

template<typename TArg1, typename TArg2, typename TArg3> string Ifu::PackMethod(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3)
{
    string res = Ifu::CombineIcSpec(aName, "1");
    AddIcSpecArg(res, Pack(aArg1));
    AddIcSpecArg(res, Pack(aArg2));
    AddIcSpecArg(res, Pack(aArg3));
    return res;
}

template<typename TArg1, typename TArg2, typename TArg3, typename TArg4> string Ifu::PackMethod(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3, TArg4 aArg4)
{
    string res = Ifu::CombineIcSpec(aName, "1");
    AddIcSpecArg(res, Pack(aArg1));
    AddIcSpecArg(res, Pack(aArg2));
    AddIcSpecArg(res, Pack(aArg3));
    AddIcSpecArg(res, Pack(aArg4));
    return res;
}



#endif
