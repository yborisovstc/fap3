Root : Elem {
    # "Test of DES context #2";
    # "Verifying context overlay";
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
                Ctx1 : State { Debug.LogLevel = "Dbg"; = "SI 33"; }
            }
            System2 : Des {
                # "Common context (stack)";
                CtxCCsm : DesCtxCsm {
                    Id = "CtxC";
                    Ctx1 : ExtdStateOutp;
                }
                CtxC : DesCtxSpl {
                    Ctx1 : TrAddVar @ {
                        Inp ~ : State { = "SI -1"; };
                    }
                }
                CtxC.Ctx1.Inp ~ CtxCCsm.Ctx1;
                System3 : Des {
                    CtxD : DesCtxSpl {
                        CtxD1 : State { Debug.LogLevel = "Dbg"; = "SI 44"; }
                    }
                    CtxC : DesCtxCsm {
                        Ctx1 : ExtdStateOutp;
                    }
                    CtxC_Dbg : State @ {
                        _@ < { Debug.LogLevel = "Dbg"; = "SI 0"; }
                        Inp ~ CtxC.Ctx1;
                    }
                }
            }
        }
    }
}
