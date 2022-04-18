MyRoot : Elem {
    Modules : Node
    {
        + DesUtils;
    } 
    # "CP of object of ordering";
    Launcher :  DesLauncher {
        StInp : State { = "SB true"; }
        Chg : DesUtils.BChange @ {
            SInp ~ StInp;
        }
        BChange_Dbg : State @ {
            _@ < {
                = "SB false";
                Debug.LogLevel = "Dbg";
            }
            Inp ~ Chg.Outp;
        }
        BChange_Cnt : DesUtils.BChangeCnt @ {
            SInp ~ StInp;
        }
        # "Data pulse";
        SDPulse : State @ {
            _@ < { Debug.LogLevel = "Dbg"; = "URI _INV"; }
            Dp : DesUtils.DPulse @ {
                InpD ~ : State { = "URI Hello.World"; };
                InpE ~ : State { = "URI _INV"; };
            }
            Dp.Delay < = "URI";
            Inp ~ Dp.Outp;
        }
    }
}
