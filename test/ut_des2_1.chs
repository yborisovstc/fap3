MyRoot : Elem {
    Launcher : DesLauncher {
        Debug.LogLevel = "Info"
        Ds1 : Des {
            Const_1 : State2 {
                = "SI 1"
            }
            St1 : State2 @  {
                _@ <  {
                    = "SI 0"
                    Debug.LogLevel = "Err"
                }
                Inp ~ Add : TrAddVar2 @  {
                    Inp ~ St1
                    Inp ~ Const_1
                }
            }
        }
    }
}
