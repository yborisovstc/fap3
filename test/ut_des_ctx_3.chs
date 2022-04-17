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
                Ctx2 : State { Debug.LogLevel = "Dbg"; = "SS This is System1 Ctx2"; }
            }
            System2 : Des {
                # "Common context (stack)";
                CtxC : DesCtxSpl {
                    CtxC : DesCtxCsm {
                        Ctx1 : ExtdStateOutp;
                    }
                    Ctx1 : TrAddVar @ {
                        Inp ~ : State { = "SI -1"; };
                        Inp ~ CtxC.Ctx1;
                    }
                }
                System3 : Des {
                    CtxD : DesCtxSpl {
                        CtxD1 : State { Debug.LogLevel = "Dbg"; = "SI 44"; }
                    }
                    CtxC : DesCtxCsm {
                        Ctx1 : ExtdStateOutp;
                        Ctx2 : ExtdStateOutp;
                    }
                    CtxC_Ctx1_Dbg : State @ {
                        _@ < { Debug.LogLevel = "Dbg"; = "SI 0"; }
                        Inp ~ CtxC.Ctx1;
                    }
                    CtxC_Ctx2_Dbg : State @ {
                        _@ < { Debug.LogLevel = "Dbg"; = "SS"; }
                        Inp ~ CtxC.Ctx2;
                    }
                }
            }
        }
    }
}
