MyRoot : Elem {
    # "Test of inputs notification"
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        Ds1 : Des {
            Const_2 : State {
                = "SI 2"
            }
            Const_1 : SI_1
            St1 : State (
                _@ <  {
                    = "SI 0"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ Add : TrAdd2Var (
                    Inp ~ Const_2
                    Inp2 ~ Const_1
                )
            )
        }
    }
}
