
#include "desio.h"
#include "data.h"


Sout::Sout(const string& aName, MEnv* aEnv): State(aName, aEnv)
{
    if (aName.empty()) mName = Type();
    //setContent(KCont_Value, "SS");
}

void Sout::update()
{
    State::update();
}

void Sout::confirm()
{
    State::confirm();
    MDVarGet* vget = dynamic_cast<MDVarGet*>(mPdata->MDVar_getLif(MDVarGet::Type()));
    MDtGet<Sdata<string>>* dget = vget ? vget->GetDObj(dget) : nullptr;
    if (dget) {
	Sdata<string> data;
	dget->DtGet(data);
	cout << data.mData << endl;
    }
}



