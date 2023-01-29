MyRoot : Elem {
    Launcher : DesLauncher {
        About : Content
        About < = "Test"
        Ds1 : Des {
            St1 : State {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            St1.Inp ~ : TrAddVar @  {
                _@ < Debug.LogLevel = "Dbg"
                _@ <  {
                    About = "Transition Add"
                }
                Inp ~ St1
                Inp ~ : State {
                    = "SI 1"
                }
            }
        }
        Ds2 : Ds1
    }
}
