Root : Elem {
    # "Test of DES context #5"
    # "Upward data flow + inputs serialization"
    + DesUtils
    # "Runner"
    Launcher : DesLauncher {
        System1 : Des {
            # "Common context (stack)"
            CtxC : DesCtxSpl {
                Ctx1 : ExtdStateInp
            }
            S1Add : TrAddVar @  {
                Inp ~ CtxC.Ctx1.Int
            }
            S1Add_Dbg : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SI <ERR>"
                }
                Inp ~ S1Add
            }
            System2 : Des {
                # "Data provider 1"
                CtxC : DesCtxCsm {
                    Ctx1 : ExtdStateInp
                }
                CtxC.Ctx1 ~ : State {
                    Debug.LogLevel = "Dbg"
                    = "SI 2"
                }
                CtxC.Ctx1 ~ : State {
                    Debug.LogLevel = "Dbg"
                    = "SI 3"
                }
            }
            System3 : Des {
                # "Data provider 2"
                CtxC : DesCtxCsm {
                    Ctx1 : ExtdStateInp
                }
                CtxC.Ctx1 ~ : State {
                    Debug.LogLevel = "Dbg"
                    = "SI 4"
                }
                CtxC.Ctx1 ~ : State {
                    = "SI 5"
                }
            }
            # "Inputs Iterator"
            Srz : DesUtils.InpItr @  {
                InpM ~ CtxC.Ctx1.Int
                InpDone ~ : State {
                    = "SB true"
                }
            }
            # "Selected input"
            SelectedInp : TrInpSel @  {
                Inp ~ CtxC.Ctx1.Int
                Idx ~ Srz.Outp
            }
            SelInp_Dbg : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SI <ERR>"
                }
                Inp ~ SelectedInp
            }
        }
    }
}
