Root : Elem {
    # "Example/test of DCS based on ADP";
    Modules : Node
    {
        + AdpComps;
    } 
    # "CP of object of ordering";
    ObjCp : Socket {
        Value : CpStateInp;
        Name : CpStateInp;
    }
    # "Pair of CP of object of ordering";
    ObjCpPair : Socket {
        Value : CpStateOutp;
        Name : CpStateOutp;
    }
    # "Object of ordering";
    Obj : Des {
        Cp : ObjCp;
        St : State { = "SI -1 "; }
        Adp : AdpComps.NodeAdp @ {
            _@ < AgentUri = "";
            MagOwnerLink ~ _$;
        }
        Cp.Name ~ Adp.Name;
        Cp.Value ~ St;
    }
    # "Ordering slot prev CP";
    OrdsPrevCp : Socket {
        Value : CpStateOutp;
        SlotName : CpStateOutp;
    }
    # "Ordering slot next CP";
    OrdsNextCp : Socket {
        Value : CpStateInp;
        SlotName : CpStateInp;
    }
    # "Ordering slot";
    OrdSlot : Des {
        # "Adapter of self";
        Adp : AdpComps.NodeAdp {
            AgentUri = "";
        }
        Adp.MagOwnerLink ~ _$;
        OCp : ObjCpPair;
        PrevCp : OrdsPrevCp;
        NextCp : OrdsNextCp;
        CtrlCp : Extd {
            Int : CpStateInp;
        }
        # "Comparator";
        CtrlCp.Int ~ : TrSwitchBool @ {
            Sel ~ Cmp_Lt : TrCmpVar @ {
                _@ < Debug.LogLevel = "Dbg";
                Inp ~ OCp.Value;
                Inp2 ~ PrevCp.Value;
            };
            Inp1 ~ : State { = "CHR2 Root : Node { }"; };
            Inp2 ~ : TrChr @ {
                Base ~ : State { = "CHR2 Root : Node { }"; };
                Mut ~ : TrMutDisconn @ {
                    Cp1 ~ : TrApndVar @ {
                        Inp1 ~ OCp.Name;
                        Inp2 ~ : State { = "SS .Cp"; };
                    };
                    Cp2 ~ : TrApndVar @ {
                        Inp1 ~ Adp.Name;
                        Inp2 ~ : State { = "SS .OCp"; };
                    };
                };
                Mut ~ : TrMutConn @ {
                    Cp1 ~ : TrApndVar @ {
                        Inp1 ~ OCp.Name;
                        Inp2 ~ : State { = "SS .Cp"; };
                    };
                    Cp2 ~ : TrApndVar @ {
                        Inp1 ~ PrevCp.SlotName;
                        Inp2 ~ : State { = "SS .OCp"; };
                    };
                };

            };
        };
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
        ObjA : Obj {
            St < = "SI 1";
        }
        ObjB : Obj {
            St < = "SI 2";
        }
        ObjC : Obj {
            St < = "SI 3";
        }
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
        Slot1.OCp ~ ObjC.Cp;
        Slot2.OCp ~ ObjB.Cp;
        Slot3.OCp ~ ObjA.Cp;
    }
}
