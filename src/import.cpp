
#include "import.h"

Import::Import(const string &aType, const string &aName, MEnv* aEnv): Node(aType, aName, aEnv)
{
}

Import::~Import()
{
}

void Import::onOwnedMutated(const MOwned* aOwned, const ChromoNode& aMut, const MutCtx& aCtx)
{
    // Just stop propagaring the mutation
}

