Root : Elem {
    # "Example/test of DCS based on ADP";
    Modules : Node
    {
        + AdpComps;
    } 
    # "CP of object of ordering";
    ObjCp : Slot {
        Value : CpStateOutp;
        Name : CpStateOutp;
    }
    # "Object of ordering";
    Obj : Syst {
        Cp : ObjCp;
        St : State { = "SI -1 "; }
        Adp : AdpComps.NodeAdp @ {
            _@ < AgentUri = "";
            MagOwnerLink ~ St;
        }
        Cp.Name ~ Adp.Name;
        Cp.Value ~ St;
    }
    # "Ordering slot prev CP";
    OrdsPrevCp : Socket {
        Value : CpStateOutp;
    }
    # "Ordering slot next CP";
    OrdsNextCp : Socket {
        Value : CpStateInp;
    }
    # "Ordering slot";
    OrdSlot : Syst {
        # "Adapter of ordered object. Not used ATM";
        ObjAdp : AdpComps.NodeAdp {
            AgentUri = "";
        }
        ObjCp : Extd {
            Int : CpStateOutp;
        }
        PrevCp : OrdsPrevCp;
        NextCp : OrdsNextCp;
        CtrlCp : CpStateOutp;
        # "Comparator";
        : TrSwitchBool @ {
            Sel ~ Cmp_Lt : TrCmpVar @ {
                Inp ~ ObjCp.Int;
                Inp2 ~ PrevCp.Value;
            }
            Inp1 ~ : TrChr {
                Base ~ State { = "CHR2 Root : Node {}"; };
                Mut ~ : TrMutConn @ {
                    Cp1 ~ 
                }
            };
        }
    }
    # "Ordering start slot";
    OrdStart : Syst {
        NextCp : OrdsNextCp;
        Value : State { = "SI -1"; } 
        NextCp.Value ~ Value;
    }
    # "Ordering controller";
    OrdController : Syst {
        About = "Ordering controller";
    }
    # "Runner";
    Launcher :  DesLauncher {
        # "Adapter of self";
        SelfAdp : AdpComps.NodeAdp {
           AgentUri = ""; 
        }
        SelfAdp.MagOwnerLink ~ _$;
        # "The ordered nodes";
        ObjA : State { = "SI 1"; }
        ObjB : State { = "SI 2"; }
        ObjC : State { = "SI 3"; }
        # "The ordering system";
        Start : OrdStart;
        Slot1 : OrdSlot;
        Start.NextCp ~ Slot1.PrevCp;
        SelfAdp.InpMut ~ Slot1.CtrlCp;
        Slot2 : OrdSlot;
        Slot1.NextCp ~ Slot2.PrevCp;
        SelfAdp.InpMut ~ Slot2.CtrlCp;
        Slot3 : OrdSlot;
        Slot2.NextCp ~ Slot3.PrevCp;
        SelfAdp.InpMut ~ Slot3.CtrlCp;
        Slot1.ObjCp ~ ObjC;
        Slot2.ObjCp ~ ObjB;
        Slot3.ObjCp ~ ObjA;
    }
}
