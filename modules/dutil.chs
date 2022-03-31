DesUtils : Elem {
    # "DES utilities";
    BChange : Des {
        # "Boolean change";
        SInp : Extd { Int : CpStateOutp; }
        Outp : Extd { Int : CpStateInp; }
        TAnd : TrAndVar @ {
            Inp ~ SInp.Int;
            Inp ~ : TrNegVar @ {
                Inp ~ Delay : State @ {
                    _@ < = "SB false";
                    _@ < Debug.LogLevel = "Dbg";
                    Inp ~ SInp.Int;
                };
            };
        }
        Outp.Int ~ TAnd;
    }
    BChangeCnt : Des {
        # "Boolean change counter";
        SInp : Extd { Int : CpStateOutp; }
        Outp : Extd { Int : CpStateInp; }
        Chg : BChange;
        Chg.SInp ~ SInp.Int;
        Cnt : State @ {
            _@ < = "SI 0";
            _@ < Debug.LogLevel = "Dbg";
            Inp ~ : TrAddVar @ {
                Inp ~ Cnt;
                Inp ~ : TrSwitchBool @ {
                    Sel ~ Chg.Outp;
                    Inp1 ~ Const_0 : State { = "SI 0"; };
                    Inp2 ~ Const_1 : State { = "SI 1"; };
                };
            };
        }
        Outp.Int ~ Cnt;
    }
    SetTg : Des {
    # "Set trigger";
        InpSet : Extd { Int : CpStateOutp; }
        Outp : Extd { Int : CpStateInp; }
        Outp.Int ~ Value : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
            Inp ~ : TrOrVar @ {
                Inp ~ InpSet.Int;
                Inp ~ Value;
            };
       };
   }
   RSTg : Des {
    # "R/S trigger, positive inputs";
        InpS : Extd { Int : CpStateOutp; }
        InpR : Extd { Int : CpStateOutp; }
        Outp : Extd { Int : CpStateInp; }
        Outp.Int ~ Value : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
            Inp ~ : TrOrVar @ {
                Inp ~ InpS.Int;
                Inp ~ : TrAndVar @ {
                    Inp ~ : TrNegVar @ { Inp ~ InpR.Int; };
                    Inp ~ Value;
                };
            };
       };
   }

}
