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
        ObjName : CpStateOutp;
        SlotName : CpStateOutp;
        Swap : CpStateOutp;
    }
    # "Ordering slot next CP";
    OrdsNextCp : Socket {
        Value : CpStateInp;
        ObjName : CpStateInp;
        SlotName : CpStateInp;
        Swap : CpStateInp;
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
        NextCp.Value ~ OCp.Value;
        NextCp.SlotName ~ Adp.Name;
        NextCp.ObjName ~ OCp.Name;
        # "Comparator";
        CtrlCp.Int ~ : TrSwitchBool @ {
            Sel ~ Swap : TrAndVar @ {
                _@ < Debug.LogLevel = "Dbg";
                Inp ~ : TrNegVar @ {
                    Inp ~ PrevCp.Swap;
                };
                Inp ~ Cmp_Lt : TrCmpVar @ {
                    _@ < Debug.LogLevel = "Dbg";
                    Inp ~ OCp.Value;
                    Inp2 ~ PrevCp.Value;
                };
            };
            Inp1 ~ : State { = "CHR2 { }"; };
            Inp2 ~ : TrChr @ {
                Base ~ : State { = "CHR2 { }"; Debug.LogLevel = "Dbg"; };
                # "Pass own object to left slot";
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
                # "Seize left slot object";
                Mut ~ : TrMutDisconn @ {
                    Cp1 ~ : TrApndVar @ {
                        Inp1 ~ PrevCp.ObjName;
                        Inp2 ~ : State { = "SS .Cp"; };
                    };
                    Cp2 ~ : TrApndVar @ {
                        Inp1 ~ PrevCp.SlotName;
                        Inp2 ~ : State { = "SS .OCp"; };
                    };
                };
                Mut ~ : TrMutConn @ {
                    Cp1 ~ : TrApndVar @ {
                        Inp1 ~ PrevCp.ObjName;
                        Inp2 ~ : State { = "SS .Cp"; };
                    };
                    Cp2 ~ : TrApndVar @ {
                        Inp1 ~ Adp.Name;
                        Inp2 ~ : State { = "SS .OCp"; };
                    };
                };
            };
        };
        NextCp.Swap ~ Swap; 
        # "Logging";
        : Sout @ {
            _@ < Debug.LogLevel = "Dbg";
            Inp ~ : TrApndVar @ {
                Inp1 ~ Adp.Name;
                Inp2 ~ : TrApndVar @ {
                    Inp1 ~ : State { = "SS  --- "; };
                    Inp2 ~ OCp.Name;
                };
            };
        }
    }
    # "Ordering start slot";
    OrdStart : Syst {
        NextCp : OrdsNextCp;
        Value : State { = "SI -1"; } 
        NextCp.Value ~ Value;
        NextCp.Swap ~ : State { = "SB false"; };
    }
    # "Ordering controller";
    OrdController : Syst {
        About = "Ordering controller";
    }
    # "Runner";
    Launcher :  DesLauncher {
        Counter : State;
        OutpCount = "y";
        # "Adapter of self";
        SelfAdp : AdpComps.NodeAdp {
           AgentUri = ""; 
        }
        SelfAdp.MagOwnerLink ~ _$;
        SelfAdp.InpMut ~ LadpChr : TrChrc;
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
        ObjD : Obj {
            St < = "SI 4";
        }
        # "The ordering system";
        Start : OrdStart;
        Slot1 : OrdSlot;
        Start.NextCp ~ Slot1.PrevCp;
        LadpChr.Inp ~ Slot1.CtrlCp;
        Slot2 : OrdSlot;
        Slot1.NextCp ~ Slot2.PrevCp;
        LadpChr.Inp ~ Slot2.CtrlCp;
        Slot3 : OrdSlot;
        Slot2.NextCp ~ Slot3.PrevCp;
        LadpChr.Inp ~ Slot3.CtrlCp;
        Slot4 : OrdSlot;
        Slot3.NextCp ~ Slot4.PrevCp;
        LadpChr.Inp ~ Slot4.CtrlCp;
        Slot1.OCp ~ ObjD.Cp;
        Slot2.OCp ~ ObjC.Cp;
        Slot3.OCp ~ ObjB.Cp;
        Slot4.OCp ~ ObjA.Cp;
    }
}
