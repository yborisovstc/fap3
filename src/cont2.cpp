
#include "cont2.h"




#if 0
Cont2::~Cont2()
{
}

bool Cont2::getData(string& aData) const
{
    aData = mData;
    return true;
}

bool Cont2::setData(const string& aData)
{
    mData = aData;
    return true;
}



ContNode2::Node::~Node() {}

MCont2* ContNode2::Node::at(int aIdx) const
{
    return nullptr;
}

MCont2* ContNode2::Node::at(const string& aName) const
{
    return nullptr;
}

bool ContNode2::Node::onContChanged(MCont2* aCnt)
{
    return true;
}

bool ContNode2::Node::addCont(const string& aName, bool aLeaf)
{
    return true;
}



bool ContNode2::getData(string& aData) const
{
    return true;
}


bool ContNode2::setData(const string& aData)
{
    return true;
}


#endif


ContNode2::Owned::~Owned()
{
}

