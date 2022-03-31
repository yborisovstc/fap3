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
        V1 ~ V2;
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
    }
}
