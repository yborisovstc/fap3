
#include "dbg.h"
#include "mdes.h"
#include "mnode.h"
#include "munit.h"
#include "melem.h"
#include "mvert.h"
#include "mdata.h"


std::string Dbg::mLog;
int Dbg::mEnableLog = 0;

MUnit* Dbg::getUnit(MNode* aNode)
{
    MUnit* res = aNode->lIf(res);
    return res;
}

MElem* Dbg::getElem(MNode* aNode)
{
    MElem* res = aNode->lIf(res);
    return res;
}

template <typename T> T* Dbg::getIface(MNode* aNode)
{
    T* res = aNode->lIf(res);
    return res;
}


static void init()
{
    Dbg::getIface<MDesSyncable>(nullptr);
    Dbg::getIface<MVert>(nullptr);
    Dbg::getIface<MDVarGet>(nullptr);
}
