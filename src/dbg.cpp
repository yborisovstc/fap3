
#include "dbg.h"
#include "mdes.h"



MUnit* Dbg::getUnit(MNode* aNode)
{
    MUnit* res = aNode->lIf(res);
    return res;
}

static void init()
{
    Dbg::getIface<MDesSyncable>(nullptr);
}
