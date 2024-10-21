MyRoot : Elem {
    # "Simple system incrementing state"
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        About = "Test"
        Ds1 : Des {
            St1 : State {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            St1.Inp ~ : TrAddVar (
                Inp ~ St1
                Inp ~ : TrSwitchBool (
                    Inp1 ~ : SI_1
                    Inp2 ~ : SI_0
                    Sel ~ Cmp_Ge : TrCmpVar (
                        Inp ~ St1
                        Inp2 ~ : Const {
                            = "SI 100"
                        }
                    )
                )
            )
            : Sout (
                Inp ~ : TrTostrVar (
                    Inp ~ St1
                )
            )
        }
    }
}
