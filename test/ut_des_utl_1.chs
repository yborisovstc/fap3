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
    }
}
