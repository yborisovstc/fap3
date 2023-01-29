MyRoot : Elem {
    + DesUtils
    Launcher : DesLauncher {
        ListElem : Syst {
            Prev : CpStateOutp
            Next : CpStateInp
        }
        ListStart : Syst {
            Prev : CpStateOutp
        }
        ListEnd : Syst {
            Next : CpStateInp
        }
        List : Syst {
            Explorable = "y"
            Start : ListStart
            E1 : ListElem @  {
                Next ~ Start.Prev
            }
            E2 : ListElem @  {
                Next ~ E1.Prev
            }
            E3 : ListElem @  {
                Next ~ E2.Prev
            }
            E4 : ListElem @  {
                Next ~ E3.Prev
            }
            End : ListEnd @  {
                Next ~ E4.Prev
            }
        }
        # "Given position passed to desas as arg"
        Pos : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI -1"
            }
            Inp ~ : TrSwitchBool @  {
                Inp1 ~ : TrAddVar @  {
                    Inp ~ Pos
                    Inp ~ : State {
                        = "SI 1"
                    }
                }
                Inp2 ~ Pos
                Sel ~ Cmp_Eq_1 : TrCmpVar @  {
                    Inp ~ Pos
                    Inp2 ~ : State {
                        = "SI 3"
                    }
                }
            }
        }
        # "Getting position active subs"
        ListPos : DesUtils.ListItemByPos @  {
            InpPos ~ Pos
            InpMagLink ~ List
        }
        AsRes : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "URI _INV"
            }
            Inp ~ ListPos.OutpNode
        }
    }
}
