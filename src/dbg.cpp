
#include "dbg.h"
#include "mdes.h"
#include "mnode.h"
#include "munit.h"


std::string Dbg::mLog;
int Dbg::mEnableLog = 0;

MUnit* Dbg::getUnit(MNode* aNode)
{
    MUnit* res = aNode->lIf(res);
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
}
