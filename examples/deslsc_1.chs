Root : Elem {
    # "Test of LSC, simple system"
    Incrementor : Des {
        Count : State @  {
            Explorable = "y"
            _@ <  {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            Inp ~ : TrAdd2Var @  {
                Inp ~ Count
                Inp2 ~ : TrSwitchBool @  {
                    Inp1 ~ SI_1
                    Inp2 ~ SI_0
                    Sel ~ Cmp_Ge : TrCmpVar @  {
                        Inp ~ Count
                        Inp2 ~ : State {
                            = "SI 8"
                        }
                    }
                }
            }
        }
        : Sout @  {
            _@ <  {
                = "SS"
            }
            Inp ~ : TrApndVar @  {
                Inp1 ~ : TrApndVar @  {
                    Inp1 ~ : SdoName
                    Inp2 ~ : State {
                        = "SS ':  '"
                    }
                }
                Inp2 ~ : TrTostrVar @  {
                    Inp ~ Count
                }
            }
        }
    }
    Launcher : DesLauncher {
        Debug.LogLevel = "Dbg"
        Controller : Des {
            # "Controller"
            Manageable : Syst {
                # "Managed system"
                DesAgt : ADes
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
                SdcPause : ASdcPause @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ : SB_True
                }
                CreateIncr1 : ASdcComp @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ SdcPause.Outp
                    Name ~ : Const {
                        = "SS Incr1"
                    }
                    Parent ~ : Const {
                        = "SS Incrementor"
                    }
                }
                CreateIncr2 : ASdcComp @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ CreateIncr1.Outp
                    Name ~ : Const {
                        = "SS Incr2"
                    }
                    Parent ~ : Const {
                        = "SS Incrementor"
                    }
                }
                SdcResume : ASdcResume @  {
                    _@ < Debug.LogLevel = "Dbg"
                    Enable ~ CreateIncr2.Outp
                }
            }
        }
    }
}
