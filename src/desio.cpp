
#include "desio.h"
#include "data.h"


Sout::Sout(const string &aType, const string& aName, MEnv* aEnv): State(aType, aName, aEnv)
{
    //setContent(KCont_Value, "SS");
}

void Sout::update()
{
    State::update();
}

void Sout::confirm()
{
    State::confirm();

    const Sdata<string>* data = reinterpret_cast<const Sdata<string>*>(mPdata);
    if (data) {
	cout << data->mData << endl;
    }
}



