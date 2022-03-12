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
}
