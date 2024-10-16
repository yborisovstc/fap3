Root : Elem {
    # "Test of LSC, control on Controllable gets idle, ref ds_desopt_au"
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        System : Des {
            # "System, including controlled subs"
            Controlled : Syst {
                Agt : ADes
                # "Managed system"
                Debug.LogLevel = "Dbg"
                Counter : State (
                    _@ < Debug.LogLevel = "Dbg"
                    _@ < = "SI 0"
                    Inp ~ Add : TrAdd2Var (
                        Inp ~ Counter
                        Inp2 ~ : TrSwitchBool (
                            Sel ~ Cmp_Ge : TrCmpVar (
                                Inp ~ Counter
                                Inp2 ~ : Const {
                                    = "SI 100"
                                }
                            )
                            Inp1 ~ : SI_1
                            Inp2 ~ : SI_0
                        )
                    )
                )
            }
            {
                # "Controlling"
                SdoIdle : SdoDesIdle {
                    Debug.LogLevel = "Dbg"
                }
                Idle_Dbg : State (
                    _@ < Debug.LogLevel = "Dbg"
                    _@ < = "SB false"
                    Inp ~ SdoIdle
                )
            }
        }
    }
}
