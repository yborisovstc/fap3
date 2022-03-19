Root : Elem {
    # "Test of simplem System DES controller (DSC) model";
    Modules : Node
    {
        + AdpComps;
    } 
    # "Runner";
    Launcher :  DesLauncher {
        # "Creating component";
        Dc_Comp : ASdcComp @ {
            Enable ~ : State { = "SB true"; };
            Name ~ : State { = "SS Comp"; };
            Parent ~ : State { = "SS Node"; };
        }
        Dc_Comp_Dbg : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
            Inp ~ Dc_Comp.Outp;
        }
        # "Connecting vert";
        Dc_V1 : ASdcComp @ {
            Enable ~ : State { = "SB true"; };
            Name ~ : State { = "SS V1"; };
            Parent ~ : State { = "SS Vert"; };
        }
        Dc_V2 : ASdcComp @ {
            Enable ~ Dc_V1.Outp;
            Name ~ : State { = "SS V2"; };
            Parent ~ : State { = "SS Vert"; };
        }
        Dc_Conn : ASdcConn @ {
            _@ < Debug.LogLevel = "Dbg";
            Enable ~ Dc_V2.Outp;
            V1 ~ : State { = "SS V1"; };
            V2 ~ : State { = "SS V2"; };
        }
        Dc_Conn_Dbg : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
            Inp ~ Dc_Conn.Outp;
        }
    }
}
