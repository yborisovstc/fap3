#ifndef __FAP3_SYST_H
#define __FAP3_SYST_H

class ConnPointu: public Vertu, public MConnPoint
{
    public:
	ConnPoint(const string &aName, MEnv* aEnv): Vertu(aName, aEnv) {}
	virtual ~ConnPoint() {}
}

#endif
