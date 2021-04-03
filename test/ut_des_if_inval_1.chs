MyRoot : Elem {
    Launcher :  DesLauncher {
        Ds1 : Des {
            St1 : State {
                Value = "SI 0";
                Debug : Content { Update : Content; }
                Debug.Update = "y";
            }
            Const_1 : State { Value = "SI 1"; }
            Add : TrAddVar;
            St1.Inp ~ Add;
            Add.Inp ~ St1;
            Add.Inp ~ Const_1;
            # "Second state";
            St2 : State {
                Value = "SI 7";
                Debug : Content { Update : Content; }
                Debug.Update = "y";
            }
            Const_2 : State { Value = "SI 2"; }
            Add2 : TrAddVar;
            St2.Inp ~ Add2;
            Add2.Inp ~ St2;
            Add2.Inp ~ Const_2;
        }
    }
}
