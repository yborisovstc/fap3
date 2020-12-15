#include <stdio.h>
#include <iostream> 

#include "cont2.h"
#include "ifu.h"


static const char KStart = '{';
static const char KEnd = '}';
static const char KValSep = ':';
static const char KCompsSep = ' ';
static const char KValQuote = '\'';
static const char KDeletion = '-';


bool Contu::isComplexContent(const string& aVal)
{
    return (!aVal.empty() && aVal.at(0) == KStart);
}


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

void Cont2::MCont2_dump(int aIdt) const
{
    cout << string(aIdt, ' ') << "_DEFAULT_ :" << mData << endl;
}



ContNode2::~ContNode2()
{
    // Reset self pointer in connector to avoid double free glibc error
    resetPx();
}

bool ContNode2::getData(string& aData) const
{
    return true;
}

bool ContNode2::setData(const string& aVal)
{
    bool res = true;
    size_t pos_beg = 0, pos = 0;
    if (!aVal.empty() && aVal.at(0) == KStart) { // Complex content
	string delims(1, KCompsSep); delims += KValQuote; delims += KEnd;
	size_t pos_beg = 1;
	// Parsing elements of content (value | comps | category)
	bool end = false;
	do { // By elements
	    // Passing thru elements separator
	    pos = aVal.find_first_not_of(KCompsSep, pos_beg);
	    if (pos != string::npos) {
		char dl = aVal.at(pos);
		if (dl == KEnd) { // Complex content closing delimiter
		    end = true;
		} else if (dl == KValQuote) { // Value
		    res = false;
		} else if (isalpha(dl)) { // Component
		    pos_beg = pos;
		    // Getting comp name
		    pos = aVal.find_first_of(KValSep, pos_beg);
		    if (pos == string::npos) { // Error: comps name separator is missing
			res = false;
		    } else {
			string sname = aVal.substr(pos_beg, pos - pos_beg);
			pos_beg = pos + 1;
			if (pos_beg >= aVal.size()) { // Error: Uncomplete comp
			    res = false;
			} else {
			    if (aVal.at(pos_beg) == KStart) { // Comps value is full content
				pos = Ifu::FindRightDelim(aVal, KStart, KEnd, pos_beg + 1);
				string value = aVal.substr(pos_beg, pos - pos_beg + 1);
				addCont(sname);
				MCont2* cont = at(sname);
				cont->setData(value);
			    } else if (aVal.at(pos_beg) == KValQuote) { // Quoted value
				pos_beg += 1;
				pos = Ifu::FindFirstCtrl(aVal, KValQuote, pos_beg);
				string value = aVal.substr(pos_beg, pos - pos_beg);
				addCont(sname, true);
				MCont2* cont = at(sname);
				cont->setData(value);
			    } else if (aVal.at(pos_beg) == KDeletion) { // Deletion
				//RemoveContentComp(aName, sname);
				//pos = pos_beg;
			    } else { // Error: Incorrect comp value start delimiter
				res = false;
			    }
			}
			if (pos == string::npos) { // Error: missing comp end delimiter
			    res = false;
			}
			pos_beg = pos + 1;
		    }
		} else { // Error: incorrect element
		    res = false;
		}
	    } else { // Error: missing closing conent delimiter
		res = false;
	    }
	} while (!end && pos != string::npos && res); // By elements
    } else { // Bare value
	res = false;
    }
    if (res) {
	//res = OnCompChanged(this, aName, aRtOnly);
	//iEnv->Observer()->OnCompChanged(this, aName, aRtOnly);
    }
    return res;
}

bool ContNode2::addCont(const CUri& aUri, bool aLeaf)
{
    bool res = addCont(0, aUri, aLeaf);
    return res;
}

MCont2* ContNode2::at(int aIdx) const
{
    return nullptr;
}

MCont2* ContNode2::at(const string& aName) const
{
    NTnip::TNode::TPair* pair =  mOwnerCp.at(aName);
    return pair ? pair->provided() : nullptr;
}

MCont2* ContNode2::getContent(const CUri& aUri) const
{
    MCont2* res = nullptr;
    const MContNode2* node = this;
    for (int i = 0; i < aUri.size(); i++) {
	res = node->at(aUri.at(i));
	if (!res) break;
	node = res->node();
    }
    return res;
}

bool ContNode2::onContChanged(MCont2* aCnt)
{
    return true;
}

bool ContNode2::addCont(int aIdx, const CUri& aUri, bool aLeaf)
{
    bool res = false;
    string name = aUri.at(aIdx);
    if (aIdx < aUri.size() - 1) {
	auto newcont = new ContNode2(name);
	res = mOwnerCp.connect(newcont);
	if (res)
	    res = newcont->addCont(++aIdx, aUri, aLeaf);
    } else {
	if (aLeaf) {
	    auto newcont = new ContLeaf2(name);
	    res = mOwnerCp.connect(newcont);
	} else {
	    auto newcont = new ContNode2(name);
	    res = mOwnerCp.connect(newcont);
	}
    }
    return res;
}

void ContNode2::MCont2_dump(int aIdt) const
{
    cout << string(aIdt, ' ') << mId << " > " << endl;
    mOwnerCp.dump(++aIdt);
}



ContLeaf2::~ContLeaf2()
{
    // Reset self pointer in connector to avoid double free glibc error
    resetPx();
}

void ContLeaf2::MCont2_dump(int aIdt) const
{
    cout << string(aIdt, ' ') << mId << " : " << mData << endl;
}
