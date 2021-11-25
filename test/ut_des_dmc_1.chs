MyRoot : Elem {
    Launcher :  DesLauncher {
        About : Content;
        About < = "Test";
        Ds1 : Des {
            St1 : State {
                = "SI 0";
                Debug : Content { Update : Content { = "y"; } }
            }
            St1.Inp ~ : TrAddVar @ {
                Inp ~ St1;
                Inp ~  : State { = "SI 1"; };
            };
        }
        Ds2 : Ds1;
    }
}
