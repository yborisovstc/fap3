Root : Elem {
    # "Test of DES context #4";
    # "Upward data flow";
    Modules : Node
    {
    } 
    # "Runner";
    Launcher :  DesLauncher {
        System1 : Des {
            # "Common context (stack)";
            CtxA : DesCtxSpl {
            }
            CtxB : DesCtxSpl {
            }
            CtxC : DesCtxSpl {
                Ctx1 : ExtdStateInp;
                Ctx2 : ExtdStateInp;
            }
            S1Add : TrAddVar @ {
                Inp ~ CtxC.Ctx1.Int;
                Inp ~ CtxC.Ctx2.Int;
            }
            S1Add_Dbg : State @ {
                _@ < { Debug.LogLevel = "Dbg"; = "SI <ERR>"; }
                Inp ~ S1Add;
            }
            System2 : Des {
                # "Data provider 1";
                CtxC : DesCtxCsm {
                    Ctx1 : ExtdStateInp;
                    Ctx2 : ExtdStateInp;
                }
                CtxC.Ctx1 ~ : State { Debug.LogLevel = "Dbg"; = "SI 2"; };
                CtxC.Ctx2 ~ : State { Debug.LogLevel = "Dbg"; = "SI 3"; };
            }
            System3 : Des {
                # "Data provider 2";
                CtxC : DesCtxCsm {
                    Ctx1 : ExtdStateInp;
                }
                CtxC.Ctx1 ~ : State { Debug.LogLevel = "Dbg"; = "SI 4"; };
            }
        }
    }
}
