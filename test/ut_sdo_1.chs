Root : Elem {
    # "Test of SDC, create and remove in cycle";
    Modules : Node
    {
    } 
    # "Runner";
    Launcher :  DesLauncher {
        Explorable = "y";
        # "Components to be checked";
        V1 : Vert;
        V2 : Vert;
        V3 : Vert;
        V4 : Vertu {
            V4_1 : Vertu;
        }
        V5 : Vertu {
            Explorable = "y";
            # "Pairs";
            DcoPairs : SdoPairs  {
                Debug.LogLevel = "Dbg";
            }
        }
        V6 : Vertu;
        V1 ~ V2;
        V1 ~ V4;
        V5 ~ V4;
        V5 ~ V3;
        V4.V4_1 ~ V6;
        # "Checking existence of component";
        Dbg_DcoComp : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
            Inp ~ DcoComp : SdoComp @ {
                _@ < Debug.LogLevel = "Dbg";
                Name ~ : State { = "SS V1"; };
            };
        }
        # "Checking connection";
        Dbg_DcoConn : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
            Inp ~ DcoConn : SdoConn @ {
                _@ < Debug.LogLevel = "Dbg";
                Vp ~ : State { = "SS V1"; };
                Vq ~ : State { = "SS V2"; };
            };
        }
        # "Checking connection V1 ~ V3";
        Dbg_DcoConn2 : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
            Inp ~ DcoConn2 : SdoConn @ {
                _@ < Debug.LogLevel = "Dbg";
                Vp ~ : State { = "SS V1"; };
                Vq ~ : State { = "SS V3"; };
            };
        }
        # "Pairs count";
        Dbg_DcoPairsCount : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SI 0"; }
            Inp ~ DcoPairsCount : SdoPairsCount @ {
                _@ < Debug.LogLevel = "Dbg";
                Vp ~ : State { = "SS V1"; };
            };
        }
        # "Target Pair";
        Dbg_DcoPair : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "URI"; }
            Inp ~ DcoPair : SdoPair @ {
                _@ < Debug.LogLevel = "Dbg";
                Vp ~ : State { = "SS V2"; };
            };
        }
        # "Component owner";
        Dbg_DcoCompOwner : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "URI <ERR>"; }
            Inp ~ DcoCompOwner : SdoCompOwner @ {
                _@ < Debug.LogLevel = "Dbg";
                Comp ~ : State { = "URI V4.V4_1"; };
            };
        }
        # "Component comp";
        Dbg_DcoCompComp : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "URI <ERR>"; }
            Inp ~ DcoCompComp : SdoCompComp @ {
                _@ < Debug.LogLevel = "Dbg";
                Comp ~ : State { = "URI V4"; };
                CompComp ~ : State { = "URI V4_1"; };
            };
        }
        # "V5 pairs";
        Dbg_DcoPairs : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "VU _INV"; }
            Inp ~ V5.DcoPairs;
        }
        # "Component V5 pairs";
        Dbg_DcoTPairs : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "VU _INV"; }
            Inp ~ DcoTPairs : SdoTPairs @ {
                _@ < Debug.LogLevel = "Dbg";
                Targ ~ : State { = "URI V5"; };
            };
        }
        # "Component of target pair";
        Dbg_DcoTcPair : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "URI <ERR>"; }
            Inp ~ DcoTcPair : SdoTcPair @ {
                _@ < Debug.LogLevel = "Dbg";
                Targ ~ : State { = "URI V4"; };
                TargComp ~ : State { = "URI V4_1"; };
            };
        }
    }
}
