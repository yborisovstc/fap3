MyRoot : Elem {
    Launcher :  DesLauncher {
        Ds1 : Des {
            St1 : State {
                = "SI 0";
                Debug.LogLevel = "Dbg";
            }
            Const_1 : State { = "SI 1"; }
            Const_2 : State { = "SI 1"; }
            Add : TrAddVar;
            St1.Inp ~ Add;
            Add.Inp ~ St1;
            Add.Inp ~ Const_2;
            Add.InpN ~ Const_1;
        }
    }
}
