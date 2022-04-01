Root : Elem {
    # "Test of SDC, list insertion/extraction";
    Modules : Node
    {
        + DesUtils; 
    } 
    # "Runner";
    Launcher :  DesLauncher {
        # "Node";
        LNode : Syst {
           Prev : ConnPointu {
               Provided = "PrevIface";
               Required = "NextIface";
           }
           Next : ConnPointu {
               Provided = "NextIface";
               Required = "PrevIface";
           }
        }
        List : Des {
            Controllable = "y";
            Start : LNode;
            End : LNode;
            Start.Prev ~ End.Next;
            # "Creating component";
            Dc_Comp : ASdcComp @ {
                _@ < Debug.LogLevel = "Dbg";
                Enable ~ : State { = "SB true"; };
                Name ~ : State { = "SS Node_1"; };
                Parent ~ : State { = "SS LNode"; };
            }
            Dc_Insert : ASdcInsert2 @ {
                _@ < Debug.LogLevel = "Dbg";
                Name ~ Dc_Comp.OutpName;
                Prev ~ : State { = "SS Prev"; };
                Next ~ : State { = "SS Next"; };
                Pname ~ : State { = "SS End"; };
            }
            Dc_Insert_Out_Dbg : State @ {
                _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
                Inp ~ Dc_Insert.Outp;
            }
            Dc_Extract : ASdcExtract @ {
                _@ < Debug.LogLevel = "Dbg";
                Enable ~ Dc_Insert.Outp;
                Name ~ Dc_Comp.OutpName;
                Prev ~ : State { = "SS Prev"; };
                Next ~ : State { = "SS Next"; };
            }
            Dc_Extract_Out_Dbg : State @ {
                _@ < { Debug.LogLevel = "Dbg"; = "SB false"; }
                Inp ~ Dc_Extract.Outp;
            }
            # "Enable insertion first on creation than on extraction";
            Dc_Insert.Enable ~ : TrSwitchBool @ {
                _@ < Debug.LogLevel = "Dbg";
                Inp1 ~ Dc_Comp.Outp;
                Inp2 ~ Dc_Extract.Outp;
                WasInserted : DesUtils.SetTg @ { InpSet ~ Dc_Insert.Outp; }
                Sel ~ WasInserted.Outp;
            };
        }
    }
}
