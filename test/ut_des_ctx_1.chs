Root : Elem {
    # "Test of DES context #1";
    # "Verifying that DES resolves suppliers stack head only, ref ds_dctx_dic_cs";
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
            }
            System2 : Des {
                # "Common context (stack)";
                CtxC : DesCtxSpl {
                    Ctx1 : State { Debug.LogLevel = "Dbg"; = "SI 0"; }
                }
                CtxC.Ctx1.Inp ~ : TrAddVar @ {
                    Inp ~ CtxC.Ctx1;
                    Inp ~ : State { = "SI 1"; };
                };
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
        # "Agent based DES";
        SystemA1 : Syst {
            DesAgt : ADes;
            # "Common context (stack)";
            CtxA : DesCtxSpl {
            }
            CtxB : DesCtxSpl {
            }
            CtxC : DesCtxSpl {
            }
            SystemA2 : Syst {
                DesAgt : ADes;
                # "Common context (stack)";
                CtxC : DesCtxSpl {
                    Ctx1 : State { Debug.LogLevel = "Dbg"; = "SI 0"; }
                }
                CtxC.Ctx1.Inp ~ : TrAddVar @ {
                    Inp ~ CtxC.Ctx1;
                    Inp ~ : State { = "SI 1"; };
                };
                SystemA3 : Syst {
                    DesAgt : ADes;
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
