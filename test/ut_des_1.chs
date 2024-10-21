MyRoot : Elem {
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        Ds1 : Des {
            Const_1 : SI_1
            St1 : State (
                _@ <  {
                    = "SI 0"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ Add : TrAddVar (
                    Inp ~ St1
                    Inp ~ Const_1
                )
            )
        }
    }
}
