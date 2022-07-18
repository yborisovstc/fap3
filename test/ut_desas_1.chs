MyRoot : Elem {
    # "DES active subsystem. Test #1";
    Launcher :  DesLauncher {
        Ds1 : Des {
            St1 : State {
                = "SI 5";
                Debug.LogLevel = "Dbg";
            }
            St1.Inp ~ : TrAddVar @ {
                Inp ~ St1;
                Inp ~ : State { = "SI 4"; };
            };
            StDone : State {
                = "SB <ERR>";
                Debug.LogLevel = "Dbg";
            }
            As1 : DesAs {
                AsInp : ExtdStateInp;
                St : State @ {
                    _@ < { = "SI 0"; Debug.LogLevel = "Dbg"; }
                    Inp ~ Sw1 : TrSwitchBool @ {
                        Inp1 ~ St;
                        Inp2 ~ : TrAddVar @ {
                            Inp ~ St;
                            Inp ~ : State { = "SI 1"; };
                        };
                        Sel ~ Cmp_Neq : TrCmpVar @ {
                            Inp ~ St;
                            Inp2 ~ AsInp.Int;
                        };
                    };
                }
            }
            As1.AsInp ~ St1;
            As1St_Dbg : State {
                = "SI <ERR>";
                Debug.LogLevel = "Dbg";
            }
            As1St_Dbg.Inp ~ As1.St; 
        }
    }
}
