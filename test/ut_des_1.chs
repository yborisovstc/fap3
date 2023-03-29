MyRoot : Elem {
    Debug.OwdLogLevel = "Dbg"
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        Ds1 : Des {
            Const_1 : State {
                = "SI 1"
            }
            St1 : State @  {
                _@ <  {
                    = "SI 0"
                    Debug.LogLevel = "Dbg"
                }
                Inp ~ Add : TrAddVar @  {
                    Inp ~ St1
                    Inp ~ Const_1
                }
            }
        }
    }
}
