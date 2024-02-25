

#ifndef __FAP3_PROF_IDS_H
#define __FAP3_PROF_IDS_H



namespace PEvents {
    enum {
	// Env based events
	EDur_Construct = 1,
	EDur_LaunchActive = 2,
	EDurStat_Trans = 100,
	EDurStat_UInvldIrm = 102,
	EDurStat_LaunchRun = 1001,
	EDurStat_LaunchActive = 1002,
	EDurStat_LaunchUpdate = 1004,
	EDurStat_LaunchConfirm = 1005,
	EDurStat_DesIfsCount = 2001,
	EDurStat_DesAsUpd = 2002,
	EDurStat_StConfirm = 3001,
	EDurStat_AMNAdpConfirm = 3002,
	EDurStat_DAdpConfirm = 3003,
	EDurStat_DAdpIAP = 3006,
	EDurStat_DAdpDes = 3007,
	EDurStat_ASdcConfirm = 3100,
	EDurStat_ASdcConfState = 3101,
	EDurStat_ASdcConfCtl = 3102,
	EDurStat_ASdcCtlCmp = 3103,
	EDurStat_Tmp = 3200,              // Temporary measuring
	EDurStat_MutCrn = 3300,           // Mutation. Creation of native agent
	EDurStat_MutConn = 3301,           // Mutation. Connect.
	// Common events
	EDurStat_IFR_IFaces = 10001,
	EDurStat_Ev2 = 10002,
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
