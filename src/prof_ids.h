

#ifndef __FAP3_PROF_IDS_H
#define __FAP3_PROF_IDS_H



namespace PEvents {
    // Duration pind
    enum {
	EDur_Construct = 1,
	EDur_LaunchActive = 2,
	EDur_Tst1 = 3,
	EDur_EnvSetChromo = 4,
    };
    // Durstat pind
    enum {
	EDurStat_Trans = 0,
	EDurStat_PvdCNode = 1,         // Provider creates node
	EDurStat_UInvldIrm = 2,
	EDurStat_IFR_IFaces= 3,
	EDurStat_LaunchRun = 4,
	EDurStat_LaunchUpdate = 5,
	EDurStat_LaunchConfirm = 6,
	EDurStat_DesRfInpObs = 7,       // Refresh input observers
	EDurStat_DesAsUpd = 8,
	EDurStat_StConfirm = 9,
	EDurStat_StUpdate = 10,
	EDurStat_AMNAdpConfirm = 11,
	EDurStat_DAdpIAP = 12,
	EDurStat_ASdcConfirm = 13,
	EDurStat_ASdcConfState = 14,
	EDurStat_ASdcCtlConn = 15,
	EDurStat_ASdcCtlCmp = 16,
	EDurStat_ASdcCtlMut = 17,
	EDurStat_ASdcUpdate = 18,
	EDurStat_Clbr = 19,              // Profiler calibration
	EDurStat_Tmp = 20,              // Temporary measuring
	EDurStat_Tmp2 = 21,              // Temporary measuring 2
	EDurStat_Tmp3 = 22,              // Temporary measuring 3
	EDurStat_MutCrn = 23,           // Mutation. Creation of native agent
	EDurStat_MutConn = 24,           // Mutation. Connect.
	EDurStat_MutDsc = 25,           // Mutation. Disconnect.
	EDurStat_MutCont = 26,           // Mutation. Content.
	EDurStat_MutRm = 27,           // Mutation. Removal.
	EDurStat_MutAtt = 28,           // Mutation. Adding node. Attaching.
	EDurStat_MutNtf = 29,           // Mutation. Owner notification
	EDurStat_MutCad = 30,           // Mutation. Adding mut to target chromo root
	EDurStat_Tst1 = 31,           // Test
	EDurStat_MAX = 50,              // Max durstat pind
    };
}

namespace PIndFId {
    enum {
	EInd_VAL = 0,
	EStat_CNT,
	EStat_MIN,
	EStat_MAX,
	EStat_SUM,
	EStat_AVG,
	EStat_SD,
	EStat_ERRCNT,
    };
}

#endif
