#ifndef __FAP3_MUNIT_H
#define __FAP3_MUNIT_H

#include "miface.h"
#include "menv.h"

class MUnit: public MIface
{
    // From MIface
    constexpr static const char* mType = "MUnit";
    virtual string Uid() const override { return MUnit_Uid();}
    virtual string MUnit_Uid() const = 0;
    // Local
    virtual string name() const = 0;
};


#endif  //  __FAP3_MUNIT_H
