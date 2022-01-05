MyRoot : Elem {
    Launcher :  DesLauncher {
        Ds1 : Des {
            St1 : State {
                = "SB true";
                Debug.LogLevel = "Dbg";
            }
            Neg : TrNegVar;
            St1.Inp ~ Neg;
            Neg.Inp ~ St1;
            # "URI";
            Const_1 : State { = "URI State1"; }
            St1 : State {
                = "URI Node1";
            }
        }
    }
}
