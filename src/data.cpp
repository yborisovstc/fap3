

//#include <stdlib.h>
//#include <stdio.h>
//#include "mvert.h"
#include "data.h"
//#include "vert.h"
#include "guri.h"
#include "rmutdata.h"
#include "rdatauri.h"



// Row data signatures
//
// Composite data
template<> const char* Vector<string>::TypeSig() { return  "VS";};
template<> const char* Vector<Pair<string>>::TypeSig() { return  "VPS";};
template<> const char* Pair<string>::TypeSig() { return  "PS";};
template<> const char* Pair<Sdata<string>>::TypeSig() { return  "PSS";};
template<> const char* Pair<Sdata<int>>::TypeSig() { return  "PSI";};
template<> const char* Vector<DGuri>::TypeSig() { return  "VDU";};
template<> const char* Pair<DGuri>::TypeSig() { return  "PDU";};
template<> const char* Vector<Pair<DGuri>>::TypeSig() { return  "VPDU";};
