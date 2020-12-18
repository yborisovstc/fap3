
#include "cont.h"


bool Cont::getContent(string& aCont) const
{
    aCont = mData;
    return true;
}

bool Cont::setContent(const string& aCont)
{
    mData = aCont;
    return true;
}

bool Cont::isCompatible(MCIface* aPair) const
{
    return true;
}

bool Cont::connect(MCIface* aPair)
{
    return true;
}

bool Cont::disconnect(MCIface* aPair)
{
    return true;
}

bool Cont::getId(string& aId) const
{
    return true;
}

bool Cont::isConnected(MCIface* aPair) const
{
    return true;
}





bool ContNode::Node::addCont(const string& aName, bool aLeaf)
{
    return false;
}


MCont* ContNode::Node::at(int aIdx) const
{
    return nullptr;
}

MCont* ContNode::Node::at(const string& aName) const
{
    return nullptr;
}

bool ContNode::Node::onContChanged(MCont* aCnt)
{
    return true;
}

bool ContNode::Node::isCompatible(MCIface* aPair) const
{
    return true;
}

bool ContNode::Node::connect(MCIface* aPair)
{
    return true;
}

bool ContNode::Node::disconnect(MCIface* aPair)
{
    return true;
}

bool ContNode::Node::getId(string& aId) const
{
    return true;
}

bool ContNode::Node::isConnected(MCIface* aPair) const
{
    return true;
}

bool ContNode::getContent(string& aCont) const
{
    return false;
}

bool ContNode::setContent(const string& aCont)
{
    return false;
}

bool ContNode::isCompatible(MCIface* aPair) const
{
    return true;
}

bool ContNode::connect(MCIface* aPair)
{
    return true;
}

bool ContNode::disconnect(MCIface* aPair)
{
    return true;
}

bool ContNode::getId(string& aId) const
{
    return true;
}

bool ContNode::isConnected(MCIface* aPair) const
{
    return true;
}

MCont* ContNode::nodeGetContent(const CUri& aUri)
{
    return nullptr;

}
