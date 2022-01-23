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
            # "Size of vector";
            St2 : State {
                = "VS Item_1 Item_2 Item_3";
            }
            St3 : State {
                = "SI -1";
                Debug.LogLevel = "Dbg";
            }
            St3.Inp ~ : TrSizeVar @ {
                Inp ~ St2;
            };
            # "Item of vector";
            St4 : State {
                = "SS ";
                Debug.LogLevel = "Dbg";
            }
            St4.Inp ~  : TrAtVar @ {
                Inp ~ St2;
                Index ~ : State { = "SI 2"; };
            };
            # "Tuple composer";
            TupleRes : State @ {
                _@ < {
                    = "TPL,SS:name,SI:value none 0";
                    Debug.LogLevel = "Dbg";
                }
                Inp ~ : TrTuple @ {
                    Inp ~ : State {
                        = "TPL,SS:name,SI:value none 0";
                    };
                    _@ < {
                        name : CpStateInp;
                        value : CpStateInp;
                    }
                    name ~ : State { = "SS Test_name"; };
                    value ~ : State { = "SI 24"; };
                };
            }
        }
    }
}
