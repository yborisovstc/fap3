Root : Elem {
    # "Test of SDC, ASdcConn connecting after target vertexes get created. Ref ISS_018"
    Launcher : DesLauncher {
        _@ < Debug.LogLevel = "Dbg"
        Controllable = "y"
        # "Model"
        Syst1 : Des {
            Controllable = "y"
            Syst1_1 : Des {
                Syst1_1_1 : Des
            }
            EnableCreate : State (
                _@ <  {
                    = "SB false"
                }
                Inp ~ : SB_True
            )
            SdcCreate : ASdcComp (
                _@ < Debug.LogLevel = "Dbg"
                Name ~ : Const {
                    = "SS Comp1"
                }
                Parent ~ : Const {
                    = "SS Vert"
                }
                Enable ~ EnableCreate
            )
            SdcCreateT : ASdcCompT (
                _@ < Debug.LogLevel = "Dbg"
                Target ~ : Const {
                    = "URI Syst1_1.Syst1_1_1"
                }
                Name ~ : Const {
                    = "SS Comp1"
                }
                Parent ~ : Const {
                    = "SS Vert"
                }
                Enable ~ EnableCreate
            )
        }
        Syst2 : Des {
            Controllable = "y"
            EnableCreate : State (
                _@ <  {
                    = "SB false"
                }
                Inp ~ : SB_True
            )
            SdcCreate : ASdcComp (
                _@ < Debug.LogLevel = "Dbg"
                Name ~ : Const {
                    = "SS Comp1"
                }
                Parent ~ : Const {
                    = "SS Vert"
                }
                Enable ~ EnableCreate
            )
        }
        # "Connect"
        Dc_Conn : ASdcConn (
            _@ < Debug.LogLevel = "Dbg"
            V1 ~ : State {
                = "SS Syst1.Comp1"
            }
            V2 ~ : State {
                = "SS Syst2.Comp1"
            }
            Enable ~ : SB_True
        )
        Conn_Dbg : State (
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ Dc_Conn.Outp
        )
    }
}
