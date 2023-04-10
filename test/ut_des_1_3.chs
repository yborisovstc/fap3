MyRoot : Elem {
    _ <  {
        Debug.OwdLogLevel = "Dbg"
    }
    Launcher : DesLauncher {
        Debug.LogLevel = "Info"
        Ds1 : Des {
            St1 : State @  {
                _@ <  {
                    = "SI 0"
                    Debug.LogLevel = "Err"
                }
                Inp ~ : TrAdd2Var @  {
                    Inp ~ St1
                    Inp2 ~ : SI_1
                }
            }
            St2 : State @  {
                _@ <  {
                    = "SI 2"
                    Debug.LogLevel = "Err"
                }
                Inp ~ : TrAdd2Var @  {
                    Inp ~ St2
                    Inp2 ~ : SI_1
                }
            }
            _ <  {
                St3 : State @  {
                    _@ <  {
                        = "SI 2"
                        Debug.LogLevel = "Err"
                    }
                    Inp ~ : TrAdd2Var @  {
                        Inp ~ St2
                        Inp2 ~ : SI_1
                    }
                }
            }
        }
    }
}
