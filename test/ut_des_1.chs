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
        }
    }
}
