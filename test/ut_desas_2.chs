MyRoot : Elem {
    # "DES active subsystem. Getting start of list";
    # "Test DESAS states initialization";
    Launcher :  DesLauncher {
        ListElem : Syst {
            Prev : CpStateOutp;
            Next : CpStateInp;
        }
        ListStart : Syst {
            Prev : CpStateOutp;
        }
        ListEnd : Syst {
            Next : CpStateInp;
        }
        List : Syst {
            Explorable = "y";
            Start : ListStart;
            E1 : ListElem @ { Next ~ Start.Prev; }
            E2 : ListElem @ { Next ~ E1.Prev; }
            E3 : ListElem @ { Next ~ E2.Prev; }
            E4 : ListElem @ { Next ~ E3.Prev; }
            End : ListEnd @ { Next ~ E4.Prev; }
        }
        ToListStart : DesAs {
            MagAdp : DAdp {
                Init : State { = "SB false"; Debug.LogLevel = "Dbg"; }
                InitStateInp : ExtdStateInp;
                Res : State { = "URI <ERR>"; Debug.LogLevel = "Dbg"; }
                # "Managed node adapter";
                PairOfNext : SdoTcPair @ {
                    Targ ~ Res;
                    TargComp ~ : State { = "URI Next"; };
                }
                PairOfNext_Dbg : State @ {
                    _@ < { Debug.LogLevel = "Dbg"; = "URI <ERR>"; }
                    Inp ~ PairOfNext;
                }
                PairOfNextOwner : SdoCompOwner @ {
                    Comp ~ PairOfNext;
                }
                Res.Inp ~ : TrSwitchBool @ {
                    Inp1 ~  : TrSwitchBool @ {
                        Inp1 ~ Res; 
                        Inp2 ~ PairOfNextOwner; 
                        Sel ~ Cmp_Neq : TrCmpVar @ {
                           Inp ~ Res;
                           Inp2 ~ : State { = "URI Start"; };
                        };
                    };
                    Inp2 ~ InitStateInp.Int;
                    Sel ~ Init;
                };
            }
        }
        Init : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "URI E2"; }
            Inp ~ Sw1 : TrSwitchBool @ {
                Inp2 ~ : State { = "URI E3"; };
                Sel ~ Cmp_Neq : TrCmpVar @ {
                    Inp ~ Init;
                    Inp2 ~ : State { = "URI E3"; };
                };
            };
        }
        Sw1.Inp1 ~ Init;
        AsRes : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "URI <ERR>"; }
            Inp ~ ToListStart.MagAdp.Res;
        }
        ToListStart.MagAdp.InitStateInp ~ Init;
        TargBaseLink : Link { Outp : CpStateMnodeOutp; }
        TargBaseLink ~ List;
        ToListStart.MagAdp.InpMagBase ~ TargBaseLink.Outp;
        ToListStart.MagAdp.InpMagUri ~ : State { = "SS"; };
    }
}
