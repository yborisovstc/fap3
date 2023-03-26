Root : Elem {
    # "Test of SDC, list insertion/extraction, number of comps"
    # "Ref DS_ISS_013 for the use-case"
    # "Runner"
    Launcher : DesLauncher {
        # "Node"
        LNode : Syst {
            Prev : ConnPointu {
                Provided = "PrevIface"
                Required = "NextIface"
            }
            Next : ConnPointu {
                Provided = "NextIface"
                Required = "PrevIface"
            }
        }
        List : Des {
            Controllable = "y"
            KS_Prev : State {
                = "SS Prev"
            }
            KS_Next : State {
                = "SS Next"
            }
            KS_End : State {
                = "SS End"
            }
            KS_LNode : State {
                = "SS LNode"
            }
            Start : LNode
            End : LNode
            Start.Prev ~ End.Next
            # "Creating components"
            Dc_Comp : ASdcComp @  {
                _@ < Debug.LogLevel = "Dbg"
                Enable ~ : State {
                    = "SB true"
                }
                Name ~ : State {
                    = "SS Node_1"
                }
                Parent ~ KS_LNode
            }
            Dc_Comp2 : ASdcComp @  {
                _@ < Debug.LogLevel = "Dbg"
                Enable ~ Dc_Comp.Outp
                Name ~ : State {
                    = "SS Node_2"
                }
                Parent ~ KS_LNode
            }
            # "With DS_ISS_013 this sdc will -insert- Node_1 twice, the second time after Node_2 inserted"
            Dc_Insert : ASdcInsert2 @  {
                _@ < Debug.LogLevel = "Dbg"
                Enable ~ Dc_Comp.Outp
                Name ~ Dc_Comp.OutpName
                Prev ~ KS_Prev
                Next ~ KS_Next
                Pname ~ KS_End
            }
            Dc_Insert_Out_Dbg : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SB false"
                }
                Inp ~ Dc_Insert.Outp
            }
            Dc_Insert2 : ASdcInsert2 @  {
                _@ < Debug.LogLevel = "Dbg"
                Enable ~ Dc_Comp2.Outp
                Name ~ Dc_Comp2.OutpName
                Prev ~ KS_Prev
                Next ~ KS_Next
                Pname ~ KS_End
            }
            Is_conn_ok : SdoConn @  {
                # "Verificator if connection in the list is correct"
                _@ < Debug.LogLevel = "Dbg"
                Vp ~ : State {
                    = "SS Node_2.Prev"
                }
                Vq ~ : State {
                    = "SS End.Next"
                }
            }
            Is_conn_ok_Dbg : State @  {
                _@ <  {
                    Debug.LogLevel = "Dbg"
                    = "SB"
                }
                Inp ~ : TrAndVar @  {
                    Inp ~ Is_conn_ok
                    Inp ~ Dc_Comp2.Outp
                }
            }
        }
    }
}
