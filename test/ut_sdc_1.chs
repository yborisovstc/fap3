Root : Elem {
    # "Test of simplem System DES controller (DSC) model"
    + AdpComps
    # "Runner"
    Launcher : DesLauncher {
        Controllable = "y"
        # "Creating component"
        Dc_Comp_Err : ASdcComp @  {
            _@ < Debug.LogLevel = "Dbg"
            Enable ~ : State {
                = "SB true"
            }
            Name ~ : State {
                = "SS _INV"
            }
            Parent ~ : State {
                = "SS Node"
            }
        }
        Dc_Comp_Err_Dbg : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ Dc_Comp_Err.Outp
        }
        # "Creating component Comp"
        Dc_Comp : ASdcComp @  {
            _@ < Debug.LogLevel = "Dbg"
            Enable ~ : State {
                = "SB true"
            }
            Name ~ : State {
                = "SS Comp"
            }
            Parent ~ : State {
                = "SS Node"
            }
        }
        Dc_Comp_Dbg : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ Dc_Comp.Outp
        }
        Dc_Rm : ASdcRm @  {
            _@ < Debug.LogLevel = "Dbg"
            Enable ~ Dc_Comp.Outp
            Name ~ Dc_Comp.OutpName
        }
        # "Creating component Comp2"
        Dc_Comp2 : ASdcComp @  {
            _@ < Debug.LogLevel = "Dbg"
            Enable ~ : State {
                = "SB true"
            }
            Name ~ : State {
                = "SS Comp2"
            }
            Parent ~ : State {
                = "SS Node"
            }
        }
        Dc_Comp2_Dbg : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ Dc_Comp2.Outp
        }

        # "Connecting vert"
        Dc_V1 : ASdcComp @  {
            _@ < Debug.LogLevel = "Dbg"
            Enable ~ : State {
                = "SB true"
            }
            Name ~ : State {
                = "SS V1"
            }
            Parent ~ : State {
                = "SS Vert"
            }
        }
        Dc_V2 : ASdcComp @  {
            _@ < Debug.LogLevel = "Dbg"
            Enable ~ Dc_V1.Outp
            Name ~ : State {
                = "SS V2"
            }
            Parent ~ : State {
                = "SS Vert"
            }
        }
        Dc_Conn : ASdcConn @  {
            _@ < Debug.LogLevel = "Dbg"
            Enable ~ Dc_V2.Outp
            V1 ~ Dc_V1.OutpName
            V2 ~ Dc_V2.OutpName
        }
        Dc_Conn_Dbg : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ Dc_Conn.Outp
        }
    }
}
