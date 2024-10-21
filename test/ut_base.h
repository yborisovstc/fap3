
#ifndef __UT_BASE_H__
#define __UT_BASE_H__

/** @brief UT base and utils
*/
class UtUtils
{
    public:
        static MNode* constructSystem(Env*& aEnv, const string& aFname);
        /** @brief Helper. Get state data string 
         * */
        static string getStateDstr(Env* aEnv, const string& aUri);
};

string UtUtils::getStateDstr(Env* aEnv, const string& aUri)
{
    MNode* st = aEnv->Root()->getNode(aUri);
    MDVarGet* stg = st ? st->lIf(stg) : nullptr;
    const DtBase* data = stg ? stg->VDtGet(string()) : nullptr;
    return data ? data->ToString(true) : string();
}

MNode* UtUtils::constructSystem(Env*& aEnv, const string& aSpecn)
{
    string ext = "chs";
    string spec = aSpecn + string(".") + "chs";
    string log = aSpecn + "_" + ext + ".log";
    aEnv = new Env(spec, log);
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", aEnv != 0);
    aEnv->ImpsMgr()->ResetImportsPaths();
    aEnv->ImpsMgr()->AddImportsPaths("../modules");
    aEnv->constructSystem();
    MNode* root = aEnv->Root();
    MElem* eroot = root ? root->lIf(eroot) : nullptr;
    CPPUNIT_ASSERT_MESSAGE("Fail to get root", root && eroot);
    return root;
}





#endif
