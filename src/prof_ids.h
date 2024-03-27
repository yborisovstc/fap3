

#ifndef __FAP3_PROF_IDS_H
#define __FAP3_PROF_IDS_H



namespace PEvents {
    enum {
	// Env based events
	EDur_Construct = 1,
	EDur_LaunchActive = 2,
	EDur_Tst1 = 3,
	EDur_EnvSetChromo = 4,
	EDurStat_Trans = 100,
	EDurStat_UInvldIrm = 102,
	EDurStat_IFR_IFaces= 104,
	EDurStat_LaunchRun = 1001,
	EDurStat_LaunchUpdate = 1004,
	EDurStat_LaunchConfirm = 1005,
	EDurStat_DesRfInpObs = 2001,       // Refresh input observers
	EDurStat_DesAsUpd = 2002,
	EDurStat_StConfirm = 3001,
	EDurStat_AMNAdpConfirm = 3002,
	EDurStat_DAdpIAP = 3006,
	EDurStat_ASdcConfirm = 3100,
	EDurStat_ASdcConfState = 3101,
	EDurStat_ASdcConfCtl = 3102,
	EDurStat_ASdcCtlCmp = 3103,
	EDurStat_Clbr = 3200,              // Profiler calibration
	EDurStat_Tmp = 3201,              // Temporary measuring
	EDurStat_Tmp2 = 3202,              // Temporary measuring 2
	EDurStat_Tmp3 = 3203,              // Temporary measuring 3
	EDurStat_MutCrn = 3300,           // Mutation. Creation of native agent
	EDurStat_MutConn = 3301,           // Mutation. Connect.
	EDurStat_MutDsc = 3302,           // Mutation. Disconnect.
	EDurStat_MutCont = 3303,           // Mutation. Content.
	EDurStat_MutRm = 3304,           // Mutation. Removal.
	EDurStat_MutAtt = 3305,           // Mutation. Adding node. Attaching.
	EDurStat_MutNtf = 3306,           // Mutation. Owner notification
	EDurStat_MutCad = 3307,           // Mutation. Adding mut to target chromo root
	EDurStat_Tst1 = 3400,           // Test
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
