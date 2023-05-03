Root : Elem {
    + DesUtils
    # "Test of LSC, simple system"
    Incrementor : Des {
        Count : State @  {
            _@ <  {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            Inp ~ : TrAdd2Var @  {
                Inp ~ Count
                Inp2 ~ SI_1
            }
        }
    }
    ListItem : Syst {
        Next : ExtdStateInp
        Prev : ExtdStateOutp @  {
            Int ~ Next.Int
            Int ~ : SI_1
        }
    }
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        Controller : Des {
            # "Controller"
            Manageable : Syst {
                # "Managed system"
                DesAgt : ADes
                Outp : ExtdStateOutp
                Add : TrAddVar @  {
                    Inp ~ Outp
                }
                AddRes : State @  {
                    _@ < Debug.LogLevel = "Dbg"
                    _@ < = "SI 0"
                    Inp ~ Add
                }
            }
            MgbLink : Link {
                MntpOutp : CpStateMnodeOutp
            }
            MgbLink ~ Manageable
            MgbAdp : DAdp @  {
                # "Adapter to manageable"
                InpMagBase ~ MgbLink.MntpOutp
                InpMagUri ~ : Const {
                    = "URI _$"
                }
            }
            MgbAdp <  {
                ItemParentName : Const {
                    = "SS ListItem"
                }
                SdcPause : ASdcPause @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ : SB_True
                }
                CreateBeg : ASdcComp @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ SdcPause.Outp
                    Name ~ : Const {
                        = "SS Beg"
                    }
                    Parent ~ ItemParentName
                }
                CreateEnd : ASdcComp @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ CreateBeg.Outp
                    Name ~ : Const {
                        = "SS End"
                    }
                    Parent ~ ItemParentName
                }
                ConnBegToEnd : ASdcConn @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ CreateEnd.Outp
                    V1 ~ : Const {
                        = "SS End.Next"
                    }
                    V2 ~ : Const {
                        = "SS Beg.Prev"
                    }
                }
                ConnToOutp : ASdcConn @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ ConnBegToEnd.Outp
                    V1 ~ : Const {
                        = "SS End.Prev"
                    }
                    V2 ~ : Const {
                        = "SS Outp.Int"
                    }
                }
                # "Items Iterator"
                ItemsIter : DesUtils.IdxItr @  {
                    InpCnt ~ : Const {
                        = "SI 100"
                    }
                    InpReset ~ : SB_False
                }
                # "Create Item"
                CreateItem : ASdcComp @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ ConnToOutp.Outp
                    Name ~ ItemName : TrApndVar @  {
                        Inp1 ~ : Const {
                            = "SS Item_"
                        }
                        Inp2 ~ : TrTostrVar @  {
                            Inp ~ ItemsIter.Outp
                        }
                    }
                    Parent ~ ItemParentName
                }
                # "Insert Item"
                InsertItem : ASdcInsert2 @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ CreateItem.Outp
                    Enable ~ CreateEnd.Outp
                    Name ~ ItemName
                    Pname ~ : Const {
                        = "SS End"
                    }
                    Prev ~ : Const {
                        = "SS Prev"
                    }
                    Next ~ : Const {
                        = "SS Next"
                    }
                }
                ItemsIter.InpDone ~ InsertItem.Outp
                SdcResume : ASdcResume @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ ItemsIter.OutpDone
                }
            }
        }
    }
}
