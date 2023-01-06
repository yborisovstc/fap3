DesUtils : Elem {
    # "DES utilities"
    BChange : Des {
        # "Boolean change"
        SInp : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        TAnd : TrAndVar @  {
            Inp ~ SInp.Int
            Inp ~ : TrNegVar @  {
                Inp ~ Delay : State @  {
                    _@ < = "SB false"
                    _@ < Debug.LogLevel = "Dbg"
                    Inp ~ SInp.Int
                }
            }
        }
        Outp.Int ~ TAnd
    }
    BChangeCnt : Des {
        # "Boolean change counter"
        SInp : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Chg : BChange
        Chg.SInp ~ SInp.Int
        Cnt : State @  {
            _@ < = "SI 0"
            _@ < Debug.LogLevel = "Dbg"
            Inp ~ : TrAddVar @  {
                Inp ~ Cnt
                Inp ~ : TrSwitchBool @  {
                    Sel ~ Chg.Outp
                    Inp1 ~ Const_0 : State {
                        = "SI 0"
                    }
                    Inp2 ~ Const_1 : State {
                        = "SI 1"
                    }
                }
            }
        }
        Outp.Int ~ Cnt
    }
    SetTg : Des {
        # "Set trigger"
        InpSet : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Outp.Int ~ Value : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ : TrOrVar @  {
                Inp ~ InpSet.Int
                Inp ~ Value
            }
        }
    }
    RSTg : Des {
        # "R/S trigger, positive inputs"
        InpS : Extd {
            Int : CpStateOutp
        }
        InpR : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Outp.Int ~ Value : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SB false"
            }
            Inp ~ : TrOrVar @  {
                Inp ~ InpS.Int
                Inp ~ : TrAndVar @  {
                    Inp ~ : TrNegVar @  {
                        Inp ~ InpR.Int
                    }
                    Inp ~ Value
                }
            }
        }
    }
    DPulse : Des {
        # "Data pulse"
        # "Set delay and const type before usage"
        # "InpD - data input, InpE - data to pass when no changes happen"
        InpD : Extd {
            Int : CpStateOutp
        }
        InpE : Extd {
            Int : CpStateOutp
        }
        Outp : Extd {
            Int : CpStateInp
        }
        Outp.Int ~ : TrSwitchBool @  {
            Sel ~ Cmp_Neq : TrCmpVar @  {
                Inp ~ InpD.Int
                Inp2 ~ Delay : State @  {
                    Inp ~ InpD.Int
                }
            }
            Inp1 ~ InpE.Int
            Inp2 ~ InpD.Int
        }
    }
    InpItr : Des {
        # "Inputs iterator"
        # "InpM - multiplexed input"
        # "InpDone - sign of selected input is handled"
        InpM : ExtdStateInp
        InpDone : ExtdStateInp
        Outp : ExtdStateOutp
        ICnt : TrInpCnt @  {
            Inp ~ InpM.Int
        }
        ICnt_Dbg : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "SI <ERR>"
            }
            Inp ~ ICnt
        }
        InpMIdx : State @  {
            # "Input index"
            _@ <  {
                = "SI 0"
                Debug.LogLevel = "Dbg"
            }
            Inp ~ : TrSwitchBool @  {
                _@ < Debug.LogLevel = "Dbg"
                Sel ~ CidxAnd1 : TrAndVar @  {
                    Inp ~ Cmp_Gt : TrCmpVar @  {
                        Inp ~ : TrAddVar @  {
                            Inp ~ ICnt
                            InpN ~ : State {
                                = "SI 1"
                            }
                        }
                        Inp2 ~ InpMIdx
                        _@ < Debug.LogLevel = "Dbg"
                    }
                    Inp ~ InpDone.Int
                }
                Inp1 ~ InpMIdx
                Inp2 ~ : TrAddVar @  {
                    Inp ~ InpMIdx
                    Inp ~ : State {
                        = "SI 1"
                    }
                }
            }
        }
        Outp.Int ~ InpMIdx
    }
}
