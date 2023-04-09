MyRoot : Elem {
    _ <  {
        Debug.OwdLogLevel = "Dbg"
    }
    Launcher : DesLauncher {
        Debug.LogLevel = "Info"
        St1 : State @  {
            _@ <  {
                = "SI 0"
                Debug.LogLevel = "Info"
            }
            Inp ~ : TrAdd2Var @  {
                Inp ~ St1
                Inp2 ~ : SI_1
            }
        }
    }
}
