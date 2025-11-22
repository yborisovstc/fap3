Root : Elem {
    # "Test of SDC, create and remove in cycle"
    Launcher : DesLauncher {
        # "Creation"
        Dc_Create : ASdcComp (
            _@ < LogLevel = "Dbg"
            Name ~ : State {
                = "SS Comp"
            }
            Parent ~ : State {
                = "SS Node"
            }
        )
        # "Removal"
        Dc_Rm : ASdcRm (
            _@ < LogLevel = "Dbg"
            Enable ~ Dc_Create.Outp
            Name ~ : State {
                = "SS Comp"
            }
        )
        # "Form start condition"
        Dc_Create.Enable ~ : TrOrVar (
            Inp ~ : State (
                _@ < = "SB true"
                Inp ~ : State {
                    = "SB false"
                }
            )
            Inp ~ Dc_Rm.Outp
        )
    }
}
